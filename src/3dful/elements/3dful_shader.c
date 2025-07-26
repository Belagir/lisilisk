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

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define SHADER_VERTEX_HEAD "shaders/wrappers/vert_head.glsl"
#define SHADER_VERTEX_TAIL "shaders/wrappers/vert_tail.glsl"

#define SHADER_FRAGMENT_HEAD "shaders/wrappers/frag_head.glsl"
#define SHADER_FRAGMENT_TAIL "shaders/wrappers/frag_tail.glsl"

/**
 * @brief Kinds of wrapping around shaders the backend supports.
 * Wrapping adds predetermined inputs/outputs, and an expected function that
 * needs to be user-defined so it can be called in an appended main().
 */
enum shader_wrapping {
    SHADER_WRAPPING_NONE,
    SHADER_WRAPPING_MATERIAL,
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define SHADER_DIAGNOSTIC_MAX_LENGTH (2048)
static char static_shader_diagnostic_buffer[SHADER_DIAGNOSTIC_MAX_LENGTH] =
    { 0 };

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static i32 check_shader_compilation(GLuint name);
static i32 check_shader_linking(GLuint program);
static GLuint shader_compile_file(const char *path, GLenum kind,
        enum shader_wrapping wrapping);
static GLuint shader_compile(const byte *shader_source, GLenum kind);
static GLuint shader_wrap_compile(const byte *shader_source, GLenum kind);

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
void shader_material_vert_mem(struct shader *shader, const byte *source)
{
    shader->vert_shader = shader_wrap_compile(source, GL_VERTEX_SHADER);
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
void shader_material_frag_mem(struct shader *shader, const byte *source)
{
    shader->frag_shader = shader_wrap_compile(source, GL_FRAGMENT_SHADER);
}

/**
 * @brief Compiles a material shader's vertex from its source found in
 * a file.
 * The shader source passed will be wrapped with material shader head and tail
 * source code to skip the boilerplate.
 *
 * @param[inout] shader Shader object to receive the compiled vertex shader.
 * @param[in] path Path to the source of a material vertex shader.
 */
void shader_material_vert(struct shader *shader, const char *path)
{
    shader->vert_shader = shader_compile_file(path, GL_VERTEX_SHADER,
            SHADER_WRAPPING_MATERIAL);

    if (shader->vert_shader == 0) {
        printf("compilation of material shader file %s failed.\n", path);
    }
}

/**
 * @brief Compiles a material shader's fragment from its source found in
 * a file.
 * The shader source passed will be wrapped with material shader head and tail
 * source code to skip the boilerplate.
 *
 * @param[inout] shader Shader object to receive the compiled fragment shader.
 * @param[in] path Path to the source of a material fragment shader.
 */
void shader_material_frag(struct shader *shader, const char *path)
{
    shader->frag_shader = shader_compile_file(path, GL_FRAGMENT_SHADER,
            SHADER_WRAPPING_MATERIAL);

    if (shader->frag_shader == 0) {
        printf("compilation of material shader file %s failed.\n", path);
    }
}

/**
 * @brief Compiles a vertex shader source found in a buffer.
 *
 * @param[inout] shader Shader object to receive the compiled vertex shader.
 * @param[in] source Buffer containing the shader's source.
 */
void shader_vert_mem(struct shader *shader, const byte *source)
{
    shader->vert_shader = shader_compile(source, GL_VERTEX_SHADER);
}

/**
 * @brief Compiles a fragment shader source found in a buffer.
 *
 * @param[inout] shader Shader object to receive the compiled fragment shader.
 * @param[in] source Buffer containing the shader's source.
 */
void shader_frag_mem(struct shader *shader, const byte *source)
{
    shader->frag_shader = shader_compile(source, GL_FRAGMENT_SHADER);
}

/**
 * @brief Compiles a vertex shader source found in a file.
 *
 * @param[inout] shader Shader object to receive the compiled vertex shader.
 * @param[in] path Path to the shader source file.
 */
void shader_vert(struct shader *shader, const char *path)
{
    shader->vert_shader = shader_compile_file(path, GL_VERTEX_SHADER,
            SHADER_WRAPPING_NONE);

    if (shader->vert_shader == 0) {
        printf("compilation of file shader %s failed.\n", path);
    }
}

/**
 * @brief Compiles a fragment shader source found in a file.
 *
 * @param[inout] shader Shader object to receive the compiled fragment shader.
 * @param[in] path Path to the shader source file.
 */
void shader_frag(struct shader *shader, const char *path)
{
    shader->frag_shader = shader_compile_file(path, GL_FRAGMENT_SHADER,
            SHADER_WRAPPING_NONE);

    if (shader->frag_shader == 0) {
        printf("compilation of file shader %s failed.\n", path);
    }
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
 * @brief Compiles a shader of some kind, from a file, executing some wrapping
 * if needed.
 *
 * @param[in] path
 * @param[in] kind
 * @param[in] wrapping
 * @return GLuint
 */
static GLuint shader_compile_file(const char *path, GLenum kind,
        enum shader_wrapping wrapping)
{
    size_t nb_bytes = file_length(path);
    byte *buffer = nullptr;
    GLuint shader_out = 0;

    if (nb_bytes == 0) {
        fprintf(stderr, "failed to read file `%s`\n", path);
        return 0;
    }

    buffer = array_create(make_system_allocator(), sizeof(*buffer), nb_bytes);

    if (file_read_to_array(path, buffer) != 0) {
        fprintf(stderr, "failed to read file `%s`\n", path);
        array_destroy(make_system_allocator(), (void **) &buffer);
        return 0;
    }

    switch (wrapping) {
        case SHADER_WRAPPING_NONE:
            shader_out = shader_compile(buffer, kind);
            break;
        case SHADER_WRAPPING_MATERIAL:
            shader_out = shader_wrap_compile(buffer, kind);
            break;
    }

    array_destroy(make_system_allocator(), (void **) &buffer);

    return shader_out;
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
static GLuint shader_compile(const byte *shader_source, GLenum kind)
{
    GLuint shader = glCreateShader(kind);
    GLint length = array_length(shader_source);

    glShaderSource(shader, 1, (const char *const *) &shader_source, &length);
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
static GLuint shader_wrap_compile(const byte *shader_source, GLenum kind)
{
    byte *source_head = nullptr;
    byte *source_tail = nullptr;
    byte *full_source = nullptr;
    GLuint out_shader = 0;

    switch (kind) {
        case GL_VERTEX_SHADER:
            source_head = array_create(make_system_allocator(),
                    sizeof(*source_head), file_length(SHADER_VERTEX_HEAD));
            file_read_to_array(SHADER_VERTEX_HEAD, source_head);
            source_tail = array_create(make_system_allocator(),
                    sizeof(*source_tail), file_length(SHADER_VERTEX_TAIL));
            file_read_to_array(SHADER_VERTEX_TAIL, source_tail);
            break;
        case GL_FRAGMENT_SHADER:
            source_head = array_create(make_system_allocator(),
                    sizeof(*source_head), file_length(SHADER_FRAGMENT_HEAD));
            file_read_to_array(SHADER_FRAGMENT_HEAD, source_head);
            source_tail = array_create(make_system_allocator(),
                    sizeof(*source_tail), file_length(SHADER_FRAGMENT_TAIL));
            file_read_to_array(SHADER_FRAGMENT_TAIL, source_tail);
            break;

        default:
            return 0;
    }

    full_source = array_create(make_system_allocator(), sizeof(*full_source),
            array_capacity(source_head)
            + array_capacity(shader_source)
            + array_capacity(source_tail));

    array_append(full_source, (void *) source_head);
    array_append(full_source, (void *) shader_source);
    array_append(full_source, (void *) source_tail);

    out_shader = shader_compile(full_source, kind);

    array_destroy(make_system_allocator(), (void **) &source_head);
    array_destroy(make_system_allocator(), (void **) &source_tail);
    array_destroy(make_system_allocator(), (void **) &full_source);

    return out_shader;
}
