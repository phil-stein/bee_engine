# Todo

- general
  - [ ] setup release build

  - [ ] setup x86 build

  - [ ] parent child rotation (vqs or v, mat, s) 

    - [x] basics

    - [x] child rotation on axis rotated by parent

      -> decompose rotation in "get_entity_global_transform()"

      -> maybe look at glm (not cglm) repo for example

    - [x] recursive, multiple childs affecting each other

      -> above needs to be finished

  - [ ] turn dir vec into rotation and reverse

    - [ ]  rot -> dir (kinda works)
    - [ ] dir -> rot (kinda works)
    
  - [ ] make entity id's guid's 
  
  - [x] write debugging / performance testing tools
  
    - [x] timer
    
    
  
- buggs

  - [ ] search for memory leaks & corruption using tool 

  - [ ] disabling msaa makes cube map be drawn over scene and the vertex info of the meshes gets all screwy  

    -> analyze using RenderDoc

  - [ ] first object in mouse_pick_buffer is rendered behind the others 

    -> analyze using RenderDoc

  - [ ] deserialization requests material with name "" 

  - [ ] the shader pointer in material gets set to null somewhere

    -> pointer address changes when stb_ds automatically reallocs more space to contain the incoming shaders, i.e. after 8 it reallocs to 16, possibly changing the location of the array entirely

    

- unfinished

  - [x] add missing asset-types to "check_asset_exists()", etc.

  - [x] make_grid_mesh()

  - [ ] mixing of the different light sources looks weird in ambient areas & shadows

    -> prob. wait till after implementing pbr

  - [ ] search function in asset browser

  - [ ] in-game ui doesnt scroll or take keyboard input while editor is act

    -> because editor ui ctx is in focus

  - [ ] revamp the uniform system to be less cumbersome

    -> directly use uniform from shader, without having to add them in the material as well

    -> maybe make files for shaders specifing vert / frag file and uniforms

  - [ ] move gizmo take camera orientation into account

  - [ ] make first entity be right-click-able & drag-able
  
  - [ ] themes
  
    - [ ] light blue theme
      - [ ] different color for node tab / tree tab 
    - [ ] make themes affect gizmo color
  
  - [ ] debug draw functions
  
    - [x] sphere
    - [x] cube
    - [x] line
    - [ ] proper line drawing using glDrawArrays(GL_LINE, ...)
    - [ ] line cube
    - [ ] text
      - [ ] text renderer "stb_truetype.h"
      - [ ] transform to world pos (scale by z pos
      - [ ] make this available to rest of engine, for f.e. player names above their heads




- graphics
  
  
  - [ ] sorting transparent objects properly
    
  - [ ] sort geometry of transparent objects
  
  - [ ] geometry shader normal direction
  
  - [ ] shadows
  
    - [x] shadow mapping
    - [ ] point shadows
    - [ ] cascaded shadow mapping (in guest articles on learnopengl.com)
  
  - [ ] parallax mapping ?
  
  - [ ] bloom (as in the cherno video)
  
  - [ ] deferred shading ?
  
  - [ ] ssao
  
  - [ ] pbr
  
    - [ ] lighting
    - [ ] diffuse irradiance
    - [ ] specular ibl
  
    
  
- editor
  - [ ] asset manager
    
    - [ ]  switch name to guid as id
    - internal assets
      -  [ ] automate the internal asset recognition
    - [ ] make assets on import be put in the right folder
    - [x] add ui for drag&drop importing
    - [ ] removing assets
    - [ ] add cube map assets
    - [ ] add font assets for nuklear / stb_truetype
    - [ ] reload assets 
      - [x] shaders
      - [ ] meshes
      - [ ] scripts
    - [ ] hot-reloading (when files edited)
    
  - [ ] !!! warn when adding f.e. a material with a name that already exists

  - [x] ui for drag & drop imported assets

  - [ ] editor cam v. game cam (camera component)
  
    - [x] assure always one and only one camera in scene
    - [ ] integrate in gravity interface

  - [ ] visualize normal-direction

  - [ ] show shader source

  - [ ] axis view gizmo like top-right in blender, unity, max, etc

  - [ ] transform gizmos
  
    - [x] move
    - [ ] scale
    - [ ] rotate
    - [ ] snap to grid
  
  - [ ] show grid in scene
  
  - [ ] structures (prefabs)
  
    - [ ] serialize entity tree
    - [ ] save & load
    - [ ] asset manager
    - [ ] ui
  
  - [x] ui for adding / removing components
  
    - [x] mesh & material
    - [x] rigidbody
    - [x] collider
  
  - [x] make entity hierarchy / properties & scene properties groups, so play button doesn't get scrolled up when scrolling entity properties, etc.
  
    
  
- [ ] input 

  - [ ] controller input

    

- [ ] audio

  - [ ] load file

  - [ ] play clip

  - [ ] loop

  - [ ] 3d audio

  - [ ] mixing

  - [ ] reverb, pitch and that sort of stuff ( i is expert)

    

- [ ] gravity scripting

  - [ ] load_file delegate for #import 

  - [ ] delegate for 'System.print()'

  - [ ] interface to create meshes / other components to create models etc in gravity

  - [ ] interface for nuklear

    - [x] float property

    - [ ] color wheel

      -> would need me to return custom object, vec3 / color

    - [x] groups

    - [ ] set style

      - [ ] text
      - [ ] main-color
      - [ ] highlight
      - [ ] inactive
      - [ ] background




- [ ] physics-system

  - [ ] aabb v sphere

  - [ ] add collision point to "collision_info" ?

  - [ ] resolution
    - [x] sphere v sphere
    
    - [ ] aabb v aabb // kinda, very kinda

      -> prob. use 3d elastic collision newtonian 
    
    - [ ] aabb v sphere
    
  - [ ] GJK

  - [ ] EPA

    

- [ ] particle system
  - [ ] object pooling
  
  - [ ] instancing 
  
  - [ ] textured & colored (also transparent)
  
  - [ ] billboards (no cam projection) 
  
    
  
- [ ] optimization
  
  
  - [x] make mesh in entity pointer
  - [ ] use uniform buffers for matrices and maybe lights
  - [ ] use proper sorting algorithm for transparent objects
  - [ ] take a look at normal-mapping chapter again and use better option
  - [ ] instanced rendering / batching
  - [ ] multithreading