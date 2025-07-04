
#include "3dful_core.h"

// -----------------------------------------------------------------------------
// UTILITY FUNCTIONS -----------------------------------------------------------
static i32 accept(const BUFFER *buffer, size_t *buffer_idx, char *alternatives, size_t nb, char *c_out);
static i32 expect(const BUFFER *buffer, size_t *buffer_idx, char *alternatives, size_t nb, char *c_out);
static i32 lookup(const BUFFER *buffer, size_t *buffer_idx, char *alternatives, size_t nb, char *c_out);

static void skip_whitespace(const BUFFER *buffer, size_t *buffer_idx);
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// PARSING ROUTINES ------------------------------------------------------------
static i32 wavefront_parse_comment(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry);
static i32 wavefront_parse_end_line(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry);
static i32 wavefront_parse_end_of_obj(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry);

static i32 wavefront_parse_obj_name(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry);
static i32 wavefront_parse_vertex(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry);
static i32 wavefront_parse_face(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry);

static i32 wavefront_parse_value(const BUFFER *buffer, size_t *buffer_idx, f32 *out_value);
static i32 wavefront_parse_value_int(const BUFFER *buffer, size_t *buffer_idx, i32 *out_value);

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

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static i32 wavefront_parse_end_line(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry)
{
    (void) out_geometry;
    return accept(buffer, buffer_idx, (char []) { '\n' }, 1, NULL);
}

static i32 wavefront_parse_comment(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry)
{
    (void) out_geometry;

    // detect comment character
    if (accept(buffer, buffer_idx, (char []) { '#' }, 1, NULL)) {
        while (!lookup(buffer, buffer_idx, (char []) { '\n' }, 1, NULL)) {
            *buffer_idx += 1;
        }
        return 1;
    }
    return 0;
}

static i32 wavefront_parse_obj_name(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry)
{
    char name_c = '\0';

    // detect 'o' starting letter
    if (accept(buffer, buffer_idx, (char []) { 'o' }, 1, NULL)) {
        skip_whitespace(buffer, buffer_idx);

        // take all characters after for the name
        while (!lookup(buffer, buffer_idx, (char []) { '\n' }, 1, &name_c)) {
            *buffer_idx += 1;
            range_push(RANGE_TO_ANY(out_geometry->name), &name_c);
        }
        range_push(RANGE_TO_ANY(out_geometry->name), &(char) { '\0' });

        return 1;
    }
    return 0;
}

static i32 wavefront_parse_vertex(const BUFFER *buffer, size_t *buffer_idx, struct geometry *out_geometry)
{
    vector3_t pos = { 0 };

    if (accept(buffer, buffer_idx, (char []) { 'v' }, 1, NULL)) {
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
    i32 read_values[3] = { 0 };

    if (accept(buffer, buffer_idx, (char []) { 'f' }, 1, NULL)) {
        skip_whitespace(buffer, buffer_idx);

        if (wavefront_parse_value_int(buffer, buffer_idx, &read_values[0])
                && wavefront_parse_value_int(buffer, buffer_idx, &read_values[1])
                && wavefront_parse_value_int(buffer, buffer_idx, &read_values[2])) {

            face.idx_vert[0] = ((u32) read_values[0]) - 1;
            face.idx_vert[1] = ((u32) read_values[1]) - 1;
            face.idx_vert[2] = ((u32) read_values[2]) - 1;

            range_push(RANGE_TO_ANY(out_geometry->faces), &face);
            return 1;
        }
    }
    return 0;
}

static i32 wavefront_parse_value(const BUFFER *buffer, size_t *buffer_idx, f32 *out_value)
{
    i32 int_part = 0;
    i32 frac_part = 0;
    u32 frac_part_length = 1;

    skip_whitespace(buffer, buffer_idx);

    if (!wavefront_parse_value_int(buffer, buffer_idx, &int_part)) {
        return 0;
    }

    if (accept(buffer, buffer_idx, (char[]) { '.' }, 1, NULL)) {
        wavefront_parse_value_int(buffer, buffer_idx, &frac_part);
    }

    while ((u32) frac_part > (frac_part_length * 10)) {
        frac_part_length *= 10;
    }

    *out_value = (f32) int_part + ((f32) frac_part / (f32) frac_part_length);

    return 1;
}

static i32 wavefront_parse_value_int(const BUFFER *buffer, size_t *buffer_idx, i32 *out_value)
{
    i32 value = 0;
    i32 sign = 1;
    i32 mult = 1;
    i32 valid = 0;
    char read_c = '+';

    skip_whitespace(buffer, buffer_idx);

    accept(buffer, buffer_idx, (char[]) { '+', '-' }, 2, &read_c);
    if (read_c == '-') sign = -1;

    if (!expect(buffer, buffer_idx, (char[]) { '0','1','2','3','4','5','6','7','8','9', }, 10, &read_c)) {
        return 0;
    }

    do {
        valid = 1;
        value *= mult;
        value += read_c - '0';
        mult *= 10;
    } while (accept(buffer, buffer_idx, (char[]) { '0','1','2','3','4','5','6','7','8','9', }, 10, &read_c));

    if (valid) {
        *out_value = sign * value;
    }

    return valid;
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
 * @param c
 * @return i32
 */
static i32 accept(const BUFFER *buffer, size_t *buffer_idx, char *alternatives, size_t nb, char *c_out)
{
    if (lookup(buffer, buffer_idx, alternatives, nb, c_out)) {
        *buffer_idx += 1;
        return 1;
    }

    return 0;
}

/**
 * @brief
 *
 * @param buffer
 * @param buffer_idx
 * @param c
 * @return i32
 */
static i32 expect(const BUFFER *buffer, size_t *buffer_idx, char *alternatives, size_t nb, char *c_out)
{
    if (accept(buffer, buffer_idx, alternatives, nb, c_out)) {
        return 1;
    }

    fprintf(stderr, "expected one of : ");
    for (size_t i = 0 ; i < nb ; i++) {
        fprintf(stderr, "%c ", alternatives[i]);
    }

    fprintf(stderr, "\nbut found : ");
    if (*buffer_idx >= buffer->length) fprintf(stderr, "end of stream.\n");
    else fprintf(stderr, "%c\n", buffer->data[*buffer_idx]);

    return 0;
}

/**
 * @brief
 *
 * @param buffer
 * @param buffer_idx
 * @param c
 * @return i32
 */
static i32 lookup(const BUFFER *buffer, size_t *buffer_idx, char *alternatives, size_t nb, char *c_out)
{
    i32 found = 0;
    size_t alt_idx = 0;

    if (*buffer_idx >= buffer->length) {
        return 0;
    }

    while (!found && (alt_idx < nb)) {
        found = buffer->data[*buffer_idx] == alternatives[alt_idx];
        alt_idx += 1;
    }

    if (c_out) {
        *c_out = buffer->data[*buffer_idx];
    }

    return found;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 * @param buffer
 * @param buffer_idx
 */
static void skip_whitespace(const BUFFER *buffer, size_t *buffer_idx)
{
    while (accept(buffer, buffer_idx, (char[]) { ' ', '\t' }, 2, NULL));
}
