#include "gravity_interface.h"

#include "asset_manager.h"
#include "scene_manager.h"
#include "file_handler.h"
#include "game_time.h"
#include "renderer.h"
#include "str_util.h"
#include "camera.h"
#include "input.h"
#include "ui.h"


// ---- vars ----
gravity_script* cur_script = NULL;
int cur_script_entity = 0;

bee_bool load_level_act = BEE_FALSE;
char load_level_name[25];

bee_bool rotate_cam_by_mouse_act = BEE_FALSE;
int      rotate_cam_by_mouse_idx = 0;
f32      rotate_cam_by_mouse_speed = 1.0f;

void set_cur_script(gravity_script* script, int entity_index)
{
    cur_script = script;
    cur_script_entity = entity_index;
}
gravity_script* get_cur_script()
{
    return cur_script;
}

void check_for_level_load()
{
    if (load_level_act && check_scene_exists(load_level_name))
    {
        load_scene(load_level_name);
    }
    load_level_act = BEE_FALSE;
}
void mouse_movement_callback()
{
    if (rotate_cam_by_mouse_act)
    {
        rotate_game_cam_by_mouse(rotate_cam_by_mouse_speed * 0.125f);
    }
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
    if (nargs != 2) { throw_error("[Entity.move_x(float)] Wrong amount of arguments, 1 arguments are needed."); return;}

    gravity_value_t v1 = GET_VALUE(1);
    if (VALUE_ISA_INT(v1) == BEE_FALSE && VALUE_ISA_FLOAT(v1) == BEE_FALSE)
    { throw_error("[World.move_x(float)] Wrong argument types."); return; }


    int ents_len; get_entity_len(&ents_len);
    entity* ent = get_entity(cur_script_entity);

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
    if (nargs != 2) { throw_error("[Entity.move_y(float)] Wrong amount of arguments, 1 arguments are needed."); return; }
    
    gravity_value_t v1 = GET_VALUE(1);
    if (VALUE_ISA_INT(v1) == BEE_FALSE && VALUE_ISA_FLOAT(v1) == BEE_FALSE)
    { throw_error("[World.move_x(float)] Wrong argument types."); return; }

    int ents_len; get_entity_len(&ents_len);
    entity* ent = get_entity(cur_script_entity);

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
    if (nargs != 2) { throw_error("[Entity.move_z(float)] Wrong amount of arguments, 1 arguments are needed."); return; }
    gravity_value_t v1 = GET_VALUE(1);
    if (VALUE_ISA_INT(v1) == BEE_FALSE && VALUE_ISA_FLOAT(v1) == BEE_FALSE)
    { throw_error("[World.move_x(float)] Wrong argument types."); return; }

    int ents_len; get_entity_len(&ents_len);
    entity* ent = get_entity(cur_script_entity);

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
    if (nargs != 1) { throw_error("[Entity.get_x()] Wrong amount of arguments, 0 arguments are needed."); return; }

    // int ents_len; get_entity_len(&ents_len);
    entity* ent = get_entity(cur_script_entity);

    f32 x = ent->pos[0];
    if (ent->pos == NULL) { x = 0; }

    // SKIPPED: check that both v1 and v2 are int numbers
    RETURN_VALUE(VALUE_FROM_FLOAT(x), rindex);
}
static bee_bool get_ent_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    if (nargs != 1) { throw_error("[Entity.get_x()] Wrong amount of arguments, 0 arguments are needed."); return; }

    int ents_len; get_entity_len(&ents_len);
    entity* ent = get_entity(cur_script_entity);

    f32 y = ent->pos[0];
    if (ent->pos == NULL) { y = 0; }

    // SKIPPED: check that both v1 and v2 are int numbers
    RETURN_VALUE(VALUE_FROM_FLOAT(y), rindex);
}
static bee_bool get_ent_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    if (nargs != 1) { throw_error("[Entity.get_x()] Wrong amount of arguments, 0 arguments are needed."); return; }

    int ents_len; get_entity_len(&ents_len);
    entity* ent = get_entity(cur_script_entity);

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
    gravity_class_bind(c, "load_level", NEW_CLOSURE_VALUE(game_load_level));

    // register class c inside VM
    gravity_vm_setvalue(vm, "Game", VALUE_FROM_OBJECT(c));
}

static bee_bool get_game_total_sec(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    if (nargs != 1) { throw_error("[Game.get_total_secs()] Wrong amount of arguments, 0 arguments are needed."); }

    // SKIPPED: check that both v1 and v2 are int numbers
    RETURN_VALUE(VALUE_FROM_FLOAT(get_total_secs()), rindex);
}
static bee_bool get_game_delta_t(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    if (nargs != 1) { throw_error("[Game.get_delta_t()] Wrong amount of arguments, 0 arguments are needed."); }

    // SKIPPED: check that both v1 and v2 are int numbers
    RETURN_VALUE(VALUE_FROM_FLOAT(get_delta_time()), rindex);
}
static bee_bool game_quit(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    if (nargs != 1) { throw_error("[Game.quit()] Wrong amount of arguments, 0 arguments are needed."); }

    // go back to edior in editor-build and quit application in game-build
#ifdef EDITOR_ACT
    set_gamestate(BEE_FALSE, BEE_TRUE);
#else
    close_window();
#endif
}

