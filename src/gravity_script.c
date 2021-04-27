#include "gravity_script.h"

#include "CGLM/cglm.h"

#include "game_time.h"
#include "renderer.h"


gravity_script* cur_script;


// a very simple report error callback function
void report_error(gravity_vm* vm, error_type_t error_type, const char* message,
    error_desc_t error_desc, void* xdata) {
    printf("GRAVITY_ERROR: %s, line: %d, column: %d\n", message, error_desc.lineno, error_desc.colno);
    assert(0 == 1); // exit(1);
}

gravity_script make_script(char* path)
{
    gravity_script script;
    script.active = BEE_TRUE;
    script.path   = path;
    script.source = NULL;

    script.entity_index = 0;

    script.closure = NULL;
    script.update_closure = NULL;

    return script;
}
void free_script(gravity_script* script)
{
    // gets alloced by read_text_file()
    if (script->source != NULL)
    {
        // free(script->source);
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


rtn_code gravity_run_init(gravity_script* script)
{
    cur_script = script;

    // setup a delegate struct
    gravity_delegate_t delegate = { .error_callback = report_error };

    // allocate a new compiler
    gravity_compiler_t* compiler = gravity_compiler_create(&delegate);

    // compile Gravity source code into bytecode
    script->closure = gravity_compiler_run(compiler, (const char*)script->source, strlen((const char*)script->source), 0, false, true);

    // allocate a new Gravity VM
    gravity_vm* vm = gravity_vm_new(&delegate);
    setup_entity_class(vm);
    setup_game_class(vm);

    // transfer memory from the compiler (front-end) to the VM (back-end)
    gravity_compiler_transfer(compiler, vm);

    // once the memory has been transferred, you can get rid of the front-end
    gravity_compiler_free(compiler);

    // load closure into VM
    gravity_vm_loadclosure(vm, script->closure);

    // lookup a reference to the mul closure into the Gravity VM
    gravity_value_t init_func = gravity_vm_getvalue(vm, "init", (uint32_t)strlen("init"));
    if (!VALUE_ISA_CLOSURE(init_func)) 
    {
        printf("Unable to find init() function in Gravity VM.\n");
        return BEE_ERROR;
    }

    // convert function to closure
    gravity_closure_t* init_closure = VALUE_AS_CLOSURE(init_func);

    // execute init closure
    gravity_vm_runclosure(vm, init_closure, VALUE_FROM_NULL, NULL, 0);

    // free VM and core libraries (implicitly allocated by the VM)
    gravity_vm_free(vm);
    gravity_core_free();

    return BEE_OK;
}

rtn_code gravity_run_update(gravity_script* script)
{
    cur_script = script;

    // setup a delegate struct
    gravity_delegate_t delegate = { .error_callback = report_error };

    // allocate a new compiler
    gravity_compiler_t* compiler = gravity_compiler_create(&delegate);

    // compile Gravity source code into bytecode
    gravity_closure_t* closure = gravity_compiler_run(compiler, (const char*)script->source, strlen((const char*)script->source), 0, 1, 1);

    // allocate a new Gravity VM
    gravity_vm* vm = gravity_vm_new(&delegate);
    setup_entity_class(vm);
    setup_game_class(vm);

    // transfer memory from the compiler (front-end) to the VM (back-end)
    gravity_compiler_transfer(compiler, vm);

    // once the memory has been transferred, you can get rid of the front-end
    gravity_compiler_free(compiler);
 
    // load closure into VM
    gravity_vm_loadclosure(vm, closure);


    // lookup a reference to the update closure into the Gravity VM
    gravity_value_t update_func = gravity_vm_getvalue(vm, "update", (uint32_t)strlen("update"));
    if (!VALUE_ISA_CLOSURE(update_func)) 
    {
        printf("Unable to find update() function in Gravity VM.\n");
        return BEE_ERROR;
    }
    // convert function to closure
    gravity_closure_t* update_closure = VALUE_AS_CLOSURE(update_func);

    // script->update_closure = update_closure;

    // prepare parameters
    gravity_value_t delta_t = VALUE_FROM_FLOAT(get_delta_time());

    // execute init closure
    gravity_vm_runclosure(vm, update_closure, VALUE_FROM_NULL, &delta_t, 1);

    // free VM and core libraries (implicitly allocated by the VM)
    gravity_vm_free(vm);
    gravity_core_free();

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
    gravity_vm_free(vm);
    gravity_core_free();

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

    int ents_len; get_entity_len(&ents_len);
    entity* ent = get_entity(cur_script->entity_index);

    f32 x = ent->pos[0];

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

    // SKIPPED: check that both v1 and v2 are int numbers
    RETURN_VALUE(VALUE_FROM_FLOAT(z), rindex);
}

void setup_game_class(gravity_vm* vm)
{
    // create a new Foo class
    gravity_class_t* c = gravity_class_new_pair(vm, "Game", NULL, 0, 0);

    // allocate and bind bar closure to the newly created class
    gravity_class_bind(c, "get_total_secs", NEW_CLOSURE_VALUE(get_game_total_sec));
    
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
