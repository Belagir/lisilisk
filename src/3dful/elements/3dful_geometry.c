
#include "3dful_core.h"

// -----------------------------------------------------------------------------
// UTILITY FUNCTIONS -----------------------------------------------------------
static void skip_whitespace(const BUFFER *buffer, size_t *buffer_idx);
static f32 read_value(const BUFFER *buffer, size_t *buffer_idx);
static i32 is_numerical(char c);
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// PARSING ROUTINES ------------------------------------------------------------
static i32 wavefront_parse_end_line(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry);
static i32 wavefront_parse_comment(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry);
static i32 wavefront_parse_obj_name(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry);
static i32 wavefront_parse_vertex(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry);
static i32 wavefront_parse_face(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry);
static i32 wavefront_parse_value(const BUFFER *buffer, size_t *buffer_idx, f32 *out_value);
static i32 wavefront_parse_value_uint(const BUFFER *buffer, size_t *buffer_idx, u32 *out_value);
static i32 wavefront_parse_end_of_obj(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 * @param geometry
 * @param path
 */
void geometry_wavobj(struct geometry *geometry, const char *path)
{
    *geometry = (struct geometry) {
            .name     = range_create_dynamic(make_system_allocator(), sizeof(*geometry->name->data), 64),
            .vertices = range_create_dynamic(make_system_allocator(), sizeof(*geometry->vertices->data), 512),
            .faces    = range_create_dynamic(make_system_allocator(), sizeof(*geometry->faces->data), 256),
    };

    BUFFER *buffer = range_create_dynamic(make_system_allocator(), sizeof(*buffer->data), 4096);

    if (file_read(path, buffer) == 0) {
        geometry_wavobj_mem(geometry, buffer);
    }

    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(buffer));
}

/**
 * @brief
 *
 * @param geometry
 * @param obj
 */
void geometry_wavobj_mem(struct geometry *geometry, BUFFER *obj)
{
    size_t buffer_idx = 0;

    while (!wavefront_parse_end_of_obj(obj, &buffer_idx, geometry)) {
        skip_whitespace(obj, &buffer_idx);

        if (wavefront_parse_end_line(obj, &buffer_idx, geometry)) {
            // NOP
        } else if (wavefront_parse_comment(obj, &buffer_idx, geometry)) {
            // NOP
        } else if (wavefront_parse_obj_name(obj, &buffer_idx, geometry)) {
            // NOP
        } else if (wavefront_parse_vertex(obj, &buffer_idx, geometry)) {
            // NOP
        } else if (wavefront_parse_face(obj, &buffer_idx, geometry)) {
            // NOP
        } else {
            // ERROR SITE
            break;
        }
    }
}

/**
 * @brief
 *
 * @param geometry
 */
void geometry_delete(struct geometry *geometry)
{
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(geometry->name));
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(geometry->vertices));
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(geometry->faces));

    *geometry = (struct geometry) { 0 };
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static i32 wavefront_parse_end_line(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry)
{
    (void) out_geometry;

    // detect end of line character
    if (buffer->data[*buffer_idx] == '\n') {
        *buffer_idx += 1;
        return 1;
    }
    return 0;
}

static i32 wavefront_parse_comment(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry)
{
    (void) out_geometry;

    // detect comment character
    if (buffer->data[*buffer_idx] == '#') {
        // skip to the end of line
        while ((*buffer_idx < buffer->length) && (buffer->data[*buffer_idx] != '\n')) {
            *buffer_idx += 1;
        }
        return 1;
    }
    return 0;
}

static i32 wavefront_parse_obj_name(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry)
{
    // detect 'o' starting letter
    if (buffer->data[*buffer_idx] == 'o') {
        *buffer_idx += 1;
        skip_whitespace(buffer, buffer_idx);

        // take all characters after for the name
        while ((*buffer_idx < buffer->length) && (buffer->data[*buffer_idx] != '\n')) {
            range_push(RANGE_TO_ANY(out_geometry->name), buffer->data + *buffer_idx);
            *buffer_idx += 1;
        }
        range_push(RANGE_TO_ANY(out_geometry->name), &(char) { '\0' });

        return 1;
    }
    return 0;
}

