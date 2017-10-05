#include "graphic_creator.h"
#include <cmath>
#include <list>

#define SPRAY_PARTICLES 30
#define RADIUS 20
#define PI 3.14159

using namespace std;

/**
 * Eraser draw mode. Draws a white 10x10 square at the mouse location while left mouse is down.
 *
 * @param program    Struct containing program data
 */
void paint_eraser (program_data &program)
{
    add_to_undo (program);
    wipe_redo (program.redo);

    while (mouse_down (LEFT_BUTTON))
    {
        process_events();
        fill_rectangle_on_bitmap (program.to_draw, COLOR_WHITE, mouse_x(), mouse_y(), 10, 10);
        draw_bitmap_on_window (program.the_window, program.to_draw, 0, 0);
        refresh_window (program.the_window);
    }    
}

/**
 * Pen draw mode. Draws a 4x4 ellipse at the mouse location while left mouse is down.
 *
 * @param program    Struct containing program data
 */
void paint_pen (program_data &program)
{
    add_to_undo (program);
    wipe_redo (program.redo);

    while (mouse_down (LEFT_BUTTON))
    {
        process_events();
        
        fill_ellipse_on_bitmap (program.to_draw, program.active_color, mouse_x(), mouse_y(), 4, 4);
        draw_bitmap_on_window (program.the_window, program.to_draw, 0, 0);

        refresh_window (program.the_window);
    }
    
}

/**
 * Spray draw mode. Draws 30 pixels randomly scattered within a radius of 20 pixels from the mouse cursor.
 *
 * @param program    Struct containing program data
 */
void paint_spray (program_data &program)
{
    double angle;
    double rad;
    double draw_x;
    double draw_y;

    add_to_undo (program);
    wipe_redo (program.redo);
    
    while (mouse_down (LEFT_BUTTON))
    {
        process_events();
    
        draw_bitmap_on_window (program.the_window, program.to_draw, 0, 0);

        // Draw 30 pixels on the bitmap at random points within a radius of 20 of the mouse cursor
        for (int i = 0; i < SPRAY_PARTICLES; i++)
        {
            angle = rnd() * PI * 2;
            rad = rnd() * RADIUS;
            draw_x = mouse_x() + rad * cos (angle);
            draw_y = mouse_y() + rad * sin (angle);
       
            draw_pixel_on_bitmap (program.to_draw, program.active_color, draw_x, draw_y);
        }
        refresh_screen();
    }
}

/**
 * Resize and draw a rectangle or ellipse to the user bitmap image.
 *
 * @param program                Struct containing program data
 * @param draw_rec_ell_to_win    A function to draw or fill a rectangle or ellipse to a window
 * @param draw_rec_ell_to_bitmap A function to draw or fill a rectangle or ellipse to a bitmap
 */
void paint_rec_ell (program_data &program, void (draw_rec_ell_to_win) (window, color, double, double, double, double), 
                    void (draw_rec_ell_to_bitmap) (bitmap, color, double, double, double, double))
{
    double width, height;
    double x = mouse_x();
    double y = mouse_y();

    add_to_undo (program);
    wipe_redo (program.redo);

    /*  Draw the current user image to the window, draw a rectangle/ellipse on the window
        whose size is defined by interaction via the mouse, then refresh the window. Looping this 
        gives a dynamic resizing effect. When the left mouse button is released, commit the 
        last drawn rectangle/ellipse to the user image. */
    while (mouse_down (LEFT_BUTTON))
    {
        process_events();    

        draw_bitmap_on_window (program.the_window, program.to_draw, 0, 0);
           
        width = mouse_x() - x;
        height = mouse_y() - y;

        // Ensure shape drawn does not overlap sidebar
        if (mouse_x() < 800)
            draw_rec_ell_to_win (program.the_window, program.active_color, x, y, width, height);
        else
            draw_rec_ell_to_win (program.the_window, program.active_color, x, y, 800-x, height);

        refresh_window(program.the_window);
    }

    draw_rec_ell_to_bitmap (program.to_draw, program.active_color, x, y, width, height);
}

/**
 * Resize and draw a triangle to the user bitmap image
 *
 * @param program               Struct containing program data
 * @param draw_tri_to_win       A function to draw or fill a triangle to a window
 * @param draw_tri_to_bitmap    A function to draw or fill a triangle to a bitmap
 */
