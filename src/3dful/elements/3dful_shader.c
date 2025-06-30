
#include "3dful_core.h"

#define SHADER_DIAGNOSTIC_MAX_LENGTH (2048)
static char static_shader_diagnostic_buffer[SHADER_DIAGNOSTIC_MAX_LENGTH] = { 0 };

static i32 check_shader_compilation(GLuint name);
static i32 check_shader_linking(GLuint program);
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
 * @param shader
 * @return struct shader
 */
void shader_destroy(struct shader shader)
{
    glDeleteShader(shader.name);
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
    struct shader shader = { .name = glCreateShader(kind) };
    const GLchar* buffer = (const GLchar*) shader_source->data;

    glShaderSource(shader.name, 1, &buffer, NULL);
    glCompileShader(shader.name);

    if (!check_shader_compilation(shader.name)) {
        glDeleteShader(shader.name);
        return (struct shader) { 0 };
    }

    return shader;
}

/**
 * @brie
 *
 * @param alloc
 * @return struct object
 */
struct geometry create_geometry_empty(struct allocator alloc)
{
    struct geometry new_geometry = {
            .name = range_create_dynamic(alloc, sizeof(&new_geometry.name->data), 64),
            .vertices = range_create_dynamic(alloc, sizeof(&new_geometry.vertices->data), 128),
            .colors = range_create_dynamic(alloc, sizeof(&new_geometry.colors->data), 128),
    };

    return new_geometry;
}

/**
 * @brief
 *
 * @param alloc
 * @param geometry
 */
void destroy_geometry(struct allocator alloc, struct geometry *geometry)
{
    if (!geometry) {
        return;
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY(geometry->name));
    range_destroy_dynamic(alloc, &RANGE_TO_ANY(geometry->vertices));
    range_destroy_dynamic(alloc, &RANGE_TO_ANY(geometry->colors));

    *geometry = (struct geometry) { 0 };
}
