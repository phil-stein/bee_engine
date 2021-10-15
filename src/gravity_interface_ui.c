#include "gravity_interface_ui.h"


#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_STANDARD_IO
#ifndef EDITOR_ACT // if editor is active is defined in editor_ui.c
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#endif
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear/nuklear.h"
#include "nuklear/nuklear_glfw_gl3.h"

#include "stb/stb_ds.h"

#include "asset_manager.h"
#include "renderer.h"
#include "window.h"


#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

enum theme { THEME_BLACK, THEME_WHITE, THEME_RED, THEME_BLUE, THEME_DARK };

// ---- vars ----
struct nk_color g_red = { 255, 0, 0, 255 };

// ---- demo --- 
struct nk_glfw g_glfw = { 0 };
struct nk_context* g_ctx;


nk_flags g_window_flags = NK_WINDOW_BORDER | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE;


void gravity_ui_init()
{
    // ---- nuklear ----

    g_ctx = nk_glfw3_init(&g_glfw, get_window(), NK_GLFW3_INSTALL_CALLBACKS);
    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    {struct nk_font_atlas* atlas;
        nk_glfw3_font_stash_begin(&g_glfw, &atlas);

        // struct nk_font* roboto = nk_font_atlas_add_from_file(atlas, "C:\\Workspace\\C\\BeeEngine\\assets\\fonts\\Roboto-Regular.ttf", 16/*14*/, 0);
        // struct nk_font* roboto_bold = nk_font_atlas_add_from_file(atlas, "C:\\Workspace\\C\\BeeEngine\\assets\\fonts\\Roboto-Bold.ttf", 16/*14*/, 0);
        struct nk_font* droid = nk_font_atlas_add_from_file(atlas, "C:\\Workspace\\C\\BeeEngine\\assets\\fonts\\DroidSans.ttf", 18, 0);
        // struct nk_font *future = nk_font_atlas_add_from_file(atlas, "C:\\Workspace\\C\\BeeEngine\\assets\\fonts\\kenvector_future_thin.ttf", 13, 0);
        // struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "C:\\Workspace\\C\\BeeEngine\\assets\\fonts\\ProggyClean.ttf", 12, 0);
        // struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "C:\\Workspace\\C\\BeeEngine\\assets\\fonts\\ProggyTiny.ttf", 10, 0);
        // struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "C:\\Workspace\\C\\BeeEngine\\assets\\fonts\\Cousine-Regular.ttf", 13, 0);

        nk_glfw3_font_stash_end(&g_glfw);
        /*nk_style_load_all_cursors(ctx, atlas->cursors);*/

        nk_style_set_font(g_ctx, &droid->handle);
    }

    // -----------------

    // set default theme
    gravity_ui_set_style(g_ctx, THEME_BLACK);

}

void gravity_ui_clear()
{
    nk_clear(g_ctx);
}

void setup_ui_class(gravity_vm* vm)
{
    // create a new Foo class
    gravity_class_t* c = gravity_class_new_pair(vm, "UI", NULL, 0, 0);

    // allocate and bind bar closure to the newly created class
    gravity_class_bind(c, "begin", NEW_CLOSURE_VALUE(gravity_begin));
    gravity_class_bind(c, "end", NEW_CLOSURE_VALUE(gravity_end));
    gravity_class_bind(c, "window_begin", NEW_CLOSURE_VALUE(gravity_window_begin));
    gravity_class_bind(c, "window_end", NEW_CLOSURE_VALUE(gravity_window_end));
    gravity_class_bind(c, "layout", NEW_CLOSURE_VALUE(gravity_layout_row));
    gravity_class_bind(c, "layout_static", NEW_CLOSURE_VALUE(gravity_layout_row_static));
    gravity_class_bind(c, "text", NEW_CLOSURE_VALUE(gravity_label));
    gravity_class_bind(c, "button", NEW_CLOSURE_VALUE(gravity_button));
    gravity_class_bind(c, "checkbox", NEW_CLOSURE_VALUE(gravity_checkbox));
    gravity_class_bind(c, "image", NEW_CLOSURE_VALUE(gravity_image));

    // register class c inside VM
    gravity_vm_setvalue(vm, "UI", VALUE_FROM_OBJECT(c));
}

