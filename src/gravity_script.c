#include "gravity_script.h"

#include "CGLM/cglm.h"

#include "game_time.h"
#include "file_handler.h"
#include "camera.h"
#include "str_util.h"
#include "renderer.h"
#include "input.h"
#include "ui.h"


gravity_script* cur_script = NULL;
bee_bool cur_script_error = BEE_FALSE;
// gravity_vm* vm;
// gravity_closure_t* closure;

// a very simple report error callback function
void report_error(gravity_vm* vm, error_type_t error_type, const char* message, error_desc_t error_desc, void* xdata) 
{
    char buf[128];
    
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
    cur_script = script;

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
    cur_script = script;
    
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
    cur_script = script;

    
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


void setup_entity_class(gravity_vm* vm)
{
    // create a new Foo class
    gravity_class_t* c = gravity_class_new_pair(vm, "Entity", NULL, 0, 0);

    // allocate and bind bar closure to the newly created class
    gravity_class_bind(c, "move_x", NEW_CLOSURE_VALUE(move_ent_x));
    gravity_class_bind(c, "move_y", NEW_CLOSURE_VALUE(move_ent_y));
    gravity_class_bind(c, "move_z", NEW_CLOSURE_VALUE(move_ent_z));
    
    gravity_class_bind(c, "get_x", NEW_CLOSURE_VALUE(get_ent_x));
    gravity_class_bind(c, "get_y", NEW_CLOSURE_VALUE(get_ent_y));
    gravity_class_bind(c, "get_z", NEW_CLOSURE_VALUE(get_ent_z));

    // register class c inside VM
    gravity_vm_setvalue(vm, "Entity", VALUE_FROM_OBJECT(c));
}

static bee_bool move_ent_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    assert(nargs == 2);
    gravity_value_t v1 = GET_VALUE(1);
    //gravity_value_t v2 = GET_VALUE(2);

    int ents_len; get_entity_len(&ents_len);
    entity* ent = get_entity(cur_script->entity_index);

    f32* x = &ent->pos[0];
    if (ent->pos == NULL) { f32 val = 0;  x = &val; }
    *x += VALUE_AS_FLOAT(v1);

    //char buffer[40];
    // printf("obj name: \"%s\", y: %f\n", ents[ents_len -2].name, ents[ents_len].pos[1]);

    // SKIPPED: check that both v1 and v2 are int numbers
    // RETURN_VALUE(VALUE_FROM_INT(v1.n + v2.n), rindex);
}
static bee_bool move_ent_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    assert(nargs == 2);
    gravity_value_t v1 = GET_VALUE(1);
    //gravity_value_t v2 = GET_VALUE(2);

    int ents_len; get_entity_len(&ents_len);
    entity* ent = get_entity(cur_script->entity_index);

    f32* y = &ent->pos[1];
    if (ent->pos == NULL) { f32 val = 0;  y = &val; }
    *y += VALUE_AS_FLOAT(v1);

    //char buffer[40];
    // printf("obj name: \"%s\", y: %f\n", ents[ents_len -2].name, ents[ents_len].pos[1]);

    // SKIPPED: check that both v1 and v2 are int numbers
    // RETURN_VALUE(VALUE_FROM_INT(v1.n + v2.n), rindex);
}
static bee_bool move_ent_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    assert(nargs == 2);
    gravity_value_t v1 = GET_VALUE(1);
    //gravity_value_t v2 = GET_VALUE(2);

    int ents_len; get_entity_len(&ents_len);
    entity* ent = get_entity(cur_script->entity_index);

    f32* z = &ent->pos[2];
    if (ent->pos == NULL) { f32 val = 0;  z = &val; }
    *z += VALUE_AS_FLOAT(v1);

    //char buffer[40];
    // printf("obj name: \"%s\", y: %f\n", ents[ents_len -2].name, ents[ents_len].pos[1]);

    // SKIPPED: check that both v1 and v2 are int numbers
    // RETURN_VALUE(VALUE_FROM_INT(v1.n + v2.n), rindex);
}

