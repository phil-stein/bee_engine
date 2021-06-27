#include "ui.h"

#include "GLAD/glad.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear/nuklear.h"
#include "nuklear/nuklear_glfw_gl3.h"

#include "file_handler.h"
#include "str_util.h"
#include "renderer.h"
#include "window.h"
#include "camera.h"
#include "app.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

// ---- vars ----
static int window_w;
static int window_h;

struct nk_color red = { 255, 0, 0, 255 };

// ---- demo --- 
struct nk_glfw glfw = { 0 };
int width = 0, height = 0;
struct nk_context* ctx;
struct nk_colorf bg;


// ---- overview ----
/* window flags */
static int show_menu = nk_true;
static int titlebar = nk_true;
static int border = nk_true;
static int resize = nk_true;
static int movable = nk_true;
static int no_scrollbar = nk_false;
static int scale_left = nk_false;
static nk_flags window_flags = 0;
static int minimizable = nk_true;
/* popups */
static enum nk_style_header_align header_align = NK_HEADER_RIGHT;
static int show_app_about = nk_false;


// ---- properties ----
int selected_entities_len = 100;
int selected_entities[100];

// ---- console ----
static char box_buffer[512];
static int box_len;

// ---- gravity error ----
static int error_popup_act = nk_false;
static char* error_msg;

static int source_code_window_act = nk_false;
static char* source_code;


struct nk_image image;


void ui_init()
{
	// ---- nuklear ----

	ctx = nk_glfw3_init(&glfw, get_window(), NK_GLFW3_INSTALL_CALLBACKS);
	/* Load Fonts: if none of these are loaded a default font will be used  */
	/* Load Cursor: if you uncomment cursor loading please hide the cursor */
	{struct nk_font_atlas* atlas;
	nk_glfw3_font_stash_begin(&glfw, &atlas);
	
        // struct nk_font* roboto = nk_font_atlas_add_from_file(atlas, "C:\\Workspace\\C\\BeeEngine\\assets\\fonts\\Roboto-Regular.ttf", 16/*14*/, 0);
        // struct nk_font* roboto_bold = nk_font_atlas_add_from_file(atlas, "C:\\Workspace\\C\\BeeEngine\\assets\\fonts\\Roboto-Bold.ttf", 16/*14*/, 0);
        struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "C:\\Workspace\\C\\BeeEngine\\assets\\fonts\\DroidSans.ttf", 18/*16*//*14*/, 0);
	    // struct nk_font *future = nk_font_atlas_add_from_file(atlas, "C:\\Workspace\\C\\BeeEngine\\assets\\fonts\\kenvector_future_thin.ttf", 13, 0);
	    // struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "C:\\Workspace\\C\\BeeEngine\\assets\\fonts\\ProggyClean.ttf", 12, 0);
	    // struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "C:\\Workspace\\C\\BeeEngine\\assets\\fonts\\ProggyTiny.ttf", 10, 0);
        // struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "C:\\Workspace\\C\\BeeEngine\\assets\\fonts\\Cousine-Regular.ttf", 13, 0);
	
    nk_glfw3_font_stash_end(&glfw);
	/*nk_style_load_all_cursors(ctx, atlas->cursors);*/
	
    nk_style_set_font(ctx, &droid->handle);
    }

	// -----------------


    bg.r = 0.1f; bg.g = 0.1f; bg.b = 0.1f; bg.a = 1.0f;


    // set default theme
    set_style(ctx, THEME_RED);
}

void ui_update()
{
    // get window size
    get_window_size(&window_w, &window_h);

	// draw ui
	nk_glfw3_new_frame(&glfw);


    // demo_window();
    // overview_window();

    int entities_len = 0;
    get_entity_len(&entities_len);
    // printf("entities_len: %d\n", entities_len);

    // resize selected
    assert(entities_len <= selected_entities_len);

    properties_window(entities_len);

    //console_window();

    asset_browser_window();

    if (error_popup_act == nk_true)
    {
        error_popup_window();
    }
    if (source_code_window_act == nk_true)
    {
        source_code_window();
    }

    nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}

void ui_cleanup()
{
    // free(selected_entities);
}