static bee_bool gravity_begin(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[UI.begin()] Wrong amount of arguments, 0 arguments are needed."); return; }
    int window_w, window_h;
    // get window size
    get_window_size(&window_w, &window_h);

    // draw ui
    nk_glfw3_new_frame(&g_glfw);
}
static bee_bool gravity_end(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[UI.end()] Wrong amount of arguments, 0 arguments are needed."); return; }
 
    nk_glfw3_render(&g_glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}


static bee_bool gravity_window_begin(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 6) { throw_error("[UI.window_begin(string, float, float, float, float)] Wrong amount of arguments, 5 arguments are needed."); return; }
    gravity_value_t v1 = GET_VALUE(1);
    gravity_value_t v2 = GET_VALUE(2);
    gravity_value_t v3 = GET_VALUE(3);
    gravity_value_t v4 = GET_VALUE(4);
    gravity_value_t v5 = GET_VALUE(5);
    if (VALUE_ISA_STRING(v1) == BEE_FALSE   ||
        (VALUE_ISA_FLOAT(v2) == BEE_FALSE && VALUE_ISA_INT(v2) == BEE_FALSE) &&
        (VALUE_ISA_FLOAT(v3) == BEE_FALSE && VALUE_ISA_INT(v3) == BEE_FALSE) &&
        (VALUE_ISA_FLOAT(v4) == BEE_FALSE && VALUE_ISA_INT(v4) == BEE_FALSE) &&
        (VALUE_ISA_FLOAT(v5) == BEE_FALSE && VALUE_ISA_INT(v5) == BEE_FALSE))
    {
        throw_error("[UI.window_begin(string, float, float, float, float)] Wrong argument types."); return;
    }

    f32 x, y, w, h;
    char* title = VALUE_AS_CSTRING(v1);
    if (VALUE_ISA_FLOAT(v2)) { x = VALUE_AS_FLOAT(v2); }
    else { x = VALUE_AS_INT(v2); }
    if (VALUE_ISA_FLOAT(v3)) { y = VALUE_AS_FLOAT(v3); }
    else { y = VALUE_AS_INT(v3); }
    if (VALUE_ISA_FLOAT(v4)) { w = VALUE_AS_FLOAT(v4); }
    else { w = VALUE_AS_INT(v4); }
    if (VALUE_ISA_FLOAT(v5)) { h = VALUE_AS_FLOAT(v5); }
    else { h = VALUE_AS_INT(v5); }


    int width, height;
    get_window_size(&width, &height);

    // less height because the window bar on top and below
    const f32 w_ratio = w / 1920.0f;
    const f32 h_ratio = h / 1020.0f;
    const f32 x_ratio = x / 1920.0f;
    const f32 y_ratio = y / 1020.0f;

    bee_bool b = nk_begin(g_ctx, title, nk_rect(x_ratio * width, y_ratio * height, w_ratio * width, h_ratio * height), g_window_flags);
    RETURN_VALUE(VALUE_FROM_INT(b), rindex);
}

static bee_bool gravity_window_end(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[UI.window_end()] Wrong amount of arguments, 0 arguments are needed."); return; }

    nk_end(g_ctx);
}

static bee_bool gravity_layout_row(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 3) { throw_error("[UI.layout(int, int)] Wrong amount of arguments, 2 arguments are needed."); return; }
    gravity_value_t v1 = GET_VALUE(1);
    gravity_value_t v2 = GET_VALUE(2);
    if ((VALUE_ISA_FLOAT(v1) == BEE_FALSE && VALUE_ISA_INT(v1) == BEE_FALSE) ||
        (VALUE_ISA_FLOAT(v2) == BEE_FALSE && VALUE_ISA_INT(v2) == BEE_FALSE))
    {
        throw_error("[UI.layout(int, int)] Wrong argument types."); return;
    }

    f32 h, w, cols;
    if (VALUE_ISA_FLOAT(v1)) { h = VALUE_AS_FLOAT(v1); }
    else { h = VALUE_AS_INT(v1); }
    if (VALUE_ISA_FLOAT(v2)) { cols = VALUE_AS_FLOAT(v2); }
    else { cols = VALUE_AS_INT(v2); }
    nk_layout_row_dynamic(g_ctx, (int)h, (int)cols);
}

