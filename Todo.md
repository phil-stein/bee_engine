Todo

- general
  - [ ] setup release build
  
  - [ ] setup x86 build
  
  - [ ] turn dir vec into rotation and reverse
  
    - [ ]  rot -> dir (kinda works)
    - [ ] dir -> rot (kinda works)
    
  - [ ] split renderer and entity-stuff to two separate file
  
    
  
- buggs

  - [ ] interacting with a popup means you cant click on the window anymore till you click on another window

  - [ ] importing assets sometimes crashes the editor

  - [ ] gravity scripts sometimes cant compile since adding the load_file delegate

  - [ ] dark areas look weird since adding gamma correction

  - [ ] added entities assume taken id's

  - [ ] disabling msaa makes cube map be drawn over scene and the vertex info of the meshes gets all screwy  

  - [ ] mixing of the different light sources looks weird in ambient areas & shadows

    

- unfinished

  - [ ] ...

    

- [ ] scene system
  - [ ] serialization
    - [ ] add cubemap & settings to scene header
  - [ ] ui
    - [ ] better saving & loading ui (double click for loading scene / instancing mesh)



- graphics
  - [ ] sort geometry of transparent objects
  
  - [ ] geometry shader normal direction
  
  - [x] anti-aliasing
  
  - [x]  blinn-phong
  
  - [x] gamma correction
  
  - [ ] shadows
  
    - [x] shadow mapping
    - [ ] point shadows
    - [ ] cascaded shadow mapping
    
  - [x] normal mapping
  
  - [ ] parallax mapping
  
  - [x] hdr
  
  - [ ] bloom (as in the cherno video)
  
  - [ ] deferred shading ?
  
  - [ ] ssao
  
    
  
- editor
  - [ ] asset manager
    - internal assets
      -  [x] default assets (missing texture, ...)
      -  [ ] automate the internal asset recognition
    - [ ] make assets on import be put in the right folder
    - [x] use shdefault() & hmdefault() to check if the requested asset exists
    - [ ] add ui for drag&drop importing
    - [ ] removing assets
    
  - [ ] hot-reload assets 
  
    - [x] shaders
    - [ ] meshes
    - [ ] scripts
  
  - [ ] !!! warn when adding f.e. a material with a name that already exists

  - [ ] search function in asset manager ui
  
  - [ ] editor cam v. game cam (camera component)
  
    - [ ] assure always one and only one camera in scene
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

  - [x] 'reset' script when pausing game-mode, i.e. rerun the init func each time you press play

  - [x] interface to instance objects 

  - [ ] interface to create meshes / other components to create models etc in gravity


  

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