void paint_tri (program_data &program, void (draw_tri_to_win) (window, color, double, double, double, double, double, double), 
                void (draw_tri_to_bitmap) (bitmap, color, double, double, double, double, double, double))
{
    double x = mouse_x();
    double y = mouse_y();

    bitmap temp = create_bitmap ("temp", 800, 600);

    add_to_undo (program);
    wipe_redo (program.redo);

    /*  Draw the current user image to the window, draw a triangle on the window
        whose size is defined by interaction via the mouse, then refresh the window. Looping this 
        gives a dynamic resizing effect. When the left mouse button is released, commit the 
        last drawn triangle to the user image. */
    while (mouse_down (LEFT_BUTTON))
    {
        process_events();    
        
        draw_bitmap_on_bitmap (temp, program.to_draw, 0, 0);
        draw_bitmap_on_window (program.the_window, program.to_draw, 0, 0);

        // Ensure triangle does not overlap sidebar
        if (mouse_x() < 800 && x-(mouse_x()-x) < 800)
        {
            draw_tri_to_win (program.the_window, program.active_color, x-(mouse_x()-x), mouse_y(), x, y, mouse_x(), mouse_y());
            draw_tri_to_bitmap (temp, program.active_color, x-(mouse_x()-x), mouse_y(), x, y, mouse_x(), mouse_y());
        }

        else if (x-(mouse_x()-x) < 800)
        {
            draw_tri_to_win (program.the_window, program.active_color, x-(mouse_x()-x), mouse_y(), x, y, 799, mouse_y());
            draw_tri_to_bitmap (temp, program.active_color, x-(mouse_x()-x), mouse_y(), x, y, 799, mouse_y());
        }
        else
        {
            draw_tri_to_win (program.the_window, program.active_color, 799, mouse_y(), x, y, x-(799-x), mouse_y());
            draw_tri_to_bitmap (temp, program.active_color, 799, mouse_y(), x, y, x-(799-x), mouse_y());
        }
        refresh_window(program.the_window);
    }

    draw_bitmap_on_bitmap (program.to_draw, temp, 0, 0);
}

/**
 * Fills the previously selected area with white, and re-draws the stored bitmap of the selected
 * area over the top. When the stored bitmap is later moved this gives the effect of it having 
 * been 'cut out' of the original image.
 *
 * @param to_draw   The main user image
 * @param x         x position of the selected area
 * @param y         y position of the selected area
 * @param width     width of the selected area
 * @param height    height of the selected area 
 *
 * @returns         Bitmap of the selected area
 */
bitmap draw_selection (bitmap &to_draw, double x, double y, double width, double height)
{
    bitmap result = create_bitmap ("selected area", width, height);

    double x_draw_start = x + width - 800;
    double y_draw_start = y + height - 600;

    draw_bitmap_on_bitmap (result, to_draw, 0, 0, option_part_bmp (x, y, width, height));
   
    fill_rectangle_on_bitmap (to_draw, COLOR_WHITE, x, y, width, height);

    return result;
}

/**
 * Select a rectangular area of the bitmap image and store it for manipulation.
 *
 * @param program    Struct containing program data
 *
 * @returns          A struct containing the selected area as a bitmap, and the coordinates relative to the main image.
 */          
select_tool_data select_area (program_data &program)
{
    double x, y;
    select_tool_data result;
    double width, height;
    
    while (! mouse_down (LEFT_BUTTON))
        process_events();
        
    x = mouse_x();
    y = mouse_y();

    while (mouse_down (LEFT_BUTTON))
    {
        process_events();    
    
        draw_bitmap_on_window (program.the_window, program.to_draw, 0, 0);
               
        width = mouse_x() - x;
        height = mouse_y() - y;
    
        // Ensure shape drawn does not overlap sidebar
        if (mouse_x() < 800)
            draw_rectangle_on_window (program.the_window, program.active_color, x, y, width, height);
        else
            draw_rectangle_on_window (program.the_window, program.active_color, x, y, 800-x, height);
    
        refresh_window(program.the_window);
    }
    
    if (mouse_x() > x && mouse_y() > y)
    {
        if (mouse_x() < 800)
            result.graphic = draw_selection (program.to_draw, x, y, width, height);
        else
            result.graphic = draw_selection (program.to_draw, x, y, 800-x, height);
        result.x = x;
        result.y = y;
    } 

    if (mouse_x() < x && mouse_y() > y)
    {
        result.graphic = draw_selection (program.to_draw, mouse_x(), y, -width, height);
        result.x = mouse_x();
        result.y = y;
    } 

    if (mouse_x() > x && mouse_y() < y)
    {
        if (mouse_x() < 800)
            result.graphic = draw_selection (program.to_draw, x, mouse_y(), width, -height);
        else
            result.graphic = draw_selection (program.to_draw, x, mouse_y(), 800-x, -height);
        result.x = x;
        result.y = mouse_y();
    } 

    if (mouse_x() < x && mouse_y() < y)
    {
        result.graphic = draw_selection (program.to_draw, mouse_x(), mouse_y(), -width, -height);
        result.x = mouse_x();
        result.y = mouse_y();
    } 

    draw_bitmap_on_window (program.the_window, program.to_draw, 0, 0);
    draw_bitmap_on_window (program.the_window, result.graphic, result.x, result.y);
    refresh_window (program.the_window);

    return result;
}

