
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
static i32 wavefront_parse_value(const BUFFER *buffer, size_t *buffer_idx, f32 *out_value);
static i32 wavefront_parse_end_of_obj(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param alloc 
 * @return struct geometry 
 */
struct geometry geometry_create_empty(struct allocator alloc)
{
    struct geometry geometry = {
            .name     = range_create_dynamic(alloc, sizeof(*geometry.name->data), 64),
            .vertices = range_create_dynamic(alloc, sizeof(*geometry.vertices->data), 512),
            .colors   = range_create_dynamic(alloc, sizeof(*geometry.colors->data), 512),
    };

    return geometry;
}

/**
 * @brief 
 * 
 * @param geometry 
 */
void geometry_destroy(struct allocator alloc, struct geometry *geometry)
{
    if (!geometry) {
        return;
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY(geometry->name));
    range_destroy_dynamic(alloc, &RANGE_TO_ANY(geometry->colors));
    range_destroy_dynamic(alloc, &RANGE_TO_ANY(geometry->vertices));

    *geometry = (struct geometry) { 0 };
}


/**
 * @brief
 *
 * @param buffer
 * @return struct geometry
 */
void geometry_from_wavefront_obj(BUFFER *buffer, struct geometry *out_geometry)
{
    size_t buffer_idx = 0;

    skip_whitespace(buffer, &buffer_idx);

    while (!wavefront_parse_end_of_obj(buffer, &buffer_idx, out_geometry)) {
        if (wavefront_parse_end_line(buffer, &buffer_idx, out_geometry)) {
            // NOP
        } else if (wavefront_parse_comment(buffer, &buffer_idx, out_geometry)) {
            // NOP
        } else if (wavefront_parse_obj_name(buffer, &buffer_idx, out_geometry)) {
            // NOP
        } else if (wavefront_parse_vertex(buffer, &buffer_idx, out_geometry)) {
            // NOP
        } else {
            // ERROR SITE
            break;
        }
    }

    for (size_t i = 0 ; i < out_geometry->vertices->length ; i++) {
        printf("(%f\t%f\t%f\t%f) -- ", 
                out_geometry->vertices->data[i].x, 
                out_geometry->vertices->data[i].y, 
                out_geometry->vertices->data[i].z, 
                out_geometry->vertices->data[i].w);
        printf("(%f\t%f\t%f)\n", 
                out_geometry->colors->data[i].r, 
                out_geometry->colors->data[i].g, 
                out_geometry->colors->data[i].b);
    }
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
    union color col = { 0 };
    f32 w_or_r = 0.f;

    if (buffer->data[*buffer_idx] == 'v') {
        *buffer_idx += 1;
        skip_whitespace(buffer, buffer_idx);
        
        if (wavefront_parse_value(buffer, buffer_idx, &pos.x)
                && wavefront_parse_value(buffer, buffer_idx, &pos.y)
                && wavefront_parse_value(buffer, buffer_idx, &pos.z))
        {
            pos.w = 1.0;

            if (wavefront_parse_value(buffer, buffer_idx, &w_or_r)) {
                if (wavefront_parse_value(buffer, buffer_idx, &col.g)
                    && wavefront_parse_value(buffer, buffer_idx, &col.b)) {
                    col.r = w_or_r;
                    wavefront_parse_value(buffer, buffer_idx, &pos.w);
                } else {
                    pos.w = w_or_r;
                }
            }
            

            range_push(RANGE_TO_ANY(out_geometry->vertices), &pos);
            range_push(RANGE_TO_ANY(out_geometry->colors), &col);
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

    return sign * integral_part + fract_part;
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