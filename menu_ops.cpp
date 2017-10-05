#include "graphic_creator.h"
#include <cmath>

#define PI 3.14159

/**
 * Redraw the screen, resetting button highlighting.
 *
 * @param the_window    The window to be drawn to
 * @param to_draw       The main user image
 * @param menu          A vector of menu items
 */
void redraw_screen (window &the_window, bitmap &to_draw, vector<menu_item> &menu)
{
    draw_bitmap_on_window (the_window, to_draw, 0, 0);
    for (menu_item item: menu)
        draw_bitmap_on_window (the_window, item.graphic, item.x, item.y);
}

/**
 * Highlight a menu item.
 * 
 * @param the_window    The window to be drawn to
 * @param highlight     Bitmap used to highlight a menu button
 * @param button        Menu item to be highlighted
 */            
void draw_highlight (window &the_window, bitmap highlight, menu_item button)
{
    draw_bitmap_on_bitmap (highlight, button.graphic, 4, 4);   
    draw_bitmap_on_window (the_window, highlight, button.x - 3, button.y - 1);
}

/**
 * Check if the mouse cursor is inside a given circle
 *
 * @param centre    The centre point of the circle
 * @param radius    The radius of the circle
 *
 * @returns     True if the cursor is within the radius, false otherwise.
 */
bool point_in_circle (point_2d &centre, int radius)
{
    return pow ((mouse_x() - centre.x), 2)
            + pow ((mouse_y() - centre.y), 2)
            <= pow (radius, 2);
}

/**
 * Update the position of the bitmap of a menu item
 *
 * @param item      The menu item to be updated
 * @param pos       The position of the menu item in a vector
 * @param length    The length of the vector containing the menu item
 */
void update_bitmap_pos (menu_item &item, int pos, int length)
{
    // The "+ PI * 1.5" isn't necessary for this to work, it's just to line things up the way I like it :)
    double x_change = cos (2 * PI * pos / length + PI * 1.5) * 2;
    double y_change = sin (2 * PI * pos / length + PI * 1.5) * 2;

    item.x += x_change;
    item.centre.x += x_change;
    item.y += y_change;
    item.centre.y += y_change;
}

/**
 * Create a new menu item for selecting drawing options
 *
 * @param   name        The name of the bitmap to be used as the menu item graphic
 * @param   x           The x position of the menu item
 * @param   y           The y position of the menu item
 *
 * @returns             The initialised menu item
 */
menu_item create_menu_item (string name, double x, double y)
{
    menu_item result;
    result.graphic = bitmap_named (name);
    result.centre.x = x + bitmap_width (result.graphic)/2;
    result.centre.y = y + bitmap_height (result.graphic)/2;
    result.x = x;
    result.y = y;

    return result;
}

/**
 * Create a vector of menu items for drawing the sub-menu on screen
 *
 * @param x     The starting x position of the menu items
 * @param y     The starting y position of the menu items
 * @param mode  The currently selected draw mode
 *
 * @returns     A vector of menu items
 */

vector<menu_item> create_sub_menu (double x, double y, mode_option mode)
{
    vector<menu_item> result;

    if (mode == DRAW_REC)
    {
        result.push_back (create_menu_item ("menu_draw_rec", x, y));
        result.push_back (create_menu_item ("menu_fill_rec", x, y));
    }

    else if (mode == DRAW_ELL)
    {
        result.push_back (create_menu_item ("menu_draw_ell", x, y));
        result.push_back (create_menu_item ("menu_fill_ell", x, y));
    }

    else if (mode == DRAW_TRI)
    {
        result.push_back (create_menu_item ("menu_draw_tri", x, y));
        result.push_back (create_menu_item ("menu_fill_tri", x, y));
    }

    return result;
}

/**
 * Create a vector of menu items for drawing the menu on screen
 *
 * @param   x   The starting x position of the menu items
 * @param   y   The starting y position of the menu items
 *
 * @returns     A vector of menu items
 */ 
vector<menu_item> create_paint_menu (double x, double y, mode_option mode)
{
    vector<menu_item> result;

    result.push_back (create_menu_item ("menu_fill_rec", x, y));
    result.push_back (create_menu_item ("menu_fill_ell", x, y));
    result.push_back (create_menu_item ("menu_fill_tri", x, y));
    result.push_back (create_menu_item ("menu_pen", x, y));
    result.push_back (create_menu_item ("menu_spray", x, y));

    return result;
}

/**
 * Process user interactions with the sub-menu
 *
 * @param   program     Struct containing program data
 * @param   menu        The vector of menu items drawn on screen to be checked for interaction
 * @param   radius      The radius of the menu items
 */