static bee_bool game_load_level(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    if (nargs != 2) { throw_error("[Game.load_level(string)] Wrong amount of arguments, 1 arguments are needed."); }

    gravity_value_t v1 = GET_VALUE(1);
    if (VALUE_ISA_STRING(v1) == BEE_FALSE)
    {
        throw_error("[[Game.load_level(string)] Wrong argument type.");
    }

    // printf("passed ent name: %s\n", VALUE_AS_CSTRING(v1));
    char* name = VALUE_AS_CSTRING(v1);
    if (str_find_last_of(name, ".scene") == NULL)
    {
        strcat(name, ".scene");
    }

    if (!check_scene_exists(name))
    {
        throw_error("[Game.load_level(string)] Scene does not exist.");
    }

    // load_scene(name);
    load_level_act = BEE_TRUE;
    strcpy(load_level_name, name);
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
    if (VALUE_ISA_STRING(v1) == BEE_FALSE)
    {
        throw_error("[World.get_entity(string)] Wrong argument type.");
    }

    // printf("passed ent name: %s\n", VALUE_AS_CSTRING(v1));
    int id = get_entity_id_by_name(VALUE_AS_CSTRING(v1));
    // printf("returned ent id: %d\n", id);

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
        throw_error("[World.move_x(int, float)] Wrong amount of arguments, 2 argument are needed."); return;
    }
    gravity_value_t v1 = GET_VALUE(1);
    gravity_value_t v2 = GET_VALUE(2);
    if (VALUE_ISA_INT(v1) == BEE_FALSE || VALUE_ISA_FLOAT(v2) == BEE_FALSE)
    {
        throw_error("[World.move_x(int, float)] Wrong argument types."); return;
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
        throw_error("[World.move_y(int, float)] Wrong amount of arguments, 2 argument are needed."); return;
    }
    gravity_value_t v1 = GET_VALUE(1);
    gravity_value_t v2 = GET_VALUE(2);
    if (VALUE_ISA_INT(v1) == BEE_FALSE || VALUE_ISA_FLOAT(v2) == BEE_FALSE)
    {
        throw_error("[World.move_y(int, float)] Wrong argument types."); return;
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
        throw_error("[World.move_z(int, float)] Wrong amount of arguments, 2 argument are needed."); return;
    }
    gravity_value_t v1 = GET_VALUE(1);
    gravity_value_t v2 = GET_VALUE(2);
    if (VALUE_ISA_INT(v1) == BEE_FALSE || VALUE_ISA_FLOAT(v2) == BEE_FALSE)
    {
        throw_error("[World.move_z(int, float)] Wrong argument types."); return;
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
        throw_error("[World.get_x(int)] Wrong amount of arguments, 1 argument is needed."); return;
    }
    gravity_value_t v1 = GET_VALUE(1);
    if (VALUE_ISA_INT(v1) == BEE_FALSE)
    {
        throw_error("[World.move_x(int)] Wrong argument types."); return;
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
        throw_error("[World.get_y(int)] Wrong amount of arguments, 1 argument is needed."); return;
    }
    gravity_value_t v1 = GET_VALUE(1);
    if (VALUE_ISA_INT(v1) == BEE_FALSE)
    {
        throw_error("[World.move_y(int)] Wrong argument types."); return;
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
        throw_error("[World.get_z(int)] Wrong amount of arguments, 1 argument is needed."); return;
    }
    gravity_value_t v1 = GET_VALUE(1);
    if (VALUE_ISA_INT(v1) == BEE_FALSE)
    {
        throw_error("[World.move_z(int)] Wrong argument types."); return;
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
    if (nargs != 2) { throw_error("[Camera.move_x(float)] Wrong amount of arguments, 1 argument are needed."); return; }
    gravity_value_t v1 = GET_VALUE(1);
    if (VALUE_ISA_INT(v1) == BEE_FALSE && VALUE_ISA_FLOAT(v1) == BEE_FALSE)
    {
        throw_error("[Camera.move_z(float)] Wrong argument type."); return;
    }

    vec3 pos; get_editor_camera_pos(&pos);
    pos[0] += VALUE_AS_FLOAT(v1);
    set_editor_camera_pos(pos);
}
static bee_bool move_cam_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    if (nargs != 2) { throw_error("[Camera.move_y(float)] Wrong amount of arguments, 1 argument are needed."); return;}
    gravity_value_t v1 = GET_VALUE(1);
    if (VALUE_ISA_INT(v1) == BEE_FALSE && VALUE_ISA_FLOAT(v1) == BEE_FALSE)
    {
        throw_error("[Camera.move_z(float)] Wrong argument type."); return;
    }

    vec3 pos; get_editor_camera_pos(&pos);
    pos[1] += VALUE_AS_FLOAT(v1);
    set_editor_camera_pos(pos);
}
static bee_bool move_cam_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    if (nargs != 2) { throw_error("[Camera.move_z(float)] Wrong amount of arguments, 1 argument are needed."); return;}
    gravity_value_t v1 = GET_VALUE(1);
    if (VALUE_ISA_INT(v1) == BEE_FALSE && VALUE_ISA_FLOAT(v1) == BEE_FALSE)
    {
        throw_error("[Camera.move_z(float)] Wrong argument type."); return;
    }

    vec3 pos; get_editor_camera_pos(&pos);
    pos[2] += VALUE_AS_FLOAT(v1);
    set_editor_camera_pos(pos);
}

static bee_bool get_cam_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    if (nargs != 1) { throw_error("[Camera.get_x()] Wrong amount of arguments, 0 arguments are needed."); return; }

    vec3 pos; get_editor_camera_pos(&pos);

    // SKIPPED: check that both v1 and v2 are int numbers
    RETURN_VALUE(VALUE_FROM_FLOAT(pos[0]), rindex);
}
static bee_bool get_cam_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    if (nargs != 1) { throw_error("[Camera.get_y()] Wrong amount of arguments, 0 arguments are needed."); return;}

    vec3 pos; get_editor_camera_pos(&pos);

    // SKIPPED: check that both v1 and v2 are int numbers
    RETURN_VALUE(VALUE_FROM_FLOAT(pos[1]), rindex);
}
static bee_bool get_cam_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    if (nargs != 1) { throw_error("[Camera.get_z()] Wrong amount of arguments, 0 arguments are needed."); return; }

    vec3 pos; get_editor_camera_pos(&pos);

    // SKIPPED: check that both v1 and v2 are int numbers
    RETURN_VALUE(VALUE_FROM_FLOAT(pos[2]), rindex);
}