static bee_bool get_ent_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    assert(nargs == 1);

    // int ents_len; get_entity_len(&ents_len);
    entity* ent = get_entity(cur_script->entity_index);

    f32 x = ent->pos[0];
    if (ent->pos == NULL) { x = 0; }

    // SKIPPED: check that both v1 and v2 are int numbers
    RETURN_VALUE(VALUE_FROM_FLOAT(x), rindex);
}
static bee_bool get_ent_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    assert(nargs == 1);

    int ents_len; get_entity_len(&ents_len);
    entity* ent = get_entity(cur_script->entity_index);

    f32 y = ent->pos[0];
    if (ent->pos == NULL) { y = 0; }

    // SKIPPED: check that both v1 and v2 are int numbers
    RETURN_VALUE(VALUE_FROM_FLOAT(y), rindex);
}
static bee_bool get_ent_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    assert(nargs == 1);

    int ents_len; get_entity_len(&ents_len);
    entity* ent = get_entity(cur_script->entity_index);

    f32 z = ent->pos[0];
    if (ent->pos == NULL) { z = 0; }

    // SKIPPED: check that both v1 and v2 are int numbers
    RETURN_VALUE(VALUE_FROM_FLOAT(z), rindex);
}

void setup_game_class(gravity_vm* vm)
{
    // create a new Foo class
    gravity_class_t* c = gravity_class_new_pair(vm, "Game", NULL, 0, 0);

    // allocate and bind bar closure to the newly created class
    gravity_class_bind(c, "get_total_secs", NEW_CLOSURE_VALUE(get_game_total_sec));
    gravity_class_bind(c, "get_delta_t", NEW_CLOSURE_VALUE(get_game_delta_t));
    gravity_class_bind(c, "quit", NEW_CLOSURE_VALUE(game_quit));
    
    // register class c inside VM
    gravity_vm_setvalue(vm, "Game", VALUE_FROM_OBJECT(c));
}

static bee_bool get_game_total_sec(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    assert(nargs == 1);

    // SKIPPED: check that both v1 and v2 are int numbers
    RETURN_VALUE(VALUE_FROM_FLOAT(get_total_secs()), rindex);
}
static bee_bool get_game_delta_t(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    assert(nargs == 1);

    // SKIPPED: check that both v1 and v2 are int numbers
    RETURN_VALUE(VALUE_FROM_FLOAT(get_delta_time()), rindex);
}
static bee_bool game_quit(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    assert(nargs == 1);
    
    close_window();
}

void setup_world_class(gravity_vm* vm)
{
    // create a new Foo class
    gravity_class_t* c = gravity_class_new_pair(vm, "World", NULL, 0, 0);

    // allocate and bind bar closure to the newly created class
    gravity_class_bind(c, "get_entity", NEW_CLOSURE_VALUE(world_get_entity));
    gravity_class_bind(c, "move_x", NEW_CLOSURE_VALUE(world_move_ent_x));
    gravity_class_bind(c, "move_y", NEW_CLOSURE_VALUE(world_move_ent_y));
    gravity_class_bind(c, "move_z", NEW_CLOSURE_VALUE(world_move_ent_z));
    gravity_class_bind(c, "get_x", NEW_CLOSURE_VALUE(world_get_ent_x));
    gravity_class_bind(c, "get_y", NEW_CLOSURE_VALUE(world_get_ent_y));
    gravity_class_bind(c, "get_z", NEW_CLOSURE_VALUE(world_get_ent_z));

    // register class c inside VM
    gravity_vm_setvalue(vm, "World", VALUE_FROM_OBJECT(c));
}

static bee_bool world_get_entity(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 2)
    {
        throw_error("[World.get_entity(string)] Wrong amount of arguments, 1 argument is needed.");
    }

    gravity_value_t v1 = GET_VALUE(1);

    printf("passed ent name: %s\n", VALUE_AS_CSTRING(v1));
    int id = get_entity_id_by_name(VALUE_AS_CSTRING(v1));
    printf("returned ent id: %d\n", id);

    if (id == 9999)
    {
        throw_error("[World.get_entity(string)] Requested entity doesn't exist.");
    }

    RETURN_VALUE(VALUE_FROM_INT(id), rindex);
}

