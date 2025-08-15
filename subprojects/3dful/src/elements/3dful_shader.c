/**
 * @file 3dful_shader.c
 * @author Gabriel Bédat
 * @brief Implementation of shader-related procedures.
 * @version 0.1
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "3dful_core.h"

#include <ustd/array.h>
#include <ustd/res.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

DECLARE_RES(vertex_head, "res_shaders_wrapper_vert_head_glsl")
DECLARE_RES(vertex_tail, "res_shaders_wrapper_vert_tail_glsl")

DECLARE_RES(fragment_head, "res_shaders_wrapper_frag_head_glsl")
DECLARE_RES(fragment_tail, "res_shaders_wrapper_frag_tail_glsl")

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define SHADER_DIAGNOSTIC_MAX_LENGTH (2048)
static char static_shader_diagnostic_buffer[SHADER_DIAGNOSTIC_MAX_LENGTH] =
        { 0 };

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static i32 check_shader_compilation(GLuint name);
static i32 check_shader_linking(GLuint program);
static GLuint shader_compile(const byte *shader_source, size_t length,
        GLenum kind);
static GLuint shader_material_compile(const byte *shader_source, size_t length,
        GLenum kind);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Compiles a material shader's vertex from its source loaded in
 * memory.
 * The shader source passed will be wrapped with material shader head and tail
 * source code to skip the boilerplate.
 *
 * @param[inout] shader Shader object to receive the compiled vertex shader.
 * @param[in] source Source as an array of bytes (created with ustd/array.h).
 */
void shader_material_vert_mem(struct shader *shader, const byte *source,
        size_t length)
{
    shader->vert_shader = shader_material_compile(source, length,
            GL_VERTEX_SHADER);
}

/**
 * @brief Compiles a material shader's fragment from its source loaded in
 * memory.
 * The shader source passed will be wrapped with material shader head and tail
 * source code to skip the boilerplate.
 *
 * @param[inout] shader Shader object to receive the compiled fragment shader.
 * @param[in] source Source as an array of bytes (created with ustd/array.h).
 */
void shader_material_frag_mem(struct shader *shader, const byte *source,
        size_t length)
{
    shader->frag_shader = shader_material_compile(source, length,
            GL_FRAGMENT_SHADER);
}

/**
 * @brief Compiles a vertex shader source found in a buffer.
 *
 * @param[inout] shader Shader object to receive the compiled vertex shader.
 * @param[in] source Buffer containing the shader's source.
 */
void shader_vert_mem(struct shader *shader, const byte *source,
        size_t length)
{
    shader->vert_shader = shader_compile(source, length, GL_VERTEX_SHADER);
}

/**
 * @brief Compiles a fragment shader source found in a buffer.
 *
 * @param[inout] shader Shader object to receive the compiled fragment shader.
 * @param[in] source Buffer containing the shader's source.
 */
void shader_frag_mem(struct shader *shader, const byte *source,
        size_t length)
{
    shader->frag_shader = shader_compile(source, length, GL_FRAGMENT_SHADER);
}

/**
 * @brief
 *
 * @param shader
 * @param name
 * @param value
 */
void shader_uniform_float(struct shader *shader, const char *name,
        float value)
{
    GLint loc = 0;

    glUseProgram(shader->program);
    loc = glGetUniformLocation(shader->program, name);
    glUniform1f(loc, value);
}

/**
 * @brief Links a shader program, assembling the vertex part and fragment part
 * into one usable program.
 *
 * @param[inout] shader Linked shader.
 */
void shader_link(struct shader *shader)
{
    shader->program = glCreateProgram();

    glAttachShader(shader->program, shader->vert_shader);
    glAttachShader(shader->program, shader->frag_shader);
    glLinkProgram(shader->program);

    if (!check_shader_linking(shader->program)) {
        shader->program = 0;
    }
}

/**
 * @brief Releases all data buffers and objects taken by a shader, invalidating
 * it.
 *
 * @param[inout] shader Destroyed shader.
 */
