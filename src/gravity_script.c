#include "gravity_script.h"

#include "CGLM/cglm.h"

#include "gravity_interface.h"
#include "game_time.h"
#include "file_handler.h"
#include "camera.h"
#include "str_util.h"
#include "renderer.h"
#include "input.h"
#include "ui.h"


bee_bool cur_script_error = BEE_FALSE;
// gravity_vm* vm;
// gravity_closure_t* closure;

// a very simple report error callback function
void report_error(gravity_vm* vm, error_type_t error_type, const char* message, error_desc_t error_desc, void* xdata) 
{
    char buf[128];
    
    gravity_script* cur_script = get_cur_script();
    char* name = str_find_last_of(cur_script->path, "\\");
    if (name == NULL)
    {
        name = str_find_last_of(cur_script->path, "/");
    }
    assert(name != NULL);
    name = str_trunc(name, 1); // cut off the last "\"
    assert(name != NULL);

    printf("GRAVITY_ERROR: %s, file: %s, line: %d, column: %d\n", message, name, error_desc.lineno, error_desc.colno);
    sprintf(buf, "GRAVITY_ERROR: %s, file: %s, line: %d, column: %d\n", message, name, error_desc.lineno, error_desc.colno);
    throw_error(buf); // assert(0 == 1); // exit(1);
}
void throw_error(char* msg)
{
    printf("GRAVITY_API_ERROR: %s\n", msg);
    set_error_popup(msg);
    // disable current script
    cur_script_error      = BEE_TRUE;
    gravity_script* cur_script = get_cur_script();
    cur_script->active    = BEE_FALSE;
    cur_script->has_error = BEE_TRUE;
}

// for including files
const char* load_file(const char* file, size_t* size, uint32_t* fileid, void* xdata, int* is_static)
{

}

gravity_script make_script(char* path)
{
    gravity_script script;
    script.active     = BEE_TRUE;
    script.has_error  = BEE_FALSE;
    script.path       = path;
    script.path_valid = file_exists_check(path);
    script.source     = NULL;

    script.entity_index = 0;

    script.closure = NULL;
    script.update_closure_assigned = BEE_FALSE;

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

rtn_code gravity_run(char* source_code)
{
    printf("gravity src: \n%s", source_code);

    // setup a delegate struct (much more options are available)
    gravity_delegate_t delegate = { .error_callback = report_error };

    // allocate a new compiler
    gravity_compiler_t* compiler = gravity_compiler_create(&delegate);

    printf("strlen(src): %d\n", (int)strlen(source_code));

    // compile Gravity source code into a closure (bytecode)
    gravity_closure_t* closure = gravity_compiler_run(compiler, source_code, strlen(source_code), 0, false, true);

    // sanity check on compiled source
    if (!closure)
    {
        // an error occurred while compiling source code and it has already been reported by the report_error callback
        gravity_compiler_free(compiler);
        assert(0 == 1);
        return BEE_ERROR;
    }

    // allocate a new Gravity VM
    gravity_vm* vm = gravity_vm_new(&delegate);

    // transfer memory from the compiler (front-end) to the VM (back-end)
    gravity_compiler_transfer(compiler, vm);

    // once the memory has been transferred, you can get rid of the front-end
    gravity_compiler_free(compiler);

    // execute main closure
    if (gravity_vm_runmain(vm, closure)) {
        // retrieve returned result
        gravity_value_t result = gravity_vm_result(vm);

        // dump result to a C string and print it to stdout
        char buffer[512];
        gravity_value_dump(vm, result, buffer, sizeof(buffer));
        printf("RESULT: %s\n", buffer);
    }

    // free VM and core libraries (implicitly allocated by the VM)
    gravity_vm_free(vm);
    gravity_core_free();

    return BEE_OK;
}


rtn_code gravity_run_init(gravity_script* script, const char* src)
{
    set_cur_script(script);

    // setup a delegate struct
    gravity_delegate_t delegate = { .error_callback = report_error};

    // allocate a new compiler
    gravity_compiler_t* compiler = gravity_compiler_create(&delegate);

    // compile Gravity source code into bytecode
    script->closure = gravity_compiler_run(compiler, src, strlen(src), 0, false, true);

    if (cur_script_error == BEE_TRUE)
    {
        cur_script_error = BEE_FALSE;
        return BEE_ERROR;
    }

    // allocate a new Gravity VM
    script->vm = gravity_vm_new(&delegate);
    setup_entity_class(script->vm);
    setup_game_class(script->vm);
    setup_input_class(script->vm);
    setup_world_class(script->vm);
    setup_camera_class(script->vm);

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
        return BEE_ERROR;
    }

    // convert function to closure
    gravity_closure_t* init_closure = VALUE_AS_CLOSURE(init_func);

    // execute init closure
    gravity_vm_runclosure(script->vm, init_closure, VALUE_FROM_NULL, NULL, 0);
    

    script->closure = init_closure;
    
    return BEE_OK;
}

rtn_code gravity_run_update(gravity_script* script)
{
    set_cur_script(script);
    
    // load closure into VM
    gravity_vm_loadclosure(script->vm, script->closure);

    if (script->update_closure_assigned == BEE_FALSE)
    {
        // lookup a reference to the update closure into the Gravity VM
        gravity_value_t update_func = gravity_vm_getvalue(script->vm, "update", (uint32_t)strlen("update"));
        if (!VALUE_ISA_CLOSURE(update_func))
        {
            printf("Unable to find update() function in Gravity VM.\n");
            assert(0 == 1);
            return BEE_ERROR;
        }
        // convert function to closure
        script->closure = VALUE_AS_CLOSURE(update_func);
        script->update_closure_assigned = BEE_TRUE;
    }

    // prepare parameters
    // gravity_value_t delta_t = VALUE_FROM_FLOAT(get_delta_time());

    // execute init closure
    gravity_vm_runclosure(script->vm, script->closure, VALUE_FROM_NULL, NULL, 0); // &delta_t, 1

    return BEE_OK;
}

rtn_code gravity_run_cleanup(gravity_script* script)
{
    set_cur_script(script);

    
    // setup a delegate struct
    gravity_delegate_t delegate = { .error_callback = report_error };

    // allocate a new compiler
    gravity_compiler_t* compiler = gravity_compiler_create(&delegate);

    // compile Gravity source code into bytecode
    gravity_closure_t* closure = gravity_compiler_run(compiler, script->source, strlen(script->source), 0, false, true);

    // allocate a new Gravity VM
    gravity_vm* vm = gravity_vm_new(&delegate);

    // transfer memory from the compiler (front-end) to the VM (back-end)
    gravity_compiler_transfer(compiler, vm);

    // once the memory has been transferred, you can get rid of the front-end
    gravity_compiler_free(compiler);

    // load closure into VM
    gravity_vm_loadclosure(vm, closure);

    // lookup a reference to the mul closure into the Gravity VM
    gravity_value_t cleanup_func = gravity_vm_getvalue(vm, "cleanup", (uint32_t)strlen("cleanup"));
    if (!VALUE_ISA_CLOSURE(cleanup_func)) 
    {
        printf("Unable to find cleanup() function in Gravity VM.\n");
        return BEE_ERROR;
    }

    // convert function to closure
    gravity_closure_t* cleanup_closure = VALUE_AS_CLOSURE(cleanup_func);

    // execute init closure
    gravity_vm_runclosure(vm, cleanup_closure, VALUE_FROM_NULL, NULL, 0);

    // free VM and core libraries (implicitly allocated by the VM)
    // gravity_vm_free(vm);
    // gravity_core_free();

    return BEE_OK;
}