static bee_bool world_move_ent_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    if (nargs != 3)
    {
        throw_error("[World.move_x(int, float)] Wrong amount of arguments, 2 argument are needed.");
    }
    gravity_value_t v1 = GET_VALUE(1);
    gravity_value_t v2 = GET_VALUE(2);
    if (VALUE_ISA_INT(v1) == BEE_FALSE || VALUE_ISA_FLOAT(v2) == BEE_FALSE)
    {
        throw_error("[World.move_x(int, float)] Wrong argument types.");
    }

    // int ents_len; get_entity_len(&ents_len);
    entity* ent = get_entity(VALUE_AS_INT(v1));

    f32* y = &ent->pos[0];
    if (ent->pos == NULL) { f32 val = 0;  y = &val; }
    *y += VALUE_AS_FLOAT(v2);
}
static bee_bool world_move_ent_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    if (nargs != 3)
    {
        throw_error("[World.move_y(int, float)] Wrong amount of arguments, 2 argument are needed.");
    }
    gravity_value_t v1 = GET_VALUE(1);
    gravity_value_t v2 = GET_VALUE(2);
    if (VALUE_ISA_INT(v1) == BEE_FALSE || VALUE_ISA_FLOAT(v2) == BEE_FALSE)
    {
        throw_error("[World.move_y(int, float)] Wrong argument types.");
    }

    // int ents_len; get_entity_len(&ents_len);
    entity* ent = get_entity(VALUE_AS_INT(v1));

    f32* y = &ent->pos[1];
    if (ent->pos == NULL) { f32 val = 0;  y = &val; }
    *y += VALUE_AS_FLOAT(v2);
}
static bee_bool world_move_ent_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 3)
    {
        throw_error("[World.move_z(int, float)] Wrong amount of arguments, 2 argument are needed.");
    }
    gravity_value_t v1 = GET_VALUE(1);
    gravity_value_t v2 = GET_VALUE(2);
    if (VALUE_ISA_INT(v1) == BEE_FALSE || VALUE_ISA_FLOAT(v2) == BEE_FALSE)
    {
        throw_error("[World.move_z(int, float)] Wrong argument types.");
    }

    // int ents_len; get_entity_len(&ents_len);
    entity* ent = get_entity(VALUE_AS_INT(v1));

    f32* y = &ent->pos[2];
    if (ent->pos == NULL) { f32 val = 0;  y = &val; }
    *y += VALUE_AS_FLOAT(v2);
}

static bee_bool world_get_ent_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 2)
    {
        throw_error("[World.get_x(int)] Wrong amount of arguments, 1 argument is needed.");
    }
    gravity_value_t v1 = GET_VALUE(1);
    if (VALUE_ISA_INT(v1) == BEE_FALSE)
    {
        throw_error("[World.move_x(int)] Wrong argument types.");
    }

    entity* ent = get_entity(VALUE_AS_INT(v1));

    f32 x = ent->pos[0];
    if (ent->pos == NULL) { x = 0; }

    RETURN_VALUE(VALUE_FROM_FLOAT(x), rindex);
}
static bee_bool world_get_ent_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 2)
    {
        throw_error("[World.get_y(int)] Wrong amount of arguments, 1 argument is needed.");
    }
    gravity_value_t v1 = GET_VALUE(1);
    if (VALUE_ISA_INT(v1) == BEE_FALSE)
    {
        throw_error("[World.move_y(int)] Wrong argument types.");
    }

    entity* ent = get_entity(VALUE_AS_INT(v1));

    f32 x = ent->pos[1];
    if (ent->pos == NULL) { x = 0; }

    RETURN_VALUE(VALUE_FROM_FLOAT(x), rindex);
}
static bee_bool world_get_ent_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 2)
    {
        throw_error("[World.get_z(int)] Wrong amount of arguments, 1 argument is needed.");
    }
    gravity_value_t v1 = GET_VALUE(1);
    if (VALUE_ISA_INT(v1) == BEE_FALSE)
    {
        throw_error("[World.move_z(int)] Wrong argument types.");
    }

    entity* ent = get_entity(VALUE_AS_INT(v1));

    f32 x = ent->pos[2];
    if (ent->pos == NULL) { x = 0; }

    RETURN_VALUE(VALUE_FROM_FLOAT(x), rindex);
}


