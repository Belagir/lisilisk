
#include "opengl_scenes.h"

// -----------------------------------------------------------------------------
// UTILITY FUNCTIONS -----------------------------------------------------------
static void skip_whitespace(const BUFFER *buffer, size_t *idx);
static f32 read_value(const BUFFER *buffer, size_t *idx);
static i32 is_numerical(char c);
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// PARSING ROUTINES ------------------------------------------------------------
static i32 wavefront_parse_end_line(const BUFFER *buffer, size_t *idx, struct geometry *out_geometry);
static i32 wavefront_parse_comment(const BUFFER *buffer, size_t *idx, struct geometry *out_geometry);
static i32 wavefront_parse_obj_name(const BUFFER *buffer, size_t *idx, struct geometry *out_geometry);
static i32 wavefront_parse_vertex(const BUFFER *buffer, size_t *idx, struct geometry *out_geometry);
static i32 wavefront_parse_vertex_component(const BUFFER *buffer, size_t *idx, struct geometry *out_geometry, u8 vertex_comp);
static i32 wavefront_parse_vertex_color(const BUFFER *buffer, size_t *idx, struct geometry *out_geometry, u8 color_comp);
static i32 wavefront_parse_end_of_obj(const BUFFER *buffer, size_t *idx, struct geometry *out_geometry);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 * @param buffer
 * @return struct geometry
 */
void wavefront_obj_load_geometry(BUFFER *buffer, struct geometry *out_geometry)
{
    size_t idx = 0;

    skip_whitespace(buffer, &idx);

    while (!wavefront_parse_end_of_obj(buffer, &idx, out_geometry)) {
        if (wavefront_parse_end_line(buffer, &idx, out_geometry)) {
            // NOP
        } else if (wavefront_parse_comment(buffer, &idx, out_geometry)) {
            // NOP
        } else if (wavefront_parse_obj_name(buffer, &idx, out_geometry)) {
            // NOP
        } else if (wavefront_parse_vertex(buffer, &idx, out_geometry)) {
            // NOP
        } else {
            // TODO : error
            break;
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static i32 wavefront_parse_end_line(const BUFFER *buffer, size_t *idx, struct geometry *out_geometry)
{
    (void) out_geometry;

    // detect end of line character
    if (buffer->data[*idx] == '\n') {
        *idx += 1;
        return 1;
    }
    return 0;
}

static i32 wavefront_parse_comment(const BUFFER *buffer, size_t *idx, struct geometry *out_geometry)
{
    (void) out_geometry;

    // detect comment character
    if (buffer->data[*idx] == '#') {
        // skip to the end of line
        while ((*idx < buffer->length) && (buffer->data[*idx] != '\n')) {
            *idx += 1;
        }
        return 1;
    }
    return 0;
}

static i32 wavefront_parse_obj_name(const BUFFER *buffer, size_t *idx, struct geometry *out_geometry)
{
    // detect 'o' starting letter
    if (buffer->data[*idx] == 'o') {
        *idx += 1;
        skip_whitespace(buffer, idx);

        // take all characters after for the name
        while ((*idx < buffer->length) && (buffer->data[*idx] != '\n')) {
            range_push(RANGE_TO_ANY(out_geometry->name), buffer->data + *idx);
            *idx += 1;
        }
        range_push(RANGE_TO_ANY(out_geometry->name), &(char) { '\0' });

        return 1;
    }
    return 0;
}

static i32 wavefront_parse_vertex(const BUFFER *buffer, size_t *idx, struct geometry *out_geometry)
{
    i32 is_valid = 0;

    if (buffer->data[*idx] == 'v') {
        *idx += 1;
        skip_whitespace(buffer, idx);

        // create a new vertex in the object
        range_push(RANGE_TO_ANY(out_geometry->vertices), &(union vertex) { 0 });
        // parse the requiered x, y, and z
        is_valid = wavefront_parse_vertex_component(buffer, idx, out_geometry, 0)
                && wavefront_parse_vertex_component(buffer, idx, out_geometry, 1)
                && wavefront_parse_vertex_component(buffer, idx, out_geometry, 2);

        range_push(RANGE_TO_ANY(out_geometry->colors), &(union color) { 0 });
        // parse the required r, g, and b
        is_valid = is_valid && wavefront_parse_vertex_color(buffer, idx, out_geometry, 0)
                && wavefront_parse_vertex_color(buffer, idx, out_geometry, 1)
                && wavefront_parse_vertex_color(buffer, idx, out_geometry, 2);

        // parse w
        if (!wavefront_parse_vertex_component(buffer, idx, out_geometry, 3)) {
            // if no w, then set the default value
            RANGE_LAST(out_geometry->vertices).array[3] =  1.0;
        }

        return is_valid;
    }

    return 0;
}

static i32 wavefront_parse_vertex_component(const BUFFER *buffer, size_t *idx, struct geometry *out_geometry, u8 vertex_comp)
{
    // read a value
    if ((buffer->data[*idx] == '+') || (buffer->data[*idx] == '-') || is_numerical(buffer->data[*idx])) {
        RANGE_LAST(out_geometry->vertices).array[vertex_comp] = read_value(buffer, idx);
        return 1;
    }

    return 0;
}

static i32 wavefront_parse_vertex_color(const BUFFER *buffer, size_t *idx, struct geometry *out_geometry, u8 color_comp)
{
    if (is_numerical(buffer->data[*idx])) {
        RANGE_LAST(out_geometry->colors).array[color_comp] = read_value(buffer, idx);
        return 1;
    }

    return 0;
}

static i32 wavefront_parse_end_of_obj(const BUFFER *buffer, size_t *idx, struct geometry *out_geometry)
{
    (void) out_geometry;

    return (*idx >= buffer->length);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 * @param buffer
 * @param idx
 * @return f32
 */
static f32 read_value(const BUFFER *buffer, size_t *idx)
{
    f32 sign = 1.f;
    f32 integral_part = 0.f;
    f32 fract_part = 0.f;
    f32 fract_div = 1.f;

    if (buffer->data[*idx] == '-') {
        sign = -1;
        *idx += 1;
    } else if (buffer->data[*idx] == '+') {
        *idx += 1;
    }

    // remove leading zeroes
    while ((*idx < buffer->length) && (buffer->data[*idx] == '0')) {
        *idx += 1;
    }

    // read integral part
    while ((*idx < buffer->length) && is_numerical(buffer->data[*idx])) {
        integral_part *= 10.f;
        integral_part += (f32) (buffer->data[*idx] - '0');
        *idx += 1;
    }

    // read optional fract part
    if ((*idx < buffer->length) && (buffer->data[*idx] == '.')) {
        *idx += 1;

        while ((*idx < buffer->length) && is_numerical(buffer->data[*idx])) {
            fract_div *= 10.f;
            fract_part += (f32) (buffer->data[*idx] - '0') / fract_div;
            *idx += 1;
        }
    }
    *idx += 1;

    return sign * integral_part + fract_part;
}

/**
 * @brief
 *
 * @param buffer
 * @param idx
 */
static void skip_whitespace(const BUFFER *buffer, size_t *idx)
{
    while ((*idx < buffer->length) && ((buffer->data[*idx] == ' ') || (buffer->data[*idx] == '\t'))) {
        *idx += 1;
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