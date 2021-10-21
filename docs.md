# bee engine documentation <br>

_**[ !!! still being written !!! ]**_

## table
  1. [general](https://github.com/phil-stein/bee_engine/blob/main/docs.md#general-)
  2. [gravity (scripting)](https://github.com/phil-stein/bee_engine/blob/main/docs.md#gravity-)
  3. [tweaking the engine](https://github.com/phil-stein/bee_engine/blob/main/docs.md#tweaking_the_engine-)
 
 ## general <br>
 bee engine is a 3d game engine, probably used best for less intense projects and defenitely don't use it for commercial purposes. 
 
 
 ## gravity <br>
 I didn't make the gravity runtimeinterpreter, it can be found [here](https://github.com/marcobambini/gravity). the creators of gravity also have provided [documentation](https://marcobambini.github.io/gravity/#/README) for the swift like language which i encourage you take a look at before reading further here about bee engine specific functions and what they do.
 
 as gravity does dynamic typing, aka. the variables seemingly have no type, when *number* is given as an argument or return type, that means integer or floating-point will work.
 
 ### table
 1. example
 2. entity interface
 3. world interface
 4. input interface
 5. game interface
 6. ui interface
 7. collision interface
 
 ### example <br>
 ```swift
 // import the diffent interfaces for the engine
extern var Entity; 
extern var World;
extern var Game;
extern var Input;
extern var UI;

// variables, dynamically typed
var check;
var btn = false;

// called once at startup
func init()
{
	System.print("hello, world");
}

// called every frame
func update()
{
	// these vars expose the functions hooked into bee engine
	var this  = Entity();
	var world = World();
	var game  = Game();
	var input = Input();
	var ui = UI();

	// ---- code ----
	var enemy = world.get_entity("enemy");
	if (input.get_key_ENTER())
	{
		var speed = 2 * game.get_delta_t();
		this.move_y(speed);
		world.move_y(enemy, speed);
		game.load_level("good_level");
	}
	
	// in game "immmediate mode ui", wrapper for nuklear ui
	ui.begin();

	if (ui.window_begin("in game nuklear ui", 900, 10, 300, 250))
	{
		ui.layout(25, 1);
		if (ui.button("button"))
		{
			btn = !btn;
		}
		if (btn)
		{
			ui.layout(25, 2);
			ui.text("checkbox describtion");
			check01 = ui.checkbox("option a", check);
			ui.layout(25, 1);
		}
	} ui.window_end();

	ui.end();
  
}
 ```
 
 ### entity interface <br>
 brief overview
 ```swift
 extern var Entity(); // import
 func init()
 {
	var this = Entity(); // accessing
	this.move_x(1); 	 // usage
 }
 ```

#### get_x
*number Entity.get_x()* <br>
get the position on the x axis of the entity the script is attached to.
 
#### get_y
*number Entity.get_y()* <br>
get the position on the y axis of the entity the script is attached to.
 
#### get_z
*number Entity.get_z()* <br>
get the position on the z axis of the entity the script is attached to.
 
#### move_x
*Entity.move_x(number distance)* <br>
moves the entity the script is attached to by the distance given on the x axis.

#### move_y
*Entity.move_y(number distance)* <br>
moves the entity the script is attached to by the distance given on the y axis.

#### move_z
*Entity.move_z(number distance)* <br>
moves the entity the script is attached to by the distance given on the z axis.

#### get_rot_x
*number Entity.get_rot_x()* <br>
get the rotation on the x axis of the entity the script is attached to, in degree.

#### get_rot_y
*number Entity.get_rot_y()* <br>
get the rotation on the y axis of the entity the script is attached to, in degree.

#### get_rot_z
*number Entity.get_rot_z()* <br>
get the rotation on the z axis of the entity the script is attached to, in degree.

#### rot_x
*Entity.rot_x(number degree)* <br>
rotates the entity the script is attached to by the degrees given on the x axis.
 
#### rot_y
*Entity.rot_y(number degree)* <br>
rotates the entity the script is attached to by the degrees given on the y axis.
 
#### rot_z
*Entity.rot_z(number degree)* <br>
rotates the entity the script is attached to by the degrees given on the y axis.

#### get_scale_x
**not finished**
*number Entity.get_scale_x()* <br>
get the scale of the entity the script is attached to on the x axis.

#### get_scale_y
**not finished**
*number Entity.get_scale_y()* <br>
get the scale of the entity the script is attached to on the y axis.

#### get_scale_z
**not finished**
*number Entity.get_scale_z()* <br>
get the scale of the entity the script is attached to on the z axis.

#### scale_x
**not finished**
*Entity.scale_x(number degree)* <br>
scale the entity the script is attached to by the degrees given on the x axis.

#### scale_y
**not finished**
*Entity.scale_y(number degree)* <br>
scale the entity the script is attached to by the degrees given on the y axis.

#### scale_z
**not finished**
*Entity.scale_z(number degree)* <br>
scale the entity the script is attached to by the degrees given on the z axis.

 
 
 ### world interface <br>
 brief overview
 ```swift
 extern var World(); // import
 func init()
 {
	var world = World(); 		// accessing
	world.get_entity("entity");	// usage
 }
 ```
 
 
 
 
 
 
 