void setup_input_class(gravity_vm* vm)
{
    // create a new Foo class
    gravity_class_t* c = gravity_class_new_pair(vm, "Input", NULL, 0, 0);

    // allocate and bind bar closure to the newly created class
    // get_key()
    gravity_class_bind(c, "set_cursor_visible", NEW_CLOSURE_VALUE(input_set_cursor));
    gravity_class_bind(c, "rot_cam_by_mouse", NEW_CLOSURE_VALUE(input_rotate_cam_by_mouse));
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
    gravity_class_bind(c, "get_key_F1", NEW_CLOSURE_VALUE(get_key_F1));
    gravity_class_bind(c, "get_key_F2", NEW_CLOSURE_VALUE(get_key_F2));
    gravity_class_bind(c, "get_key_F3", NEW_CLOSURE_VALUE(get_key_F3));
    gravity_class_bind(c, "get_key_F4", NEW_CLOSURE_VALUE(get_key_F4));
    gravity_class_bind(c, "get_key_F5", NEW_CLOSURE_VALUE(get_key_F5));
    gravity_class_bind(c, "get_key_F6", NEW_CLOSURE_VALUE(get_key_F6));
    gravity_class_bind(c, "get_key_F7", NEW_CLOSURE_VALUE(get_key_F7));
    gravity_class_bind(c, "get_key_F8", NEW_CLOSURE_VALUE(get_key_F8));
    gravity_class_bind(c, "get_key_F9", NEW_CLOSURE_VALUE(get_key_F9));
    gravity_class_bind(c, "get_key_F10", NEW_CLOSURE_VALUE(get_key_F10));
    gravity_class_bind(c, "get_key_F11", NEW_CLOSURE_VALUE(get_key_F11));
    gravity_class_bind(c, "get_key_F12", NEW_CLOSURE_VALUE(get_key_F12));
    // ...
    gravity_class_bind(c, "get_key_DECIMAL", NEW_CLOSURE_VALUE(get_key_DECIMAL));
    gravity_class_bind(c, "get_key_DIVIDE", NEW_CLOSURE_VALUE(get_key_DIVIDE));
    gravity_class_bind(c, "get_key_MULTIPLY", NEW_CLOSURE_VALUE(get_key_MULTIPLY));
    gravity_class_bind(c, "get_key_ADD", NEW_CLOSURE_VALUE(get_key_ADD));
    gravity_class_bind(c, "get_key_EQUAL", NEW_CLOSURE_VALUE(get_key_EQUAL));
    gravity_class_bind(c, "get_key_SUPER", NEW_CLOSURE_VALUE(get_key_SUPER));
    gravity_class_bind(c, "get_key_WIN_MAC_SYMBOL", NEW_CLOSURE_VALUE(get_key_WIN_MAC_SYMBOL));
    gravity_class_bind(c, "get_key_MENU", NEW_CLOSURE_VALUE(get_key_MENU));

    // get_key_down()
    gravity_class_bind(c, "get_key_down_SPACE", NEW_CLOSURE_VALUE(get_key_down_SPACE));
    gravity_class_bind(c, "get_key_down_APOSTROPHE", NEW_CLOSURE_VALUE(get_key_down_APOSTROPHE));
    gravity_class_bind(c, "get_key_down_COMMA", NEW_CLOSURE_VALUE(get_key_down_COMMA));
    gravity_class_bind(c, "get_key_down_MINUS", NEW_CLOSURE_VALUE(get_key_down_MINUS));
    gravity_class_bind(c, "get_key_down_PERIOD", NEW_CLOSURE_VALUE(get_key_down_PERIOD));
    gravity_class_bind(c, "get_key_down_SLASH", NEW_CLOSURE_VALUE(get_key_down_SLASH));
    gravity_class_bind(c, "get_key_down_0", NEW_CLOSURE_VALUE(get_key_down_0));
    gravity_class_bind(c, "get_key_down_1", NEW_CLOSURE_VALUE(get_key_down_1));
    gravity_class_bind(c, "get_key_down_2", NEW_CLOSURE_VALUE(get_key_down_2));
    gravity_class_bind(c, "get_key_down_3", NEW_CLOSURE_VALUE(get_key_down_3));
    gravity_class_bind(c, "get_key_down_4", NEW_CLOSURE_VALUE(get_key_down_4));
    gravity_class_bind(c, "get_key_down_5", NEW_CLOSURE_VALUE(get_key_down_5));
    gravity_class_bind(c, "get_key_down_6", NEW_CLOSURE_VALUE(get_key_down_6));
    gravity_class_bind(c, "get_key_down_7", NEW_CLOSURE_VALUE(get_key_down_7));
    gravity_class_bind(c, "get_key_down_8", NEW_CLOSURE_VALUE(get_key_down_8));
    gravity_class_bind(c, "get_key_down_9", NEW_CLOSURE_VALUE(get_key_down_9));
    gravity_class_bind(c, "get_key_down_SEMICOLON", NEW_CLOSURE_VALUE(get_key_down_SEMICOLON));
    gravity_class_bind(c, "get_key_down_EQUAL", NEW_CLOSURE_VALUE(get_key_down_EQUAL));
    gravity_class_bind(c, "get_key_down_A", NEW_CLOSURE_VALUE(get_key_down_A));
    gravity_class_bind(c, "get_key_down_B", NEW_CLOSURE_VALUE(get_key_down_B));
    gravity_class_bind(c, "get_key_down_C", NEW_CLOSURE_VALUE(get_key_down_C));
    gravity_class_bind(c, "get_key_down_D", NEW_CLOSURE_VALUE(get_key_down_D));
    gravity_class_bind(c, "get_key_down_E", NEW_CLOSURE_VALUE(get_key_down_E));
    gravity_class_bind(c, "get_key_down_F", NEW_CLOSURE_VALUE(get_key_down_F));
    gravity_class_bind(c, "get_key_down_G", NEW_CLOSURE_VALUE(get_key_down_G));
    gravity_class_bind(c, "get_key_down_H", NEW_CLOSURE_VALUE(get_key_down_H));
    gravity_class_bind(c, "get_key_down_I", NEW_CLOSURE_VALUE(get_key_down_I));
    gravity_class_bind(c, "get_key_down_J", NEW_CLOSURE_VALUE(get_key_down_J));
    gravity_class_bind(c, "get_key_down_K", NEW_CLOSURE_VALUE(get_key_down_K));
    gravity_class_bind(c, "get_key_down_L", NEW_CLOSURE_VALUE(get_key_down_L));
    gravity_class_bind(c, "get_key_down_M", NEW_CLOSURE_VALUE(get_key_down_M));
    gravity_class_bind(c, "get_key_down_N", NEW_CLOSURE_VALUE(get_key_down_N));
    gravity_class_bind(c, "get_key_down_O", NEW_CLOSURE_VALUE(get_key_down_O));
    gravity_class_bind(c, "get_key_down_P", NEW_CLOSURE_VALUE(get_key_down_P));
    gravity_class_bind(c, "get_key_down_Q", NEW_CLOSURE_VALUE(get_key_down_Q));
    gravity_class_bind(c, "get_key_down_R", NEW_CLOSURE_VALUE(get_key_down_R));
    gravity_class_bind(c, "get_key_down_S", NEW_CLOSURE_VALUE(get_key_down_S));
    gravity_class_bind(c, "get_key_down_T", NEW_CLOSURE_VALUE(get_key_down_T));
    gravity_class_bind(c, "get_key_down_U", NEW_CLOSURE_VALUE(get_key_down_U));
    gravity_class_bind(c, "get_key_down_V", NEW_CLOSURE_VALUE(get_key_down_V));
    gravity_class_bind(c, "get_key_down_W", NEW_CLOSURE_VALUE(get_key_down_W));
    gravity_class_bind(c, "get_key_down_X", NEW_CLOSURE_VALUE(get_key_down_X));
    gravity_class_bind(c, "get_key_down_Y", NEW_CLOSURE_VALUE(get_key_down_Y));
    gravity_class_bind(c, "get_key_down_Z", NEW_CLOSURE_VALUE(get_key_down_Z));
    gravity_class_bind(c, "get_key_down_LEFT_BRACKET", NEW_CLOSURE_VALUE(get_key_down_LEFT_BRACKET));
    gravity_class_bind(c, "get_key_down_BACKSLASH", NEW_CLOSURE_VALUE(get_key_down_BACKSLASH));
    gravity_class_bind(c, "get_key_down_RIGHT_BRACKET", NEW_CLOSURE_VALUE(get_key_down_RIGHT_BRACKET));
    gravity_class_bind(c, "get_key_down_GRAVE_ACCENT", NEW_CLOSURE_VALUE(get_key_down_GRAVE_ACCENT));
    gravity_class_bind(c, "get_key_down_WORLD_1", NEW_CLOSURE_VALUE(get_key_down_WORLD_1));
    gravity_class_bind(c, "get_key_down_WORLD_2", NEW_CLOSURE_VALUE(get_key_down_WORLD_2));
    gravity_class_bind(c, "get_key_down_ESCAPE", NEW_CLOSURE_VALUE(get_key_down_ESCAPE));
    gravity_class_bind(c, "get_key_down_ENTER", NEW_CLOSURE_VALUE(get_key_down_ENTER));
    gravity_class_bind(c, "get_key_down_TAB", NEW_CLOSURE_VALUE(get_key_down_TAB));
    gravity_class_bind(c, "get_key_down_BACKSPACE", NEW_CLOSURE_VALUE(get_key_down_BACKSPACE));
    gravity_class_bind(c, "get_key_down_INSERT", NEW_CLOSURE_VALUE(get_key_down_INSERT));
    gravity_class_bind(c, "get_key_down_DELETE", NEW_CLOSURE_VALUE(get_key_down_DELETE));
    gravity_class_bind(c, "get_key_down_ARROW_LEFT", NEW_CLOSURE_VALUE(get_key_down_ARROW_LEFT));
    gravity_class_bind(c, "get_key_down_ARROW_RIGHT", NEW_CLOSURE_VALUE(get_key_down_ARROW_RIGHT));
    gravity_class_bind(c, "get_key_down_ARROW_UP", NEW_CLOSURE_VALUE(get_key_down_ARROW_UP));
    gravity_class_bind(c, "get_key_down_ARROW_DOWN", NEW_CLOSURE_VALUE(get_key_down_ARROW_DOWN));
    gravity_class_bind(c, "get_key_down_PAGE_UP", NEW_CLOSURE_VALUE(get_key_down_PAGE_UP));
    gravity_class_bind(c, "get_key_down_PAGE_DOWN", NEW_CLOSURE_VALUE(get_key_down_PAGE_DOWN));
    gravity_class_bind(c, "get_key_down_HOME", NEW_CLOSURE_VALUE(get_key_down_HOME));
    gravity_class_bind(c, "get_key_down_END", NEW_CLOSURE_VALUE(get_key_down_END));
    gravity_class_bind(c, "get_key_down_CAPS_LOCK", NEW_CLOSURE_VALUE(get_key_down_CAPS_LOCK));
    gravity_class_bind(c, "get_key_down_SCROLL_LOCK", NEW_CLOSURE_VALUE(get_key_down_SCROLL_LOCK));
    gravity_class_bind(c, "get_key_down_NUM_LOCK", NEW_CLOSURE_VALUE(get_key_down_NUM_LOCK));
    gravity_class_bind(c, "get_key_down_PRINT", NEW_CLOSURE_VALUE(get_key_down_PRINT));
    gravity_class_bind(c, "get_key_down_PAUSE", NEW_CLOSURE_VALUE(get_key_down_PAUSE));
    gravity_class_bind(c, "get_key_down_F1", NEW_CLOSURE_VALUE(get_key_down_F1));
    gravity_class_bind(c, "get_key_down_F2", NEW_CLOSURE_VALUE(get_key_down_F2));
    gravity_class_bind(c, "get_key_down_F3", NEW_CLOSURE_VALUE(get_key_down_F3));
    gravity_class_bind(c, "get_key_down_F4", NEW_CLOSURE_VALUE(get_key_down_F4));
    gravity_class_bind(c, "get_key_down_F5", NEW_CLOSURE_VALUE(get_key_down_F5));
    gravity_class_bind(c, "get_key_down_F6", NEW_CLOSURE_VALUE(get_key_down_F6));
    gravity_class_bind(c, "get_key_down_F7", NEW_CLOSURE_VALUE(get_key_down_F7));
    gravity_class_bind(c, "get_key_down_F8", NEW_CLOSURE_VALUE(get_key_down_F8));
    gravity_class_bind(c, "get_key_down_F9", NEW_CLOSURE_VALUE(get_key_down_F9));
    gravity_class_bind(c, "get_key_down_F10", NEW_CLOSURE_VALUE(get_key_down_F10));
    gravity_class_bind(c, "get_key_down_F11", NEW_CLOSURE_VALUE(get_key_down_F11));
    gravity_class_bind(c, "get_key_down_F12", NEW_CLOSURE_VALUE(get_key_down_F12));
    // ...
    gravity_class_bind(c, "get_key_down_DECIMAL", NEW_CLOSURE_VALUE(get_key_down_DECIMAL));
    gravity_class_bind(c, "get_key_down_DIVIDE", NEW_CLOSURE_VALUE(get_key_down_DIVIDE));
    gravity_class_bind(c, "get_key_down_MULTIPLY", NEW_CLOSURE_VALUE(get_key_down_MULTIPLY));
    gravity_class_bind(c, "get_key_down_ADD", NEW_CLOSURE_VALUE(get_key_down_ADD));
    gravity_class_bind(c, "get_key_down_EQUAL", NEW_CLOSURE_VALUE(get_key_down_EQUAL));
    gravity_class_bind(c, "get_key_down_SUPER", NEW_CLOSURE_VALUE(get_key_down_SUPER));
    gravity_class_bind(c, "get_key_down_WIN_MAC_SYMBOL", NEW_CLOSURE_VALUE(get_key_down_WIN_MAC_SYMBOL));
    gravity_class_bind(c, "get_key_down_MENU", NEW_CLOSURE_VALUE(get_key_down_MENU));



    // register class c inside VM
    gravity_vm_setvalue(vm, "Input", VALUE_FROM_OBJECT(c));
}