/*
void overview_window()
{
    // window flags
    window_flags = 0;
    ctx->style.window.header.align = header_align;
    if (border) window_flags |= NK_WINDOW_BORDER;
    if (resize) window_flags |= NK_WINDOW_SCALABLE;
    if (movable) window_flags |= NK_WINDOW_MOVABLE;
    if (no_scrollbar) window_flags |= NK_WINDOW_NO_SCROLLBAR;
    if (scale_left) window_flags |= NK_WINDOW_SCALE_LEFT;
    if (minimizable) window_flags |= NK_WINDOW_MINIMIZABLE;

    if (nk_begin(ctx, "Overview", nk_rect(270, 10, 400, 600), window_flags))
    {
        if (show_menu)
        {
            // menubar
            enum menu_states { MENU_DEFAULT, MENU_WINDOWS };
            static nk_size mprog = 60;
            static int mslider = 10;
            static int mcheck = nk_true;
            nk_menubar_begin(ctx);

            // menu #1
            nk_layout_row_begin(ctx, NK_STATIC, 25, 5);
            nk_layout_row_push(ctx, 45);
            if (nk_menu_begin_label(ctx, "MENU", NK_TEXT_LEFT, nk_vec2(120, 200)))
            {
                static size_t prog = 40;
                static int slider = 10;
                static int check = nk_true;
                nk_layout_row_dynamic(ctx, 25, 1);
                if (nk_menu_item_label(ctx, "Hide", NK_TEXT_LEFT))
                    show_menu = nk_false;
                if (nk_menu_item_label(ctx, "About", NK_TEXT_LEFT))
                    show_app_about = nk_true;
                nk_progress(ctx, &prog, 100, NK_MODIFIABLE);
                nk_slider_int(ctx, 0, &slider, 16, 1);
                nk_checkbox_label(ctx, "check", &check);
                nk_menu_end(ctx);
            }
            //menu #2
            nk_layout_row_push(ctx, 60);
            if (nk_menu_begin_label(ctx, "ADVANCED", NK_TEXT_LEFT, nk_vec2(200, 600)))
            {
                enum menu_state { MENU_NONE, MENU_FILE, MENU_EDIT, MENU_VIEW, MENU_CHART };
                static enum menu_state menu_state = MENU_NONE;
                enum nk_collapse_states state;

                state = (menu_state == MENU_FILE) ? NK_MAXIMIZED : NK_MINIMIZED;
                if (nk_tree_state_push(ctx, NK_TREE_TAB, "FILE", &state)) {
                    menu_state = MENU_FILE;
                    nk_menu_item_label(ctx, "New", NK_TEXT_LEFT);
                    nk_menu_item_label(ctx, "Open", NK_TEXT_LEFT);
                    nk_menu_item_label(ctx, "Save", NK_TEXT_LEFT);
                    nk_menu_item_label(ctx, "Close", NK_TEXT_LEFT);
                    nk_menu_item_label(ctx, "Exit", NK_TEXT_LEFT);
                    nk_tree_pop(ctx);
                }
                else menu_state = (menu_state == MENU_FILE) ? MENU_NONE : menu_state;

                state = (menu_state == MENU_EDIT) ? NK_MAXIMIZED : NK_MINIMIZED;
                if (nk_tree_state_push(ctx, NK_TREE_TAB, "EDIT", &state)) {
                    menu_state = MENU_EDIT;
                    nk_menu_item_label(ctx, "Copy", NK_TEXT_LEFT);
                    nk_menu_item_label(ctx, "Delete", NK_TEXT_LEFT);
                    nk_menu_item_label(ctx, "Cut", NK_TEXT_LEFT);
                    nk_menu_item_label(ctx, "Paste", NK_TEXT_LEFT);
                    nk_tree_pop(ctx);
                }
                else menu_state = (menu_state == MENU_EDIT) ? MENU_NONE : menu_state;

                state = (menu_state == MENU_VIEW) ? NK_MAXIMIZED : NK_MINIMIZED;
                if (nk_tree_state_push(ctx, NK_TREE_TAB, "VIEW", &state)) {
                    menu_state = MENU_VIEW;
                    nk_menu_item_label(ctx, "About", NK_TEXT_LEFT);
                    nk_menu_item_label(ctx, "Options", NK_TEXT_LEFT);
                    nk_menu_item_label(ctx, "Customize", NK_TEXT_LEFT);
                    nk_tree_pop(ctx);
                }
                else menu_state = (menu_state == MENU_VIEW) ? MENU_NONE : menu_state;

                state = (menu_state == MENU_CHART) ? NK_MAXIMIZED : NK_MINIMIZED;
                if (nk_tree_state_push(ctx, NK_TREE_TAB, "CHART", &state)) {
                    size_t i = 0;
                    const float values[] = { 26.0f,13.0f,30.0f,15.0f,25.0f,10.0f,20.0f,40.0f,12.0f,8.0f,22.0f,28.0f };
                    menu_state = MENU_CHART;
                    nk_layout_row_dynamic(ctx, 150, 1);
                    nk_chart_begin(ctx, NK_CHART_COLUMN, NK_LEN(values), 0, 50);
                    for (i = 0; i < NK_LEN(values); ++i)
                        nk_chart_push(ctx, values[i]);
                    nk_chart_end(ctx);
                    nk_tree_pop(ctx);
                }
                else menu_state = (menu_state == MENU_CHART) ? MENU_NONE : menu_state;
                nk_menu_end(ctx);
            }
            //menu widgets
            nk_layout_row_push(ctx, 70);
            nk_progress(ctx, &mprog, 100, NK_MODIFIABLE);
            nk_slider_int(ctx, 0, &mslider, 16, 1);
            nk_checkbox_label(ctx, "check", &mcheck);
            nk_menubar_end(ctx);
        }

        if (show_app_about)
        {
            // about popup
            static struct nk_rect s = { 20, 100, 300, 190 };
            if (nk_popup_begin(ctx, NK_POPUP_STATIC, "About", NK_WINDOW_CLOSABLE, s))
            {
                nk_layout_row_dynamic(ctx, 20, 1);
                nk_label(ctx, "Nuklear", NK_TEXT_LEFT);
                nk_label(ctx, "By Micha Mettke", NK_TEXT_LEFT);
                nk_label(ctx, "nuklear is licensed under the public domain License.", NK_TEXT_LEFT);
                nk_popup_end(ctx);
            }
            else show_app_about = nk_false;
        }

        // window flags
        if (nk_tree_push(ctx, NK_TREE_TAB, "Window", NK_MINIMIZED)) {
            nk_layout_row_dynamic(ctx, 30, 2);
            nk_checkbox_label(ctx, "Titlebar", &titlebar);
            nk_checkbox_label(ctx, "Menu", &show_menu);
            nk_checkbox_label(ctx, "Border", &border);
            nk_checkbox_label(ctx, "Resizable", &resize);
            nk_checkbox_label(ctx, "Movable", &movable);
            nk_checkbox_label(ctx, "No Scrollbar", &no_scrollbar);
            nk_checkbox_label(ctx, "Minimizable", &minimizable);
            nk_checkbox_label(ctx, "Scale Left", &scale_left);
            nk_tree_pop(ctx);
        }

        if (nk_tree_push(ctx, NK_TREE_TAB, "Widgets", NK_MINIMIZED))
        {
            enum options { A, B, C };
            static int checkbox;
            static int option;
            if (nk_tree_push(ctx, NK_TREE_NODE, "Text", NK_MINIMIZED))
            {
                // Text Widgets
                nk_layout_row_dynamic(ctx, 20, 1);
                nk_label(ctx, "Label aligned left", NK_TEXT_LEFT);
                nk_label(ctx, "Label aligned centered", NK_TEXT_CENTERED);
                nk_label(ctx, "Label aligned right", NK_TEXT_RIGHT);
                nk_label_colored(ctx, "Blue text", NK_TEXT_LEFT, nk_rgb(0, 0, 255));
                nk_label_colored(ctx, "Yellow text", NK_TEXT_LEFT, nk_rgb(255, 255, 0));
                nk_text(ctx, "Text without /0", 15, NK_TEXT_RIGHT);

                nk_layout_row_static(ctx, 100, 200, 1);
                nk_label_wrap(ctx, "This is a very long line to hopefully get this text to be wrapped into multiple lines to show line wrapping");
                nk_layout_row_dynamic(ctx, 100, 1);
                nk_label_wrap(ctx, "This is another long text to show dynamic window changes on multiline text");
                nk_tree_pop(ctx);
            }

            if (nk_tree_push(ctx, NK_TREE_NODE, "Button", NK_MINIMIZED))
            {
                // Buttons Widgets
                nk_layout_row_static(ctx, 30, 100, 3);
                if (nk_button_label(ctx, "Button"))
                    fprintf(stdout, "Button pressed!\n");
                nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
                if (nk_button_label(ctx, "Repeater"))
                    fprintf(stdout, "Repeater is being pressed!\n");
                nk_button_set_behavior(ctx, NK_BUTTON_DEFAULT);
                nk_button_color(ctx, nk_rgb(0, 0, 255));

                nk_layout_row_static(ctx, 25, 25, 8);
                nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_SOLID);
                nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_OUTLINE);
                nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID);
                nk_button_symbol(ctx, NK_SYMBOL_RECT_OUTLINE);
                nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP);
                nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN);
                nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT);
                nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT);

                nk_layout_row_static(ctx, 30, 100, 2);
                nk_button_symbol_label(ctx, NK_SYMBOL_TRIANGLE_LEFT, "prev", NK_TEXT_RIGHT);
                nk_button_symbol_label(ctx, NK_SYMBOL_TRIANGLE_RIGHT, "next", NK_TEXT_LEFT);
                nk_tree_pop(ctx);
            }

            if (nk_tree_push(ctx, NK_TREE_NODE, "Basic", NK_MINIMIZED))
            {
                // Basic widgets
                static int int_slider = 5;
                static float float_slider = 2.5f;
                static size_t prog_value = 40;
                static float property_float = 2;
                static int property_int = 10;
                static int property_neg = 10;

                static float range_float_min = 0;
                static float range_float_max = 100;
                static float range_float_value = 50;
                static int range_int_min = 0;
                static int range_int_value = 2048;
                static int range_int_max = 4096;
                static const float ratio[] = { 120, 150 };

                nk_layout_row_static(ctx, 30, 100, 1);
                nk_checkbox_label(ctx, "Checkbox", &checkbox);

                nk_layout_row_static(ctx, 30, 80, 3);
                option = nk_option_label(ctx, "optionA", option == A) ? A : option;
                option = nk_option_label(ctx, "optionB", option == B) ? B : option;
                option = nk_option_label(ctx, "optionC", option == C) ? C : option;

                nk_layout_row(ctx, NK_STATIC, 30, 2, ratio);
                nk_labelf(ctx, NK_TEXT_LEFT, "Slider int");
                nk_slider_int(ctx, 0, &int_slider, 10, 1);

                nk_label(ctx, "Slider float", NK_TEXT_LEFT);
                nk_slider_float(ctx, 0, &float_slider, 5.0, 0.5f);
                nk_labelf(ctx, NK_TEXT_LEFT, "Progressbar: %zu", prog_value);
                nk_progress(ctx, &prog_value, 100, NK_MODIFIABLE);

                nk_layout_row(ctx, NK_STATIC, 25, 2, ratio);
                nk_label(ctx, "Property float:", NK_TEXT_LEFT);
                nk_property_float(ctx, "Float:", 0, &property_float, 64.0f, 0.1f, 0.2f);
                nk_label(ctx, "Property int:", NK_TEXT_LEFT);
                nk_property_int(ctx, "Int:", 0, &property_int, 100, 1, 1);
                nk_label(ctx, "Property neg:", NK_TEXT_LEFT);
                nk_property_int(ctx, "Neg:", -10, &property_neg, 10, 1, 1);

                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label(ctx, "Range:", NK_TEXT_LEFT);
                nk_layout_row_dynamic(ctx, 25, 3);
                nk_property_float(ctx, "#min:", 0, &range_float_min, range_float_max, 1.0f, 0.2f);
                nk_property_float(ctx, "#float:", range_float_min, &range_float_value, range_float_max, 1.0f, 0.2f);
                nk_property_float(ctx, "#max:", range_float_min, &range_float_max, 100, 1.0f, 0.2f);

                nk_property_int(ctx, "#min:", INT_MIN, &range_int_min, range_int_max, 1, 10);
                nk_property_int(ctx, "#neg:", range_int_min, &range_int_value, range_int_max, 1, 10);
                nk_property_int(ctx, "#max:", range_int_min, &range_int_max, INT_MAX, 1, 10);

                nk_tree_pop(ctx);
            }

            if (nk_tree_push(ctx, NK_TREE_NODE, "Inactive", NK_MINIMIZED))
            {
                static int inactive = 1;
                nk_layout_row_dynamic(ctx, 30, 1);
                nk_checkbox_label(ctx, "Inactive", &inactive);

                nk_layout_row_static(ctx, 30, 80, 1);
                if (inactive) {
                    struct nk_style_button button;
                    button = ctx->style.button;
                    ctx->style.button.normal = nk_style_item_color(nk_rgb(40, 40, 40));
                    ctx->style.button.hover = nk_style_item_color(nk_rgb(40, 40, 40));
                    ctx->style.button.active = nk_style_item_color(nk_rgb(40, 40, 40));
                    ctx->style.button.border_color = nk_rgb(60, 60, 60);
                    ctx->style.button.text_background = nk_rgb(60, 60, 60);
                    ctx->style.button.text_normal = nk_rgb(60, 60, 60);
                    ctx->style.button.text_hover = nk_rgb(60, 60, 60);
                    ctx->style.button.text_active = nk_rgb(60, 60, 60);
                    nk_button_label(ctx, "button");
                    ctx->style.button = button;
                }
                else if (nk_button_label(ctx, "button"))
                    fprintf(stdout, "button pressed\n");
                nk_tree_pop(ctx);
            }


            if (nk_tree_push(ctx, NK_TREE_NODE, "Selectable", NK_MINIMIZED))
            {
                if (nk_tree_push(ctx, NK_TREE_NODE, "List", NK_MINIMIZED))
                {
                    static int selected[4] = { nk_false, nk_false, nk_true, nk_false };
                    nk_layout_row_static(ctx, 18, 100, 1);
                    nk_selectable_label(ctx, "Selectable", NK_TEXT_LEFT, &selected[0]);
                    nk_selectable_label(ctx, "Selectable", NK_TEXT_LEFT, &selected[1]);
                    nk_label(ctx, "Not Selectable", NK_TEXT_LEFT);
                    nk_selectable_label(ctx, "Selectable", NK_TEXT_LEFT, &selected[2]);
                    nk_selectable_label(ctx, "Selectable", NK_TEXT_LEFT, &selected[3]);
                    nk_tree_pop(ctx);
                }
                if (nk_tree_push(ctx, NK_TREE_NODE, "Grid", NK_MINIMIZED))
                {
                    int i;
                    static int selected[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
                    nk_layout_row_static(ctx, 50, 50, 4);
                    for (i = 0; i < 16; ++i) {
                        if (nk_selectable_label(ctx, "Z", NK_TEXT_CENTERED, &selected[i])) {
                            int x = (i % 4), y = i / 4;
                            if (x > 0) selected[i - 1] ^= 1;
                            if (x < 3) selected[i + 1] ^= 1;
                            if (y > 0) selected[i - 4] ^= 1;
                            if (y < 3) selected[i + 4] ^= 1;
                        }
                    }
                    nk_tree_pop(ctx);
                }
                nk_tree_pop(ctx);
            }

            if (nk_tree_push(ctx, NK_TREE_NODE, "Combo", NK_MINIMIZED))
            {
                // Combobox Widgets
                // In this library comboboxes are not limited to being a popup
                // list of selectable text. Instead it is a abstract concept of
                // having something that is *selected* or displayed, a popup window
                // which opens if something needs to be modified and the content
                // of the popup which causes the *selected* or displayed value to
                // change or if wanted close the combobox.
                // 
                // While strange at first handling comboboxes in a abstract way
                // solves the problem of overloaded window content. For example
                // changing a color value requires 4 value modifier (slider, property,...)
                // for RGBA then you need a label and ways to display the current color.
                // If you want to go fancy you even add rgb and hsv ratio boxes.
                // While fine for one color if you have a lot of them it because
                // tedious to look at and quite wasteful in space. You could add
                // a popup which modifies the color but this does not solve the
                // fact that it still requires a lot of cluttered space to do.
                // 
                // In these kind of instance abstract comboboxes are quite handy. All
                // value modifiers are hidden inside the combobox popup and only
                // the color is shown if not open. This combines the clarity of the
                // popup with the ease of use of just using the space for modifiers.
                // 
                // Other instances are for example time and especially date picker,
                // which only show the currently activated time/data and hide the
                // selection logic inside the combobox popup.
                
                static float chart_selection = 8.0f;
                static int current_weapon = 0;
                static int check_values[5];
                static float position[3];
                static struct nk_color combo_color = { 130, 50, 50, 255 };
                static struct nk_colorf combo_color2 = { 0.509f, 0.705f, 0.2f, 1.0f };
                static size_t prog_a = 20, prog_b = 40, prog_c = 10, prog_d = 90;
                static const char* weapons[] = { "Fist","Pistol","Shotgun","Plasma","BFG" };

                char buffer[64];
                size_t sum = 0;

                // default combobox
                nk_layout_row_static(ctx, 25, 200, 1);
                current_weapon = nk_combo(ctx, weapons, NK_LEN(weapons), current_weapon, 25, nk_vec2(200, 200));

                // slider color combobox
                if (nk_combo_begin_color(ctx, combo_color, nk_vec2(200, 200))) {
                    float ratios[] = { 0.15f, 0.85f };
                    nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios);
                    nk_label(ctx, "R:", NK_TEXT_LEFT);
                    combo_color.r = (nk_byte)nk_slide_int(ctx, 0, combo_color.r, 255, 5);
                    nk_label(ctx, "G:", NK_TEXT_LEFT);
                    combo_color.g = (nk_byte)nk_slide_int(ctx, 0, combo_color.g, 255, 5);
                    nk_label(ctx, "B:", NK_TEXT_LEFT);
                    combo_color.b = (nk_byte)nk_slide_int(ctx, 0, combo_color.b, 255, 5);
                    nk_label(ctx, "A:", NK_TEXT_LEFT);
                    combo_color.a = (nk_byte)nk_slide_int(ctx, 0, combo_color.a, 255, 5);
                    nk_combo_end(ctx);
                }
                // complex color combobox
                if (nk_combo_begin_color(ctx, nk_rgb_cf(combo_color2), nk_vec2(200, 400))) {
                    enum color_mode { COL_RGB, COL_HSV };
                    static int col_mode = COL_RGB;
#ifndef DEMO_DO_NOT_USE_COLOR_PICKER
                    nk_layout_row_dynamic(ctx, 120, 1);
                    combo_color2 = nk_color_picker(ctx, combo_color2, NK_RGBA);
#endif

                    nk_layout_row_dynamic(ctx, 25, 2);
                    col_mode = nk_option_label(ctx, "RGB", col_mode == COL_RGB) ? COL_RGB : col_mode;
                    col_mode = nk_option_label(ctx, "HSV", col_mode == COL_HSV) ? COL_HSV : col_mode;

                    nk_layout_row_dynamic(ctx, 25, 1);
                    if (col_mode == COL_RGB) {
                        combo_color2.r = nk_propertyf(ctx, "#R:", 0, combo_color2.r, 1.0f, 0.01f, 0.005f);
                        combo_color2.g = nk_propertyf(ctx, "#G:", 0, combo_color2.g, 1.0f, 0.01f, 0.005f);
                        combo_color2.b = nk_propertyf(ctx, "#B:", 0, combo_color2.b, 1.0f, 0.01f, 0.005f);
                        combo_color2.a = nk_propertyf(ctx, "#A:", 0, combo_color2.a, 1.0f, 0.01f, 0.005f);
                    }
                    else {
                        float hsva[4];
                        nk_colorf_hsva_fv(hsva, combo_color2);
                        hsva[0] = nk_propertyf(ctx, "#H:", 0, hsva[0], 1.0f, 0.01f, 0.05f);
                        hsva[1] = nk_propertyf(ctx, "#S:", 0, hsva[1], 1.0f, 0.01f, 0.05f);
                        hsva[2] = nk_propertyf(ctx, "#V:", 0, hsva[2], 1.0f, 0.01f, 0.05f);
                        hsva[3] = nk_propertyf(ctx, "#A:", 0, hsva[3], 1.0f, 0.01f, 0.05f);
                        combo_color2 = nk_hsva_colorfv(hsva);
                    }
                    nk_combo_end(ctx);
                }
                // progressbar combobox
                sum = prog_a + prog_b + prog_c + prog_d;
                sprintf(buffer, "%zu", sum);
                if (nk_combo_begin_label(ctx, buffer, nk_vec2(200, 200))) {
                    nk_layout_row_dynamic(ctx, 30, 1);
                    nk_progress(ctx, &prog_a, 100, NK_MODIFIABLE);
                    nk_progress(ctx, &prog_b, 100, NK_MODIFIABLE);
                    nk_progress(ctx, &prog_c, 100, NK_MODIFIABLE);
                    nk_progress(ctx, &prog_d, 100, NK_MODIFIABLE);
                    nk_combo_end(ctx);
                }

                // checkbox combobox
                sum = (size_t)(check_values[0] + check_values[1] + check_values[2] + check_values[3] + check_values[4]);
                sprintf(buffer, "%zu", sum);
                if (nk_combo_begin_label(ctx, buffer, nk_vec2(200, 200))) {
                    nk_layout_row_dynamic(ctx, 30, 1);
                    nk_checkbox_label(ctx, weapons[0], &check_values[0]);
                    nk_checkbox_label(ctx, weapons[1], &check_values[1]);
                    nk_checkbox_label(ctx, weapons[2], &check_values[2]);
                    nk_checkbox_label(ctx, weapons[3], &check_values[3]);
                    nk_combo_end(ctx);
                }

                // complex text combobox
                sprintf(buffer, "%.2f, %.2f, %.2f", position[0], position[1], position[2]);
                if (nk_combo_begin_label(ctx, buffer, nk_vec2(200, 200))) {
                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_property_float(ctx, "#X:", -1024.0f, &position[0], 1024.0f, 1, 0.5f);
                    nk_property_float(ctx, "#Y:", -1024.0f, &position[1], 1024.0f, 1, 0.5f);
                    nk_property_float(ctx, "#Z:", -1024.0f, &position[2], 1024.0f, 1, 0.5f);
                    nk_combo_end(ctx);
                }

                //chart combobox
                sprintf(buffer, "%.1f", chart_selection);
                if (nk_combo_begin_label(ctx, buffer, nk_vec2(200, 250))) {
                    size_t i = 0;
                    static const float values[] = { 26.0f,13.0f,30.0f,15.0f,25.0f,10.0f,20.0f,40.0f, 12.0f, 8.0f, 22.0f, 28.0f, 5.0f };
                    nk_layout_row_dynamic(ctx, 150, 1);
                    nk_chart_begin(ctx, NK_CHART_COLUMN, NK_LEN(values), 0, 50);
                    for (i = 0; i < NK_LEN(values); ++i) {
                        nk_flags res = nk_chart_push(ctx, values[i]);
                        if (res & NK_CHART_CLICKED) {
                            chart_selection = values[i];
                            nk_combo_close(ctx);
                        }
                    }
                    nk_chart_end(ctx);
                    nk_combo_end(ctx);
                }
                nk_tree_pop(ctx);
            }

            if (nk_tree_push(ctx, NK_TREE_NODE, "Input", NK_MINIMIZED))
            {
                static const float ratio[] = { 120, 150 };
                static char field_buffer[64];
                static char text[9][64];
                static int text_len[9];
                static char box_buffer[512];
                static int field_len;
                static int box_len;
                nk_flags active;

                nk_layout_row(ctx, NK_STATIC, 25, 2, ratio);
                nk_label(ctx, "Default:", NK_TEXT_LEFT);

                nk_edit_string(ctx, NK_EDIT_SIMPLE, text[0], &text_len[0], 64, nk_filter_default);
                nk_label(ctx, "Int:", NK_TEXT_LEFT);
                nk_edit_string(ctx, NK_EDIT_SIMPLE, text[1], &text_len[1], 64, nk_filter_decimal);
                nk_label(ctx, "Float:", NK_TEXT_LEFT);
                nk_edit_string(ctx, NK_EDIT_SIMPLE, text[2], &text_len[2], 64, nk_filter_float);
                nk_label(ctx, "Hex:", NK_TEXT_LEFT);
                nk_edit_string(ctx, NK_EDIT_SIMPLE, text[4], &text_len[4], 64, nk_filter_hex);
                nk_label(ctx, "Octal:", NK_TEXT_LEFT);
                nk_edit_string(ctx, NK_EDIT_SIMPLE, text[5], &text_len[5], 64, nk_filter_oct);
                nk_label(ctx, "Binary:", NK_TEXT_LEFT);
                nk_edit_string(ctx, NK_EDIT_SIMPLE, text[6], &text_len[6], 64, nk_filter_binary);

                nk_label(ctx, "Password:", NK_TEXT_LEFT);
                {
                    int i = 0;
                    int old_len = text_len[8];
                    char buffer[64];
                    for (i = 0; i < text_len[8]; ++i) buffer[i] = '*';
                    nk_edit_string(ctx, NK_EDIT_FIELD, buffer, &text_len[8], 64, nk_filter_default);
                    if (old_len < text_len[8])
                        memcpy(&text[8][old_len], &buffer[old_len], (nk_size)(text_len[8] - old_len));
                }

                nk_label(ctx, "Field:", NK_TEXT_LEFT);
                nk_edit_string(ctx, NK_EDIT_FIELD, field_buffer, &field_len, 64, nk_filter_default);

                nk_label(ctx, "Box:", NK_TEXT_LEFT);
                nk_layout_row_static(ctx, 180, 278, 1);
                nk_edit_string(ctx, NK_EDIT_BOX, box_buffer, &box_len, 512, nk_filter_default);

                nk_layout_row(ctx, NK_STATIC, 25, 2, ratio);
                active = nk_edit_string(ctx, NK_EDIT_FIELD | NK_EDIT_SIG_ENTER, text[7], &text_len[7], 64, nk_filter_ascii);
                if (nk_button_label(ctx, "Submit") ||
                    (active & NK_EDIT_COMMITED))
                {
                    text[7][text_len[7]] = '\n';
                    text_len[7]++;
                    memcpy(&box_buffer[box_len], &text[7], (nk_size)text_len[7]);
                    box_len += text_len[7];
                    text_len[7] = 0;
                }
                nk_tree_pop(ctx);
            }

            nk_tree_pop(ctx);
            // end of widgets
        }

        if (nk_tree_push(ctx, NK_TREE_TAB, "Chart", NK_MINIMIZED))
        {
            // Chart Widgets
            // This library has two different rather simple charts. The line and the
            // column chart. Both provide a simple way of visualizing values and
            // have a retained mode and immediate mode API version. For the retain
            // mode version `nk_plot` and `nk_plot_function` you either provide
            // an array or a callback to call to handle drawing the graph.
            // For the immediate mode version you start by calling `nk_chart_begin`
            // and need to provide min and max values for scaling on the Y-axis.
            // and then call `nk_chart_push` to push values into the chart.
            // Finally `nk_chart_end` needs to be called to end the process. 
            
            float id = 0;
            static int col_index = -1;
            static int line_index = -1;
            float step = (2 * 3.141592654f) / 32;

            int i;
            int index = -1;
            struct nk_rect bounds;

            // line chart
            id = 0;
            index = -1;
            nk_layout_row_dynamic(ctx, 100, 1);
            bounds = nk_widget_bounds(ctx);
            if (nk_chart_begin(ctx, NK_CHART_LINES, 32, -1.0f, 1.0f)) {
                for (i = 0; i < 32; ++i) {
                    nk_flags res = nk_chart_push(ctx, (float)cos(id));
                    if (res & NK_CHART_HOVERING)
                        index = (int)i;
                    if (res & NK_CHART_CLICKED)
                        line_index = (int)i;
                    id += step;
                }
                nk_chart_end(ctx);
            }

            if (index != -1)
                nk_tooltipf(ctx, "Value: %.2f", (float)cos((float)index * step));
            if (line_index != -1) {
                nk_layout_row_dynamic(ctx, 20, 1);
                nk_labelf(ctx, NK_TEXT_LEFT, "Selected value: %.2f", (float)cos((float)index * step));
            }

            // column chart
            nk_layout_row_dynamic(ctx, 100, 1);
            bounds = nk_widget_bounds(ctx);
            if (nk_chart_begin(ctx, NK_CHART_COLUMN, 32, 0.0f, 1.0f)) {
                for (i = 0; i < 32; ++i) {
                    nk_flags res = nk_chart_push(ctx, (float)fabs(sin(id)));
                    if (res & NK_CHART_HOVERING)
                        index = (int)i;
                    if (res & NK_CHART_CLICKED)
                        col_index = (int)i;
                    id += step;
                }
                nk_chart_end(ctx);
            }
            if (index != -1)
                nk_tooltipf(ctx, "Value: %.2f", (float)fabs(sin(step * (float)index)));
            if (col_index != -1) {
                nk_layout_row_dynamic(ctx, 20, 1);
                nk_labelf(ctx, NK_TEXT_LEFT, "Selected value: %.2f", (float)fabs(sin(step * (float)col_index)));
            }

            // mixed chart
            nk_layout_row_dynamic(ctx, 100, 1);
            bounds = nk_widget_bounds(ctx);
            if (nk_chart_begin(ctx, NK_CHART_COLUMN, 32, 0.0f, 1.0f)) {
                nk_chart_add_slot(ctx, NK_CHART_LINES, 32, -1.0f, 1.0f);
                nk_chart_add_slot(ctx, NK_CHART_LINES, 32, -1.0f, 1.0f);
                for (id = 0, i = 0; i < 32; ++i) {
                    nk_chart_push_slot(ctx, (float)fabs(sin(id)), 0);
                    nk_chart_push_slot(ctx, (float)cos(id), 1);
                    nk_chart_push_slot(ctx, (float)sin(id), 2);
                    id += step;
                }
            }
            nk_chart_end(ctx);

            // mixed colored chart
            nk_layout_row_dynamic(ctx, 100, 1);
            bounds = nk_widget_bounds(ctx);
            if (nk_chart_begin_colored(ctx, NK_CHART_LINES, nk_rgb(255, 0, 0), nk_rgb(150, 0, 0), 32, 0.0f, 1.0f)) {
                nk_chart_add_slot_colored(ctx, NK_CHART_LINES, nk_rgb(0, 0, 255), nk_rgb(0, 0, 150), 32, -1.0f, 1.0f);
                nk_chart_add_slot_colored(ctx, NK_CHART_LINES, nk_rgb(0, 255, 0), nk_rgb(0, 150, 0), 32, -1.0f, 1.0f);
                for (id = 0, i = 0; i < 32; ++i) {
                    nk_chart_push_slot(ctx, (float)fabs(sin(id)), 0);
                    nk_chart_push_slot(ctx, (float)cos(id), 1);
                    nk_chart_push_slot(ctx, (float)sin(id), 2);
                    id += step;
                }
            }
            nk_chart_end(ctx);
            nk_tree_pop(ctx);
        }

        if (nk_tree_push(ctx, NK_TREE_TAB, "Popup", NK_MINIMIZED))
        {
            static struct nk_color color = { 255,0,0, 255 };
            static int select[4];
            static int popup_active;
            const struct nk_input* in = &ctx->input;
            struct nk_rect bounds;

            // menu contextual
            nk_layout_row_static(ctx, 30, 160, 1);
            bounds = nk_widget_bounds(ctx);
            nk_label(ctx, "Right click me for menu", NK_TEXT_LEFT);

            if (nk_contextual_begin(ctx, 0, nk_vec2(100, 300), bounds)) {
                static size_t prog = 40;
                static int slider = 10;

                nk_layout_row_dynamic(ctx, 25, 1);
                nk_checkbox_label(ctx, "Menu", &show_menu);
                nk_progress(ctx, &prog, 100, NK_MODIFIABLE);
                nk_slider_int(ctx, 0, &slider, 16, 1);
                if (nk_contextual_item_label(ctx, "About", NK_TEXT_CENTERED))
                    show_app_about = nk_true;
                nk_selectable_label(ctx, select[0] ? "Unselect" : "Select", NK_TEXT_LEFT, &select[0]);
                nk_selectable_label(ctx, select[1] ? "Unselect" : "Select", NK_TEXT_LEFT, &select[1]);
                nk_selectable_label(ctx, select[2] ? "Unselect" : "Select", NK_TEXT_LEFT, &select[2]);
                nk_selectable_label(ctx, select[3] ? "Unselect" : "Select", NK_TEXT_LEFT, &select[3]);
                nk_contextual_end(ctx);
            }

            // color contextual
            nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
            nk_layout_row_push(ctx, 120);
            nk_label(ctx, "Right Click here:", NK_TEXT_LEFT);
            nk_layout_row_push(ctx, 50);
            bounds = nk_widget_bounds(ctx);
            nk_button_color(ctx, color);
            nk_layout_row_end(ctx);

            if (nk_contextual_begin(ctx, 0, nk_vec2(350, 60), bounds)) {
                nk_layout_row_dynamic(ctx, 30, 4);
                color.r = (nk_byte)nk_propertyi(ctx, "#r", 0, color.r, 255, 1, 1);
                color.g = (nk_byte)nk_propertyi(ctx, "#g", 0, color.g, 255, 1, 1);
                color.b = (nk_byte)nk_propertyi(ctx, "#b", 0, color.b, 255, 1, 1);
                color.a = (nk_byte)nk_propertyi(ctx, "#a", 0, color.a, 255, 1, 1);
                nk_contextual_end(ctx);
            }

            // popup
            nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
            nk_layout_row_push(ctx, 120);
            nk_label(ctx, "Popup:", NK_TEXT_LEFT);
            nk_layout_row_push(ctx, 50);
            if (nk_button_label(ctx, "Popup"))
                popup_active = 1;
            nk_layout_row_end(ctx);

            if (popup_active)
            {
                static struct nk_rect s = { 20, 100, 220, 90 };
                if (nk_popup_begin(ctx, NK_POPUP_STATIC, "Error", 0, s))
                {
                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_label(ctx, "A terrible error as occured", NK_TEXT_LEFT);
                    nk_layout_row_dynamic(ctx, 25, 2);
                    if (nk_button_label(ctx, "OK")) {
                        popup_active = 0;
                        nk_popup_close(ctx);
                    }
                    if (nk_button_label(ctx, "Cancel")) {
                        popup_active = 0;
                        nk_popup_close(ctx);
                    }
                    nk_popup_end(ctx);
                }
                else popup_active = nk_false;
            }

            // tooltip
            nk_layout_row_static(ctx, 30, 150, 1);
            bounds = nk_widget_bounds(ctx);
            nk_label(ctx, "Hover me for tooltip", NK_TEXT_LEFT);
            if (nk_input_is_mouse_hovering_rect(in, bounds))
                nk_tooltip(ctx, "This is a tooltip");

            nk_tree_pop(ctx);
        }

        if (nk_tree_push(ctx, NK_TREE_TAB, "Layout", NK_MINIMIZED))
        {
            if (nk_tree_push(ctx, NK_TREE_NODE, "Widget", NK_MINIMIZED))
            {
                float ratio_two[] = { 0.2f, 0.6f, 0.2f };
                float width_two[] = { 100, 200, 50 };

                nk_layout_row_dynamic(ctx, 30, 1);
                nk_label(ctx, "Dynamic fixed column layout with generated position and size:", NK_TEXT_LEFT);
                nk_layout_row_dynamic(ctx, 30, 3);
                nk_button_label(ctx, "button");
                nk_button_label(ctx, "button");
                nk_button_label(ctx, "button");

                nk_layout_row_dynamic(ctx, 30, 1);
                nk_label(ctx, "static fixed column layout with generated position and size:", NK_TEXT_LEFT);
                nk_layout_row_static(ctx, 30, 100, 3);
                nk_button_label(ctx, "button");
                nk_button_label(ctx, "button");
                nk_button_label(ctx, "button");

                nk_layout_row_dynamic(ctx, 30, 1);
                nk_label(ctx, "Dynamic array-based custom column layout with generated position and custom size:", NK_TEXT_LEFT);
                nk_layout_row(ctx, NK_DYNAMIC, 30, 3, ratio_two);
                nk_button_label(ctx, "button");
                nk_button_label(ctx, "button");
                nk_button_label(ctx, "button");

                nk_layout_row_dynamic(ctx, 30, 1);
                nk_label(ctx, "Static array-based custom column layout with generated position and custom size:", NK_TEXT_LEFT);
                nk_layout_row(ctx, NK_STATIC, 30, 3, width_two);
                nk_button_label(ctx, "button");
                nk_button_label(ctx, "button");
                nk_button_label(ctx, "button");

                nk_layout_row_dynamic(ctx, 30, 1);
                nk_label(ctx, "Dynamic immediate mode custom column layout with generated position and custom size:", NK_TEXT_LEFT);
                nk_layout_row_begin(ctx, NK_DYNAMIC, 30, 3);
                nk_layout_row_push(ctx, 0.2f);
                nk_button_label(ctx, "button");
                nk_layout_row_push(ctx, 0.6f);
                nk_button_label(ctx, "button");
                nk_layout_row_push(ctx, 0.2f);
                nk_button_label(ctx, "button");
                nk_layout_row_end(ctx);

                nk_layout_row_dynamic(ctx, 30, 1);
                nk_label(ctx, "Static immediate mode custom column layout with generated position and custom size:", NK_TEXT_LEFT);
                nk_layout_row_begin(ctx, NK_STATIC, 30, 3);
                nk_layout_row_push(ctx, 100);
                nk_button_label(ctx, "button");
                nk_layout_row_push(ctx, 200);
                nk_button_label(ctx, "button");
                nk_layout_row_push(ctx, 50);
                nk_button_label(ctx, "button");
                nk_layout_row_end(ctx);

                nk_layout_row_dynamic(ctx, 30, 1);
                nk_label(ctx, "Static free space with custom position and custom size:", NK_TEXT_LEFT);
                nk_layout_space_begin(ctx, NK_STATIC, 60, 4);
                nk_layout_space_push(ctx, nk_rect(100, 0, 100, 30));
                nk_button_label(ctx, "button");
                nk_layout_space_push(ctx, nk_rect(0, 15, 100, 30));
                nk_button_label(ctx, "button");
                nk_layout_space_push(ctx, nk_rect(200, 15, 100, 30));
                nk_button_label(ctx, "button");
                nk_layout_space_push(ctx, nk_rect(100, 30, 100, 30));
                nk_button_label(ctx, "button");
                nk_layout_space_end(ctx);

                nk_layout_row_dynamic(ctx, 30, 1);
                nk_label(ctx, "Row template:", NK_TEXT_LEFT);
                nk_layout_row_template_begin(ctx, 30);
                nk_layout_row_template_push_dynamic(ctx);
                nk_layout_row_template_push_variable(ctx, 80);
                nk_layout_row_template_push_static(ctx, 80);
                nk_layout_row_template_end(ctx);
                nk_button_label(ctx, "button");
                nk_button_label(ctx, "button");
                nk_button_label(ctx, "button");

                nk_tree_pop(ctx);
            }

            if (nk_tree_push(ctx, NK_TREE_NODE, "Group", NK_MINIMIZED))
            {
                static int group_titlebar = nk_false;
                static int group_border = nk_true;
                static int group_no_scrollbar = nk_false;
                static int group_width = 320;
                static int group_height = 200;

                nk_flags group_flags = 0;
                if (group_border) group_flags |= NK_WINDOW_BORDER;
                if (group_no_scrollbar) group_flags |= NK_WINDOW_NO_SCROLLBAR;
                if (group_titlebar) group_flags |= NK_WINDOW_TITLE;

                nk_layout_row_dynamic(ctx, 30, 3);
                nk_checkbox_label(ctx, "Titlebar", &group_titlebar);
                nk_checkbox_label(ctx, "Border", &group_border);
                nk_checkbox_label(ctx, "No Scrollbar", &group_no_scrollbar);

                nk_layout_row_begin(ctx, NK_STATIC, 22, 3);
                nk_layout_row_push(ctx, 50);
                nk_label(ctx, "size:", NK_TEXT_LEFT);
                nk_layout_row_push(ctx, 130);
                nk_property_int(ctx, "#Width:", 100, &group_width, 500, 10, 1);
                nk_layout_row_push(ctx, 130);
                nk_property_int(ctx, "#Height:", 100, &group_height, 500, 10, 1);
                nk_layout_row_end(ctx);

                nk_layout_row_static(ctx, (float)group_height, group_width, 2);
                if (nk_group_begin(ctx, "Group", group_flags)) {
                    int i = 0;
                    static int selected[16];
                    nk_layout_row_static(ctx, 18, 100, 1);
                    for (i = 0; i < 16; ++i)
                        nk_selectable_label(ctx, (selected[i]) ? "Selected" : "Unselected", NK_TEXT_CENTERED, &selected[i]);
                    nk_group_end(ctx);
                }
                nk_tree_pop(ctx);
            }
            if (nk_tree_push(ctx, NK_TREE_NODE, "Tree", NK_MINIMIZED))
            {
                static int root_selected = 0;
                int sel = root_selected;
                if (nk_tree_element_push(ctx, NK_TREE_NODE, "Root", NK_MINIMIZED, &sel)) {
                    static int selected[8];
                    int i = 0, node_select = selected[0];
                    if (sel != root_selected) {
                        root_selected = sel;
                        for (i = 0; i < 8; ++i)
                            selected[i] = sel;
                    }
                    if (nk_tree_element_push(ctx, NK_TREE_NODE, "Node", NK_MINIMIZED, &node_select)) {
                        int j = 0;
                        static int sel_nodes[4];
                        if (node_select != selected[0]) {
                            selected[0] = node_select;
                            for (i = 0; i < 4; ++i)
                                sel_nodes[i] = node_select;
                        }
                        nk_layout_row_static(ctx, 18, 100, 1);
                        for (j = 0; j < 4; ++j)
                            nk_selectable_symbol_label(ctx, NK_SYMBOL_CIRCLE_SOLID, (sel_nodes[j]) ? "Selected" : "Unselected", NK_TEXT_RIGHT, &sel_nodes[j]);
                        nk_tree_element_pop(ctx);
                    }
                    nk_layout_row_static(ctx, 18, 100, 1);
                    for (i = 1; i < 8; ++i)
                        nk_selectable_symbol_label(ctx, NK_SYMBOL_CIRCLE_SOLID, (selected[i]) ? "Selected" : "Unselected", NK_TEXT_RIGHT, &selected[i]);
                    nk_tree_element_pop(ctx);
                }
                nk_tree_pop(ctx);
            }
            if (nk_tree_push(ctx, NK_TREE_NODE, "Notebook", NK_MINIMIZED))
            {
                static int current_tab = 0;
                struct nk_rect bounds;
                float step = (2 * 3.141592654f) / 32;
                enum chart_type { CHART_LINE, CHART_HISTO, CHART_MIXED };
                const char* names[] = { "Lines", "Columns", "Mixed" };
                float id = 0;
                int i;

                // Header
                nk_style_push_vec2(ctx, &ctx->style.window.spacing, nk_vec2(0, 0));
                nk_style_push_float(ctx, &ctx->style.button.rounding, 0);
                nk_layout_row_begin(ctx, NK_STATIC, 20, 3);
                for (i = 0; i < 3; ++i) {
                    // make sure button perfectly fits text
                    const struct nk_user_font* f = ctx->style.font;
                    float text_width = f->width(f->userdata, f->height, names[i], nk_strlen(names[i]));
                    float widget_width = text_width + 3 * ctx->style.button.padding.x;
                    nk_layout_row_push(ctx, widget_width);
                    if (current_tab == i) {
                        // active tab gets highlighted
                        struct nk_style_item button_color = ctx->style.button.normal;
                        ctx->style.button.normal = ctx->style.button.active;
                        current_tab = nk_button_label(ctx, names[i]) ? i : current_tab;
                        ctx->style.button.normal = button_color;
                    }
                    else current_tab = nk_button_label(ctx, names[i]) ? i : current_tab;
                }
                nk_style_pop_float(ctx);

                // Body
                nk_layout_row_dynamic(ctx, 140, 1);
                if (nk_group_begin(ctx, "Notebook", NK_WINDOW_BORDER))
                {
                    nk_style_pop_vec2(ctx);
                    switch (current_tab) {
                    default: break;
                    case CHART_LINE:
                        nk_layout_row_dynamic(ctx, 100, 1);
                        bounds = nk_widget_bounds(ctx);
                        if (nk_chart_begin_colored(ctx, NK_CHART_LINES, nk_rgb(255, 0, 0), nk_rgb(150, 0, 0), 32, 0.0f, 1.0f)) {
                            nk_chart_add_slot_colored(ctx, NK_CHART_LINES, nk_rgb(0, 0, 255), nk_rgb(0, 0, 150), 32, -1.0f, 1.0f);
                            for (i = 0, id = 0; i < 32; ++i) {
                                nk_chart_push_slot(ctx, (float)fabs(sin(id)), 0);
                                nk_chart_push_slot(ctx, (float)cos(id), 1);
                                id += step;
                            }
                        }
                        nk_chart_end(ctx);
                        break;
                    case CHART_HISTO:
                        nk_layout_row_dynamic(ctx, 100, 1);
                        bounds = nk_widget_bounds(ctx);
                        if (nk_chart_begin_colored(ctx, NK_CHART_COLUMN, nk_rgb(255, 0, 0), nk_rgb(150, 0, 0), 32, 0.0f, 1.0f)) {
                            for (i = 0, id = 0; i < 32; ++i) {
                                nk_chart_push_slot(ctx, (float)fabs(sin(id)), 0);
                                id += step;
                            }
                        }
                        nk_chart_end(ctx);
                        break;
                    case CHART_MIXED:
                        nk_layout_row_dynamic(ctx, 100, 1);
                        bounds = nk_widget_bounds(ctx);
                        if (nk_chart_begin_colored(ctx, NK_CHART_LINES, nk_rgb(255, 0, 0), nk_rgb(150, 0, 0), 32, 0.0f, 1.0f)) {
                            nk_chart_add_slot_colored(ctx, NK_CHART_LINES, nk_rgb(0, 0, 255), nk_rgb(0, 0, 150), 32, -1.0f, 1.0f);
                            nk_chart_add_slot_colored(ctx, NK_CHART_COLUMN, nk_rgb(0, 255, 0), nk_rgb(0, 150, 0), 32, 0.0f, 1.0f);
                            for (i = 0, id = 0; i < 32; ++i) {
                                nk_chart_push_slot(ctx, (float)fabs(sin(id)), 0);
                                nk_chart_push_slot(ctx, (float)fabs(cos(id)), 1);
                                nk_chart_push_slot(ctx, (float)fabs(sin(id)), 2);
                                id += step;
                            }
                        }
                        nk_chart_end(ctx);
                        break;
                    }
                    nk_group_end(ctx);
                }
                else nk_style_pop_vec2(ctx);
                nk_tree_pop(ctx);
            }

            if (nk_tree_push(ctx, NK_TREE_NODE, "Simple", NK_MINIMIZED))
            {
                nk_layout_row_dynamic(ctx, 300, 2);
                if (nk_group_begin(ctx, "Group_Without_Border", 0)) {
                    int i = 0;
                    char buffer[64];
                    nk_layout_row_static(ctx, 18, 150, 1);
                    for (i = 0; i < 64; ++i) {
                        sprintf(buffer, "0x%02x", i);
                        nk_labelf(ctx, NK_TEXT_LEFT, "%s: scrollable region", buffer);
                    }
                    nk_group_end(ctx);
                }
                if (nk_group_begin(ctx, "Group_With_Border", NK_WINDOW_BORDER)) {
                    int i = 0;
                    char buffer[64];
                    nk_layout_row_dynamic(ctx, 25, 2);
                    for (i = 0; i < 64; ++i) {
                        sprintf(buffer, "%08d", ((((i % 7) * 10) ^ 32)) + (64 + (i % 2) * 2));
                        nk_button_label(ctx, buffer);
                    }
                    nk_group_end(ctx);
                }
                nk_tree_pop(ctx);
            }

            if (nk_tree_push(ctx, NK_TREE_NODE, "Complex", NK_MINIMIZED))
            {
                int i;
                nk_layout_space_begin(ctx, NK_STATIC, 500, 64);
                nk_layout_space_push(ctx, nk_rect(0, 0, 150, 500));
                if (nk_group_begin(ctx, "Group_left", NK_WINDOW_BORDER)) {
                    static int selected[32];
                    nk_layout_row_static(ctx, 18, 100, 1);
                    for (i = 0; i < 32; ++i)
                        nk_selectable_label(ctx, (selected[i]) ? "Selected" : "Unselected", NK_TEXT_CENTERED, &selected[i]);
                    nk_group_end(ctx);
                }

                nk_layout_space_push(ctx, nk_rect(160, 0, 150, 240));
                if (nk_group_begin(ctx, "Group_top", NK_WINDOW_BORDER)) {
                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_button_label(ctx, "#FFAA");
                    nk_button_label(ctx, "#FFBB");
                    nk_button_label(ctx, "#FFCC");
                    nk_button_label(ctx, "#FFDD");
                    nk_button_label(ctx, "#FFEE");
                    nk_button_label(ctx, "#FFFF");
                    nk_group_end(ctx);
                }

                nk_layout_space_push(ctx, nk_rect(160, 250, 150, 250));
                if (nk_group_begin(ctx, "Group_buttom", NK_WINDOW_BORDER)) {
                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_button_label(ctx, "#FFAA");
                    nk_button_label(ctx, "#FFBB");
                    nk_button_label(ctx, "#FFCC");
                    nk_button_label(ctx, "#FFDD");
                    nk_button_label(ctx, "#FFEE");
                    nk_button_label(ctx, "#FFFF");
                    nk_group_end(ctx);
                }

                nk_layout_space_push(ctx, nk_rect(320, 0, 150, 150));
                if (nk_group_begin(ctx, "Group_right_top", NK_WINDOW_BORDER)) {
                    static int selected[4];
                    nk_layout_row_static(ctx, 18, 100, 1);
                    for (i = 0; i < 4; ++i)
                        nk_selectable_label(ctx, (selected[i]) ? "Selected" : "Unselected", NK_TEXT_CENTERED, &selected[i]);
                    nk_group_end(ctx);
                }

                nk_layout_space_push(ctx, nk_rect(320, 160, 150, 150));
                if (nk_group_begin(ctx, "Group_right_center", NK_WINDOW_BORDER)) {
                    static int selected[4];
                    nk_layout_row_static(ctx, 18, 100, 1);
                    for (i = 0; i < 4; ++i)
                        nk_selectable_label(ctx, (selected[i]) ? "Selected" : "Unselected", NK_TEXT_CENTERED, &selected[i]);
                    nk_group_end(ctx);
                }

                nk_layout_space_push(ctx, nk_rect(320, 320, 150, 150));
                if (nk_group_begin(ctx, "Group_right_bottom", NK_WINDOW_BORDER)) {
                    static int selected[4];
                    nk_layout_row_static(ctx, 18, 100, 1);
                    for (i = 0; i < 4; ++i)
                        nk_selectable_label(ctx, (selected[i]) ? "Selected" : "Unselected", NK_TEXT_CENTERED, &selected[i]);
                    nk_group_end(ctx);
                }
                nk_layout_space_end(ctx);
                nk_tree_pop(ctx);
            }

            if (nk_tree_push(ctx, NK_TREE_NODE, "Splitter", NK_MINIMIZED))
            {
                const struct nk_input* in = &ctx->input;
                nk_layout_row_static(ctx, 20, 320, 1);
                nk_label(ctx, "Use slider and spinner to change tile size", NK_TEXT_LEFT);
                nk_label(ctx, "Drag the space between tiles to change tile ratio", NK_TEXT_LEFT);

                if (nk_tree_push(ctx, NK_TREE_NODE, "Vertical", NK_MINIMIZED))
                {
                    static float a = 100, b = 100, c = 100;
                    struct nk_rect bounds;

                    float row_layout[5];
                    row_layout[0] = a;
                    row_layout[1] = 8;
                    row_layout[2] = b;
                    row_layout[3] = 8;
                    row_layout[4] = c;

                    // header
                    nk_layout_row_static(ctx, 30, 100, 2);
                    nk_label(ctx, "left:", NK_TEXT_LEFT);
                    nk_slider_float(ctx, 10.0f, &a, 200.0f, 10.0f);

                    nk_label(ctx, "middle:", NK_TEXT_LEFT);
                    nk_slider_float(ctx, 10.0f, &b, 200.0f, 10.0f);

                    nk_label(ctx, "right:", NK_TEXT_LEFT);
                    nk_slider_float(ctx, 10.0f, &c, 200.0f, 10.0f);

                    // tiles
                    nk_layout_row(ctx, NK_STATIC, 200, 5, row_layout);

                    // left space
                    if (nk_group_begin(ctx, "left", NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_button_label(ctx, "#FFAA");
                        nk_button_label(ctx, "#FFBB");
                        nk_button_label(ctx, "#FFCC");
                        nk_button_label(ctx, "#FFDD");
                        nk_button_label(ctx, "#FFEE");
                        nk_button_label(ctx, "#FFFF");
                        nk_group_end(ctx);
                    }

                    // scaler
                    bounds = nk_widget_bounds(ctx);
                    nk_spacing(ctx, 1);
                    if ((nk_input_is_mouse_hovering_rect(in, bounds) ||
                        nk_input_is_mouse_prev_hovering_rect(in, bounds)) &&
                        nk_input_is_mouse_down(in, NK_BUTTON_LEFT))
                    {
                        a = row_layout[0] + in->mouse.delta.x;
                        b = row_layout[2] - in->mouse.delta.x;
                    }

                    // middle space
                    if (nk_group_begin(ctx, "center", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_button_label(ctx, "#FFAA");
                        nk_button_label(ctx, "#FFBB");
                        nk_button_label(ctx, "#FFCC");
                        nk_button_label(ctx, "#FFDD");
                        nk_button_label(ctx, "#FFEE");
                        nk_button_label(ctx, "#FFFF");
                        nk_group_end(ctx);
                    }

                    // sscaler
                    bounds = nk_widget_bounds(ctx);
                    nk_spacing(ctx, 1);
                    if ((nk_input_is_mouse_hovering_rect(in, bounds) ||
                        nk_input_is_mouse_prev_hovering_rect(in, bounds)) &&
                        nk_input_is_mouse_down(in, NK_BUTTON_LEFT))
                    {
                        b = (row_layout[2] + in->mouse.delta.x);
                        c = (row_layout[4] - in->mouse.delta.x);
                    }

                    // right space
                    if (nk_group_begin(ctx, "right", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_button_label(ctx, "#FFAA");
                        nk_button_label(ctx, "#FFBB");
                        nk_button_label(ctx, "#FFCC");
                        nk_button_label(ctx, "#FFDD");
                        nk_button_label(ctx, "#FFEE");
                        nk_button_label(ctx, "#FFFF");
                        nk_group_end(ctx);
                    }

                    nk_tree_pop(ctx);
                }

                if (nk_tree_push(ctx, NK_TREE_NODE, "Horizontal", NK_MINIMIZED))
                {
                    static float a = 100, b = 100, c = 100;
                    struct nk_rect bounds;

                    // header
                    nk_layout_row_static(ctx, 30, 100, 2);
                    nk_label(ctx, "top:", NK_TEXT_LEFT);
                    nk_slider_float(ctx, 10.0f, &a, 200.0f, 10.0f);

                    nk_label(ctx, "middle:", NK_TEXT_LEFT);
                    nk_slider_float(ctx, 10.0f, &b, 200.0f, 10.0f);

                    nk_label(ctx, "bottom:", NK_TEXT_LEFT);
                    nk_slider_float(ctx, 10.0f, &c, 200.0f, 10.0f);

                    // top space
                    nk_layout_row_dynamic(ctx, a, 1);
                    if (nk_group_begin(ctx, "top", NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER)) {
                        nk_layout_row_dynamic(ctx, 25, 3);
                        nk_button_label(ctx, "#FFAA");
                        nk_button_label(ctx, "#FFBB");
                        nk_button_label(ctx, "#FFCC");
                        nk_button_label(ctx, "#FFDD");
                        nk_button_label(ctx, "#FFEE");
                        nk_button_label(ctx, "#FFFF");
                        nk_group_end(ctx);
                    }

                    // scaler
                    nk_layout_row_dynamic(ctx, 8, 1);
                    bounds = nk_widget_bounds(ctx);
                    nk_spacing(ctx, 1);
                    if ((nk_input_is_mouse_hovering_rect(in, bounds) ||
                        nk_input_is_mouse_prev_hovering_rect(in, bounds)) &&
                        nk_input_is_mouse_down(in, NK_BUTTON_LEFT))
                    {
                        a = a + in->mouse.delta.y;
                        b = b - in->mouse.delta.y;
                    }

                    // middle space
                    nk_layout_row_dynamic(ctx, b, 1);
                    if (nk_group_begin(ctx, "middle", NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER)) {
                        nk_layout_row_dynamic(ctx, 25, 3);
                        nk_button_label(ctx, "#FFAA");
                        nk_button_label(ctx, "#FFBB");
                        nk_button_label(ctx, "#FFCC");
                        nk_button_label(ctx, "#FFDD");
                        nk_button_label(ctx, "#FFEE");
                        nk_button_label(ctx, "#FFFF");
                        nk_group_end(ctx);
                    }

                    {
                        // scaler
                        nk_layout_row_dynamic(ctx, 8, 1);
                        bounds = nk_widget_bounds(ctx);
                        if ((nk_input_is_mouse_hovering_rect(in, bounds) ||
                            nk_input_is_mouse_prev_hovering_rect(in, bounds)) &&
                            nk_input_is_mouse_down(in, NK_BUTTON_LEFT))
                        {
                            b = b + in->mouse.delta.y;
                            c = c - in->mouse.delta.y;
                        }
                    }

                    // bottom space
                    nk_layout_row_dynamic(ctx, c, 1);
                    if (nk_group_begin(ctx, "bottom", NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER)) {
                        nk_layout_row_dynamic(ctx, 25, 3);
                        nk_button_label(ctx, "#FFAA");
                        nk_button_label(ctx, "#FFBB");
                        nk_button_label(ctx, "#FFCC");
                        nk_button_label(ctx, "#FFDD");
                        nk_button_label(ctx, "#FFEE");
                        nk_button_label(ctx, "#FFFF");
                        nk_group_end(ctx);
                    }
                    nk_tree_pop(ctx);
                }
                nk_tree_pop(ctx);
            }
            nk_tree_pop(ctx);
        }
    }
    nk_end(ctx);
}
*/

