
#include "3dful_core.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#define SHADER_DIAGNOSTIC_MAX_LENGTH (2048)
static char static_shader_diagnostic_buffer[SHADER_DIAGNOSTIC_MAX_LENGTH] = { 0 };

#define SHADER_FILEREAD_MAX_LENGTH (4096)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static i32 check_shader_compilation(GLuint name);
static i32 check_shader_linking(GLuint program);
static GLuint shader_compile(BUFFER *shader_source, GLenum kind);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param shader
 * @param source
 */
void shader_vert_mem(struct shader *shader, BUFFER *source)
{
    shader->vert_shader = shader_compile(source, GL_VERTEX_SHADER);
}

/**
 * @brief
 *
 * @param shader
 * @param source
 */
void shader_frag_mem(struct shader *shader, BUFFER *source)
{
    shader->frag_shader = shader_compile(source, GL_FRAGMENT_SHADER);
}

/**
 * @brief
 *
 * @param shader
 * @param path
 */
void shader_vert(struct shader *shader, const char *path)
{
    BUFFER *buffer = range_create_dynamic(make_system_allocator(), sizeof(*buffer->data), 4096);

    if (file_read(path, buffer) == 0) {
        shader_vert_mem(shader, buffer);
    } else {
        fprintf(stderr, "failed to read file `%s`\n", path);
    }

    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(buffer));
}

/**
 * @brief
 *
 * @param shader
 * @param path
 */
void shader_frag(struct shader *shader, const char *path)
{
    BUFFER *buffer = range_create_dynamic(make_system_allocator(), sizeof(*buffer->data), 4096);

    if (file_read(path, buffer) == 0) {
        shader_frag_mem(shader, buffer);
    } else {
        fprintf(stderr, "failed to read file `%s`\n", path);
    }

    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(buffer));
}

/**
 * @brief
 *
 * @param shader
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
 * @brief
 *
 * @param shader
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

/**
 * @brief
 *
 * @param shader
 * @param index
 * @param stride
 * @param offset
 */
void shader_vertex_attrib_vec3(struct shader *shader, u32 index, size_t stride, size_t offset)
{
    glUseProgram(shader->program);

    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, stride, (void *) offset);
    glEnableVertexAttribArray(index);

    glUseProgram(0);
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
static GLuint shader_compile(BUFFER *shader_source, GLenum kind)
{
    GLuint shader = glCreateShader(kind);
    const GLchar* buffer = (const GLchar*) shader_source->data;
    // GLint length = shader_source->length;

    glShaderSource(shader, 1, &buffer, NULL);
    glCompileShader(shader);

    if (!check_shader_compilation(shader)) {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}