static bee_bool input_set_cursor(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    if (nargs != 2) { throw_error("[Input.set_cursor_visible(bool)] Wrong amount of arguments, 1 argument are needed."); return; }
    gravity_value_t v1 = GET_VALUE(1);
    if (VALUE_ISA_BOOL(v1) == BEE_FALSE)
    {
        throw_error("[Input.set_cursor_visible(bool)] Wrong argument type."); return;
    }

    bee_bool b = VALUE_AS_BOOL(v1);
    set_cursor_visible(b);
}

static bee_bool input_rotate_cam_by_mouse(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 2) { throw_error("[Input.rot_cam_by_mouse(float)] Wrong amount of arguments, 1 argument is needed."); return; }
    gravity_value_t v1 = GET_VALUE(1);
    if (VALUE_ISA_INT(v1) == BEE_FALSE && VALUE_ISA_FLOAT(v1) == BEE_FALSE)
    {
        throw_error("[Input.rot_cam_by_mouse(float)] Wrong argument type."); return;
    }
    if (rotate_cam_by_mouse_act) { printf("already active \n");  return 1; }

    rotate_cam_by_mouse_act = BEE_TRUE;
    rotate_cam_by_mouse_idx = register_mouse_pos_callback(mouse_movement_callback);
    rotate_cam_by_mouse_speed = VALUE_ISA_FLOAT(v1) ? VALUE_AS_FLOAT(v1) : VALUE_AS_INT(v1);
}


