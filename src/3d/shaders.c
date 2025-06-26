
#include "opengl_scenes.h"

#define SHADER_DIAGNOSTIC_MAX_LENGTH (2048)
static char static_shader_diagnostic_buffer[SHADER_DIAGNOSTIC_MAX_LENGTH] = { 0 };

static i32 check_shader_compilation(GLuint shader_handle);
static struct shader shader_compile(BUFFER *shader_source, GLenum kind);

/**
 * @brief
 *
 * @param shader_source
 * @return struct shader
 */
struct shader shader_compile_vertex(BUFFER *shader_source)
{
    return shader_compile(shader_source, GL_VERTEX_SHADER);
}
/**
 * @brief
 *
 * @param shader_source
 * @return struct shader
 */
struct shader shader_compile_fragment(BUFFER *shader_source)
{
    return shader_compile(shader_source, GL_FRAGMENT_SHADER);
}

/**
 * @brief
 *
 * @param shader_handle
 * @return i32
 */
i32 check_shader_compilation(GLuint shader_handle)
{
    i32 is_compiled = 0;
    GLint diag_length = SHADER_DIAGNOSTIC_MAX_LENGTH;

    glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &is_compiled);

    if (!is_compiled) {
        glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &diag_length);
        glGetShaderInfoLog(shader_handle, diag_length, &diag_length, static_shader_diagnostic_buffer);
        fprintf(stderr, "%s\n", static_shader_diagnostic_buffer);
    }

    return is_compiled;
}

/**
 * @brief
 *
 * @param shader
 * @return struct shader
 */
void shader_destroy(struct shader shader)
{
    glDeleteShader(shader.shader_handle);
}

/**
 * @brief
 *
 * @param shader_source
 * @param kind
 * @return struct shader
 */
static struct shader shader_compile(BUFFER *shader_source, GLenum kind)
{
    struct shader shader = { .shader_handle = glCreateShader(kind) };
    const GLchar* buffer = (const GLchar*) shader_source->data;

    glShaderSource(shader.shader_handle, 1, &buffer, NULL);
    glCompileShader(shader.shader_handle);

    if (!check_shader_compilation(shader.shader_handle)) {
        glDeleteShader(shader.shader_handle);
        return (struct shader) { 0 };
    }

    return shader;
}
