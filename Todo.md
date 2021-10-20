Todo

- general
  - [ ] setup release build
  
  - [ ] setup x86 build
  
  - [ ] parent child rotation (vqs or v, mat, s)
  
  - [ ] turn dir vec into rotation and reverse
  
    - [ ]  rot -> dir (kinda works)
    - [ ] dir -> rot (kinda works)
    
  - [ ] make mesh pointer
  
    
  
- buggs

  - [ ] search for memory leaks & corruption using tool 

  - [ ] disabling msaa makes cube map be drawn over scene and the vertex info of the meshes gets all screwy  

    -> analyze using RenderDoc

  - [ ] first object in mouse_pick_buffer is rendered behind the others 

    -> analyze using RenderDoc

  - [ ] normal mapped occasionally turn mostly white

    -> after removing entities and serializing

    -> prob. something todo with deserialization

    -> camera id is not its id_idx
    
    

- unfinished

  - [ ] mixing of the different light sources looks weird in ambient areas & shadows

    -> prob. wait till after implementing pbr

  - [ ] search function in asset browser

  - [ ] in-game ui doesnt scroll or take keyboard input while editor is act

    -> because editor ui ctx is in focus
    
  - [ ] divide big ui functions into sub-functions

  - [ ] revamp the uniform system to be less cumbersome

  - [ ] move gizmo take camera rotation into account

  - [ ] make first entity be right-click-able & dragable

  - [ ] themes

    - [ ] light blue theme
    - [ ] make themes affect gizmo color
    
  - [ ] debug draw functions

    - [x] sphere
    - [x] cube
    - [ ] line
    - [ ] line cube

    

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

  - [ ] structures (prefabs)

    - [ ] serialize entity tree
    - [ ] save & load
    - [ ] asset manager
    - [ ] ui

  - [ ] ui for adding / removing components

    - [ ] mesh & material
    - [ ] rigidbody
    - [ ] collider

    

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
  - [x] sphere v sphere

  - [x] dynamics

  - [x] aabb v aabb

  - [ ] aabb v sphere

  - [x] trigger

  - [x] gravity interface
  
  - [x] make functions for rb and collider
  
  - [ ] resolution
    - [x] sphere v sphere
    - [ ] aabb v aabb // kinda, very kinda
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