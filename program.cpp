#include "graphic_creator.h"

int main()
{
    load_graphics();

    program_data program;
    program = new_program_data();   

    draw_title_screen (program.the_window);
    
    while (not quit_requested())
    {
        process_events(); 

        process_input(program);

        draw_bitmap_on_window (program.the_window, program.to_draw, 0, 0);        
        refresh_window(program.the_window);
    }
    
    return 0;
}