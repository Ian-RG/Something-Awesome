#include "graphic_creator.h"

/**
 * Process the active drawing mode
 *
 * @param program    Struct containing program data
 */
void process_mode (program_data &program)
{
    switch (program.mode)
    {
        case DRAW_REC: paint_rec_ell (program, draw_rectangle_on_window, draw_rectangle_on_bitmap);
                       break;
        case FILL_REC: paint_rec_ell (program, fill_rectangle_on_window, fill_rectangle_on_bitmap);
                       break;
        case DRAW_ELL: paint_rec_ell (program, draw_ellipse_on_window, draw_ellipse_on_bitmap);
                       break;
        case FILL_ELL: paint_rec_ell (program, fill_ellipse_on_window, fill_ellipse_on_bitmap);
                       break;
        case DRAW_TRI: paint_tri (program, draw_triangle_on_window, draw_triangle_on_bitmap);
                       break;
        case FILL_TRI: paint_tri (program, fill_triangle_on_window, fill_triangle_on_bitmap);
                       break;
        case SPRAY:    paint_spray (program);
                       break;
        case PEN:      paint_pen (program);
                       break;
        case ERASER:   paint_eraser (program);
                       break;
        case SELECT:   select_tool (program);
                       break;
        case FILL:     fill_tool (program);
        }
}

/**
 * Draw title screen on window
 *
 * @param the_window    The window to be drawn to
 */
void draw_title_screen (window &the_window)
{
    draw_bitmap_on_window (the_window, bitmap_named ("title_screen"), 0, 0);
    refresh_window (the_window);
    delay (3000);
}

/**
 * Draw sidebar menu on the window
 *
 * @param the_window    The window to be drawn to
 * @param active_color  The current active color selected by the user
 */
void draw_sidebar (window &the_window, color &active_color)
{
    color colors_used[] = {COLOR_BLACK, COLOR_WHITE, COLOR_GRAY, COLOR_DARK_GRAY, COLOR_AQUA, COLOR_LIGHT_BLUE, COLOR_BLUE, COLOR_DARK_BLUE, COLOR_BROWN,
                         COLOR_GREEN, COLOR_BRIGHT_GREEN, COLOR_DARK_GREEN, COLOR_LIGHT_YELLOW, COLOR_YELLOW, COLOR_YELLOW_GREEN, COLOR_GOLD, COLOR_ORANGE, COLOR_ORANGE_RED, COLOR_PURPLE,
                         COLOR_LAVENDER, COLOR_PINK, COLOR_HOT_PINK, COLOR_RED, COLOR_CRIMSON}; 
    
    // The first color to be drawn on the gradient block
    color draw_gradient = active_color;
    double saturation = saturation_of (draw_gradient);
    double hue = hue_of (draw_gradient);
    double brightness = brightness_of (draw_gradient);

    // The y position of the first pair of icons
    int y_pos = 26;

    // Draw tool icons
    draw_text_on_window (the_window, "TOOLS", COLOR_BLACK, "Bold Font", 22, 805, 10);

    draw_bitmap_on_window (the_window, bitmap_named ("eraser_icon"), 801, y_pos);
    draw_bitmap_on_window (the_window, bitmap_named ("save_icon"), 827, y_pos);

    y_pos += 25;

    draw_bitmap_on_window (the_window, bitmap_named ("select_icon"), 801, y_pos);
    draw_bitmap_on_window (the_window, bitmap_named ("fill_icon"), 827, y_pos);

    y_pos += 25;

    // Draw color saturation block
    draw_text_on_window (the_window, "COLOR", COLOR_BLACK, "Bold Font", 22, 805, 80);
    draw_text_on_window (the_window, "SAT.", COLOR_BLACK, "Bold Font", 22, 810, 90);

    y_pos += 25;

    for (int j = 0; j < 50; j++)
    {
        draw_line_on_window (the_window, draw_gradient, 801 + j, y_pos, 801 + j, y_pos + 50);  
        draw_gradient = hsb_color (hue, saturation, brightness);        
        saturation -= 0.02;
    }

    y_pos += 25 * 2;

    // Draw color selection boxes
    for (int j = 0; j < 24; j += 2)
    {
        fill_rectangle_on_window (the_window, colors_used[j], 801, y_pos, 24, 24);                
        fill_rectangle_on_window (the_window, colors_used[j+1], 826, y_pos, 24, 24);
        y_pos += 25;
    }            

    // Draw active color block
    fill_rectangle_on_window (the_window, active_color, 801, y_pos, 51, 600 - y_pos);

    // Draw sidebar outline
    draw_line_on_window (the_window, COLOR_BLACK, 800, 0, 800, 600);
    draw_line_on_window (the_window, COLOR_BLACK, 825, 25, 825, 75);
    draw_line_on_window (the_window, COLOR_BLACK, 825, 150, 825, y_pos - 1);
    draw_line_on_window (the_window, COLOR_BLACK, 850, 0, 850, 600);

    for (int i = 0; i < 19; i++)
    {
        if (i != 5)
            draw_line_on_window (the_window, COLOR_BLACK, 800, i*25, 850, i*25);
    }
}

/**
 * Create the set of data for use by the program
 *
 * @returns     The initialised program_data struct
 */