void setup_camera_class(gravity_vm* vm)
{
    // create a new Foo class
    gravity_class_t* c = gravity_class_new_pair(vm, "Camera", NULL, 0, 0);

    // allocate and bind bar closure to the newly created class
    gravity_class_bind(c, "move_x", NEW_CLOSURE_VALUE(move_cam_x));
    gravity_class_bind(c, "move_y", NEW_CLOSURE_VALUE(move_cam_y));
    gravity_class_bind(c, "move_z", NEW_CLOSURE_VALUE(move_cam_z));

    gravity_class_bind(c, "get_x", NEW_CLOSURE_VALUE(get_cam_x));
    gravity_class_bind(c, "get_y", NEW_CLOSURE_VALUE(get_cam_y));
    gravity_class_bind(c, "get_z", NEW_CLOSURE_VALUE(get_cam_z));

    // register class c inside VM
    gravity_vm_setvalue(vm, "Camera", VALUE_FROM_OBJECT(c));
}

static bee_bool move_cam_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    assert(nargs == 2);
    gravity_value_t v1 = GET_VALUE(1);

    vec3 pos; get_camera_pos(&pos);
    pos[0] += VALUE_AS_FLOAT(v1);
    set_camera_pos(pos);
}
static bee_bool move_cam_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    assert(nargs == 2);
    gravity_value_t v1 = GET_VALUE(1);
    //gravity_value_t v2 = GET_VALUE(2);

    vec3 pos; get_camera_pos(&pos);
    pos[1] += VALUE_AS_FLOAT(v1);
    set_camera_pos(pos);
}
static bee_bool move_cam_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    assert(nargs == 2);
    gravity_value_t v1 = GET_VALUE(1);

    vec3 pos; get_camera_pos(&pos);
    pos[2] += VALUE_AS_FLOAT(v1);
    set_camera_pos(pos);
}

static bee_bool get_cam_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    assert(nargs == 1);

    vec3 pos; get_camera_pos(&pos);

    // SKIPPED: check that both v1 and v2 are int numbers
    RETURN_VALUE(VALUE_FROM_FLOAT(pos[0]), rindex);
}
static bee_bool get_cam_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    assert(nargs == 1);

    vec3 pos; get_camera_pos(&pos);

    // SKIPPED: check that both v1 and v2 are int numbers
    RETURN_VALUE(VALUE_FROM_FLOAT(pos[1]), rindex);
}
static bee_bool get_cam_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    assert(nargs == 1);

    vec3 pos; get_camera_pos(&pos);

    // SKIPPED: check that both v1 and v2 are int numbers
    RETURN_VALUE(VALUE_FROM_FLOAT(pos[2]), rindex);
}

