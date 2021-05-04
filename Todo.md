Todo

- general
  - [ ] setup release build
  - [ ] setup x86 build
- buggs
  - [x] loading images is really slow
  - [x] vm corrupts between run_init() and run_update()
  
- [ ] gravity scripting
  - [x] game is slow as h***
  - [x] camera interface
  - make it not crash when
    - [x] .gravity file not found
    - [x] .gravity file has errors
    - [ ] wrong argument passed to func
    - [ ] wrong argument amount
  - [ ] entity get interface (World -> world.get_entity("name"))
  - [ ] finish input interface
    - [ ] all keys
    - [ ] get_key() & get_key_down()
  - [ ] make attach script not be a popup but a window
  
- [ ] in-engine text editor
  - [ ] view source
  - [ ] edit source
  - [ ] syntax highlighting
  
- [ ] play-mode
  - [ ] make it not be clear-color
  - [ ] pause / start scripts
  - [x] ui
- [ ] physics-system

  - [ ] aabb v aabb
  - [ ] aabb v sphere
  - [ ] aabb v plane
- [ ] scene system

  - [ ] serialization
  - [ ] deserialization
  - [ ] switching
- [ ] particle system
- [ ] optimization
  - [ ] use uniform buffers for matrices and maybe lights