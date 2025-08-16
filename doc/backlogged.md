
Important stuff

- [x] MEMORY LEAKS !!!!
- [x] documentation of the 3dful module
- [ ] more documentation ! for the lisilisk interface & internals !
- [x] Detect channels (rgb / rgba) set in surfaces to set the correct value when we load the texture on GPU
- [x] Include the engine-specific resources within the executable
- [ ] The engine should have some error procedure that fires when the user provides incorrect arguments
- [ ] Add the possibility to pass arbitrary uniforms through materials.

Half implemented

- [ ] Lights are expressed with vec4. Make the last componenent the strength of the light.
- [ ] Make the fog have an alpha component, and make it interfere with the sky.
- [x] Add flags to the geometry to decide how it is rendered : culling, smooth, etc
- [ ] Implement custom textures passing to material shader
- [x] Fog has been deactivated, reactivate it
- [ ] ~~Allow the user to set custom shaders on a model~~

Refactor ideas

- [x] Set the handle id counter to be model-stored rather than static
- [x] Add more coherence for when we pass pointers vs. when we pass values
- [x] Add null-detection when the state of the data passed is acceptable (e.g. a scene without environment)
- [x] Change skybox shape from cube to ~~icosphere~~ UV sphere
- [ ] Add null-checks at the lowest levels of the functions
- [ ] Separate light procedures in material fragment shader to clean up the clutter
- [ ] The shader_material_*() interface is as coupled to the material struct as the model struct. Find a way to express it
- [ ] Resourceful module should use the same IO functions as the rest of the codebase
- [ ] Resourceful top level struct has no need to be opaque

- [ ] BIG THINK NEEDED Abstract the stores to one simple module

- [ ] Explain in the file headers the principles of design behind the data structures/interfaces
