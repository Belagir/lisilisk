
Important stuff

- [x] MEMORY LEAKS !!!!
- [x] documentation
- [x] Detect channels (rgb / rgba) set in surfaces to set the correct value when we load the texture on GPU

Half implemented

- [ ] Lights are expressed with vec4. Make the last componenent the strength of the light.
- [ ] Add flags to the geometry to decide how it is rendered : culling, smooth, etc
- [ ] Implement custom textures passing to material shader
- [ ] Fog has been deactivated

Refactor ideas

- [x] Add more coherence for when we pass pointers vs. when we pass values
- [x] Add null-detection when the state of the data passed is acceptable (e.g. a scene without environment)
- [x] change skybox shape from cube to ~~icosphere~~ UV sphere
- [ ] Separate light procedures in material fragment shader to clean up the clutter
- [ ] The shader_material_*() interface is as coupled to the material struct as the model struct. Find a way to express it

- [ ] Explain in the file headers the principles of design behind the data structures/interfaces