static bee_bool gravity_layout_row_static(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 4) { throw_error("[UI.layout_static(int, int, int)] Wrong amount of arguments, 3 arguments are needed."); return; }
    gravity_value_t v1 = GET_VALUE(1);
    gravity_value_t v2 = GET_VALUE(2);
    gravity_value_t v3 = GET_VALUE(3);
    if ((VALUE_ISA_FLOAT(v1) == BEE_FALSE && VALUE_ISA_INT(v1) == BEE_FALSE) ||
        (VALUE_ISA_FLOAT(v2) == BEE_FALSE && VALUE_ISA_INT(v2) == BEE_FALSE) ||
        (VALUE_ISA_FLOAT(v3) == BEE_FALSE && VALUE_ISA_INT(v3) == BEE_FALSE))
    {
        throw_error("[UI.layout_static(int, int, int)]  Wrong argument types."); return;
    }

    f32 h, w, cols;
    if (VALUE_ISA_FLOAT(v1)) { h = VALUE_AS_FLOAT(v1); }
    else { h = VALUE_AS_INT(v1); }
    if (VALUE_ISA_FLOAT(v2)) { w = VALUE_AS_FLOAT(v2); }
    else { w = VALUE_AS_INT(v2); }
    if (VALUE_ISA_FLOAT(v3)) { cols = VALUE_AS_FLOAT(v3); }
    else { cols = VALUE_AS_INT(v3); }
    nk_layout_row_static(g_ctx, (int)h, (int)w, (int)cols);
}

static bee_bool gravity_label(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 2) { throw_error("[UI.text(string)] Wrong amount of arguments, 1 argument is needed."); return; }
    gravity_value_t v1 = GET_VALUE(1);
    if (VALUE_ISA_STRING(v1) == BEE_FALSE)
    {
        throw_error("[UI.text(string)] Wrong argument types."); return;
    }

    char* str = VALUE_AS_CSTRING(v1);
    nk_label(g_ctx, str, NK_TEXT_LEFT);
}

static bee_bool gravity_button(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 2) { throw_error("[UI.button(string)] Wrong amount of arguments, 1 argument is needed."); return; }
    gravity_value_t v1 = GET_VALUE(1);
    if (VALUE_ISA_STRING(v1) == BEE_FALSE)
    {
        throw_error("[UI.button(string)] Wrong argument types."); return;
    }

    char* str = VALUE_AS_CSTRING(v1);
    bee_bool b = nk_button_label(g_ctx, str);
    RETURN_VALUE(VALUE_FROM_INT(b), rindex);
}

static bee_bool gravity_checkbox(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 3) { throw_error("[UI.checkbox(string, bool)] Wrong amount of arguments, 2 arguments are needed."); return; }
    gravity_value_t v1 = GET_VALUE(1);
    gravity_value_t v2 = GET_VALUE(2);
    if (VALUE_ISA_STRING(v1) == BEE_FALSE || 
       (VALUE_ISA_BOOL(v2) != BEE_TRUE && VALUE_ISA_INT(v2) != BEE_TRUE))
    {
        assert(0);
        throw_error("[UI.checkbox(string, bool)] Wrong argument types."); return;
    }
    char* str    = VALUE_AS_CSTRING(v1);
    bee_bool arg = VALUE_AS_BOOL(v2);

    nk_checkbox_label(g_ctx, str, &arg);
    RETURN_VALUE(VALUE_FROM_INT(arg), rindex);
}

static bee_bool gravity_image(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 2) { throw_error("[UI.image(string)] Wrong amount of arguments, 1 argument is needed."); return; }
    gravity_value_t v1 = GET_VALUE(1);
    if (VALUE_ISA_STRING(v1) == BEE_FALSE)
    {
        throw_error("[UI.image(string)] Wrong argument types."); return;
    }

    char* str = VALUE_AS_CSTRING(v1);
    
#ifdef EDITOR_ACT
    struct nk_image img = nk_image_id(get_texture(str).icon_handle);
#else
    struct nk_image img = nk_image_id(get_texture(str).handle);
#endif
    nk_image(g_ctx, img);
}


void gravity_ui_set_style(struct nk_context* ctx, enum theme theme)
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
