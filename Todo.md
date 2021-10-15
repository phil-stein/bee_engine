Todo

- general
  - [ ] setup release build
  
  - [ ] setup x86 build
  
  - [ ] parent child rotation (vqs or v, mat, s)
  
  - [ ] turn dir vec into rotation and reverse
  
    - [ ]  rot -> dir (kinda works)
    - [ ] dir -> rot (kinda works)
    
    
    
  
- buggs

  - [x] dark areas look weird since adding gamma correction

  - [ ] disabling msaa makes cube map be drawn over scene and the vertex info of the meshes gets all screwy  

  - [x] world.add_entity() sometimes freezes editor, prob in ui -> entity hierarchy

    -> only happens when editor ui is open

  - [x] adding camera can freeze the editor

    -> in scene "default"

  - [ ] first object in mouse_pick_buffer is rendered behind the others 

  - [x] keyboard input in ui doesn't work since adding gravity ui

  - [x] "scripts over bounds" in properties_window() ->  scripts

  - [x] entity hierarchy type is wrong 

  - [ ] shader name got swapped with a script name 

    -> after dividing properties_window() into sub-functions

    -> happened after recompile

    

- unfinished

  - [ ] mixing of the different light sources looks weird in ambient areas & shadows

    -> prob. wait till after implementing pbr

  - [ ] search function in asset browser

  - [ ] object outline 

  - [ ] in-game ui doesnt scroll or take keyboard input while editor is act

    -> because editor ui ctx is in focus
    
  - [ ] divide big ui functions into sub-functions

  - [x] mouse over ui for entity mouse selection

  - [ ] entities.h function comments

  - [ ] renderer.h function comments 

  - [ ] gravity_interface.h function comments

  - [ ] gravity_interface_ui.h function comments

  - [ ] look for more uncommented functions

    

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
  
  - [ ] transform gizmos
  
  - [ ] structures (prefabs)
  
    - [ ] serialize entity tree
    - [ ] save & load
    - [ ] asset manager
    - [ ] ui
  
    
  
- [ ] input 

  - [ ] controller input

  

- [ ] gravity scripting

  - [ ] finish input interface
    - [ ] controller interface
    - [ ] actions like unity ???

  - [ ] load_file delegate for #import 

  - [ ] delegate for 'System.print()'

  - [ ] interface to create meshes / other components to create models etc in gravity

  - [ ] interface for nuklear

    - [x] window

    - [x] layout

    - [x] label

    - [x] button

    - [x] image

    - [ ] checkbox

    - [ ] float property

    - [ ] color wheel

    - [ ] groups

    - [ ] set style

      - [ ] through functions or style-sheets ?

        -> both prob.




- [ ] physics-system

  - [ ] aabb v aabb
  
  - [ ] aabb v sphere
  
  - [ ] aabb v plane
  
    
  
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