void setup_input_class(gravity_vm* vm)
{
    // create a new Foo class
    gravity_class_t* c = gravity_class_new_pair(vm, "Input", NULL, 0, 0);

    // allocate and bind bar closure to the newly created class
    gravity_class_bind(c, "get_key_SPACE", NEW_CLOSURE_VALUE(get_key_SPACE));
    gravity_class_bind(c, "get_key_APOSTROPHE", NEW_CLOSURE_VALUE(get_key_APOSTROPHE));
    gravity_class_bind(c, "get_key_COMMA", NEW_CLOSURE_VALUE(get_key_COMMA));
    gravity_class_bind(c, "get_key_MINUS", NEW_CLOSURE_VALUE(get_key_MINUS));
    gravity_class_bind(c, "get_key_PERIOD", NEW_CLOSURE_VALUE(get_key_PERIOD));
    gravity_class_bind(c, "get_key_SLASH", NEW_CLOSURE_VALUE(get_key_SLASH));
    gravity_class_bind(c, "get_key_0", NEW_CLOSURE_VALUE(get_key_0));
    gravity_class_bind(c, "get_key_1", NEW_CLOSURE_VALUE(get_key_1));
    gravity_class_bind(c, "get_key_2", NEW_CLOSURE_VALUE(get_key_2));
    gravity_class_bind(c, "get_key_3", NEW_CLOSURE_VALUE(get_key_3));
    gravity_class_bind(c, "get_key_4", NEW_CLOSURE_VALUE(get_key_4));
    gravity_class_bind(c, "get_key_5", NEW_CLOSURE_VALUE(get_key_5));
    gravity_class_bind(c, "get_key_6", NEW_CLOSURE_VALUE(get_key_6));
    gravity_class_bind(c, "get_key_7", NEW_CLOSURE_VALUE(get_key_7));
    gravity_class_bind(c, "get_key_8", NEW_CLOSURE_VALUE(get_key_8));
    gravity_class_bind(c, "get_key_9", NEW_CLOSURE_VALUE(get_key_9));
    gravity_class_bind(c, "get_key_SEMICOLON", NEW_CLOSURE_VALUE(get_key_SEMICOLON));
    gravity_class_bind(c, "get_key_EQUAL", NEW_CLOSURE_VALUE(get_key_EQUAL));
    gravity_class_bind(c, "get_key_A", NEW_CLOSURE_VALUE(get_key_A));
    gravity_class_bind(c, "get_key_B", NEW_CLOSURE_VALUE(get_key_B));
    gravity_class_bind(c, "get_key_C", NEW_CLOSURE_VALUE(get_key_C));
    gravity_class_bind(c, "get_key_D", NEW_CLOSURE_VALUE(get_key_D));
    gravity_class_bind(c, "get_key_E", NEW_CLOSURE_VALUE(get_key_E));
    gravity_class_bind(c, "get_key_F", NEW_CLOSURE_VALUE(get_key_F));
    gravity_class_bind(c, "get_key_G", NEW_CLOSURE_VALUE(get_key_G));
    gravity_class_bind(c, "get_key_H", NEW_CLOSURE_VALUE(get_key_H));
    gravity_class_bind(c, "get_key_I", NEW_CLOSURE_VALUE(get_key_I));
    gravity_class_bind(c, "get_key_J", NEW_CLOSURE_VALUE(get_key_J));
    gravity_class_bind(c, "get_key_K", NEW_CLOSURE_VALUE(get_key_K));
    gravity_class_bind(c, "get_key_L", NEW_CLOSURE_VALUE(get_key_L));
    gravity_class_bind(c, "get_key_M", NEW_CLOSURE_VALUE(get_key_M));
    gravity_class_bind(c, "get_key_N", NEW_CLOSURE_VALUE(get_key_N));
    gravity_class_bind(c, "get_key_O", NEW_CLOSURE_VALUE(get_key_O));
    gravity_class_bind(c, "get_key_P", NEW_CLOSURE_VALUE(get_key_P));
    gravity_class_bind(c, "get_key_Q", NEW_CLOSURE_VALUE(get_key_Q));
    gravity_class_bind(c, "get_key_R", NEW_CLOSURE_VALUE(get_key_R));
    gravity_class_bind(c, "get_key_S", NEW_CLOSURE_VALUE(get_key_S));
    gravity_class_bind(c, "get_key_T", NEW_CLOSURE_VALUE(get_key_T));
    gravity_class_bind(c, "get_key_U", NEW_CLOSURE_VALUE(get_key_U));
    gravity_class_bind(c, "get_key_V", NEW_CLOSURE_VALUE(get_key_V));
    gravity_class_bind(c, "get_key_W", NEW_CLOSURE_VALUE(get_key_W));
    gravity_class_bind(c, "get_key_X", NEW_CLOSURE_VALUE(get_key_X));
    gravity_class_bind(c, "get_key_Y", NEW_CLOSURE_VALUE(get_key_Y));
    gravity_class_bind(c, "get_key_Z", NEW_CLOSURE_VALUE(get_key_Z));
    gravity_class_bind(c, "get_key_LEFT_BRACKET", NEW_CLOSURE_VALUE(get_key_LEFT_BRACKET));
    gravity_class_bind(c, "get_key_BACKSLASH", NEW_CLOSURE_VALUE(get_key_BACKSLASH));
    gravity_class_bind(c, "get_key_RIGHT_BRACKET", NEW_CLOSURE_VALUE(get_key_RIGHT_BRACKET));
    gravity_class_bind(c, "get_key_GRAVE_ACCENT", NEW_CLOSURE_VALUE(get_key_GRAVE_ACCENT));
    gravity_class_bind(c, "get_key_WORLD_1", NEW_CLOSURE_VALUE(get_key_WORLD_1));
    gravity_class_bind(c, "get_key_WORLD_2", NEW_CLOSURE_VALUE(get_key_WORLD_2));
    gravity_class_bind(c, "get_key_ESCAPE", NEW_CLOSURE_VALUE(get_key_ESCAPE));
    gravity_class_bind(c, "get_key_ENTER", NEW_CLOSURE_VALUE(get_key_ENTER));
    gravity_class_bind(c, "get_key_TAB", NEW_CLOSURE_VALUE(get_key_TAB));
    gravity_class_bind(c, "get_key_BACKSPACE", NEW_CLOSURE_VALUE(get_key_BACKSPACE));
    gravity_class_bind(c, "get_key_INSERT", NEW_CLOSURE_VALUE(get_key_INSERT));
    gravity_class_bind(c, "get_key_DELETE", NEW_CLOSURE_VALUE(get_key_DELETE));
    gravity_class_bind(c, "get_key_ARROW_LEFT", NEW_CLOSURE_VALUE(get_key_ARROW_LEFT));
    gravity_class_bind(c, "get_key_ARROW_RIGHT", NEW_CLOSURE_VALUE(get_key_ARROW_RIGHT));
    gravity_class_bind(c, "get_key_ARROW_UP", NEW_CLOSURE_VALUE(get_key_ARROW_UP));
    gravity_class_bind(c, "get_key_ARROW_DOWN", NEW_CLOSURE_VALUE(get_key_ARROW_DOWN));
    gravity_class_bind(c, "get_key_PAGE_UP", NEW_CLOSURE_VALUE(get_key_PAGE_UP));
    gravity_class_bind(c, "get_key_PAGE_DOWN", NEW_CLOSURE_VALUE(get_key_PAGE_DOWN));
    gravity_class_bind(c, "get_key_HOME", NEW_CLOSURE_VALUE(get_key_HOME));
    gravity_class_bind(c, "get_key_END", NEW_CLOSURE_VALUE(get_key_END));
    gravity_class_bind(c, "get_key_CAPS_LOCK", NEW_CLOSURE_VALUE(get_key_CAPS_LOCK));
    gravity_class_bind(c, "get_key_SCROLL_LOCK", NEW_CLOSURE_VALUE(get_key_SCROLL_LOCK));
    gravity_class_bind(c, "get_key_NUM_LOCK", NEW_CLOSURE_VALUE(get_key_NUM_LOCK));
    gravity_class_bind(c, "get_key_PRINT", NEW_CLOSURE_VALUE(get_key_PRINT));
    gravity_class_bind(c, "get_key_PAUSE", NEW_CLOSURE_VALUE(get_key_PAUSE));


    // register class c inside VM
    gravity_vm_setvalue(vm, "Input", VALUE_FROM_OBJECT(c));
}

