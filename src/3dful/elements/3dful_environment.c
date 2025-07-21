
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
    env->unit_cube = cube;
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
void environment_skybox(struct environment *env, struct texture *(*cubemap)[CUBEMAP_FACES_NUMBER])
{
    for (size_t i = 0 ; i < CUBEMAP_FACES_NUMBER ; i++) {
        env->cubemap[i] = (*cubemap)[i];
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
        geometry_load(env->unit_cube);

        // create cubemap vao
        glGenVertexArrays(1, &env->gpu_side.vao);

        // cube map texturing
        glGenTextures(1, &env->gpu_side.cubemap_texture);

        glBindTexture(GL_TEXTURE_CUBE_MAP, env->gpu_side.cubemap_texture);
        {
            for (size_t i = 0 ; i < CUBEMAP_FACES_NUMBER ; i++) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, env->cubemap[i]->image->w, env->cubemap[i]->image->h,
                        0, GL_RGB, GL_UNSIGNED_BYTE, env->cubemap[i]->image->pixels);
            }

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        // actual laoding scenario
        glUseProgram(env->shader->program);
        {
            glBindVertexArray(env->gpu_side.vao);
            {
                glBindBuffer(GL_ARRAY_BUFFER, env->unit_cube->gpu_side.vbo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, env->unit_cube->gpu_side.ebo);

                glVertexAttribPointer(SHADER_VERT_POS, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void *) OFFSET_OF(struct vertex, pos));
                glEnableVertexAttribArray(SHADER_VERT_POS);

                glBindTexture(GL_TEXTURE_CUBE_MAP, env->gpu_side.cubemap_texture);
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
        for (size_t i = 0 ; i < CUBEMAP_FACES_NUMBER ; i++) {
            texture_unload(env->cubemap[i]);
        }

        geometry_unload(env->unit_cube);

        glDeleteTextures(1, &env->gpu_side.cubemap_texture);

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
            glBindTexture(GL_TEXTURE_CUBE_MAP, env->gpu_side.cubemap_texture);

            glDrawElements(GL_TRIANGLES, array_length(env->unit_cube->faces_array)*3, GL_UNSIGNED_INT, nullptr);
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
    }
    glUseProgram(0);

}