void properties_window(int ent_len)
{
    if (nk_begin(ctx, "Properties", nk_rect(10, 10, 350, 1000),
        NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
        NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
    {
        static int scripts_act = 0;
        static float property_pos_x = 0.0f;
        static float property_pos_y = 0.0f;
        static float property_pos_z = 0.0f;
        static const float ratio[] = { 120, 150 };
        char buffer[50];


        // ---- theme menu ----
        
        nk_menubar_begin(ctx);
        nk_layout_row_begin(ctx, NK_STATIC, 35, 3);
        nk_layout_row_push(ctx, 65);
        if (nk_menu_begin_label(ctx, "Add Entity", NK_TEXT_LEFT, nk_vec2(80, 200)))
        {
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_menu_item_label(ctx, "Cube", NK_TEXT_LEFT))
            {
                add_entity_cube();
            }
            if (nk_menu_item_label(ctx, "Dir Light", NK_TEXT_LEFT))
            {
                printf("Add Entity >> Dir Light not implemented\n");
            }
            if (nk_menu_item_label(ctx, "Point Light", NK_TEXT_LEFT))
            {
                printf("Add Entity >> Point Light not implemented\n");
            }
            if (nk_menu_item_label(ctx, "Spot Light", NK_TEXT_LEFT))
            {
                printf("Add Entity >> Spot Light not implemented\n");
            }
            nk_menu_end(ctx);
        }
        if (nk_menu_begin_label(ctx, "Theme", NK_TEXT_LEFT, nk_vec2(80, 200)))
        {
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_menu_item_label(ctx, "black", NK_TEXT_LEFT))
            {
                set_style(ctx, THEME_BLACK);
            }
            if (nk_menu_item_label(ctx, "white", NK_TEXT_LEFT))
            {
                set_style(ctx, THEME_WHITE);
            }
            if (nk_menu_item_label(ctx, "red", NK_TEXT_LEFT))
            {
                set_style(ctx, THEME_RED);
            }
            if (nk_menu_item_label(ctx, "blue", NK_TEXT_LEFT))
            {
                set_style(ctx, THEME_BLUE);
            }
            if (nk_menu_item_label(ctx, "dark", NK_TEXT_LEFT))
            {
                set_style(ctx, THEME_DARK);
            }
            nk_menu_end(ctx);
        }
        nk_layout_row_push(ctx, 70);
        nk_menubar_end(ctx);

        // ---- play / pause ----
        
        nk_layout_row_static(ctx, 25, 80, 1);
        if (nk_button_label(ctx, scripts_act == 0 ? "Play" : "Pause"))
        {
            
            set_all_scripts(scripts_act == 0 ? BEE_TRUE : BEE_FALSE);
            scripts_act = scripts_act == 0 ? 1 : 0;
        }
        
        // spacing
        nk_layout_row_static(ctx, 5, 10, 1);
        nk_label(ctx, " ", NK_TEXT_ALIGN_CENTERED);

        // --------------------
        if (nk_tree_push(ctx, NK_TREE_TAB, "Scene", NK_MINIMIZED))
        {

            if (nk_tree_push(ctx, NK_TREE_TAB, "Settings", NK_MINIMIZED))
            {
                settings _settings = get_settings();

                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label(ctx, "free-look mode", NK_TEXT_LEFT);
                nk_property_float(ctx, "mosue sensitivity", -1.0f, _settings.free_look_mouse_sensitivity, 1.0f, 0.1f, 0.01f);

                // spacing
                nk_layout_row_static(ctx, 5, 10, 1);
                nk_label(ctx, " ", NK_TEXT_ALIGN_CENTERED);

                static int skybox_enabled = 1;
                static int skybox_enabled_last_frame = 1;
                nk_layout_row_dynamic(ctx, 15, 1);
                nk_checkbox_label(ctx, " draw skybox", &skybox_enabled);
                if (skybox_enabled == 0 && skybox_enabled_last_frame == 1)
                {
                    renderer_set_skybox_active(BEE_FALSE);

                    skybox_enabled_last_frame = 0;
                }
                else if (skybox_enabled == 1 && skybox_enabled_last_frame == 0)
                {
                    renderer_set_skybox_active(BEE_TRUE);

                    skybox_enabled_last_frame = 1;
                }

                nk_layout_row_dynamic(ctx, 30, 1);
                nk_label(ctx, "background color", NK_TEXT_LEFT);
                nk_layout_row_dynamic(ctx, 25, 1);

                struct nk_colorf old_bg = bg;

                // complex color combobox
                if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(200, 400))) 
                {
                    enum color_mode { COL_RGB, COL_HSV };
                    static int col_mode = COL_RGB;

                    nk_layout_row_dynamic(ctx, 120, 1);
                    bg = nk_color_picker(ctx, bg, NK_RGB);

                    nk_layout_row_dynamic(ctx, 25, 2);
                    col_mode = nk_option_label(ctx, "RGB", col_mode == COL_RGB) ? COL_RGB : col_mode;
                    col_mode = nk_option_label(ctx, "HSV", col_mode == COL_HSV) ? COL_HSV : col_mode;

                    nk_layout_row_dynamic(ctx, 25, 1);
                    if (col_mode == COL_RGB) {
                        bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
                        bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
                        bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
                    }
                    else {
                        float hsva[4];
                        nk_colorf_hsva_fv(hsva, bg);
                        hsva[0] = nk_propertyf(ctx, "#H:", 0, hsva[0], 1.0f, 0.01f, 0.05f);
                        hsva[1] = nk_propertyf(ctx, "#S:", 0, hsva[1], 1.0f, 0.01f, 0.05f);
                        hsva[2] = nk_propertyf(ctx, "#V:", 0, hsva[2], 1.0f, 0.01f, 0.05f);
                        bg = nk_hsva_colorfv(hsva);
                    }
                    nk_combo_end(ctx);

                    // check if the color was changed
                    if (bg.r != old_bg.r || bg.g != old_bg.g || bg.b != old_bg.b || bg.a != old_bg.a)
                    {
                        glClearColor(bg.r, bg.g, bg.b, bg.a);
                    }
                }

                nk_tree_pop(ctx);
            }

            if (nk_tree_push(ctx, NK_TREE_TAB, "Debug", NK_MINIMIZED))
            {
                renderer_properties prop = get_renderer_properties();

                const struct nk_input* in = &ctx->input;
                struct nk_rect bounds;

                static int wireframe_mode_enabled = 0;
                static int wireframe_mode_enabled_last_frame = 0;
                nk_layout_row_dynamic(ctx, 15, 1);
                bounds = nk_widget_bounds(ctx);
                nk_checkbox_label(ctx, "Wireframe", &wireframe_mode_enabled);
                if (nk_input_is_mouse_hovering_rect(in, bounds))
                    nk_tooltip(ctx, " Activate Wireframe-Mode, you can also use Tab.");

                static int normal_mode_enabled = 0;
                static int normal_mode_enabled_last_frame = 0;
                nk_layout_row_dynamic(ctx, 15, 1);
                bounds = nk_widget_bounds(ctx);
                nk_checkbox_label(ctx, "Normals", &normal_mode_enabled);
                if (nk_input_is_mouse_hovering_rect(in, bounds))
                    nk_tooltip(ctx, " Not implemented yet.");

                static int uv_mode_enabled = 0;
                static int uv_mode_enabled_last_frame = 0;
                nk_layout_row_dynamic(ctx, 15, 1);
                bounds = nk_widget_bounds(ctx);
                nk_checkbox_label(ctx, "UVs", &uv_mode_enabled);
                if (nk_input_is_mouse_hovering_rect(in, bounds))
                    nk_tooltip(ctx, " Not implemented yet.");

                if (wireframe_mode_enabled == 0 && wireframe_mode_enabled_last_frame == 1)
                {
                    // draw in solid-mode
                    renderer_enable_wireframe_mode(BEE_FALSE);

                    wireframe_mode_enabled_last_frame = 0;
                }
                else if ( wireframe_mode_enabled == 1 && wireframe_mode_enabled_last_frame == 0)
                {
                    // draw in wireframe-mode
                    renderer_enable_wireframe_mode(BEE_TRUE);

                    wireframe_mode_enabled_last_frame = 1;
                }
                if (normal_mode_enabled == 0 && normal_mode_enabled_last_frame == 1)
                {
                    // draw in solid-mode
                    renderer_enable_normal_mode(BEE_FALSE);

                    normal_mode_enabled_last_frame = 0;
                }
                else if (normal_mode_enabled == 1 && normal_mode_enabled_last_frame == 0)
                {
                    // draw in wireframe-mode
                    renderer_enable_normal_mode(BEE_TRUE);

                    normal_mode_enabled_last_frame = 1;
                }
                if (uv_mode_enabled == 0 && uv_mode_enabled_last_frame == 1)
                {
                    // draw in solid-mode
                    renderer_enable_uv_mode(BEE_FALSE);

                    uv_mode_enabled_last_frame = 0;
                }
                else if (uv_mode_enabled == 1 && uv_mode_enabled_last_frame == 0)
                {
                    // draw in wireframe-mode
                    renderer_enable_uv_mode(BEE_TRUE);

                    uv_mode_enabled_last_frame = 1;
                }

                // complex color combobox
                struct nk_colorf col = { *prop.wireframe_col_r, *prop.wireframe_col_g, *prop.wireframe_col_b, 0.0f };
                struct nk_colorf old_col = col;
                nk_layout_row_dynamic(ctx, 30, 1);
                nk_label(ctx, "wireframe color", NK_TEXT_LEFT);
                nk_layout_row_dynamic(ctx, 25, 1);
                if (nk_combo_begin_color(ctx, nk_rgb_cf(col), nk_vec2(200, 400)))
                {
                    enum color_mode { COL_RGB, COL_HSV };
                    static int col_mode = COL_RGB;

                    nk_layout_row_dynamic(ctx, 120, 1);
                    col = nk_color_picker(ctx, col, NK_RGB);

                    nk_layout_row_dynamic(ctx, 25, 2);
                    col_mode = nk_option_label(ctx, "RGB", col_mode == COL_RGB) ? COL_RGB : col_mode;
                    col_mode = nk_option_label(ctx, "HSV", col_mode == COL_HSV) ? COL_HSV : col_mode;

                    nk_layout_row_dynamic(ctx, 25, 1);
                    if (col_mode == COL_RGB) {
                        col.r = nk_propertyf(ctx, "#R:", 0, col.r, 1.0f, 0.01f, 0.005f);
                        col.g = nk_propertyf(ctx, "#G:", 0, col.g, 1.0f, 0.01f, 0.005f);
                        col.b = nk_propertyf(ctx, "#B:", 0, col.b, 1.0f, 0.01f, 0.005f);
                    }
                    else {
                        float hsva[4];
                        nk_colorf_hsva_fv(hsva, col);
                        hsva[0] = nk_propertyf(ctx, "#H:", 0, hsva[0], 1.0f, 0.01f, 0.05f);
                        hsva[1] = nk_propertyf(ctx, "#S:", 0, hsva[1], 1.0f, 0.01f, 0.05f);
                        hsva[2] = nk_propertyf(ctx, "#V:", 0, hsva[2], 1.0f, 0.01f, 0.05f);
                        col = nk_hsva_colorfv(hsva);
                    }

                    // check if the color was changed
                    if (col.r != old_col.r || col.g != old_col.g || col.b != old_col.b || col.a != old_col.a)
                    {
                        *prop.wireframe_col_r = col.r;
                        *prop.wireframe_col_g = col.g;
                        *prop.wireframe_col_b = col.b;
                    }

                    nk_combo_end(ctx);
                }


                if (nk_tree_push(ctx, NK_TREE_TAB, "FPS Diagnostics", NK_MINIMIZED))
                {
                    float id = 0;
                    static int col_index = -1;
                    static int line_index = -1;
                    float step = (2 * 3.141592654f) / 32;

                    int i;
                    int index = -1;
                    struct nk_rect bounds;

                    /* line chart */
                    id = 0;
                    index = -1;
                    nk_layout_row_dynamic(ctx, 100, 1);
                    bounds = nk_widget_bounds(ctx);
                    if (nk_chart_begin(ctx, NK_CHART_LINES, 32, -1.0f, 1.0f)) {
                        for (i = 0; i < 32; ++i) {
                            nk_flags res = nk_chart_push(ctx, (float)cos(id));
                            if (res & NK_CHART_HOVERING)
                                index = (int)i;
                            if (res & NK_CHART_CLICKED)
                                line_index = (int)i;
                            id += step;
                        }
                        nk_chart_end(ctx);
                    }

                    if (index != -1)
                        nk_tooltipf(ctx, "Value: %.2f", (float)cos((float)index * step));
                    if (line_index != -1) {
                        nk_layout_row_dynamic(ctx, 20, 1);
                        nk_labelf(ctx, NK_TEXT_LEFT, "Selected value: %.2f", (float)cos((float)index * step));
                    }


                    nk_tree_pop(ctx);
                }

                nk_tree_pop(ctx);
            }

            if (nk_tree_push(ctx, NK_TREE_TAB, "Camera", NK_MINIMIZED))
            {
                renderer_properties prop = get_renderer_properties();

                if (nk_tree_push(ctx, NK_TREE_NODE, "Position", NK_MINIMIZED))
                {
                    static vec3 old_pos;
                    vec3 pos;
                    get_camera_pos(pos);

                    nk_property_float(ctx, "Pos X:", -1024.0f, &pos[0], 1024.0f, 0.1f, 0.2f);

                    nk_property_float(ctx, "Pos Y:", -1024.0f, &pos[1], 1024.0f, 0.1f, 0.2f);

                    nk_property_float(ctx, "Pos Z:", -1024.0f, &pos[2], 1024.0f, 0.1f, 0.2f);

                    if (old_pos != pos)
                    {
                        set_camera_pos(pos);
                        glm_vec3_copy(pos, old_pos);
                    }

                    nk_tree_pop(ctx);
                }

                if (nk_tree_push(ctx, NK_TREE_NODE, "Direction", NK_MINIMIZED))
                {
                    static vec3 old_front;
                    vec3 front;
                    get_camera_front(front);

                    nk_property_float(ctx, "Dir X:", -1024.0f, &front[0], 1024.0f, 0.1f, 0.2f);
                                            
                    nk_property_float(ctx, "Dir Y:", -1024.0f, &front[1], 1024.0f, 0.1f, 0.2f);
                                            
                    nk_property_float(ctx, "Dir Z:", -1024.0f, &front[2], 1024.0f, 0.1f, 0.2f);

                    if (old_front != front)
                    {
                        set_camera_front(front);
                        glm_vec3_copy(front, old_front);
                    }

                    nk_tree_pop(ctx);
                }


                nk_property_float(ctx, "Perspective:", 1.0f, prop.perspective, 200.0f, 0.1f, 0.2f);
                
                nk_property_float(ctx, "Near Plane:", 0.0001f, prop.near_plane, 10.0f, 0.1f, 0.01f);
                
                nk_property_float(ctx, "Far Plane:", 1.0f, prop.far_plane, 500.0f, 0.1f, 0.2f);

                nk_tree_pop(ctx);
            }

            nk_tree_pop(ctx);
        }

        // spacing
        nk_layout_row_static(ctx, 5, 10, 1);
        nk_label(ctx, " ", NK_TEXT_ALIGN_CENTERED);

        // ---- entity tree ----
        if (nk_tree_push(ctx, NK_TREE_TAB, "Entities", NK_MAXIMIZED)) // NK_MAXIMIZED: open on start
        {
            static int selected_entities_old[100];
            for (int i = 0; i < 100; ++i)
            {
                selected_entities_old[i] = selected_entities[i];
            }

            if (ent_len <= 0)
            {
                nk_layout_row(ctx, NK_STATIC, 25, 1, ratio);
                nk_label_colored(ctx, "no entities", NK_TEXT_LEFT, red);
            }
            else
            {
                nk_layout_row_static(ctx, 18, 100, 1);
            
                for (int i = 0; i < ent_len; ++i)
                {
                    entity_properties prop = get_entity_properties(i);
                    nk_selectable_label(ctx, prop.ent_name, NK_TEXT_LEFT, &selected_entities[i]);
                }
            }

            // spacing
            nk_layout_row_static(ctx, 5, 10, 1);
            nk_label(ctx, " ", NK_TEXT_ALIGN_CENTERED);

            // ---- button ----
            // nk_layout_row_static(ctx, 25, 80, 1);
            // if (nk_button_label(ctx, "deselect"))
            // {
            //     for (int i = 0; i < ent_len; ++i)
            //     {
            //         selected_entities[i] = nk_false;
            //     }
            // }
            // ----------------
            
            // spacing
            nk_layout_row_static(ctx, 5, 10, 1);
            nk_label(ctx, " ", NK_TEXT_ALIGN_CENTERED);

            static int selected_entity_old;
            static int selected_entity = -999;

            for (int i = 0; i < ent_len; ++i)
            {
                if (selected_entities[i] == nk_true && selected_entities_old[i] == nk_false)
                {
                    selected_entity = i;
                    break;
                }
            }
            if (selected_entity != selected_entity_old)
            {
                for (int i = 0; i < ent_len; ++i)
                {
                    if (selected_entities[i] == nk_true && selected_entities_old[i] == nk_true)
                    {
                        selected_entities[i] = nk_false;
                        break;
                    }
                }
            }
            selected_entity_old = selected_entity;

            nk_bool is_selected = nk_false;
            for (int i = 0; i < ent_len; ++i)
            {
                if (selected_entities[i] == nk_true)
                {
                    is_selected = nk_true;
                    break;
                }
            }

            if (is_selected == nk_false || selected_entity == -999 || selected_entity >= ent_len || selected_entity < 0)
            {
                nk_layout_row(ctx, NK_STATIC, 25, 1, ratio);
                nk_label_colored(ctx, "no entity selected", NK_TEXT_LEFT, red);
            }
            else 
            {
                const struct nk_input* in = &ctx->input;
                struct nk_rect bounds;

                entity_properties prop = get_entity_properties(selected_entity);

                nk_layout_row_dynamic(ctx, 30, 2);
                nk_label(ctx, "Name: ", NK_TEXT_LEFT);
                nk_label(ctx, prop.ent_name, NK_TEXT_LEFT);
            
                if (*prop.has_trans == BEE_TRUE && nk_tree_push(ctx, NK_TREE_TAB, "Transform", NK_MINIMIZED)) {

                    if (nk_tree_push(ctx, NK_TREE_NODE, "Position", NK_MINIMIZED))
                    {
                        nk_property_float(ctx, "Pos X:", -1024.0f, prop.pos_x, 1024.0f, 0.1f, 0.2f);
                       
                        nk_property_float(ctx, "Pos Y:", -1024.0f, prop.pos_y, 1024.0f, 0.1f, 0.2f);
                        
                        nk_property_float(ctx, "Pos Z:", -1024.0f, prop.pos_z, 1024.0f, 0.1f, 0.2f);

                        nk_tree_pop(ctx);
                    }
                    if (nk_tree_push(ctx, NK_TREE_NODE, "Rotation", NK_MINIMIZED))
                    {
                        nk_layout_row_static(ctx, 30, 80, 2);
                        static int option;
                        option = nk_option_label(ctx, "global", option == 0) ? 0 : option;
                        option = nk_option_label(ctx, "local",  option == 1) ? 1 : option;

                        nk_layout_row_dynamic(ctx, 20, 1);
                        nk_property_float(ctx, "Rot X:", -1024.0f, prop.rot_x, 1024.0f, 0.1f, 0.2f);
                        
                        nk_property_float(ctx, "Rot Y:", -1024.0f, prop.rot_y, 1024.0f, 0.1f, 0.2f);
                        
                        nk_property_float(ctx, "Rot Z:", -1024.0f, prop.rot_z, 1024.0f, 0.1f, 0.2f);

                        nk_tree_pop(ctx);
                    }
                    if (nk_tree_push(ctx, NK_TREE_NODE, "Scale", NK_MINIMIZED))
                    {
                        nk_property_float(ctx, "Scale X:", -1024.0f, prop.scale_x, 1024.0f, 0.1f, 0.2f);
                        
                        nk_property_float(ctx, "Scale Y:", -1024.0f, prop.scale_y, 1024.0f, 0.1f, 0.2f);
                        
                        nk_property_float(ctx, "Scale Z:", -1024.0f, prop.scale_z, 1024.0f, 0.1f, 0.2f);
                    
                        nk_tree_pop(ctx);
                    }
                    nk_tree_pop(ctx);
                }
                if (*prop.has_trans == BEE_FALSE)
                {
                    nk_layout_row(ctx, NK_STATIC, 25, 1, ratio);
                    nk_label_colored(ctx, "no transform", NK_TEXT_LEFT, red);
                }

                if (*prop.has_model == BEE_TRUE && nk_tree_push(ctx, NK_TREE_TAB, "Mesh", NK_MINIMIZED))
                {

                    nk_layout_row_dynamic(ctx, 25, 2);
                    nk_label(ctx, "Name", NK_TEXT_LEFT);
                    nk_label(ctx, prop.mesh_name, NK_TEXT_RIGHT);

                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_checkbox_label(ctx, " visible", prop.mesh_visible);

                    nk_layout_row_dynamic(ctx, 25, 2);
                
                    sprintf(buffer, "%d", *prop.verts_len);
                    nk_label(ctx, "Vertices: ", NK_TEXT_LEFT);
                    nk_label(ctx, buffer, NK_TEXT_RIGHT);
            
                    sprintf(buffer, "%d", *prop.indices_len);
                    nk_label(ctx, "Indices: ", NK_TEXT_LEFT);
                    nk_label(ctx, buffer, NK_TEXT_RIGHT);
            
                    nk_label(ctx, "Indexed: ", NK_TEXT_LEFT);
                    nk_label(ctx, *prop.mesh_indexed == 0 ? "false" : "true", NK_TEXT_RIGHT);
            
            
                    nk_tree_pop(ctx);
                }
            
                if (*prop.has_model == BEE_TRUE && nk_tree_push(ctx, NK_TREE_TAB, "Material", NK_MINIMIZED))
                {
                    nk_layout_row_dynamic(ctx, 25, 2);
                    nk_label(ctx, "Name", NK_TEXT_LEFT);
                    nk_label(ctx, prop.material_name, NK_TEXT_RIGHT);


                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_property_float(ctx, "Shininess", 0.0f, prop.shininess, 1.0f, 0.1f, 0.002f);
                    nk_layout_row_dynamic(ctx, 25, 2);
                    nk_property_float(ctx, "Tile X", 0.0f, prop.tile_x, 100.0f, 0.1f, 0.1f);
                    nk_property_float(ctx, "Tile Y", 0.0f, prop.tile_y, 100.0f, 0.1f, 0.1f);
            
                    // tint-color
                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_label(ctx, "Tint Color", NK_TEXT_LEFT);
                    struct nk_colorf tint = { *prop.tint_r, *prop.tint_g, *prop.tint_b };
                    if (nk_combo_begin_color(ctx, nk_rgb_cf(tint), nk_vec2(200, 400)))
                    {
                        enum color_mode { COL_RGB, COL_HSV };
                        static int col_mode = COL_RGB;

                        nk_layout_row_dynamic(ctx, 120, 1);
                        tint = nk_color_picker(ctx, tint, NK_RGB);

                        nk_layout_row_dynamic(ctx, 25, 2);
                        col_mode = nk_option_label(ctx, "RGB", col_mode == COL_RGB) ? COL_RGB : col_mode;
                        col_mode = nk_option_label(ctx, "HSV", col_mode == COL_HSV) ? COL_HSV : col_mode;

                        nk_layout_row_dynamic(ctx, 25, 1);
                        if (col_mode == COL_RGB) {
                            tint.r = nk_propertyf(ctx, "#R:", 0, tint.r, 1.0f, 0.01f, 0.005f);
                            tint.g = nk_propertyf(ctx, "#G:", 0, tint.g, 1.0f, 0.01f, 0.005f);
                            tint.b = nk_propertyf(ctx, "#B:", 0, tint.b, 1.0f, 0.01f, 0.005f);
                        }
                        else {
                            float hsva[4];
                            nk_colorf_hsva_fv(hsva, tint);
                            hsva[0] = nk_propertyf(ctx, "#H:", 0, hsva[0], 1.0f, 0.01f, 0.05f);
                            hsva[1] = nk_propertyf(ctx, "#S:", 0, hsva[1], 1.0f, 0.01f, 0.05f);
                            hsva[2] = nk_propertyf(ctx, "#V:", 0, hsva[2], 1.0f, 0.01f, 0.05f);
                            tint = nk_hsva_colorfv(hsva);
                        }
                        nk_combo_end(ctx);

                        // assign the altered color 
                        *prop.tint_r = tint.r;
                        *prop.tint_g = tint.g;
                        *prop.tint_b = tint.b;
                    }

                    // spacing
                    nk_layout_row_static(ctx, 5, 10, 1);
                    nk_label(ctx, " ", NK_TEXT_ALIGN_CENTERED);

                    nk_layout_row_dynamic(ctx, 25, 2);
                    if (nk_button_label(ctx, "Vert Source Code"))
                    {
                        char* src = "\n\r\tNot Implemented lol\n...\r";
                        set_source_code_window(src);
                    }
                    if (nk_button_label(ctx, "Frag Source Code"))
                    {
                        char* src = "\n\r\tNot Implemented lol\n...\r";
                        set_source_code_window(src);
                    }

                    // spacing
                    nk_layout_row_static(ctx, 5, 10, 1);
                    nk_label(ctx, " ", NK_TEXT_ALIGN_CENTERED);

                    if (nk_tree_push(ctx, NK_TREE_NODE, "Textures", NK_MINIMIZED))
                    {
                        nk_layout_row_dynamic(ctx, 25, 2);
                        nk_label(ctx, "Is transparent: ", NK_TEXT_LEFT);
                        nk_label(ctx, *prop.is_transparent == BEE_TRUE ? "true" : "false", NK_TEXT_RIGHT);

                        nk_label(ctx, "Diffuse Texture: ", NK_TEXT_LEFT);
                        nk_label(ctx, prop.dif_tex_name, NK_TEXT_RIGHT);

                        struct nk_image img = nk_image_id(*prop.dif_tex_handle);
                        nk_layout_row_static(ctx, 150, 150, 1);
                        nk_image(ctx, img);

                        nk_layout_row_dynamic(ctx, 25, 2);
                        nk_label(ctx, "Specular Texture: ", NK_TEXT_LEFT);
                        nk_label(ctx, prop.spec_tex_name, NK_TEXT_RIGHT);
                        
                        img = nk_image_id(*prop.spec_tex_handle);
                        nk_layout_row_static(ctx, 150, 150, 1);
                        nk_image(ctx, img);

                        nk_tree_pop(ctx);
                    }            
            
                    nk_tree_pop(ctx);
                }
                
                if (*prop.has_light == BEE_TRUE && nk_tree_push(ctx, NK_TREE_TAB, "Light", NK_MINIMIZED))
                {
                    nk_layout_row_dynamic(ctx, 25, 2);
                    nk_label(ctx, "Type: ", NK_TEXT_LEFT);
                    nk_label(ctx, *prop._light_type == DIR_LIGHT ? "Dir. Light" : *prop._light_type == POINT_LIGHT ? "Point Light" : *prop._light_type == SPOT_LIGHT ? "Spot Light" : "Unknown", NK_TEXT_LEFT);


                    // ambient, diffuse, specular
                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_label(ctx, "Ambient", NK_TEXT_LEFT);
                    // ambient complex color combobox
                    struct nk_colorf ambient = { *prop.ambient_r, *prop.ambient_g, *prop.ambient_b };
                    if (nk_combo_begin_color(ctx, nk_rgb_cf(ambient), nk_vec2(200, 400)))
                    {
                        enum color_mode { COL_RGB, COL_HSV };
                        static int col_mode = COL_RGB;

                        nk_layout_row_dynamic(ctx, 120, 1);
                        ambient = nk_color_picker(ctx, ambient, NK_RGB);

                        nk_layout_row_dynamic(ctx, 25, 2);
                        col_mode = nk_option_label(ctx, "RGB", col_mode == COL_RGB) ? COL_RGB : col_mode;
                        col_mode = nk_option_label(ctx, "HSV", col_mode == COL_HSV) ? COL_HSV : col_mode;

                        nk_layout_row_dynamic(ctx, 25, 1);
                        if (col_mode == COL_RGB) {
                            ambient.r = nk_propertyf(ctx, "#R:", 0, ambient.r, 1.0f, 0.01f, 0.005f);
                            ambient.g = nk_propertyf(ctx, "#G:", 0, ambient.g, 1.0f, 0.01f, 0.005f);
                            ambient.b = nk_propertyf(ctx, "#B:", 0, ambient.b, 1.0f, 0.01f, 0.005f);
                        }
                        else {
                            float hsva[4];
                            nk_colorf_hsva_fv(hsva, ambient);
                            hsva[0] = nk_propertyf(ctx, "#H:", 0, hsva[0], 1.0f, 0.01f, 0.05f);
                            hsva[1] = nk_propertyf(ctx, "#S:", 0, hsva[1], 1.0f, 0.01f, 0.05f);
                            hsva[2] = nk_propertyf(ctx, "#V:", 0, hsva[2], 1.0f, 0.01f, 0.05f);
                            ambient = nk_hsva_colorfv(hsva);
                        }
                        nk_combo_end(ctx);

                        // assign the altered color 
                        *prop.ambient_r = ambient.r;
                        *prop.ambient_g = ambient.g;
                        *prop.ambient_b = ambient.b;
                    }
                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_label(ctx, "Diffuse", NK_TEXT_LEFT);
                    // ambient complex color combobox
                    struct nk_colorf diffuse = { *prop.diffuse_r, *prop.diffuse_g, *prop.diffuse_b };
                    if (nk_combo_begin_color(ctx, nk_rgb_cf(diffuse), nk_vec2(200, 400)))
                    {
                        enum color_mode { COL_RGB, COL_HSV };
                        static int col_mode = COL_RGB;

                        nk_layout_row_dynamic(ctx, 120, 1);
                        diffuse = nk_color_picker(ctx, diffuse, NK_RGB);

                        nk_layout_row_dynamic(ctx, 25, 2);
                        col_mode = nk_option_label(ctx, "RGB", col_mode == COL_RGB) ? COL_RGB : col_mode;
                        col_mode = nk_option_label(ctx, "HSV", col_mode == COL_HSV) ? COL_HSV : col_mode;

                        nk_layout_row_dynamic(ctx, 25, 1);
                        if (col_mode == COL_RGB) {
                            diffuse.r = nk_propertyf(ctx, "#R:", 0, diffuse.r, 1.0f, 0.01f, 0.005f);
                            diffuse.g = nk_propertyf(ctx, "#G:", 0, diffuse.g, 1.0f, 0.01f, 0.005f);
                            diffuse.b = nk_propertyf(ctx, "#B:", 0, diffuse.b, 1.0f, 0.01f, 0.005f);
                        }
                        else {
                            float hsva[4];
                            nk_colorf_hsva_fv(hsva, diffuse);
                            hsva[0] = nk_propertyf(ctx, "#H:", 0, hsva[0], 1.0f, 0.01f, 0.05f);
                            hsva[1] = nk_propertyf(ctx, "#S:", 0, hsva[1], 1.0f, 0.01f, 0.05f);
                            hsva[2] = nk_propertyf(ctx, "#V:", 0, hsva[2], 1.0f, 0.01f, 0.05f);
                            diffuse = nk_hsva_colorfv(hsva);
                        }
                        nk_combo_end(ctx);

                        // assign the altered color 
                        *prop.diffuse_r = diffuse.r;
                        *prop.diffuse_g = diffuse.g;
                        *prop.diffuse_b = diffuse.b;
                    }
                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_label(ctx, "Specular", NK_TEXT_LEFT);
                    // ambient complex color combobox
                    struct nk_colorf specular = { *prop.specular_r, *prop.specular_g, *prop.specular_b };
                    if (nk_combo_begin_color(ctx, nk_rgb_cf(specular), nk_vec2(200, 400)))
                    {
                        enum color_mode { COL_RGB, COL_HSV };
                        static int col_mode = COL_RGB;

                        nk_layout_row_dynamic(ctx, 120, 1);
                        specular = nk_color_picker(ctx, specular, NK_RGB);

                        nk_layout_row_dynamic(ctx, 25, 2);
                        col_mode = nk_option_label(ctx, "RGB", col_mode == COL_RGB) ? COL_RGB : col_mode;
                        col_mode = nk_option_label(ctx, "HSV", col_mode == COL_HSV) ? COL_HSV : col_mode;

                        nk_layout_row_dynamic(ctx, 25, 1);
                        if (col_mode == COL_RGB) {
                            diffuse.r = nk_propertyf(ctx, "#R:", 0, specular.r, 1.0f, 0.01f, 0.005f);
                            diffuse.g = nk_propertyf(ctx, "#G:", 0, specular.g, 1.0f, 0.01f, 0.005f);
                            diffuse.b = nk_propertyf(ctx, "#B:", 0, specular.b, 1.0f, 0.01f, 0.005f);
                        }
                        else {
                            float hsva[4];
                            nk_colorf_hsva_fv(hsva, specular);
                            hsva[0] = nk_propertyf(ctx, "#H:", 0, hsva[0], 1.0f, 0.01f, 0.05f);
                            hsva[1] = nk_propertyf(ctx, "#S:", 0, hsva[1], 1.0f, 0.01f, 0.05f);
                            hsva[2] = nk_propertyf(ctx, "#V:", 0, hsva[2], 1.0f, 0.01f, 0.05f);
                            specular = nk_hsva_colorfv(hsva);
                        }
                        nk_combo_end(ctx);

                        // assign the altered color 
                        *prop.specular_r = specular.r;
                        *prop.specular_g = specular.g;
                        *prop.specular_b = specular.b;
                    }

                    // direction
                    if (*prop._light_type == DIR_LIGHT || *prop._light_type == SPOT_LIGHT)
                    {
                        if (nk_tree_push(ctx, NK_TREE_NODE, "Direction", NK_MINIMIZED))
                        {
                            nk_layout_row_static(ctx, 30, 80, 2);
                            static int option;
                            option = nk_option_label(ctx, "global", option == 0) ? 0 : option;
                            option = nk_option_label(ctx, "local", option == 1) ? 1 : option;

                            nk_layout_row_dynamic(ctx, 20, 1);
                            nk_property_float(ctx, "Dir X:", -1024.0f, prop.direction_x, 1024.0f, 0.1f, 0.2f);

                            nk_property_float(ctx, "Dir Y:", -1024.0f, prop.direction_y, 1024.0f, 0.1f, 0.2f);

                            nk_property_float(ctx, "Dir Z:", -1024.0f, prop.direction_z, 1024.0f, 0.1f, 0.2f);

                            nk_tree_pop(ctx);
                        }
                    }

                    // spacing
                    nk_layout_row_static(ctx, 5, 20, 1);
                    nk_label(ctx, " ", NK_TEXT_ALIGN_CENTERED);

                    nk_layout_row_dynamic(ctx, 25, 1);
                    // constant, linear, quadratic
                    if (*prop._light_type == POINT_LIGHT || *prop._light_type == SPOT_LIGHT)
                    {
                        nk_property_float(ctx, "constant",  0.0f,      prop.constant,  2.0f, 0.1f, 0.2f);
                        nk_property_float(ctx, "linear",    0.0014f,   prop.linear,    0.7f, 0.1f, 0.0005f);
                        nk_property_float(ctx, "quadratic", 0.000007f, prop.quadratic, 1.8f, 0.1f, 0.0005f);
                    }

                    // cut_off
                    if (*prop._light_type == SPOT_LIGHT)
                    {
                        nk_property_float(ctx, "cut off", 1.0f, prop.cut_off, 90.0f, 0.1f, 0.2f);
                        nk_property_float(ctx, "outer cut off", *prop.cut_off, prop.outer_cut_off, 90.0f, 0.1f, 0.2f);
                    }

                    nk_tree_pop(ctx);
                }


                // scritps always the last element
                sprintf(buffer, "Scripts - %d", *prop.scripts_len);
                if (*prop.scripts_len > 0 && nk_tree_push(ctx, NK_TREE_TAB, *prop.scripts_len == 1 ? "Script" : buffer , NK_MINIMIZED))
                {
                    for (int i = 0; i < *prop.scripts_len; ++i)
                    {
                        static int popup_active;

                        if (prop.scripts[i].path_valid == BEE_FALSE)
                        {
                            nk_layout_row(ctx, NK_STATIC, 25, 1, ratio);
                            nk_label_colored(ctx, "path not valid", NK_TEXT_LEFT, red);
                        }
                        else if (prop.scripts[i].has_error == BEE_TRUE)
                        {
                            nk_layout_row(ctx, NK_STATIC, 25, 1, ratio);
                            nk_label_colored(ctx, "has error", NK_TEXT_LEFT, red);
                        }

                        char* name = str_find_last_of(prop.scripts[i].path, "\\");
                        if (name == NULL)
                        {
                            name = str_find_last_of(prop.scripts[i].path, "/");
                        }
                        assert(name != NULL);
                        name = str_trunc(name, 1); // cut off the last "\"
                        assert(name != NULL);

                        nk_layout_row_dynamic(ctx, 25, 2);
                        nk_label(ctx, "Name", NK_TEXT_LEFT);
                        nk_label(ctx, name, NK_TEXT_RIGHT);


                        if (prop.scripts[i].path_valid == BEE_TRUE)
                        {
                            nk_layout_row_dynamic(ctx, 25, 1);
                            if (nk_button_label(ctx, "Source Code"))
                            {
                                char* src = read_text_file(prop.scripts[i].path);
                                set_source_code_window(src);
                            }
                        }

                        if (prop.scripts[i].path_valid == BEE_TRUE && prop.scripts[i].has_error == BEE_FALSE)
                        {
                            nk_layout_row_dynamic(ctx, 25, 1);
                            if (nk_button_label(ctx, prop.scripts[i].active == BEE_TRUE ? "Pause" : "Continue"))
                            {
                                prop.scripts[i].active = !prop.scripts[i].active;
                            }
                        }

                        nk_layout_row_dynamic(ctx, 25, 1);
                        if (nk_button_label(ctx, "Remove"))
                        {
                            entity_remove_script(selected_entity, i);
                        }

                    }
                    nk_tree_pop(ctx);
                    
                }
                
                // spacing
                nk_layout_row_static(ctx, 5, 10, 1);
                nk_label(ctx, " ", NK_TEXT_ALIGN_CENTERED);

                static int attach_popup_active;
                nk_layout_row_dynamic(ctx, 25, 1);
                if (nk_button_label(ctx, "Attach Script"))
                {
                    attach_popup_active = 1;
                }

                if (attach_popup_active)
                {
                    // static struct nk_rect s = ;
                    if (nk_popup_begin(ctx, NK_POPUP_DYNAMIC, "Attach Script", NK_WINDOW_CLOSABLE | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE, (struct nk_rect) { 600, 200, 500, 600 }))
                    {
                        static char text[64];
                        static int text_len;
                        char path_buffer[128];

                        nk_layout_row_dynamic(ctx, 25, 2);
                        nk_label(ctx, "Name", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, &text, &text_len, 64, nk_filter_default);

                        sprintf(path_buffer, "C:\\Workspace\\C\\BeeEngine\\assets\\gravity\\%s.gravity\0", strlen(text) == 0 ? "[NAME]" : text);
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_label(ctx, "Path", NK_TEXT_LEFT);
                        nk_label(ctx, path_buffer, NK_TEXT_LEFT);

                        nk_layout_row_dynamic(ctx, 25, 2);
                        if (nk_button_label(ctx, "Attach"))
                        {
                            // @BUGG: doesnt work
                            printf("path: %s\n", &path_buffer[0]);

                            char* buf = NULL;
                            buf = malloc(strlen(path_buffer));
                            assert(buf != NULL);
                            memcpy(buf, path_buffer, strlen(path_buffer));
                            //buf[strlen(path_buffer)- 1] = '\0';
                            printf("copied path: %s\n", buf);

                            entity_add_script(selected_entity, buf);

                            // free(buf);
                            attach_popup_active = 0;
                            nk_popup_close(ctx);
                        }
                        if (nk_button_label(ctx, "Close"))
                        {
                            attach_popup_active = 0;
                            nk_popup_close(ctx);
                        }
                        nk_popup_end(ctx);
                    }
                    else attach_popup_active = nk_false;
                }
}
            nk_tree_pop(ctx);
        }

    }
    nk_end(ctx);
}