static bee_bool get_key_SPACE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_SPACE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Space)), rindex);
}
static bee_bool get_key_APOSTROPHE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_APOSTROPHE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Apostrophe)), rindex);
}
static bee_bool get_key_COMMA(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_COMMA()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Comma)), rindex);
}
static bee_bool get_key_MINUS(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_MINUS()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Minus)), rindex);
}
static bee_bool get_key_PERIOD(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_PERIOD()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Period)), rindex);
}
static bee_bool get_key_SLASH(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_SLASH()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Slash)), rindex);
}
static bee_bool get_key_0(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_0()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha0 | KEY_Numpad0)), rindex);
}
static bee_bool get_key_1(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_1()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha1 | KEY_Numpad1)), rindex);
}
static bee_bool get_key_2(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_2()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha2 | KEY_Numpad2)), rindex);
}
static bee_bool get_key_3(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_3()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha3 | KEY_Numpad3)), rindex);
}
static bee_bool get_key_4(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_4()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha4 | KEY_Numpad4)), rindex);
}
static bee_bool get_key_5(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_5()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha5 | KEY_Numpad5)), rindex);
}
static bee_bool get_key_6(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_6()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha6 | KEY_Numpad6)), rindex);
}
static bee_bool get_key_7(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_7()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha7 | KEY_Numpad7)), rindex);
}
static bee_bool get_key_8(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_8()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha8 | KEY_Numpad8)), rindex);
}
static bee_bool get_key_9(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_9()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha9 | KEY_Numpad9)), rindex);
}
static bee_bool get_key_SEMICOLON(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_SEMICOLON()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_SemiColon)), rindex);
}
static bee_bool get_key_EQUAL(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_EQUAL()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Equal)), rindex);
}
static bee_bool get_key_A(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_A()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_A)), rindex);
}
static bee_bool get_key_B(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_B()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_B)), rindex);
}
static bee_bool get_key_C(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_C()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_C)), rindex);
}
static bee_bool get_key_D(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_D()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_D)), rindex);
}
static bee_bool get_key_E(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_E()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_E)), rindex);
}
static bee_bool get_key_F(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_F)), rindex);
}
static bee_bool get_key_G(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_G()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_F)), rindex);
}
static bee_bool get_key_H(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_H()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_H)), rindex);
}
static bee_bool get_key_I(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_I()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_I)), rindex);
}
static bee_bool get_key_J(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_J()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_J)), rindex);
}
static bee_bool get_key_K(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_K()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_K)), rindex);
}
static bee_bool get_key_L(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_L()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_L)), rindex);
}
static bee_bool get_key_M(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_M()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_M)), rindex);
}
static bee_bool get_key_N(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_N()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_N)), rindex);
}
static bee_bool get_key_O(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_O()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_O)), rindex);
}
static bee_bool get_key_P(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_P()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_P)), rindex);
}
static bee_bool get_key_Q(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_Q()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Q)), rindex);
}
static bee_bool get_key_R(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_R()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_R)), rindex);
}
static bee_bool get_key_S(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_S()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_S)), rindex);
}
static bee_bool get_key_T(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_T()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_T)), rindex);
}
static bee_bool get_key_U(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_U()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_U)), rindex);
}
static bee_bool get_key_V(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_V()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_V)), rindex);
}
static bee_bool get_key_W(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_W()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_W)), rindex);
}
static bee_bool get_key_X(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_X()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_X)), rindex);
}
static bee_bool get_key_Y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_Y()] Wrong amount of arguments, 0 arguments are needed."); }    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Y)), rindex);
}
static bee_bool get_key_Z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1)  { throw_error("[Input.get_key_Z()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Z)), rindex);
}
static bee_bool get_key_LEFT_BRACKET(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_LEFT_BRACKET()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_LeftBracket)), rindex);
}
static bee_bool get_key_BACKSLASH(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_BACKSLASH()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Backslash)), rindex);
}
static bee_bool get_key_RIGHT_BRACKET(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_RIGHT_BRACKET()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_RightBracket)), rindex);
}
static bee_bool get_key_GRAVE_ACCENT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_GRAVE_ACCENT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_GraveAccent)), rindex);
}
static bee_bool get_key_WORLD_1(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_WORLD_1()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_World1)), rindex);
}
static bee_bool get_key_WORLD_2(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_WORLD_2()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_World2)), rindex);
}
static bee_bool get_key_ESCAPE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ESCAPE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Escape)), rindex);
}
static bee_bool get_key_ENTER(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ENTER()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Enter)), rindex);
}
static bee_bool get_key_TAB(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_TAB()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Tab)), rindex);
}
static bee_bool get_key_BACKSPACE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_BACKSPACE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Backspace)), rindex);
}
static bee_bool get_key_INSERT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_INSERT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Insert)), rindex);
}
static bee_bool get_key_DELETE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_DELETE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Delete)), rindex);
}
static bee_bool get_key_ARROW_RIGHT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ARROW_RIGHT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_RightArrow)), rindex);
}
static bee_bool get_key_ARROW_LEFT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ARROW_LEFT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_LeftArrow)), rindex);
}static bee_bool get_key_ARROW_DOWN(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ARROW_DOWN()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_DownArrow)), rindex);
}
static bee_bool get_key_ARROW_UP(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ARROW_UP()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_UpArrow)), rindex);
}
static bee_bool get_key_PAGE_UP(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_PAGE_UP()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_PageUp)), rindex);
}
static bee_bool get_key_PAGE_DOWN(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_PAGE_DOWN()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_PageDown)), rindex);
}
static bee_bool get_key_HOME(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_HOME()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Home)), rindex);
}
static bee_bool get_key_END(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_END()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_End)), rindex);
}
static bee_bool get_key_CAPS_LOCK(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_CAPS_LOCK()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_CapsLock)), rindex);
}
static bee_bool get_key_SCROLL_LOCK(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_SCROLL_LOCK()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_ScrollLock)), rindex);
}
static bee_bool get_key_NUM_LOCK(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_NUM_LOCK()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_NumLock)), rindex);
}
static bee_bool get_key_PRINT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_PRINT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_PrintScreen)), rindex);
}
static bee_bool get_key_PAUSE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_PAUSE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Pause)), rindex);
}










