
#include "3dful_core.h"

#define SHADER_DIAGNOSTIC_MAX_LENGTH (2048)
static char static_shader_diagnostic_buffer[SHADER_DIAGNOSTIC_MAX_LENGTH] = { 0 };

static i32 check_shader_compilation(GLuint name);
static i32 check_shader_linking(GLuint program);
static GLuint shader_compile(BUFFER *shader_source, GLenum kind);

/**
 * @brief 
 * 
 * @param frag_shader_source 
 * @param vert_shader_source 
 * @return struct shader_program 
 */
struct shader_program shader_program_create(BUFFER *frag_shader_source, BUFFER *vert_shader_source)
{
    struct shader_program shaders = { 0 };

    if (!frag_shader_source || !vert_shader_source) {
        return (struct shader_program) { 0 };
    }

    shaders.frag_shader = shader_compile(frag_shader_source, GL_FRAGMENT_SHADER);
    shaders.vert_shader = shader_compile(vert_shader_source, GL_VERTEX_SHADER);

    if (!shaders.frag_shader || !shaders.vert_shader) {
        shader_program_destroy(&shaders);
        return (struct shader_program) { 0 };
    }

    shaders.program = glCreateProgram();

    glAttachShader(shaders.program, shaders.vert_shader);
    glAttachShader(shaders.program, shaders.frag_shader);
    glBindAttribLocation(shaders.program, 0, "in_Position");
    glBindAttribLocation(shaders.program, 1, "in_Color");
    glLinkProgram(shaders.program);

    if (!check_shader_linking(shaders.program)) {
        shader_program_destroy(&shaders);
        return (struct shader_program) { 0 };
    }

    return shaders;
}

void shader_program_destroy(struct shader_program *shaders)
{
    if (!shaders) {
        return;
    }

    glDetachShader(shaders->program, shaders->frag_shader);
    glDetachShader(shaders->program, shaders->vert_shader);
    glDeleteProgram(shaders->program);
    glDeleteShader(shaders->frag_shader);
    glDeleteShader(shaders->vert_shader);

    *shaders = (struct shader_program) { 0 };
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
 * @param shader_source
 * @param kind
 * @return struct shader
 */
static GLuint shader_compile(BUFFER *shader_source, GLenum kind)
{
    GLuint shader = glCreateShader(kind);
    const GLchar* buffer = (const GLchar*) shader_source->data;

    glShaderSource(shader, 1, &buffer, NULL);
    glCompileShader(shader);

    if (!check_shader_compilation(shader)) {
        glDeleteShader(shader);
        return 0;
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
