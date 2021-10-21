# bee engine documentation <br>

_**[ !!! still being written !!! ]**_

## table
  1. [general](https://github.com/phil-stein/bee_engine/blob/main/docs.md#general-)
  2. [gravity (scripting)](https://github.com/phil-stein/bee_engine/blob/main/docs.md#gravity-)
  3. [tweaking the engine](https://github.com/phil-stein/bee_engine/blob/main/docs.md#tweaking_the_engine-)
 
 ## general <br>
 bee engine is a 3d game engine, probably used best for less intense projects and defenitely don't use it for commercial purposes. 
 
 
 ## gravity <br>
 I didn't make the gravity runtimeinterpreter, it can be found [here](https://github.com/marcobambini/gravity). The creators of gravity also have provided [documentation](https://marcobambini.github.io/gravity/#/README) for the swift like language which i encourage you take a look at before reading further here about bee engine specific functions and what they do.
 
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
extern var Entity;
extern var World;
extern var Game;
extern var Input;
extern var UI;

var check;
var btn = false;

func init()
{
  System.print("hello, wolrd");
}

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
    this.move_y(1.0);
    world.move_y(enemy, 1.0);
    game.load_level("good_level");
  }

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
 