program_data new_program_data()
{
    program_data result;

    result.active_color = COLOR_BLACK;
    result.the_window = open_window ("Image Editor", WINDOW_WIDTH, HEIGHT);
    result.to_draw = create_bitmap ("to_draw", IMAGE_WIDTH, HEIGHT);

    clear_bitmap (result.to_draw, COLOR_WHITE);
    clear_window (result.the_window, COLOR_WHITE);
    draw_sidebar (result.the_window, result.active_color);

    return result;
}

/**
 * Process input from the user
 *
 * @param program    Struct containing program data
 */
void process_input (program_data &program)
{
    if (mouse_x() > 799)
        process_sidebar (program);

    else if (mouse_clicked (RIGHT_BUTTON))
        draw_menu (program, create_paint_menu, process_paint_menu);

    else if ((key_down (LEFT_CTRL_KEY) || key_down (RIGHT_CTRL_KEY)) && (key_typed (Z_KEY)))
        undo_changes (program);

    else if ((key_down (LEFT_CTRL_KEY) || key_down (RIGHT_CTRL_KEY)) && (key_typed (Y_KEY)))
        redo_changes (program);

    else if (mouse_down (LEFT_BUTTON))
        process_mode (program);
}

/**
 * Undo the last action done by user
 *
 * @param program    Struct containing program data
 */
void undo_changes (program_data &program)
{
    if (program.undo.size() > 0)
    {
        bitmap temp = create_bitmap ("temp", 800, 600);
        draw_bitmap_on_bitmap (temp, program.to_draw, 0, 0);
        program.redo.push_back (temp);

        draw_bitmap_on_bitmap (program.to_draw, program.undo.back(), 0, 0); 
        program.undo.erase (program.undo.end() - 1);
    }    
}

/**
 * Redo last undone action by user
 *
 * @param program    Struct containing program data
 */
void redo_changes (program_data &program)
{
    if (program.redo.size() > 0)
    {
        add_to_undo (program);
        draw_bitmap_on_bitmap (program.to_draw, program.redo.back(), 0, 0);
        program.redo.erase (program.redo.end() - 1);
    }    
}

/**
 * Add current drawn image to the undo vector
 *
 * @param program    Struct containing program data
 */
void add_to_undo (program_data &program)
{
    bitmap temp = create_bitmap ("temp", 800, 600);
    draw_bitmap_on_bitmap (temp, program.to_draw, 0, 0);
    program.undo.push_back (temp);
    trim_undo (program.undo);
}

/**
 * Trim undo vector to keep within length 10
 *
 * @param undo  The vector of images to be checked and trimmed
 */
void trim_undo (vector<bitmap> &undo)
{
    if (undo.size() > 10)
        undo.erase (undo.begin());
}

/**
 * Empty redo vector
 *
 * @param redo  The vector of images to be erased
 */
void wipe_redo (vector<bitmap> &redo)
{
    redo.clear();
}

/**
 * Load image resources
 */
void load_graphics()
{
    load_bitmap ("eraser_icon", "eraser_icon.bmp");
    load_bitmap ("save_icon", "save_icon.bmp");
    load_bitmap ("menu_draw_ell", "menu_draw_ell.xcf");
    load_bitmap ("menu_draw_rec", "menu_draw_rec.xcf");
    load_bitmap ("menu_draw_tri", "menu_draw_tri.xcf");
    load_bitmap ("menu_fill_ell", "menu_fill_ell.xcf");
    load_bitmap ("menu_fill_rec", "menu_fill_rec.xcf");
    load_bitmap ("menu_fill_tri", "menu_fill_tri.xcf");
    load_bitmap ("menu_pen", "menu_pen.xcf");
    load_bitmap ("menu_spray", "menu_spray.xcf");
    load_bitmap ("title_screen", "Title Screen.png");
    load_bitmap ("fill_icon", "fill_icon.bmp");
    load_bitmap ("select_icon", "select_icon.bmp");
}

/**
 * Save currently drawn image to the desktop
 *
 * @param image     The image to be saved
 */
void save_image (bitmap image)
{
    save_bitmap (image, "User_image");
    write_line ("Saved");
}

/**
 * Stores selected color in program data for use by user
 *
 * @param the_window    The window to be drawn to
 * @param active_color  The current active color selected by the user
 */
void get_color (window &the_window, color &active_color)
{
    point_2d mouse_loc;

    if (mouse_clicked (LEFT_BUTTON))
    {
        mouse_loc.x = mouse_x();
        mouse_loc.y = mouse_y();

        active_color = get_pixel (mouse_loc);

        draw_sidebar (the_window, active_color);
        refresh_window (the_window);  
    }
}

/**
 * Process user interaction with sidebar icons
 *
 * @param program    Struct containing program data
 */
void process_sidebar (program_data &program)
{   
    while (mouse_x() > 800)
    {
        process_events();

        if ((mouse_y() > 100 && mouse_y() < 475))
            get_color (program.the_window, program.active_color);

        else if (mouse_y() < 50 && mouse_clicked (LEFT_BUTTON))
        {
            if (mouse_x() < 826)
                program.mode = ERASER;
            else
                save_image(program.to_draw);
        }

        else if (mouse_clicked (LEFT_BUTTON) && mouse_y() < 75)  
        {
            if (mouse_x() < 826)             
                program.mode = SELECT;
            else
                program.mode = FILL;
        }
    }
}