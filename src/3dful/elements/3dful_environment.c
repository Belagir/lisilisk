
#include "3dful_core.h"

#include <ustd/array.h>

/**
 * @brief
 *
 * @param env
 * @param cube
 */
void environment_cube(struct environment *env, struct geometry *cube)
{
    if (env->load_state.flags & LOADABLE_FLAG_LOADED) {
        if (env->cube) {
            geometry_unload(env->cube);
        }
        if (cube) {
            geometry_load(cube);
        }
    }

    env->cube = cube;
}

/**
 * @brief
 *
 * @param env
 * @param light
 */
void environment_ambient(struct environment *env, struct light light)
{
    env->ambient_light = light;
}

/**
 * @brief
 *
 * @param env
 * @param shader
 */
void environment_shader(struct environment *env, struct shader *shader)
{
    env->shader = shader;
}

/**
 * @brief
 *
 * @param env
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
 * @brief
 *
 * @param env
 * @param color
 * @param distance
 */
void environment_fog(struct environment *env, f32 color[3], f32 distance)
{
    for (size_t i = 0 ; i < 3 ; i++) {
        env->fog_color[i] = color[i];
    }
    env->fog_distance = distance;
}

/**
 * @brief
 *
 * @param env
 * @param color
 */
void environment_bg(struct environment *env, f32 color[3])
{
    for (size_t i = 0 ; i < 3 ; i++) {
        env->bg_color[i] = color[i];
    }
}

/**
 * @brief
 *
 * @param env
 */
void environment_load(struct environment *env)
{
    loadable_add_user((struct loadable *) env);

    if (loadable_needs_loading((struct loadable *) env)) {
        if (env->cube) geometry_load(env->cube);
        if (env->cube_texture) texture_load(env->cube_texture);

        // create cubemap vao
        glGenVertexArrays(1, &env->gpu_side.vao);

        // actual laoding scenario
        glUseProgram(env->shader->program);
        {
            glBindVertexArray(env->gpu_side.vao);
            {
                if (env->cube) {
                    glBindBuffer(GL_ARRAY_BUFFER, env->cube->gpu_side.vbo);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, env->cube->gpu_side.ebo);

                    glVertexAttribPointer(SHADER_VERT_POS, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void *) OFFSET_OF(struct vertex, pos));
                    glEnableVertexAttribArray(SHADER_VERT_POS);
                }

                if (env->cube_texture) {
                    glBindTexture(GL_TEXTURE_CUBE_MAP, env->cube_texture->gpu_side.name);
                }
            }
            glBindVertexArray(0);
        }
        glUseProgram(0);

        env->load_state.flags |= LOADABLE_FLAG_LOADED;
    }
}

/**
 * @brief
 *
 * @param env
 */
void environment_unload(struct environment *env)
{
    loadable_remove_user((struct loadable *) env);

    if (loadable_needs_unloading((struct loadable *) env)) {

        if (env->cube) geometry_unload(env->cube);
        if (env->cube_texture) texture_unload(env->cube_texture);

        glDeleteVertexArrays(1, &env->gpu_side.vao);
        env->gpu_side.vao = 0;

        env->load_state.flags &= ~LOADABLE_FLAG_LOADED;
    }

}

/**
 * @brief
 *
 * @param env
 */
void environment_draw(struct environment *env)
{
    glDepthMask(GL_FALSE);

    glUseProgram(env->shader->program);
    {
        glBindVertexArray(env->gpu_side.vao);
        {
            // TODO : make it clear the cube + texture is requiered to draw a cubemap background !
            if (env->cube && env->cube_texture) glDrawElements(GL_TRIANGLES, array_length(env->cube->faces_array)*3, GL_UNSIGNED_INT, nullptr);
        }
        glBindVertexArray(0);
    }
    glUseProgram(0);

    glDepthMask(GL_TRUE);
}

/**
 * @brief
 *
 * @param env
 * @param shader
 */
void environment_send_uniforms(struct environment *env, struct shader *shader)
{
    GLint uniform_name = -1;

    glUseProgram(shader->program);
    {
        uniform_name = glGetUniformLocation(shader->program, "LIGHT_AMBIENT");
        glUniform4fv(uniform_name, 1, env->ambient_light.color);

        uniform_name = glGetUniformLocation(shader->program, "FOG_COLOR");
        glUniform3fv(uniform_name, 1, env->fog_color);
        uniform_name = glGetUniformLocation(shader->program, "FOG_DISTANCE");
        glUniform1f(uniform_name, env->fog_distance);
    }
    glUseProgram(0);

}
