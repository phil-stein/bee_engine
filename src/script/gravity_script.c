#include "gravity_script.h"

#include "CGLM/cglm.h"

#include "gravity_interface_ui.h"
#include "gravity_interface.h"
#include "files/file_handler.h"
#include "types/camera.h"
#include "game_time.h"
#include "editor_ui.h"
#include "str_util.h"
#include "renderer.h"
#include "input.h"


bee_bool cur_script_error = BEE_FALSE;
int script_id = 0;
// gravity_vm* vm;
// gravity_closure_t* closure;

// a very simple report error callback function
void report_error(gravity_vm* vm, error_type_t error_type, const char* message, error_desc_t error_desc, void* xdata) 
{
    char buf[248];
    
    gravity_script* cur_script = get_cur_script();

    // printf("GRAVITY_ERROR: %s, file: %s, line: %d, column: %d\n", message, cur_script->name, error_desc.lineno, error_desc.colno);
    // sprintf(buf, "%s, file: %s, line: %d, column: %d\n", message, cur_script->name, error_desc.lineno, error_desc.colno);
    sprintf_s(buf, 248, "%s, file: %s, line: %d", message, cur_script->name, error_desc.lineno);
    throw_error(buf); // assert(0 == 1); // exit(1);
}
void throw_error(char* msg)
{
    char error[248];
    sprintf_s(error, 248, "[GRAVITY_ERROR] %s", msg);
    printf("%s\n", error);
#ifdef EDITOR_ACT
    set_error_popup(GRAVITY_ERROR, msg);
    submit_txt_console(error);
    set_gamestate(BEE_FALSE, BEE_FALSE);
#endif
    // disable current script
    cur_script_error      = BEE_TRUE;
    gravity_script* cur_script = get_cur_script();
    cur_script->active    = BEE_FALSE;
    cur_script->has_error = BEE_TRUE;
}

void print_callback(gravity_vm* vm, const char* message, void* xdata)
{
    char* msg = "[GRVTY] ";
    strcat(msg, message);
    printf(msg);
    submit_txt_console(msg);
}

// for including files
const char* load_file(const char* file, size_t* size, uint32_t* fileid, void* xdata, int* is_static)
{
    // printf("load file: %s\n", file);
    char* path = get_asset_dir();
    strcat(path, "\\gravity\\");
    strcat(path, file);
    // printf(" -> path: %s\n", path);
    
    char* txt = read_text_file(path);
    // printf("read text:\n %s\n", txt);

    *size = strlen(txt);
    *fileid = 0;
    *is_static = BEE_TRUE; // false
    return txt;
}

gravity_script make_script(char* path)
{
    gravity_script script;
    script.active     = BEE_TRUE;
    script.has_error  = BEE_FALSE;
    script.path       = path;
    script.path_valid = file_exists_check(path);
    script.source     = NULL;

    // script.entity_index = 0;

    script.closure = NULL;
    script.init_closure_assigned      = BEE_FALSE;
    script.update_closure_assigned    = BEE_FALSE;
    script.trigger_closure_assigned   = BEE_FALSE;
    script.has_on_trigger             = BEE_TRUE;
    script.collision_closure_assigned = BEE_FALSE;
    script.has_on_collision           = BEE_TRUE;

    char* name = str_find_last_of(path, "\\");
    if (name == NULL)
    {
        name = str_find_last_of(path, "/");
    }
    assert(name != NULL);
    name = str_trunc(name, 1); // cut off the last "\"
    assert(name != NULL);

    script.name = name;

    return script;
}
void free_script(gravity_script* script)
{
    // gets alloced by read_text_file()
    if (script->source != NULL && script->has_error == BEE_FALSE)
    {
        gravity_vm_free(script->vm);
        gravity_core_free();
    }
}

rtn_code gravity_run_init(gravity_script* script, const char* src, int entity_index)
{
    set_cur_script(script, entity_index);

    // setup a delegate struct
    gravity_delegate_t delegate = { .error_callback = report_error,
                                    // .loadfile_callback = load_file,
                                    .log_callback = print_callback };

    // allocate a new compiler
    gravity_compiler_t* compiler = gravity_compiler_create(&delegate);
    assert(compiler != NULL);

    // compile Gravity source code into bytecode
    script->closure = gravity_compiler_run(compiler, src, strlen(src), 0, BEE_FALSE, BEE_TRUE);
    script->init_closure_assigned = BEE_TRUE;
    script->source  = NULL; // gravity_run_compiler corrupts the source string
    // assert(script->closure != NULL);
    if (cur_script_error == BEE_TRUE)
    {
        cur_script_error = BEE_FALSE;
        return BEE_ERROR;
    }
    if (script->closure == NULL)
    {
        char buffer[60];
        sprintf(buffer, "failed to compile \"%s\"", script->name);
        throw_error(buffer);
        cur_script_error = BEE_FALSE;
        return BEE_ERROR;
    }

    // allocate a new Gravity VM
    script->vm = gravity_vm_new(&delegate);
    if (script->vm == NULL)
    {
        char buffer[80];
        sprintf(buffer, "failed create vm for \"%s\"", script->name);
        throw_error(buffer);
        // cur_script_error = BEE_FALSE;
        return BEE_ERROR;
    }
    setup_entity_class(script->vm);
    setup_game_class(script->vm);
    setup_input_class(script->vm);
    setup_world_class(script->vm);
    // setup_camera_class(script->vm);
    setup_ui_class(script->vm);

    // transfer memory from the compiler (front-end) to the VM (back-end)
    gravity_compiler_transfer(compiler, script->vm);

    // once the memory has been transferred, you can get rid of the front-end
    gravity_compiler_free(compiler);

    // load closure into VM
    gravity_vm_loadclosure(script->vm, script->closure);

    // lookup a reference to the mul closure into the Gravity VM
    gravity_value_t init_func = gravity_vm_getvalue(script->vm, "init", (uint32_t)strlen("init"));
    if (!VALUE_ISA_CLOSURE(init_func)) 
    {
        printf("Unable to find init() function in Gravity VM.\n");
        throw_error("Unable to find init() function in Gravity VM.\n");
        return BEE_ERROR;
    }

    // convert function to closure
    gravity_closure_t* init_closure = VALUE_AS_CLOSURE(init_func);

    // execute init closure
    gravity_vm_runclosure(script->vm, init_closure, VALUE_FROM_NULL, NULL, 0);
    

    script->closure = init_closure;

    // check for on_trigger() closure
    gravity_value_t trigger_func = gravity_vm_getvalue(script->vm, "on_trigger", (uint32_t)strlen("on_trigger"));
    if (!VALUE_ISA_CLOSURE(trigger_func))
    {
        script->has_on_trigger = BEE_FALSE;
    }
    else { printf("found on_trigger() in %s\n", script->name); }
    gravity_value_t collision_func = gravity_vm_getvalue(script->vm, "on_collision", (uint32_t)strlen("on_collision"));
    if (!VALUE_ISA_CLOSURE(collision_func))
    {
        script->has_on_collision = BEE_FALSE;
    }
    else { printf("found on_collision() in %s\n", script->name); }
    
    return BEE_OK;
}