void process_sub_menu (program_data &program, vector<menu_item> &menu, int radius)
{
    double width = bitmap_width (menu[0].graphic);
    bitmap highlight = create_bitmap ("highlight", width + 7, width + 9);

    while (program.mode == NONE)
    {
        fill_ellipse_on_bitmap (highlight, COLOR_RED, 0, 0, width + 6, width + 8);
        process_events();

        // Redraw screen without highlights
        redraw_screen (program.the_window, program.to_draw, menu);

        if (point_in_circle (menu[0].centre, radius))
        {
            draw_highlight (program.the_window, highlight, menu[0]);

            if (mouse_clicked (LEFT_BUTTON))
                program.mode = program.select[0];
        }

        else if (point_in_circle (menu[1].centre, radius))
        {
            draw_highlight (program.the_window, highlight, menu[1]);

            if (mouse_clicked (LEFT_BUTTON))
                program.mode = program.select[1];
        }

        else if (mouse_clicked (LEFT_BUTTON))
            break;  
        
        refresh_window (program.the_window);
    }
}


/**
 * Process user interactions with the main paint menu
 *
 * @param   program     Struct containing program data
 * @param   menu        The vector of menu items drawn on screen to be checked for interaction
 * @param   radius      The radius of the menu items
 */
void process_paint_menu (program_data &program, vector<menu_item> &menu, int radius)
{
    program.mode = NONE;
    double width = bitmap_width (menu[0].graphic);
    bitmap highlight = create_bitmap ("highlight", width + 7, width + 9);    

    while (program.mode == NONE)
    {
        process_events();

        fill_ellipse_on_bitmap (highlight, COLOR_RED, 0, 0, width + 6, width + 8);

        // Redraw screen without highlights
        redraw_screen (program.the_window, program.to_draw, menu);

        if (point_in_circle (menu[0].centre, radius))
        {
            draw_highlight (program.the_window, highlight, menu[0]);                    

            if (mouse_clicked (LEFT_BUTTON))
            {
                program.select[0] = DRAW_REC;
                program.select[1] = FILL_REC;
                draw_menu (program, create_sub_menu, process_sub_menu);
            }
        }

        else if (point_in_circle (menu[1].centre, radius))
        {
            draw_highlight (program.the_window, highlight, menu[1]);

            if (mouse_clicked (LEFT_BUTTON))
            {
                program.select[0] = DRAW_ELL;
                program.select[1] = FILL_ELL;
                draw_menu (program, create_sub_menu, process_sub_menu);
            }
        }

        else if (point_in_circle (menu[2].centre, radius))
        {
            draw_highlight (program.the_window, highlight, menu[2]);

            if (mouse_clicked (LEFT_BUTTON))
            {
                program.select[0] = DRAW_TRI;
                program.select[1] = FILL_TRI;
                draw_menu (program, create_sub_menu, process_sub_menu);
            }
        }

        else if (point_in_circle (menu[3].centre, radius))
        {
            draw_highlight (program.the_window, highlight, menu[3]);

            if (mouse_clicked (LEFT_BUTTON))
                program.mode = PEN;
        }

        else if (point_in_circle (menu[4].centre, radius))
        {
            draw_highlight (program.the_window, highlight, menu[4]);

            if (mouse_clicked (LEFT_BUTTON))
                program.mode = SPRAY;
        }

        else if (mouse_clicked (LEFT_BUTTON))
            break;

        refresh_window (program.the_window);
    }
}

/**
 * Draws either the main paint menu, or the sub-menu
 *
 * @param program       Struct containing program data
 * @param create_menu   A function to create menu items for either the paint menu, or sub-menu
 * @param process_menu  A function to process interations with either the paint menu, or sub-menu
 */
void draw_menu(program_data &program, vector<menu_item>(create_menu)(double, double, mode_option), 
                void (process_menu)(program_data&, vector<menu_item>&, int))
{
    double x = mouse_x()-35;
    double y = mouse_y()-35;
    int separation = 0;
    bool bitmap_collided = true;

    // Ensure that the menu won't be drawn off screen
    if (x < 100) x = 100;
    if (x > 650) x = 650;
    if (y < 100) y = 100;
    if (y > 450) y = 450;

    vector<menu_item>  menu = create_menu (x, y, program.select[0]);     

    while (separation < 5)
    {
        draw_bitmap (program.to_draw, 0, 0);
        for (int i = 0; i < menu.size(); i++)
        {
            draw_bitmap (menu[i].graphic, menu[i].x, menu[i].y);
            update_bitmap_pos (menu[i], i, menu.size());
        }     
        refresh_window(program.the_window);

        // If the first 2 bitmaps in the menu are no longer colliding, increment the separation var
        if (not bitmap_collision (menu[0].graphic, menu[0].x, menu[0].y,
            menu[1].graphic, menu[1].x, menu[1].y))
            separation++;
    }  

    process_menu (program, menu, bitmap_width(menu[0].graphic)/2);
}