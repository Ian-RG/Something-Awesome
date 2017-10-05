#include "splashkit.h"
#include <vector>

#define WINDOW_WIDTH 851
#define IMAGE_WIDTH 800
#define HEIGHT 600

enum mode_option
{
    NONE,
    ERASER,
    PEN,
    SPRAY,
    DRAW_REC,
    FILL_REC,
    DRAW_ELL,
    FILL_ELL,
    DRAW_TRI,
    FILL_TRI,
    SELECT,
    FILL   
};

struct program_data
{
    window the_window;
    bitmap to_draw;
    vector<bitmap> undo;
    vector<bitmap> redo;
    mode_option mode;
    mode_option select[2];
    color active_color;
};

struct menu_item
{
    bitmap graphic;
    point_2d centre;
    double x;
    double y;
};

struct select_tool_data
{
    bitmap graphic;
    double x;
    double y;
};


void paint_eraser (program_data &program);
void paint_pen (program_data &program);
void paint_spray (program_data &program);
void paint_rec_ell (program_data &program, void (draw_rec_ell_to_win) (window, color, double, double, double, double), 
                    void (draw_rec_ell_to_bitmap) (bitmap, color, double, double, double, double));
void paint_tri (program_data &program, void (draw_tri_to_win) (window, color, double, double, double, double, double, double), 
                void (draw_tri_to_bitmap) (bitmap, color, double, double, double, double, double, double));
void select_tool (program_data &program);
void fill_tool (program_data &program);

void process_mode (program_data &program);
program_data new_program_data();
void process_input (program_data &program);
void undo_changes (program_data &program);
void redo_changes (program_data &program);
void add_to_undo (program_data &program);
void trim_undo (vector<bitmap> &undo);
void wipe_redo (vector<bitmap> &redo);
void load_graphics();
void get_color (color &select_color);
void process_sidebar (program_data &program);
void draw_sidebar (window &the_window, color &active_color);
void draw_title_screen (window &the_window);

void draw_menu(program_data &program, vector<menu_item>(create_menu)(double, double, mode_option), void (process_menu)(program_data&, vector<menu_item>&, int));
void process_sub_menu (program_data &program, vector<menu_item> &menu, int width);
void process_paint_menu (program_data &program, vector<menu_item> &menu, int width);
vector<menu_item> create_paint_menu (double x, double y, mode_option mode);