rtn_code gravity_run_update(gravity_script* script, int entity_index)
{
    set_cur_script(script, entity_index);
    
    // load closure into VM
    gravity_vm_loadclosure(script->vm, script->closure);

    if (script->update_closure_assigned == BEE_FALSE)
    {
        // lookup a reference to the update closure into the Gravity VM
        gravity_value_t update_func = gravity_vm_getvalue(script->vm, "update", (uint32_t)strlen("update"));
        if (!VALUE_ISA_CLOSURE(update_func))
        {
            printf("Unable to find update() function in Gravity VM.\n");
            throw_error("Unable to find update() function in Gravity VM.\n");
            return BEE_ERROR;
        }
        // convert function to closure
        script->closure = VALUE_AS_CLOSURE(update_func);
        script->update_closure_assigned  = BEE_TRUE;
        script->trigger_closure_assigned = BEE_FALSE;
        script->collision_closure_assigned = BEE_FALSE;
    }

    // execute init closure
    gravity_vm_runclosure(script->vm, script->closure, VALUE_FROM_NULL, NULL, 0); // &delta_t, 1

    return BEE_OK;
}

rtn_code gravity_run_on_trigger(gravity_script* script, int entity_index, int collision_id)
{
    if (!script->has_on_trigger) { return BEE_ERROR; }
    set_cur_script(script, entity_index);

    // load closure into VM
    gravity_vm_loadclosure(script->vm, script->closure);

    if (script->trigger_closure_assigned == BEE_FALSE)
    {
        // lookup a reference to the update closure into the Gravity VM
        gravity_value_t trigger_func = gravity_vm_getvalue(script->vm, "on_trigger", (uint32_t)strlen("on_trigger"));
        if (!VALUE_ISA_CLOSURE(trigger_func))
        {
            printf("Unable to find on_trigger() function in Gravity VM.\n");
            throw_error("Unable to find on_trigger() function in Gravity VM.\n");
            return BEE_ERROR;
        }
        // convert function to closure
        script->closure = VALUE_AS_CLOSURE(trigger_func);
        script->trigger_closure_assigned    = BEE_TRUE;
        script->update_closure_assigned     = BEE_FALSE;
        script->collision_closure_assigned  = BEE_FALSE;
    }

    gravity_value_t arg = VALUE_FROM_INT(collision_id);

    // execute init closure
    gravity_vm_runclosure(script->vm, script->closure, VALUE_FROM_NULL, &arg, 1);


    return BEE_OK;
}

rtn_code gravity_run_on_collision(gravity_script* script, int entity_index, int collision_id)
{
    if (!script->has_on_collision) { return BEE_ERROR; }
    set_cur_script(script, entity_index);

    // load closure into VM
    gravity_vm_loadclosure(script->vm, script->closure);

    if (script->collision_closure_assigned == BEE_FALSE)
    {
        // lookup a reference to the update closure into the Gravity VM
        gravity_value_t func = gravity_vm_getvalue(script->vm, "on_collision", (uint32_t)strlen("on_collision"));
        if (!VALUE_ISA_CLOSURE(func))
        {
            printf("Unable to find on_collision() function in Gravity VM.\n");
            throw_error("Unable to find on_collision() function in Gravity VM.\n");
            return BEE_ERROR;
        }
        // convert function to closure
        script->closure = VALUE_AS_CLOSURE(func);
        script->collision_closure_assigned = BEE_FALSE;
        script->trigger_closure_assigned   = BEE_FALSE;
        script->update_closure_assigned    = BEE_FALSE;
    }

    gravity_value_t arg = VALUE_FROM_INT(collision_id);

    // execute init closure
    gravity_vm_runclosure(script->vm, script->closure, VALUE_FROM_NULL, &arg, 1);


    return BEE_OK;
}