static i32 wavefront_parse_vertex(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry)
{
    union vertex pos = { 0 };

    if (buffer->data[*buffer_idx] == 'v') {
        *buffer_idx += 1;
        skip_whitespace(buffer, buffer_idx);

        if (wavefront_parse_value(buffer, buffer_idx, &pos.x)
                && wavefront_parse_value(buffer, buffer_idx, &pos.y)
                && wavefront_parse_value(buffer, buffer_idx, &pos.z))
        {
            range_push(RANGE_TO_ANY(out_geometry->vertices), &pos);
            return 1;
        }
    }

    return 0;
}

static i32 wavefront_parse_face(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry)
{
    struct face face = { 0 };

    if (buffer->data[*buffer_idx] == 'f') {
        *buffer_idx += 1;
        skip_whitespace(buffer, buffer_idx);

        if (wavefront_parse_value_uint(buffer, buffer_idx, &face.indices[0])
                && wavefront_parse_value_uint(buffer, buffer_idx, &face.indices[1])
                && wavefront_parse_value_uint(buffer, buffer_idx, &face.indices[2])) {

            face.indices[0] -= 1;
            face.indices[1] -= 1;
            face.indices[2] -= 1;

            range_push(RANGE_TO_ANY(out_geometry->faces), &face);
            return 1;
        }
    }
    return 0;
}

static i32 wavefront_parse_value(const BUFFER *buffer, size_t *buffer_idx, f32 *out_value)
{
    skip_whitespace(buffer, buffer_idx);

    if ((buffer->data[*buffer_idx] == '+') || (buffer->data[*buffer_idx] == '-') || is_numerical(buffer->data[*buffer_idx])) {
        *out_value =  read_value(buffer, buffer_idx);
        return 1;
    }

    return 0;
}

static i32 wavefront_parse_value_uint(const BUFFER *buffer, size_t *buffer_idx, u32 *out_value)
{
    f32 tmp = 0;

    if (wavefront_parse_value(buffer, buffer_idx, &tmp)) {
        *out_value = (u32) tmp;
        return 1;
    }
    return 0;
}

static i32 wavefront_parse_end_of_obj(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry)
{
    (void) out_geometry;

    return (*buffer_idx >= buffer->length);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 * @param buffer
 * @param buffer_idx
 * @return f32
 */
static f32 read_value(const BUFFER *buffer, size_t *buffer_idx)
{
    f32 sign = 1.f;
    f32 integral_part = 0.f;
    f32 fract_part = 0.f;
    f32 fract_div = 1.f;

    if (buffer->data[*buffer_idx] == '-') {
        sign = -1;
        *buffer_idx += 1;
    } else if (buffer->data[*buffer_idx] == '+') {
        *buffer_idx += 1;
    }

    // remove leading zeroes
    while ((*buffer_idx < buffer->length) && (buffer->data[*buffer_idx] == '0')) {
        *buffer_idx += 1;
    }

    // read integral part
    while ((*buffer_idx < buffer->length) && is_numerical(buffer->data[*buffer_idx])) {
        integral_part *= 10.f;
        integral_part += (f32) (buffer->data[*buffer_idx] - '0');
        *buffer_idx += 1;
    }

    // read optional fract part
    if ((*buffer_idx < buffer->length) && (buffer->data[*buffer_idx] == '.')) {
        *buffer_idx += 1;

        while ((*buffer_idx < buffer->length) && is_numerical(buffer->data[*buffer_idx])) {
            fract_div *= 10.f;
            fract_part += (f32) (buffer->data[*buffer_idx] - '0') / fract_div;
            *buffer_idx += 1;
        }
    }
    *buffer_idx += 1;

    return sign * (integral_part + fract_part);
}

/**
 * @brief
 *
 * @param buffer
 * @param buffer_idx
 */
static void skip_whitespace(const BUFFER *buffer, size_t *buffer_idx)
{
    while ((*buffer_idx < buffer->length) && ((buffer->data[*buffer_idx] == ' ') || (buffer->data[*buffer_idx] == '\t'))) {
        *buffer_idx += 1;
    }
}

/**
 * @brief
 *
 * @param c
 * @return i32
 */
static i32 is_numerical(char c) {
    return (c >= '0') && (c <= '9');
}