// get_key()
static bee_bool get_key_SPACE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_SPACE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Space)), rindex);
}
static bee_bool get_key_APOSTROPHE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_APOSTROPHE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Apostrophe)), rindex);
}
static bee_bool get_key_COMMA(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_COMMA()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Comma)), rindex);
}
static bee_bool get_key_MINUS(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_MINUS()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Minus) || is_key_pressed(KEY_NumpadSubtract)), rindex);
}
static bee_bool get_key_PERIOD(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_PERIOD()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Period)), rindex);
}
static bee_bool get_key_SLASH(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_SLASH()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Slash)), rindex);
}
static bee_bool get_key_0(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_0()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Alpha0) || is_key_pressed(KEY_Numpad0)), rindex);
}
static bee_bool get_key_1(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_1()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Alpha1) || is_key_pressed(KEY_Numpad1)), rindex);
}
static bee_bool get_key_2(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_2()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Alpha2) || is_key_pressed(KEY_Numpad2)), rindex);
}
static bee_bool get_key_3(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_3()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Alpha3) || is_key_pressed(KEY_Numpad3)), rindex);
}
static bee_bool get_key_4(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_4()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Alpha4) || is_key_pressed(KEY_Numpad4)), rindex);
}
static bee_bool get_key_5(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_5()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Alpha5) || is_key_pressed(KEY_Numpad5)), rindex);
}
static bee_bool get_key_6(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_6()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Alpha6) || is_key_pressed(KEY_Numpad6)), rindex);
}
static bee_bool get_key_7(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_7()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Alpha7) || is_key_pressed(KEY_Numpad7)), rindex);
}
static bee_bool get_key_8(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_8()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Alpha8) || is_key_pressed(KEY_Numpad8)), rindex);
}
static bee_bool get_key_9(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_9()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Alpha9) || is_key_pressed(KEY_Numpad9)), rindex);
}
static bee_bool get_key_SEMICOLON(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_SEMICOLON()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_SemiColon)), rindex);
}
static bee_bool get_key_EQUAL(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_EQUAL()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Equal) || is_key_pressed(KEY_NumpadEqual)), rindex);
}
static bee_bool get_key_A(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_A()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_A)), rindex);
}
static bee_bool get_key_B(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_B()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_B)), rindex);
}
static bee_bool get_key_C(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_C()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_C)), rindex);
}
static bee_bool get_key_D(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_D()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_D)), rindex);
}
static bee_bool get_key_E(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_E()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_E)), rindex);
}
static bee_bool get_key_F(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_F)), rindex);
}
static bee_bool get_key_G(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_G()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_F)), rindex);
}
static bee_bool get_key_H(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_H()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_H)), rindex);
}
static bee_bool get_key_I(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_I()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_I)), rindex);
}
static bee_bool get_key_J(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_J()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_J)), rindex);
}
static bee_bool get_key_K(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_K()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_K)), rindex);
}
static bee_bool get_key_L(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_L()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_L)), rindex);
}
static bee_bool get_key_M(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_M()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_M)), rindex);
}
static bee_bool get_key_N(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_N()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_N)), rindex);
}
static bee_bool get_key_O(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_O()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_O)), rindex);
}
static bee_bool get_key_P(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_P()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_P)), rindex);
}
static bee_bool get_key_Q(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_Q()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Q)), rindex);
}
static bee_bool get_key_R(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_R()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_R)), rindex);
}
static bee_bool get_key_S(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_S()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_S)), rindex);
}
static bee_bool get_key_T(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_T()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_T)), rindex);
}
static bee_bool get_key_U(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_U()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_U)), rindex);
}
static bee_bool get_key_V(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_V()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_V)), rindex);
}
static bee_bool get_key_W(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_W()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_W)), rindex);
}
static bee_bool get_key_X(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_X()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_X)), rindex);
}
static bee_bool get_key_Y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_Y()] Wrong amount of arguments, 0 arguments are needed."); }    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Y)), rindex);
}
static bee_bool get_key_Z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_Z()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Z)), rindex);
}
static bee_bool get_key_LEFT_BRACKET(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_LEFT_BRACKET()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_LeftBracket)), rindex);
}
static bee_bool get_key_BACKSLASH(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_BACKSLASH()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Backslash)), rindex);
}
static bee_bool get_key_RIGHT_BRACKET(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_RIGHT_BRACKET()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_RightBracket)), rindex);
}
static bee_bool get_key_GRAVE_ACCENT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_GRAVE_ACCENT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_GraveAccent)), rindex);
}
static bee_bool get_key_WORLD_1(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_WORLD_1()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_World1)), rindex);
}
static bee_bool get_key_WORLD_2(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_WORLD_2()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_World2)), rindex);
}
static bee_bool get_key_ESCAPE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ESCAPE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Escape)), rindex);
}
static bee_bool get_key_ENTER(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ENTER()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Enter) || is_key_pressed(KEY_NumpadEnter)), rindex);
}
static bee_bool get_key_TAB(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_TAB()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Tab)), rindex);
}
static bee_bool get_key_BACKSPACE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_BACKSPACE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Backspace)), rindex);
}
static bee_bool get_key_INSERT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_INSERT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Insert)), rindex);
}
static bee_bool get_key_DELETE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_DELETE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Delete)), rindex);
}
static bee_bool get_key_ARROW_RIGHT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ARROW_RIGHT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_RightArrow)), rindex);
}
static bee_bool get_key_ARROW_LEFT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ARROW_LEFT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_LeftArrow)), rindex);
}static bee_bool get_key_ARROW_DOWN(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ARROW_DOWN()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_DownArrow)), rindex);
}
static bee_bool get_key_ARROW_UP(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ARROW_UP()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_UpArrow)), rindex);
}
static bee_bool get_key_PAGE_UP(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_PAGE_UP()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_PageUp)), rindex);
}
static bee_bool get_key_PAGE_DOWN(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_PAGE_DOWN()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_PageDown)), rindex);
}
static bee_bool get_key_HOME(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_HOME()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Home)), rindex);
}
static bee_bool get_key_END(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_END()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_End)), rindex);
}
static bee_bool get_key_CAPS_LOCK(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_CAPS_LOCK()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_CapsLock)), rindex);
}
static bee_bool get_key_SCROLL_LOCK(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_SCROLL_LOCK()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_ScrollLock)), rindex);
}
static bee_bool get_key_NUM_LOCK(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_NUM_LOCK()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_NumLock)), rindex);
}
static bee_bool get_key_PRINT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_PRINT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_PrintScreen)), rindex);
}
static bee_bool get_key_PAUSE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_PAUSE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Pause)), rindex);
}
static bee_bool get_key_F1(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F1()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_F1)), rindex);
}
static bee_bool get_key_F2(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F2()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_F2)), rindex);
}
static bee_bool get_key_F3(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F3()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_F3)), rindex);
}
static bee_bool get_key_F4(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F4()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_F4)), rindex);
}
static bee_bool get_key_F5(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F5()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_F5)), rindex);
}
static bee_bool get_key_F6(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F6()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_F6)), rindex);
}
static bee_bool get_key_F7(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F7()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_F7)), rindex);
}
static bee_bool get_key_F8(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F8()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_F8)), rindex);
}
static bee_bool get_key_F9(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F9()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_F9)), rindex);
}
static bee_bool get_key_F10(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F10()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_F10)), rindex);
}
static bee_bool get_key_F11(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F11()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_F11)), rindex);
}
static bee_bool get_key_F12(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F12()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_F12)), rindex);
}
// ...
static bee_bool get_key_DECIMAL(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_DECIMAL()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_NumpadDecimal)), rindex);
}
static bee_bool get_key_DIVIDE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_DIVIDE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_NumpadDivide)), rindex);
}
static bee_bool get_key_MULTIPLY(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_MULTIPLY()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_NumpadMultiply)), rindex);
}
static bee_bool get_key_ADD(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ADD()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_NumpadAdd)), rindex);
}
static bee_bool get_key_SHIFT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_SHIFT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_LeftShift) || is_key_pressed(KEY_RightShift)), rindex);
}
static bee_bool get_key_CONTROL(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_CONTROL()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_LeftControl) || is_key_pressed(KEY_RightControl)), rindex);
}
static bee_bool get_key_ALT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ALT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_LeftAlt) || is_key_pressed(KEY_RightAlt)), rindex);
}
static bee_bool get_key_SUPER(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_SUPER()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_LeftSuper) || is_key_pressed(KEY_RightSuper)), rindex);
}
static bee_bool get_key_WIN_MAC_SYMBOL(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_WIN_MAC_SYMBOL()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_LeftSuper) || is_key_pressed(KEY_RightSuper)), rindex);
}
static bee_bool get_key_MENU(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_MENU()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_pressed(KEY_Menu)), rindex);
}

