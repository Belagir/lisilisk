# LISILISK

## TODO list

So much to do, so many other projects calling my name.

### `struct objects` should be renamed to "model" or some less general name

`object`s aggregate a shader, a geometry and a material to supply intances. "Object" is just too broad of a term to actually represent the intention behind the data layout.

### Dynamic Loading & unloading

Data objects tied to some OpenGL data should be able to be loaded when at least another object need them, and unloaded when nothing needs them.

```c
#define OPENGL_OBJECT_FLAG_NONE     (0x0)
#define OPENGL_OBJECT_FLAG_LOADED   (0x1)
#define OPENGL_OBJECT_FLAG_NOTSHOWN (0x2)
// ...

struct opengl_object {
    u32 flags;
    u32 nb_users;
}
```

Data structures could start with this `opengl_object` that count users and other information about the state of the object. Functions modifying this data would `load()`, `draw()` or `unload()` depending of the actual number of users and flags.

### Review uniform setting procedures

Not very confident in how I handle uniforms and uniform blocks buffers.

### Textures

There should be a new data object to deal with textures. Those textures should be passable to shaders as samplers.

### UVs & quads

The parser for obj files should be able to load UVs and deal with faces made of more than 3 vertices.

### Dynamic updates of GPU data

Data objects that send data to the GPU do so only on `load()`. When their data change, they should update the data in the buffers.

### Base shaders & shaders slices

There should be a better way to have the user supply shaders, without having the burden to redefine the uniforms & co.

The backend should take shader fragments rather than whole shaders. Those fragments should define a `vertex()` function and a `fragment()` function that will be used in the actual compiled shaders. Thoses fragments are appended to a set of uniform and interfaces declarations and a `main()` function that invokes the user functions. This is this shader that is actually compiled.

### Freestanding instance objects

When instancing a model, creating a light or a camera, the user should be given an instance handle that can act on the corresponding data (transforms, colors etc.).

### Spaghetti interface

There should be an interface to the backend that handles all the boring parts. For instance, the user could just :

```c
struct model_handle m = lisk_model("some_mesh.obj");
lisk_model_material(m, some_material)
lisk_add("some_scene_identifier", lisk_instanciate(m, (vector3) { 0,0,0 }));
lisk_load("some_scene_identifier");
```

The snippet would create a geometry from the file, build a model with it using a default shader and a default material, and register the geometry to the path of the file.

The code would then change the material of the model to a new material the user loaded before.

Then, the engine checks for the existence of the scene (creating it if not found) and adds the object + instance in the scene.

Finally, the scene is loaded : the object is loaded, the geometry is loaded, the material is loaded, and the scene is added to the redendered scenes in the main loop.