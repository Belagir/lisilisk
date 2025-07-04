
#include "3ful_geometry_parsing.h"

#include <stdio.h>

// -----------------------------------------------------------------------------
// UTILITY FUNCTIONS -----------------------------------------------------------
static i32 accept(const BUFFER *buffer, size_t *buffer_idx, char *alternatives, size_t nb, char *c_out);
static i32 expect(const BUFFER *buffer, size_t *buffer_idx, char *alternatives, size_t nb, char *c_out);
static i32 lookup(const BUFFER *buffer, size_t *buffer_idx, char *alternatives, size_t nb, char *c_out);

static void skip_whitespace(const BUFFER *buffer, size_t *buffer_idx);
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// PARSING ROUTINES ------------------------------------------------------------
static i32 wavefront_parse_comment(const BUFFER *buffer, size_t *buffer_idx, struct wavefront_obj *out_obj);
static i32 wavefront_parse_end_line(const BUFFER *buffer, size_t *buffer_idx, struct wavefront_obj *out_obj);
static i32 wavefront_parse_end_of_obj(const BUFFER *buffer, size_t *buffer_idx, struct wavefront_obj *out_obj);

static i32 wavefront_parse_obj_name(const BUFFER *buffer, size_t *buffer_idx, struct wavefront_obj *out_obj);
static i32 wavefront_parse_vertex(const BUFFER *buffer, size_t *buffer_idx, struct wavefront_obj *out_obj);
static i32 wavefront_parse_vertex_pos(const BUFFER *buffer, size_t *buffer_idx, struct wavefront_obj *out_obj);
static i32 wavefront_parse_vertex_normal(const BUFFER *buffer, size_t *buffer_idx, struct wavefront_obj *out_obj);
static i32 wavefront_parse_face(const BUFFER *buffer, size_t *buffer_idx, struct wavefront_obj *out_obj);

static i32 wavefront_parse_value(const BUFFER *buffer, size_t *buffer_idx, f32 *out_value);
static i32 wavefront_parse_value_int(const BUFFER *buffer, size_t *buffer_idx, i32 *out_value);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 * @param obj
 */
void wavefront_obj_create(struct wavefront_obj *obj)
{
    *obj = (struct wavefront_obj) {
            .v  = range_create_dynamic(make_system_allocator(), sizeof(*obj->v->data), 256),
            .vn = range_create_dynamic(make_system_allocator(), sizeof(*obj->vn->data), 256),
            .f  = range_create_dynamic(make_system_allocator(), sizeof(*obj->f->data), 256),
    };
}

/**
 * @brief
 *
 * @param obj
 */
void wavefront_obj_delete(struct wavefront_obj *obj)
{
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(obj->v));
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(obj->vn));
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(obj->f));
}

/**
 * @brief
 *
 * @param obj
 * @param buffer
 */
