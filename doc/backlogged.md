
- [ ] Add more coherence for when we pass pointers vs. when we pass values
- [ ] Detect channels (rgb / rgba) set in surfaces to set the correct value when we load the texture on GPU
- [ ] Add null-detection when the state of the data passed is acceptable (e.g. a scene without environment)
- [ ] Separate light procedures in material fragment shader to clean up the clutter
- [ ] The shader_material_*() interface is as coupled to the material struct as the model struct. Find a way to express it