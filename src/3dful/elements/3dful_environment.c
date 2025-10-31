/**
 * @file 3dful_environment.c
 * @author Gabriel Bédat
 * @brief Implementation of environment-related procedures.
 * @version 0.1
 * @date 2025-07-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "3dful_core.h"

#include <ustd/array.h>

/**
 * @brief Sets the geometry of the surrounding environment. This will decide
 * how the interpolation of the cubemap will be made.
 *
 * @param[inout] env Modified environment.
 * @param[in] shape New shape used to render the cubemap.
 */
void environment_geometry(struct environment *env, struct geometry *shape)
{
    if (env->load_state.flags & LOADABLE_FLAG_LOADED) {
        if (env->shape) {
            geometry_unload(env->shape);
        }
        if (shape) {
            geometry_load(shape);
        }
    }

    env->shape = shape;
}

/**
 * @brief Changes the ambient light provided by this environment.
 *
 * @param[inout] env Target environment.
 * @param[in] light New light information.
 */
void environment_ambient(struct environment *env, struct light light)
{
    env->ambient_light = light;
}

/**
 * @brief Sets the shader used to render the skybox.
 *
 * @param[inout] env Target environment.
 * @param[in] shader New shader.
 */
void environment_shader(struct environment *env, struct shader *shader)
{
    env->shader = shader;
}

/**
 * @brief Sets the cubemap used for the skybox of an environment.
 *
 * @param[inout] env Target environment.
 * @param[in] cubemap Texture holding a cubemap.
 */
void environment_skybox(struct environment *env, struct texture *cubemap)
{
    if (cubemap && (cubemap->flavor != TEXTURE_FLAVOR_CUBEMAP)) {
        return;
    }

    if (env->load_state.flags & LOADABLE_FLAG_LOADED) {
        if (env->cube_texture) {
            texture_unload(env->cube_texture);
        }
        if (cubemap) {
            texture_load(cubemap);
        }
    }
    env->cube_texture = cubemap;
}

/**
 * @brief Sets the ambient fog information of an environment.
 *
 * @param[inout] env Target environment.
 * @param[in] color Color of the fog.
 * @param[in] distance Distance to the opaque fog.
 */
void environment_fog(struct environment *env, f32 color[3], f32 distance)
{
    for (size_t i = 0 ; i < 3 ; i++) {
        env->fog_color[i] = color[i];
    }
    env->fog_distance = distance;
}

/**
 * @brief Sets the background color, used in absence of a cubemap texture.
 *
 * @param[inout] env Target environment.
 * @param[in] color New background color.
 */
void environment_bg(struct environment *env, f32 color[3])
{
    for (size_t i = 0 ; i < 3 ; i++) {
        env->bg_color[i] = color[i];
    }
}

/**
 * @brief Sends the environment data to the GPU the first time this procedure
 * is called.
 *
 * @param[inout] env Loaded environment.
 */
void environment_load(struct environment *env)
{
    loadable_add_user((struct loadable *) env);

    if (!loadable_needs_loading((struct loadable *) env)) {
        return;
    }

    if (env->shape) {
        geometry_load(env->shape);
    }
    if (env->cube_texture) {
        texture_load(env->cube_texture);
    }

    // create cubemap vao
    glGenVertexArrays(1, &env->gpu_side.vao);

    if (env->shader) {
        glUseProgram(env->shader->program);
        glBindVertexArray(env->gpu_side.vao);

        if (env->shape) {
            glBindBuffer(GL_ARRAY_BUFFER, env->shape->gpu_side.vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                    env->shape->gpu_side.ebo);

            glVertexAttribPointer(SHADER_VERT_POS, 3, GL_FLOAT,
                    GL_FALSE, sizeof(struct vertex),
                    (void *) OFFSET_OF(struct vertex, pos));
            glEnableVertexAttribArray(SHADER_VERT_POS);
        }

        if (env->cube_texture) {
            glBindTexture(GL_TEXTURE_CUBE_MAP,
                    env->cube_texture->gpu_side.name);
        }

        glBindVertexArray(0);
        glUseProgram(0);
    }


    env->load_state.flags |= LOADABLE_FLAG_LOADED;
}

/**
 * @brief Removes the environment data from the GPU memory if needed.
 *
 * @param[inout] env Unloaded environment.
 */
void environment_unload(struct environment *env)
{
    loadable_remove_user((struct loadable *) env);

    if (loadable_needs_unloading((struct loadable *) env)) {

        if (env->shape) geometry_unload(env->shape);
        if (env->cube_texture) texture_unload(env->cube_texture);

        glDeleteVertexArrays(1, &env->gpu_side.vao);
        env->gpu_side.vao = 0;

        env->load_state.flags &= ~LOADABLE_FLAG_LOADED;
    }

}

/**
 * @brief Draws the cubemap skybox to the global OpenGL context.
 *
 * @param[in] env Drawn environment.
 */
void environment_draw(struct environment *env)
{
    glDepthMask(GL_FALSE);
    glCullFace(GL_FRONT);

    glUseProgram(env->shader->program);
    glBindVertexArray(env->gpu_side.vao);
    // TODO : make it clear the cube + texture is requiered to draw a
    // cubemap background !
    if (env->shape && env->cube_texture) {
        glDrawElements(GL_TRIANGLES,
                array_length(env->shape->faces)*3,
                GL_UNSIGNED_INT, nullptr);
    }
    glBindVertexArray(0);
    glUseProgram(0);

    glCullFace(GL_BACK);
    glDepthMask(GL_TRUE);
}

/**
 * @brief Sends an environment's variables to a shader.
 * The expected uniforms are :
 * - vec4 LIGHT_AMBIENT ;
 * - vec3 FOG_COLOR ;
 * - float FOG_DISTANCE.
 *
 * @param[in] env Source environment.
 * @param[in] shader Shader supporting the uniforms.
 */
void environment_send_uniforms(struct environment *env, struct shader *shader)
{
    GLint uniform_name = -1;

    glUseProgram(shader->program);

    uniform_name = glGetUniformLocation(shader->program, "LIGHT_AMBIENT");
    glUniform4fv(uniform_name, 1, env->ambient_light.color);

    uniform_name = glGetUniformLocation(shader->program, "FOG_COLOR");
    glUniform3fv(uniform_name, 1, env->fog_color);
    uniform_name = glGetUniformLocation(shader->program, "FOG_DISTANCE");
    glUniform1f(uniform_name, env->fog_distance);

    glUseProgram(0);
}
