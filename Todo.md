Todo

- general
  - [ ] setup release build
  
  - [ ] setup x86 build
  
    
  
- buggs
  - [x] loading images is really slow
  
  - [x] vm corrupts between run_init() and run_update()
  
  - [x] editor [show source code] cuts off the last line 
  
  - [x] solid objects don't get rendered in build_mode
  
  - [x] ~5-10 fps in build_mode *(prop. something being called that's unnecessary without the editor)*
  
  - [x] add entity >> cube doesn't get rendered
  
  - [x] attach script doesn't work anymore 
  
  - [x] name & path of textures get randomly freed [prob. fixed]
  
  - [x] new camera component sometimes frees values
  
  - [ ] importing assets sometimes crashes the editor
  
  - [ ] gravity scripts sometimes cant compile since adding the load_file delegate
  
  - [x] 'upper_hierachy_ents' sometimes crashes the engine when sorting the hierarchy
  
  - [ ] saving scenes corrupts the program, probably in write_text_file()
  
  - [x] editor starts in play-mode for some reason
  
  - [x] after changing scene scripts dont work anymore
  
  - [ ] crash when exiting play mode sometimes (in update_entity())
  
    
  
- unfinished
  - [ ] entity_remove() in renderer.c
  
  - [ ] entity_switch_light_type() in renderer.c
  
- [ ] scene system
  - [ ] serialization
    - [x] integer, float, u32, bool
    - [x] string (integer length & char's)
    - [x] texture, shader
    - [x] components
    - [x] entities
    - [ ] scene
      - [x] make scene struct
      - [x] shader & material serialization
      - [x] entities
      - [ ] add cubemap & use_msaa to scene header
  - [x] saving scenes
  - [x] loading scenes
  - [x] shaders still get duplicated on deserialization
  - [x] switching
  - [x] make scene deserialization check if material / shader already exists 
  - [ ] ui
    - [x] saving & loading
    - [ ] better saving & loading ui (double click for loading scene / instancing mesh)
    - [x] add scenes to the asset manager
  - [x] gravity interface
  



- [ ] material / shader system

  - allow for different shaders to get passed their own needed uniform values

  - [ ] save the uniforms in a file
  - [ ] uniform array in material that gets send one after another



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
    - [x] register all assets
    - [x] load & store on demand
    - internal assets
      -  [x] default assets (missing texture, ...)
      -  [ ] automate the internal asset recognition
    - [ ] make assets on import be put in the right folder
    -  [x] use shdefault() & hmdefault() to check if the requested asset exists
    -  [ ] add ui for drag&drop importing
    
  - [x] use different datastructure for entities, with GUID's  
  
  - [ ] !!! warn when adding f.e. a material with a name that already exists
  
  - [ ] search function in asset manager ui
  
  - [ ] editor cam v. game cam (camera component)
    - [x] switching between editor / in-game camera
    - [x] camera gizmo
    - [x] getting the view-matrix
    - [ ] assure always one and only one camera in scene
    - [ ] integrate in gravity interface
    
  - [x] make the gizmos be drawn in renderer separately instead of mesh components
  
  - [x] make the scene get serialized before playing and then reset when stopped 
  
  - [x] add entity
  
  - [x] deselect entities
  
  - [ ] visualize normal-direction
  
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
  - [x] game is slow as h***
  
  - [x] camera interface
  
  - make it not crash when
    - [x] .gravity file not found
    - [x] .gravity file has errors
    - [x] wrong argument passed to function
    - [x] wrong argument amount
    
  - [x] this entity interface (`Entity -> this.rot_x()`)
  
  - [x] other entity interface (`World -> world.get_entity("name")`)
  
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
  
  - [x] viewability
  
  - [ ] syntax highlighting
  
  - [ ] edit source ???
  
    
  
- [x] play-mode
  - [x] make it not be clear-color
  
  - [x] pause / start scripts
  
  - [x] ui
  
    
  
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