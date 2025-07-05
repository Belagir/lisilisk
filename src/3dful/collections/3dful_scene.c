
#include "3dful_collections.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void scene_send_light_uniforms(struct scene *scene, struct object object);
static void scene_send_camera_uniforms(struct scene *scene, struct object object);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param scene
 */
void scene_create(struct scene *scene)
{
    *scene = (struct scene) {
            .objects = range_create_dynamic(make_system_allocator(), sizeof(*scene->objects->data), 256),
            .camera = { matrix4_identity(), matrix4_identity() },
            .ambient_light = { .color = { 1, 1, 1 }, .strength = .1 },
    };
}

/**
 * @brief
 *
 * @param scene
 */
void scene_delete(struct scene *scene)
{
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(scene->objects));
}

/**
 * @brief
 *
 * @param scene
 * @param object
 */
void scene_add(struct scene *scene, struct object object)
{
    scene_send_light_uniforms(scene, object);
    scene_send_camera_uniforms(scene, object);

    range_push(RANGE_TO_ANY(scene->objects), &object);
}

/**
 * @brief
 *
 * @param scene
 * @param camera
 */
void scene_camera(struct scene *scene, struct camera camera)
{
    scene->camera = camera;

    for (size_t i = 0 ; i < scene->objects->length ; i++) {
        scene_send_camera_uniforms(scene, scene->objects->data[i]);
    }
}

/**
 * @brief
 *
 * @param scene
 * @param color
 */
void scene_ambient_light_color(struct scene *scene, f32 color[3])
{
    scene->ambient_light.color[0] = color[0];
    scene->ambient_light.color[1] = color[1];
    scene->ambient_light.color[2] = color[2];

    for (size_t i = 0 ; i < scene->objects->length ; i++) {
        scene_send_light_uniforms(scene, scene->objects->data[i]);
    }
}

/**
 * @brief
 *
 * @param scene
 * @param pos
 */
void scene_point_light_pos(struct scene *scene, vector3 pos)
{
    scene->point_light.pos = pos;

    for (size_t i = 0 ; i < scene->objects->length ; i++) {
        scene_send_light_uniforms(scene, scene->objects->data[i]);
    }
}

/**
 * @brief
 *
 * @param scene
 */
void scene_draw(struct scene scene)
{
    for (size_t i = 0 ; i < scene.objects->length ; i++) {
        object_draw(scene.objects->data[i]);
    }
}

/**
 * @brief
 *
 * @param scene
 */
void scene_load(struct scene *scene)
{
    for (size_t i = 0 ; i < scene->objects->length ; i++) {
        object_load(&scene->objects->data[i]);
    }
}

/**
 * @brief
 *
 * @param scene
 */
void scene_unload(struct scene *scene)
{
    for (size_t i = 0 ; i < scene->objects->length ; i++) {
        object_unload(&scene->objects->data[i]);
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param scene
 * @param object
 */
static void scene_send_light_uniforms(struct scene *scene, struct object object)
{
    GLint uniform_name = -1;

    glUseProgram(object.shader->program);

    uniform_name = glGetUniformLocation(object.shader->program, "AMBIENT_LIGHT_COLOR");
    glUniform3fv(uniform_name, 1, scene->ambient_light.color);

    uniform_name = glGetUniformLocation(object.shader->program, "POINT_LIGHT_POS");
    glUniform3f(uniform_name, scene->point_light.pos.x, scene->point_light.pos.y, scene->point_light.pos.z);

    glUseProgram(0);
}

/**
 * @brief
 *
 * @param scene
 * @param object
 */
static void scene_send_camera_uniforms(struct scene *scene, struct object object)
{
    f32 tmp[16] = { };
    vector3 cam_pos = { };
    GLint uniform_name = -1;

    glUseProgram(object.shader->program);

    uniform_name = glGetUniformLocation(object.shader->program, "VIEW_MATRIX");
    matrix4_to_array(scene->camera.view, &tmp);
    glUniformMatrix4fv(uniform_name, 1, GL_FALSE, (const GLfloat *) tmp);

    uniform_name = glGetUniformLocation(object.shader->program, "PROJECTION_MATRIX");
    matrix4_to_array(scene->camera.projection, &tmp);
    glUniformMatrix4fv(uniform_name, 1, GL_FALSE, (const GLfloat *) tmp);

    uniform_name = glGetUniformLocation(object.shader->program, "CAMERA_POS");
    cam_pos = matrix_origin(scene->camera.view);
    glUniform3f(uniform_name, cam_pos.x, cam_pos.y, cam_pos.z);

    glUseProgram(0);
}