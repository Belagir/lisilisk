
#include "3dful_core.h"

#include <ustd/array.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#define SHADER_VERTEX_HEAD "shaders/wrappers/vert_head.glsl"
#define SHADER_VERTEX_TAIL "shaders/wrappers/vert_tail.glsl"

#define SHADER_FRAGMENT_HEAD "shaders/wrappers/frag_head.glsl"
#define SHADER_FRAGMENT_TAIL "shaders/wrappers/frag_tail.glsl"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#define SHADER_DIAGNOSTIC_MAX_LENGTH (2048)
static char static_shader_diagnostic_buffer[SHADER_DIAGNOSTIC_MAX_LENGTH] = { 0 };

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static i32 check_shader_compilation(GLuint name);
static i32 check_shader_linking(GLuint program);
static GLuint shader_compile(byte *shader_source, GLenum kind);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Compiles a vertex shader from its source loaded in memory.
 *
 * @param[inout] shader
 * @param[in] source
 */
void shader_vert_mem(struct shader *shader, byte *source)
{
    byte *source_head = array_create(make_system_allocator(), sizeof(*source_head), file_length(SHADER_VERTEX_HEAD));
    byte *source_tail = array_create(make_system_allocator(), sizeof(*source_tail), file_length(SHADER_VERTEX_TAIL));

    byte *full_source = array_create(make_system_allocator(), sizeof(*full_source),
            array_capacity(source_head) + array_capacity(source) + array_capacity(source_tail));

    file_read_to_array(SHADER_VERTEX_HEAD, source_head);
    file_read_to_array(SHADER_VERTEX_TAIL, source_tail);

    array_append(full_source, source_head);
    array_append(full_source, source);
    array_append(full_source, source_tail);

    shader->vert_shader = shader_compile(full_source, GL_VERTEX_SHADER);

    array_destroy(make_system_allocator(), (void **) &source_head);
    array_destroy(make_system_allocator(), (void **) &source_tail);
    array_destroy(make_system_allocator(), (void **) &full_source);
}

/**
 * @brief Compiles a fragment shader from its source loaded in memory.
 *
 * @param[inout] shader
 * @param[in] source
 */
void shader_frag_mem(struct shader *shader, byte *source)
{
    byte *source_head = array_create(make_system_allocator(), sizeof(*source_head), file_length(SHADER_FRAGMENT_HEAD));
    byte *source_tail = array_create(make_system_allocator(), sizeof(*source_tail), file_length(SHADER_FRAGMENT_TAIL));

    byte *full_source = array_create(make_system_allocator(), sizeof(*full_source),
            array_capacity(source_head) + array_capacity(source) + array_capacity(source_tail));

    file_read_to_array(SHADER_FRAGMENT_HEAD, source_head);
    file_read_to_array(SHADER_FRAGMENT_TAIL, source_tail);

    array_append(full_source, source_head);
    array_append(full_source, source);
    array_append(full_source, source_tail);

    shader->frag_shader = shader_compile(full_source, GL_FRAGMENT_SHADER);

    array_destroy(make_system_allocator(), (void **) &source_head);
    array_destroy(make_system_allocator(), (void **) &source_tail);
    array_destroy(make_system_allocator(), (void **) &full_source);

}

/**
 * @brief Compiles a vertex shader from its source found in a file.
 *
 * @param[inout] shader
 * @param[in] path
 */
void shader_vert(struct shader *shader, const char *path)
{
    size_t nb_bytes = file_length(path);
    byte *buffer = nullptr;

    if (nb_bytes == 0) {
        fprintf(stderr, "failed to read file `%s`\n", path);
        return;
    }

    buffer = array_create(make_system_allocator(), sizeof(*buffer), nb_bytes);

    if (file_read_to_array(path, buffer) == 0) {
        shader_vert_mem(shader, buffer);
    } else {
        fprintf(stderr, "failed to read file `%s`\n", path);
    }

    array_destroy(make_system_allocator(), (void **) &buffer);
}

/**
 * @brief Compiles a fragment shader from its source found in a file.
 *
 * @param[inout] shader
 * @param[in] path
 */
void shader_frag(struct shader *shader, const char *path)
{
    byte *buffer = array_create(make_system_allocator(), sizeof(*buffer), file_length(path));

    if (file_read_to_array(path, buffer) == 0) {
        shader_frag_mem(shader, buffer);
    } else {
        fprintf(stderr, "failed to read file `%s`\n", path);
    }

    array_destroy(make_system_allocator(), (void **) &buffer);
}

/**
 * @brief Links a shader program, assembling the vertex part and fragment part into one usable object.
 *
 * @param[inout] shader
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
 * @brief Releases all data buffers and objects taken by a shader, invalidating it.
 *
 * @param[inout] shader
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

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param name
 * @return i32
 */
static i32 check_shader_linking(GLuint program)
{
    i32 is_linked = 0;
    GLint diag_length = SHADER_DIAGNOSTIC_MAX_LENGTH;

    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);

    if (!is_linked) {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &diag_length);
        glGetProgramInfoLog(program, diag_length, &diag_length, static_shader_diagnostic_buffer);
        fprintf(stderr, "%s\n", static_shader_diagnostic_buffer);
    }

    return is_linked;
}


/**
 * @brief
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
        glGetShaderInfoLog(name, diag_length, &diag_length, static_shader_diagnostic_buffer);
        fprintf(stderr, "%s\n", static_shader_diagnostic_buffer);
    }

    return is_compiled;
}

/**
 * @brief
 *
 * @param shader_source
 * @param kind
 * @return struct shader
 */
static GLuint shader_compile(byte *shader_source, GLenum kind)
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
