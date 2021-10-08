Todo

- general
  - [ ] setup release build
  
  - [ ] setup x86 build
  
  - [ ] turn dir vec into rotation and reverse
  
    
  
- buggs

  - [ ] importing assets sometimes crashes the editor

  - [ ] gravity scripts sometimes cant compile since adding the load_file delegate

  - [x] 'upper_hierachy_ents' sometimes crashes the engine when sorting the hierarchy

  - [ ] saving scenes corrupts the program, probably in write_text_file()

  - [ ] dark areas look weird since adding gamma correction

    

- unfinished
  - [x] entity_remove() in renderer.c
  
  - [x] entity_switch_light_type() in renderer.c
  
- [ ] scene system
  - [ ] serialization
    - [ ] add cubemap & use_msaa to scene header
  - [ ] ui
    - [x] saving & loading
    - [ ] better saving & loading ui (double click for loading scene / instancing mesh)
    - [x] add scenes to the asset manager
  - [x] gravity interface



- [ ] material / shader system

  - allow for different shaders to get passed their own needed uniform values
  - [x] save uniform in shader
  - [x] uniform array in material that gets send one after another
  - [ ] @UNCLEAR: move uniforms to material
    - save the allowed uniforms in shader and the values in material



- graphics
  - [ ] sort geometry of transparent objects
  
  - [ ] geometry shader normal direction
  
  - [x] anti-aliasing
  
  - [x]  blinn-phong
  
  - [x] gamma correction
  
  - [ ] shadows
  
    - [ ] shadow mapping
    - [ ] point shadows
    - [ ] cascaded shadow mapping
    
  - [ ] normal mapping
  
  - [ ] parallax mapping
  
  - [ ] hdr
  
  - [ ] bloom
  
  - [ ] deferred shading
  
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
    
      
    
  - [ ] !!! warn when adding f.e. a material with a name that already exists

  - [ ] search function in asset manager ui

  - [ ] editor cam v. game cam (camera component)

    - [ ] assure always one and only one camera in scene
    - [ ] integrate in gravity interface

  - [ ] visualize normal-direction

  - [ ] material / asset editor in asset manager

  - [ ] show shader source

  - [ ] transform gizmos

  - [ ] structures (prefabs)

    - [ ] serialize entity tree
    - [ ] save & load
    - [ ] asset manager
    - [ ] ui

    

- [ ] input 
  - [x] mouse input 
  - [ ] controller input
  
  
  
- [ ] gravity scripting

  - [ ] finish input interface
    - [x] all keys
    - [x] get_key() & get_key_down()
    - [ ] mouse interface
    - [ ] controller interface
    - [ ] actions like unity ???
    
  - [ ] load_file delegate for #import 

  - [ ] delegate for 'System.print()'

  - [ ] 'reset' script when pausing game-mode, i.e. rerun the init func each time you press play

  - [ ] interface to instance objects 

  - [ ] interface to create meshes / other components to create models etc in gravity

    

- [ ] in-engine text editor
  - [x] view source
  
  - [ ] syntax highlighting
  
  - [ ] edit source ???
  
  
  
  
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