/**
 * Main module for the selection and moving of an area of the user image.
 *
 * @param program    Struct containing program data
 */
void select_tool (program_data &program)
{
    add_to_undo (program);
    wipe_redo (program.redo);

    select_tool_data selection;
    double last_x, last_y;

    // Get area of image to be moved
    selection = select_area (program);    
    
    // Wait for the user to interact
    while (! mouse_down (LEFT_BUTTON))
        process_events();
    
    last_x = mouse_x();
    last_y = mouse_y();

    // Move selected area over the main image while left mouse is held down
    while (mouse_down (LEFT_BUTTON))
    {
        process_events();

        selection.x += mouse_x() - last_x;
        selection.y += mouse_y() - last_y;

        last_x = mouse_x();
        last_y = mouse_y();

        draw_bitmap_on_window (program.the_window, program.to_draw, 0, 0);
        draw_bitmap_on_window (program.the_window, selection.graphic, selection.x, selection.y);
        draw_sidebar (program.the_window, program.active_color);

        refresh_window (program.the_window);

    }
    draw_bitmap_on_bitmap (program.to_draw, selection.graphic, selection.x, selection.y);    
}

/**
 * Fills an enclosed area on the user image with a selected color. 
 *
 * @param program    Struct containing program data
 * @param target     Target color being replaced
 */
void fill_area (program_data &program, color target)
{
    int x = mouse_x();
    int y = mouse_y();
    list<point_2d> queue;
    point_2d temp, left, right;
    color current_pixel = get_pixel (program.to_draw, x, y);
    double target_blue = blue_of (target);
    double target_red = red_of (target);
    double target_green = green_of (target);

    // If the color at the mouse cursor is the same as the replacement color, return
    if (blue_of (current_pixel) != target_blue
        && red_of (current_pixel) != target_red
        && green_of (current_pixel) != target_green)
        return;

    // Store mouse position as first position in the queue
    temp.x = x;
    temp.y = y;
    queue.push_back (temp);

    for (point_2d n : queue)
    {
        left = n;
        right = n;

        // Move left until a pixel of non-target color is hit
        current_pixel = get_pixel (program.to_draw, left.x, left.y);
        while (blue_of (current_pixel) == target_blue
               && red_of (current_pixel) == target_red
               && green_of (current_pixel) == target_green
               && left.x >= 0)
        {
            left.x--;
            current_pixel = get_pixel (program.to_draw, left.x, left.y);
        }

        // Move right until a pixel of non-target color is hit
        current_pixel = get_pixel (program.to_draw, right.x, right.y);
        while (blue_of (current_pixel) == target_blue
               && red_of (current_pixel) == target_red
               && green_of (current_pixel) == target_green
               && right.x < 800)
        {
            right.x++;
            current_pixel = get_pixel (program.to_draw, right.x, right.y);
        }        

        // Move from left marker to right marker
        for (int i = left.x+1; i < right.x; i++)
        {       
            // Replace current pixel with selected color
            draw_pixel_on_bitmap (program.to_draw, program.active_color, i, n.y);   

            // If the pixel above is of the target color, add that pixel to the queue
            current_pixel = get_pixel (program.to_draw, i, n.y-1);
            if (blue_of (current_pixel) == target_blue
                && red_of (current_pixel) == target_red
                && green_of (current_pixel) == target_green)
            {
                temp.x = i;
                temp.y = n.y-1;
                queue.push_back (temp);
            }

            // If the pixel below is of the target color, add that pixel to the queue
            current_pixel = get_pixel (program.to_draw, i, n.y+1);
            if (blue_of (current_pixel) == target_blue
                && red_of (current_pixel) == target_red
                && green_of (current_pixel) == target_green)
            {
                temp.x = i;
                temp.y = n.y+1;
                queue.push_back (temp);
            }
        }            
    } 
}

/**
 * Main module for the fill tool
 *
 * @param program    Struct containing program data
 */
void fill_tool (program_data &program)
{
    color target = get_pixel (program.to_draw, mouse_x(), mouse_y());

    fill_area (program, target);   
    draw_bitmap_on_window (program.the_window, program.to_draw, 0, 0);
    refresh_window (program.the_window); 
}