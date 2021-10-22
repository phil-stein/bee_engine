Todo

- general
  - [ ] setup release build

  - [ ] setup x86 build

  - [ ] parent child rotation (vqs or v, mat, s) 

    - [x] basics

    - [ ] child rotation on axis rotated by parent

      -> decompose rotation in "get_entity_global_transform()"

      -> maybe look at glm (not cglm) repo for example

    - [ ] recursive, multiple childs affecting each other

      -> above needs to be finished

  - [ ] turn dir vec into rotation and reverse

    - [ ]  rot -> dir (kinda works)
    - [ ] dir -> rot (kinda works)
    
  - [ ] make mesh in entity pointer

    

- buggs

  - [ ] search for memory leaks & corruption using tool 

  - [ ] disabling msaa makes cube map be drawn over scene and the vertex info of the meshes gets all screwy  

    -> analyze using RenderDoc

  - [ ] first object in mouse_pick_buffer is rendered behind the others 

    -> analyze using RenderDoc

    

- unfinished

  - [ ] make_grid_mesh()

  - [ ] mixing of the different light sources looks weird in ambient areas & shadows

    -> prob. wait till after implementing pbr

  - [ ] search function in asset browser

  - [ ] in-game ui doesnt scroll or take keyboard input while editor is act

    -> because editor ui ctx is in focus

  - [ ] revamp the uniform system to be less cumbersome

    -> directly use uniform from shader, without having to add them in the material as well

  - [ ] move gizmo take camera rotation into account

  - [ ] make first entity be right-click-able & drag-able

  - [ ] themes

    - [ ] light blue theme
      - [ ] different color for node tab / tree tab 
    - [ ] make themes affect gizmo color

  - [ ] debug draw functions

    - [x] sphere
    - [x] cube
    - [ ] line
    - [ ] line cube
    - [ ] text
      - [ ] text renderer "stb_truetype.h"
      - [ ] transform to world pos (scale by z pos
      - [ ] make this available to rest of engine, for f.e. player names above their heads

    

- [ ] scene system
  - [ ] serialization

    - [ ] add cubemap  to scene header

      

- graphics
  - [ ] sort geometry of transparent objects
  
  - [ ] geometry shader normal direction
  
  - [ ] shadows
  
    - [x] shadow mapping
    - [ ] point shadows
    - [ ] cascaded shadow mapping
    
  - [ ] parallax mapping
  
  - [ ] bloom (as in the cherno video)
  
  - [ ] deferred shading ?
  
  - [ ] ssao
  
  - [ ] pbr
  
    - [ ] lighting
    - [ ] diffuse irradiance
    - [ ] specular ibl
    
    
  
- editor
  - [ ] asset manager
    - internal assets
      -  [ ] automate the internal asset recognition
    - [ ] make assets on import be put in the right folder
    - [x] add ui for drag&drop importing
    - [ ] removing assets
    - [ ] add cube map assets
    - [ ] add font assets for nuklear
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

  

- [ ] gravity scripting

  - [ ] load_file delegate for #import 

  - [ ] delegate for 'System.print()'

  - [ ] interface to create meshes / other components to create models etc in gravity

  - [ ] interface for nuklear

    - [ ] float property

    - [ ] color wheel

    - [ ] groups

    - [ ] set style

      - [ ] through functions or style-sheets ?

        -> both prob.




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
  - [ ] use uniform buffers for matrices and maybe lights
  - [ ] use proper sorting algorithm for transparent objects
  - [ ] take a look at normal-mapping chapter again and use better option
  - [ ] instanced rendering / batching
  - [ ] multithreading ?