void shader_delete(struct shader *shader)
{
    glDetachShader(shader->program, shader->frag_shader);
    glDetachShader(shader->program, shader->vert_shader);
    glDeleteProgram(shader->program);
    glDeleteShader(shader->frag_shader);
    glDeleteShader(shader->vert_shader);

    *shader = (struct shader) { 0 };
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Checks wether or not a shader program correctly linked, and report a
 * status on stderr if not.
 *
 * @param[in] program
 * @return i32
 */
static i32 check_shader_linking(GLuint program)
{
    i32 is_linked = 0;
    GLint diag_length = SHADER_DIAGNOSTIC_MAX_LENGTH;

    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);

    if (!is_linked) {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &diag_length);
        glGetProgramInfoLog(program, diag_length, &diag_length,
                static_shader_diagnostic_buffer);
        fprintf(stderr, "%s\n", static_shader_diagnostic_buffer);
    }

    return is_linked;
}

/**
 * @brief Checks wether or not a shader program correctly compiled, and report a
 * status on stderr if not.
 *
 * @param name
 * @return i32
 */
static i32 check_shader_compilation(GLuint name)
{
    i32 is_compiled = 0;
    GLint diag_length = SHADER_DIAGNOSTIC_MAX_LENGTH;

    glGetShaderiv(name, GL_COMPILE_STATUS, &is_compiled);

    if (!is_compiled) {
        glGetShaderiv(name, GL_INFO_LOG_LENGTH, &diag_length);
        glGetShaderInfoLog(name, diag_length, &diag_length,
                static_shader_diagnostic_buffer);
        fprintf(stderr, "%s\n", static_shader_diagnostic_buffer);
    }

    return is_compiled;
}

/**
 * @brief Compiles a shader of some kind, from a source loaded in a buffer.
 *
 * @param[in] path
 * @param[in] kind
 * @return GLuint
 */
static GLuint shader_compile(const byte *shader_source, size_t length,
        GLenum kind)
{
    GLuint shader = glCreateShader(kind);
    const GLint gl_length = (GLint) length;

    glShaderSource(shader, 1, (const char *const *) &shader_source, &gl_length);
    glCompileShader(shader);

    if (!check_shader_compilation(shader)) {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

/**
 * @brief Wraps and compiles a shader of some kind, from a source loaded in a
 * buffer.
 *
 * @param shader_source
 * @param kind
 * @return GLuint
 */
static GLuint shader_material_compile(const byte *shader_source, size_t length,
        GLenum kind)
{
    GLuint out_shader = 0;
    struct allocator alloc = make_system_allocator();

    ARRAY(byte) full_source = nullptr;
    size_t full_source_length = 0;

    switch (kind) {
        case GL_VERTEX_SHADER:
            full_source_length =
                    (size_t)&vertex_head_size
                    + length
                    + (size_t)&vertex_tail_size;
            break;
        case GL_FRAGMENT_SHADER:
            full_source_length =
                    (size_t)&fragment_head_size
                    + length
                    + (size_t)&fragment_tail_size;
            break;
        default:
            break;
    }

    full_source = array_create(alloc, sizeof(*full_source), full_source_length);

    switch (kind) {
        case GL_VERTEX_SHADER:
            array_append_mem(full_source,
                    vertex_head_start, (size_t)&vertex_head_size);
            array_append_mem(full_source,
                    shader_source, length);
            array_append_mem(full_source,
                    vertex_tail_start, (size_t)&vertex_tail_size);
            break;
        case GL_FRAGMENT_SHADER:
            array_append_mem(full_source,
                    fragment_head_start, (size_t)&fragment_head_size);
            array_append_mem(full_source,
                    shader_source, length);
            array_append_mem(full_source,
                    fragment_tail_start, (size_t)&fragment_tail_size);
            break;
        default:
            break;
    }

    out_shader = shader_compile(full_source, array_length(full_source), kind);

    array_destroy(alloc, (ARRAY_ANY *) &full_source);

    return out_shader;
}