void wavefront_obj_parse(struct wavefront_obj *obj, BUFFER *buffer)
{
    range_clear(RANGE_TO_ANY(obj->f));
    range_clear(RANGE_TO_ANY(obj->v));
    range_clear(RANGE_TO_ANY(obj->vn));

    size_t buffer_idx = 0;

    while (!wavefront_parse_end_of_obj(buffer, &buffer_idx, obj)) {
        skip_whitespace(buffer, &buffer_idx);

        if (wavefront_parse_end_line(buffer, &buffer_idx, obj)) {
            // NOP
        } else if (wavefront_parse_comment(buffer, &buffer_idx, obj)) {
            // NOP
        } else if (wavefront_parse_obj_name(buffer, &buffer_idx, obj)) {
            // NOP
        } else if (wavefront_parse_vertex(buffer, &buffer_idx, obj)) {
            // NOP
        } else if (wavefront_parse_face(buffer, &buffer_idx, obj)) {
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
 * @param obj
 * @param geometry
 */
void wavefront_obj_to(struct wavefront_obj *obj, struct geometry *geometry)
{
    u32 idx_face = 0;
    struct wavefront_obj_face face = { };
    u32 face_generated_indices[3] = { 0 };

    for (size_t i = 0 ; i < obj->f->length ; i++) {

        face = obj->f->data[i];
        for (size_t j = 0 ; j < 3 ; j++) {
            geometry_push_vertex(geometry, &face_generated_indices[j]);

            geometry_vertex_pos(geometry, face_generated_indices[j], obj->v->data[face.v_idx[j]]);
            geometry_vertex_normal(geometry, face_generated_indices[j], obj->vn->data[face.vn_idx[j]]);
        }

        geometry_push_face(geometry, &idx_face);
        geometry_face_indices(geometry, idx_face, face_generated_indices);
    }

    struct vertex v = { };
    for (size_t i = 0 ; i < geometry->vertices->length ; i++) {
        v = geometry->vertices->data[i];
        printf("% 4.1f % 4.1f % 4.1f ; % 4.1f % 4.1f % 4.1f\n", v.pos.x, v.pos.y, v.pos.z, v.normal.x, v.normal.y, v.normal.z);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static i32 wavefront_parse_end_line(const BUFFER *buffer, size_t *buffer_idx, struct wavefront_obj *out_obj)
{
    (void) out_obj;
    return accept(buffer, buffer_idx, (char []) { '\n' }, 1, NULL);
}

static i32 wavefront_parse_comment(const BUFFER *buffer, size_t *buffer_idx, struct wavefront_obj *out_obj)
{
    (void) out_obj;

    // detect comment character
    if (accept(buffer, buffer_idx, (char []) { '#' }, 1, NULL)) {
        while (!lookup(buffer, buffer_idx, (char []) { '\n' }, 1, NULL)) {
            *buffer_idx += 1;
        }
        return 1;
    }
    return 0;
}

static i32 wavefront_parse_obj_name(const BUFFER *buffer, size_t *buffer_idx, struct wavefront_obj *out_obj)
{
    (void) out_obj;

    // detect 'o' starting letter
    if (accept(buffer, buffer_idx, (char []) { 'o' }, 1, NULL)) {
        while (!lookup(buffer, buffer_idx, (char []) { '\n' }, 1, NULL)) {
            *buffer_idx += 1;
        }
        return 1;
    }
    return 0;
}

static i32 wavefront_parse_vertex(const BUFFER *buffer, size_t *buffer_idx, struct wavefront_obj *out_obj)
{
    skip_whitespace(buffer, buffer_idx);

    if (accept(buffer, buffer_idx, (char []) { 'v' }, 1, NULL)) {

        if (accept(buffer, buffer_idx, (char []) { 'n' }, 1, NULL)) {
            return wavefront_parse_vertex_normal(buffer, buffer_idx, out_obj);
        } else {
            return wavefront_parse_vertex_pos(buffer, buffer_idx, out_obj);
        }
    }

    return 0;
}

static i32 wavefront_parse_vertex_pos(const BUFFER *buffer, size_t *buffer_idx, struct wavefront_obj *out_obj)
{
    vector3_t pos = { 0 };

    skip_whitespace(buffer, buffer_idx);

    if (wavefront_parse_value(buffer, buffer_idx, &pos.x)
            && wavefront_parse_value(buffer, buffer_idx, &pos.y)
            && wavefront_parse_value(buffer, buffer_idx, &pos.z)) {

        range_push(RANGE_TO_ANY(out_obj->v), &pos);
        return 1;
    }

    return 0;
}

static i32 wavefront_parse_vertex_normal(const BUFFER *buffer, size_t *buffer_idx, struct wavefront_obj *out_obj)
{
    vector3_t normal = { 0 };

    skip_whitespace(buffer, buffer_idx);

    if (wavefront_parse_value(buffer, buffer_idx, &normal.x)
            && wavefront_parse_value(buffer, buffer_idx, &normal.y)
            && wavefront_parse_value(buffer, buffer_idx, &normal.z)) {

        range_push(RANGE_TO_ANY(out_obj->vn), &normal);
        return 1;
    }

    return 0;

}

static i32 wavefront_parse_face(const BUFFER *buffer, size_t *buffer_idx, struct wavefront_obj *out_obj)
{
    struct wavefront_obj_face face = { 0 };
    i32 read_values[3] = { 0 };

    if (accept(buffer, buffer_idx, (char []) { 'f' }, 1, NULL)) {
        skip_whitespace(buffer, buffer_idx);

        if (wavefront_parse_value_int(buffer, buffer_idx, &read_values[0])
                && wavefront_parse_value_int(buffer, buffer_idx, &read_values[1])
                && wavefront_parse_value_int(buffer, buffer_idx, &read_values[2])) {

            face.v_idx[0] = ((u32) read_values[0]) - 1;
            face.v_idx[1] = ((u32) read_values[1]) - 1;
            face.v_idx[2] = ((u32) read_values[2]) - 1;

            range_push(RANGE_TO_ANY(out_obj->f), &face);
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

static i32 wavefront_parse_end_of_obj(const BUFFER *buffer, size_t *buffer_idx, struct wavefront_obj *out_obj)
{
    (void) out_obj;

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