void console_window()
{
    if (nk_begin(ctx, "Console", nk_rect(1600, 700, 300, 300),
        NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
        NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
    {
        static const float ratio[] = { 120, 150 };
        static char text[9][64];
        static int text_len[9];
        nk_flags active;

        nk_label(ctx, "Console:", NK_TEXT_LEFT);
        nk_layout_row_static(ctx, 180, 278, 1);
        nk_edit_string(ctx, NK_EDIT_BOX, box_buffer, &box_len, 512, nk_filter_default);
        
        nk_layout_row_dynamic(ctx, 25, 2);
        active = nk_edit_string(ctx, NK_EDIT_FIELD | NK_EDIT_SIG_ENTER, text[7], &text_len[7], 64, nk_filter_ascii);
        if (nk_button_label(ctx, "Submit") ||
            (active & NK_EDIT_COMMITED))
        {
            text[7][text_len[7]] = '\n';
            text_len[7]++;
            memcpy(&box_buffer[box_len], &text[7], (nk_size)text_len[7]);
            box_len += text_len[7];
            text_len[7] = 0;
        }
    }
    nk_end(ctx);
}

void asset_browser_window()
{
    int x = 1600;
    int y = 700;
    int w = 300;
    int h = 300;
    if (nk_begin(ctx, "Asset Browser", nk_rect(x, y, w, h),
        NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
        NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
    {
       //  nk_layout_row_dynamic(ctx, (f32)h - 25.0f, 2);
        if (nk_tree_push(ctx, NK_TREE_TAB, "Images", NK_MINIMIZED))
        {
            int i = 0;
            static nk_bool selected[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
            nk_layout_row_static(ctx, 50, 50, 4);

            if (nk_selectable_label(ctx, "Image", NK_TEXT_ALIGN_BOTTOM, &selected[i])) 
            {
                selected[i] ^= 1;
            }
            i++;
            if (nk_selectable_label(ctx, "Image", NK_TEXT_ALIGN_BOTTOM, &selected[i]))
            {
                selected[i] ^= 1;
            }
            i++;
            if (nk_selectable_label(ctx, "Image", NK_TEXT_ALIGN_BOTTOM, &selected[i]))
            {
                selected[i] ^= 1;
            }
            i++;
            if (nk_selectable_label(ctx, "Image", NK_TEXT_ALIGN_BOTTOM, &selected[i]))
            {
                selected[i] ^= 1;
            }
            i++;

            // for (i = 0; i < 16; ++i) {
            //     if (nk_selectable_label(ctx, "Z", NK_TEXT_CENTERED, &selected[i])) {
            //         int x = (i % 4), y = i / 4;
            //         if (x > 0) selected[i - 1] ^= 1;
            //         if (x < 3) selected[i + 1] ^= 1;
            //         if (y > 0) selected[i - 4] ^= 1;
            //         if (y < 3) selected[i + 4] ^= 1;
            //     }
            // }
            nk_tree_pop(ctx);
        }
        if (nk_tree_push(ctx, NK_TREE_TAB, "Meshes", NK_MINIMIZED))
        {
            int i = 0;
            static nk_bool selected[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
            nk_layout_row_static(ctx, 50, 50, 4);

            for (i = 0; i < 4; ++i) {
                if (nk_selectable_label(ctx, "Mesh", NK_TEXT_CENTERED, &selected[i])) {
                    int x = (i % 4), y = i / 4;
                    if (x > 0) selected[i - 1] ^= 1;
                    if (x < 3) selected[i + 1] ^= 1;
                    if (y > 0) selected[i - 4] ^= 1;
                    if (y < 3) selected[i + 4] ^= 1;
                }
            }
            nk_tree_pop(ctx);

        }

    }
    nk_end(ctx);
}

void error_popup_window()
{
    int x = 800;
    int y = 200;
    int w = 300;
    int h = 300;
    if (nk_begin(ctx, "Gravity Error", nk_rect(x, y, w, h),
        NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
        NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
    {
        nk_layout_row_dynamic(ctx, 150, 1);
        nk_label_wrap(ctx, error_msg, NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 25, 1);
        if (nk_button_label(ctx, "Close"))
        {
            error_popup_act = nk_false;
            free(error_msg);
        }
    }
    nk_end(ctx);
}

void source_code_window()
{

    int x = 400;
    int y = 10;
    int w = 600;
    int h = 950;
    if (nk_begin(ctx, "Source Code", nk_rect(x, y, w, h),
        NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
        NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
    {
        
        assert(source_code != NULL);

        if (source_code != NULL)
        {
            char buffer[512];
            int buffer_pos = 0;
            int line_index = 0;
            nk_layout_row_dynamic(ctx, 15, 1);
            char* src = NULL;
            src = calloc(strlen(source_code), sizeof(char));
            strcpy(src, source_code);
            src[strlen(source_code) - 1] = '\0';
            // printf("source code copy: \n%s\n", src);
            for ( int c = 0; c < strlen(source_code); ++c)
            {
                /*
                if (src[c] == '\n')
                {
                    src[c] = ' ';

                    // char* line = str_trunc(src, -1 * (strlen(source_code) - ( c - line_index)));
                    str_trunc(src, 1 * (strlen(source_code) - ( c - line_index)));
                    if (line_index != 0)
                    {
                        // line = str_trunc(line, line_index);
                        src = str_trunc(src, line_index);
                    }
                    // printf("line: %s\n", line);
                    // assert(line != NULL);
                    line_index = c;
                    // nk_label(ctx, line, NK_TEXT_LEFT);
                    nk_label(ctx, src, NK_TEXT_LEFT);
                }
                else if (isspace(src[c]))
                {
                    src[c] = ' ';
                }
                // printf("finished source code\n");
                // printf("source code: \n%s\n", source_code);
                */

                if ( src[c] == '\n' || src[c] == '\0' )
                {
                    buffer[buffer_pos] = '\0';
                    // new line
                    nk_label(ctx, buffer, NK_TEXT_LEFT);
                    line_index = c;
                    buffer_pos = 0;
                    for (int i = 0; i < 512; ++i) { buffer[i] = ' '; }
                }
                else if( isspace(src[c]) )
                {
                    // printf("whitesp. char: %c", src[c]);
                    if (src[c] == '\t' || src[c] == '\r')
                    {
                        buffer[buffer_pos] = ' ';
                        buffer_pos++;
                        buffer[buffer_pos] = ' ';
                        buffer_pos++;
                        buffer[buffer_pos] = ' ';
                        buffer_pos++;
                        buffer[buffer_pos] = ' ';
                        buffer_pos++;
                    }
                    else
                    {
                        buffer[buffer_pos] = ' ';
                        buffer_pos++;
                    }
                }
                else
                {
                    buffer[buffer_pos] = src[c];
                    buffer_pos++;
                }

            }
            // src = realloc(src, 0);
            // free(src);
        }

        nk_layout_row_dynamic(ctx, 25, 1);
        if ( nk_button_label(ctx, "Close") )
        {
            source_code_window_act = nk_false;
            // free(source_code);
        }
    }
    nk_end(ctx);

}


void set_error_popup(char* msg)
{
    error_popup_act = nk_true;
    error_msg = calloc(strlen(msg), sizeof(char));
    strcpy(error_msg, msg);
    error_msg[strlen(msg) - 1] = '\0';
}
void set_source_code_window(char* src)
{
    source_code_window_act = nk_true;
    source_code = calloc(strlen(src), sizeof(char));
    strcpy(source_code, src);
    source_code[strlen(src) - 1] = '\0';
}
static void set_style(struct nk_context* ctx, enum theme theme)
{
    struct nk_color table[NK_COLOR_COUNT];
    if (theme == THEME_WHITE) {
        table[NK_COLOR_TEXT] = nk_rgba(70, 70, 70, 255);
        table[NK_COLOR_WINDOW] = nk_rgba(175, 175, 175, 255);
        table[NK_COLOR_HEADER] = nk_rgba(175, 175, 175, 255);
        table[NK_COLOR_BORDER] = nk_rgba(0, 0, 0, 255);
        table[NK_COLOR_BUTTON] = nk_rgba(185, 185, 185, 255);
        table[NK_COLOR_BUTTON_HOVER] = nk_rgba(170, 170, 170, 255);
        table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(160, 160, 160, 255);
        table[NK_COLOR_TOGGLE] = nk_rgba(150, 150, 150, 255);
        table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(120, 120, 120, 255);
        table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(175, 175, 175, 255);
        table[NK_COLOR_SELECT] = nk_rgba(190, 190, 190, 255);
        table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(175, 175, 175, 255);
        table[NK_COLOR_SLIDER] = nk_rgba(190, 190, 190, 255);
        table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(80, 80, 80, 255);
        table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(70, 70, 70, 255);
        table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(60, 60, 60, 255);
        table[NK_COLOR_PROPERTY] = nk_rgba(175, 175, 175, 255);
        table[NK_COLOR_EDIT] = nk_rgba(150, 150, 150, 255);
        table[NK_COLOR_EDIT_CURSOR] = nk_rgba(0, 0, 0, 255);
        table[NK_COLOR_COMBO] = nk_rgba(175, 175, 175, 255);
        table[NK_COLOR_CHART] = nk_rgba(160, 160, 160, 255);
        table[NK_COLOR_CHART_COLOR] = nk_rgba(45, 45, 45, 255);
        table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
        table[NK_COLOR_SCROLLBAR] = nk_rgba(180, 180, 180, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(140, 140, 140, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(150, 150, 150, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(160, 160, 160, 255);
        table[NK_COLOR_TAB_HEADER] = nk_rgba(180, 180, 180, 255);
        nk_style_from_table(ctx, table);
    }
    else if (theme == THEME_RED) {
        table[NK_COLOR_TEXT] = nk_rgba(190, 190, 190, 255);
        table[NK_COLOR_WINDOW] = nk_rgba(30, 33, 40, 255);  // a: 215
        table[NK_COLOR_HEADER] = nk_rgba(181, 45, 69, 255); // a: 220 
        table[NK_COLOR_BORDER] = nk_rgba(51, 55, 67, 255);
        table[NK_COLOR_BUTTON] = nk_rgba(181, 45, 69, 255);
        table[NK_COLOR_BUTTON_HOVER] = nk_rgba(190, 50, 70, 255);
        table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(195, 55, 75, 255);
        table[NK_COLOR_TOGGLE] = nk_rgba(51, 55, 67, 255);
        table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 60, 60, 255);
        table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(181, 45, 69, 255);
        table[NK_COLOR_SELECT] = nk_rgba(51, 55, 67, 255);
        table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(181, 45, 69, 255);
        table[NK_COLOR_SLIDER] = nk_rgba(51, 55, 67, 255);
        table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(181, 45, 69, 255);
        table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(186, 50, 74, 255);
        table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(191, 55, 79, 255);
        table[NK_COLOR_PROPERTY] = nk_rgba(51, 55, 67, 255);
        table[NK_COLOR_EDIT] = nk_rgba(51, 55, 67, 225);
        table[NK_COLOR_EDIT_CURSOR] = nk_rgba(190, 190, 190, 255);
        table[NK_COLOR_COMBO] = nk_rgba(51, 55, 67, 255);
        table[NK_COLOR_CHART] = nk_rgba(51, 55, 67, 255);
        table[NK_COLOR_CHART_COLOR] = nk_rgba(170, 40, 60, 255);
        table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
        table[NK_COLOR_SCROLLBAR] = nk_rgba(30, 33, 40, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(64, 84, 95, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(70, 90, 100, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(75, 95, 105, 255);
        table[NK_COLOR_TAB_HEADER] = nk_rgba(181, 45, 69, 255); // a: 220
        nk_style_from_table(ctx, table);
    }
    else if (theme == THEME_BLUE) {
        table[NK_COLOR_TEXT] = nk_rgba(20, 20, 20, 255);
        table[NK_COLOR_WINDOW] = nk_rgba(202, 212, 214, 215);
        table[NK_COLOR_HEADER] = nk_rgba(137, 182, 224, 220);
        table[NK_COLOR_BORDER] = nk_rgba(140, 159, 173, 255);
        table[NK_COLOR_BUTTON] = nk_rgba(137, 182, 224, 255);
        table[NK_COLOR_BUTTON_HOVER] = nk_rgba(142, 187, 229, 255);
        table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(147, 192, 234, 255);
        table[NK_COLOR_TOGGLE] = nk_rgba(177, 210, 210, 255);
        table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(182, 215, 215, 255);
        table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(137, 182, 224, 255);
        table[NK_COLOR_SELECT] = nk_rgba(177, 210, 210, 255);
        table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(137, 182, 224, 255);
        table[NK_COLOR_SLIDER] = nk_rgba(177, 210, 210, 255);
        table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(137, 182, 224, 245);
        table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(142, 188, 229, 255);
        table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(147, 193, 234, 255);
        table[NK_COLOR_PROPERTY] = nk_rgba(210, 210, 210, 255);
        table[NK_COLOR_EDIT] = nk_rgba(210, 210, 210, 225);
        table[NK_COLOR_EDIT_CURSOR] = nk_rgba(20, 20, 20, 255);
        table[NK_COLOR_COMBO] = nk_rgba(210, 210, 210, 255);
        table[NK_COLOR_CHART] = nk_rgba(210, 210, 210, 255);
        table[NK_COLOR_CHART_COLOR] = nk_rgba(137, 182, 224, 255);
        table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
        table[NK_COLOR_SCROLLBAR] = nk_rgba(190, 200, 200, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(64, 84, 95, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(70, 90, 100, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(75, 95, 105, 255);
        table[NK_COLOR_TAB_HEADER] = nk_rgba(156, 193, 220, 255);
        nk_style_from_table(ctx, table);
    }
    else if (theme == THEME_DARK) {
        table[NK_COLOR_TEXT] = nk_rgba(210, 210, 210, 255);
        table[NK_COLOR_WINDOW] = nk_rgba(57, 67, 71, 215);
        table[NK_COLOR_HEADER] = nk_rgba(51, 51, 56, 220);
        table[NK_COLOR_BORDER] = nk_rgba(46, 46, 46, 255);
        table[NK_COLOR_BUTTON] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_BUTTON_HOVER] = nk_rgba(58, 93, 121, 255);
        table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(63, 98, 126, 255);
        table[NK_COLOR_TOGGLE] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 53, 56, 255);
        table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SELECT] = nk_rgba(57, 67, 61, 255);
        table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SLIDER] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(48, 83, 111, 245);
        table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
        table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
        table[NK_COLOR_PROPERTY] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_EDIT] = nk_rgba(50, 58, 61, 225);
        table[NK_COLOR_EDIT_CURSOR] = nk_rgba(210, 210, 210, 255);
        table[NK_COLOR_COMBO] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_CHART] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_CHART_COLOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
        table[NK_COLOR_SCROLLBAR] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
        table[NK_COLOR_TAB_HEADER] = nk_rgba(48, 83, 111, 255);
        nk_style_from_table(ctx, table);
    }
    else {
        nk_style_default(ctx);
    }
}