// get_key_down()
static bee_bool get_key_down_SPACE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_SPACE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Space)), rindex);
}
static bee_bool get_key_down_APOSTROPHE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_APOSTROPHE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Apostrophe)), rindex);
}
static bee_bool get_key_down_COMMA(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_COMMA()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Comma)), rindex);
}
static bee_bool get_key_down_MINUS(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_MINUS()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Minus) || is_key_down(KEY_NumpadSubtract)), rindex);
}
static bee_bool get_key_down_PERIOD(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_PERIOD()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Period)), rindex);
}
static bee_bool get_key_down_SLASH(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_SLASH()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Slash)), rindex);
}
static bee_bool get_key_down_0(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_0()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha0) || is_key_down(KEY_Numpad0)), rindex);
}
static bee_bool get_key_down_1(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_1()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha1) || is_key_down(KEY_Numpad1)), rindex);
}
static bee_bool get_key_down_2(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_2()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha2) || is_key_down(KEY_Numpad2)), rindex);
}
static bee_bool get_key_down_3(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_3()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha3) || is_key_down(KEY_Numpad3)), rindex);
}
static bee_bool get_key_down_4(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_4()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha4) || is_key_down(KEY_Numpad4)), rindex);
}
static bee_bool get_key_down_5(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_5()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha5) || is_key_down(KEY_Numpad5)), rindex);
}
static bee_bool get_key_down_6(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_6()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha6) || is_key_down(KEY_Numpad6)), rindex);
}
static bee_bool get_key_down_7(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_7()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha7) || is_key_down(KEY_Numpad7)), rindex);
}
static bee_bool get_key_down_8(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_8()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha8) || is_key_down(KEY_Numpad8)), rindex);
}
static bee_bool get_key_down_9(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_9()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Alpha9) || is_key_down(KEY_Numpad9)), rindex);
}
static bee_bool get_key_down_SEMICOLON(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_SEMICOLON()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_SemiColon)), rindex);
}
static bee_bool get_key_down_EQUAL(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_EQUAL()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Equal) || is_key_down(KEY_NumpadEqual)), rindex);
}
static bee_bool get_key_down_A(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_A()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_A)), rindex);
}
static bee_bool get_key_down_B(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_B()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_B)), rindex);
}
static bee_bool get_key_down_C(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_C()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_C)), rindex);
}
static bee_bool get_key_down_D(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_D()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_D)), rindex);
}
static bee_bool get_key_down_E(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_E()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_E)), rindex);
}
static bee_bool get_key_down_F(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_F)), rindex);
}
static bee_bool get_key_down_G(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_G()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_F)), rindex);
}
static bee_bool get_key_down_H(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_H()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_H)), rindex);
}
static bee_bool get_key_down_I(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_I()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_I)), rindex);
}
static bee_bool get_key_down_J(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_J()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_J)), rindex);
}
static bee_bool get_key_down_K(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_K()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_K)), rindex);
}
static bee_bool get_key_down_L(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_L()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_L)), rindex);
}
static bee_bool get_key_down_M(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_M()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_M)), rindex);
}
static bee_bool get_key_down_N(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_N()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_N)), rindex);
}
static bee_bool get_key_down_O(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_O()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_O)), rindex);
}
static bee_bool get_key_down_P(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_P()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_P)), rindex);
}
static bee_bool get_key_down_Q(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_Q()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Q)), rindex);
}
static bee_bool get_key_down_R(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_R()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_R)), rindex);
}
static bee_bool get_key_down_S(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_S()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_S)), rindex);
}
static bee_bool get_key_down_T(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_T()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_T)), rindex);
}
static bee_bool get_key_down_U(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_U()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_U)), rindex);
}
static bee_bool get_key_down_V(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_V()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_V)), rindex);
}
static bee_bool get_key_down_W(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_W()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_W)), rindex);
}
static bee_bool get_key_down_X(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_X()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_X)), rindex);
}
static bee_bool get_key_down_Y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_Y()] Wrong amount of arguments, 0 arguments are needed."); }    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Y)), rindex);
}
static bee_bool get_key_down_Z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_Z()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Z)), rindex);
}
static bee_bool get_key_down_LEFT_BRACKET(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_LEFT_BRACKET()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_LeftBracket)), rindex);
}
static bee_bool get_key_down_BACKSLASH(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_BACKSLASH()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Backslash)), rindex);
}
static bee_bool get_key_down_RIGHT_BRACKET(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_RIGHT_BRACKET()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_RightBracket)), rindex);
}
static bee_bool get_key_down_GRAVE_ACCENT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_GRAVE_ACCENT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_GraveAccent)), rindex);
}
static bee_bool get_key_down_WORLD_1(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_WORLD_1()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_World1)), rindex);
}
static bee_bool get_key_down_WORLD_2(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_WORLD_2()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_World2)), rindex);
}
static bee_bool get_key_down_ESCAPE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ESCAPE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Escape)), rindex);
}
static bee_bool get_key_down_ENTER(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ENTER()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Enter) || is_key_down(KEY_NumpadEnter)), rindex);
}
static bee_bool get_key_down_TAB(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_TAB()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Tab)), rindex);
}
static bee_bool get_key_down_BACKSPACE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_BACKSPACE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Backspace)), rindex);
}
static bee_bool get_key_down_INSERT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_INSERT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Insert)), rindex);
}
static bee_bool get_key_down_DELETE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_DELETE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Delete)), rindex);
}
static bee_bool get_key_down_ARROW_RIGHT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ARROW_RIGHT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_RightArrow)), rindex);
}
static bee_bool get_key_down_ARROW_LEFT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ARROW_LEFT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_LeftArrow)), rindex);
}
static bee_bool get_key_down_ARROW_DOWN(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ARROW_DOWN()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_DownArrow)), rindex);
}
static bee_bool get_key_down_ARROW_UP(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ARROW_UP()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_UpArrow)), rindex);
}
static bee_bool get_key_down_PAGE_UP(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_PAGE_UP()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_PageUp)), rindex);
}
static bee_bool get_key_down_PAGE_DOWN(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_PAGE_DOWN()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_PageDown)), rindex);
}
static bee_bool get_key_down_HOME(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_HOME()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Home)), rindex);
}
static bee_bool get_key_down_END(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_END()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_End)), rindex);
}
static bee_bool get_key_down_CAPS_LOCK(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_CAPS_LOCK()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_CapsLock)), rindex);
}
static bee_bool get_key_down_SCROLL_LOCK(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_SCROLL_LOCK()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_ScrollLock)), rindex);
}
static bee_bool get_key_down_NUM_LOCK(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_NUM_LOCK()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_NumLock)), rindex);
}
static bee_bool get_key_down_PRINT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_PRINT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_PrintScreen)), rindex);
}
static bee_bool get_key_down_PAUSE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_PAUSE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Pause)), rindex);
}
static bee_bool get_key_down_F1(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F1()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_F1)), rindex);
}
static bee_bool get_key_down_F2(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F2()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_F2)), rindex);
}
static bee_bool get_key_down_F3(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F3()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_F3)), rindex);
}
static bee_bool get_key_down_F4(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F4()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_F4)), rindex);
}
static bee_bool get_key_down_F5(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F5()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_F5)), rindex);
}
static bee_bool get_key_down_F6(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F6()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_F6)), rindex);
}
static bee_bool get_key_down_F7(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F7()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_F7)), rindex);
}
static bee_bool get_key_down_F8(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F8()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_F8)), rindex);
}
static bee_bool get_key_down_F9(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F9()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_F9)), rindex);
}
static bee_bool get_key_down_F10(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F10()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_F10)), rindex);
}
static bee_bool get_key_down_F11(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F11()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_F11)), rindex);
}
static bee_bool get_key_down_F12(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_F12()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_F12)), rindex);
}
// ...
static bee_bool get_key_down_DECIMAL(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_DECIMAL()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_NumpadDecimal)), rindex);
}
static bee_bool get_key_down_DIVIDE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_DIVIDE()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_NumpadDivide)), rindex);
}
static bee_bool get_key_down_MULTIPLY(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_MULTIPLY()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_NumpadMultiply)), rindex);
}
static bee_bool get_key_down_ADD(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ADD()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_NumpadAdd)), rindex);
}
static bee_bool get_key_down_SHIFT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_SHIFT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_LeftShift) || is_key_down(KEY_RightShift)), rindex);
}
static bee_bool get_key_down_CONTROL(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_CONTROL()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_LeftControl) || is_key_down(KEY_RightControl)), rindex);
}
static bee_bool get_key_down_ALT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_ALT()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_LeftAlt) || is_key_down(KEY_RightAlt)), rindex);
}
static bee_bool get_key_down_SUPER(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_SUPER()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_LeftSuper) || is_key_down(KEY_RightSuper)), rindex);
}
static bee_bool get_key_down_WIN_MAC_SYMBOL(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_WIN_MAC_SYMBOL()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_LeftSuper) || is_key_down(KEY_RightSuper)), rindex);
}
static bee_bool get_key_down_MENU(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex)
{
    if (nargs != 1) { throw_error("[Input.get_key_MENU()] Wrong amount of arguments, 0 arguments are needed."); }
    RETURN_VALUE(VALUE_FROM_BOOL(is_key_down(KEY_Menu)), rindex);
}
