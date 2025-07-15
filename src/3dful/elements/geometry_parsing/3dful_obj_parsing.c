
#include "3ful_geometry_parsing.h"

#include <stdio.h>

#include <ustd/array.h>

struct parser_state {
    const BUFFER *buffer;
    size_t buffer_idx;
    u32 line, column;
};

static void parser_state_advance(struct parser_state *state);

// -----------------------------------------------------------------------------
// UTILITY FUNCTIONS -----------------------------------------------------------
static i32 accept(struct parser_state *state, char *alternatives, size_t nb, char *c_out);
static i32 expect(struct parser_state *state, char *alternatives, size_t nb, char *c_out);
static i32 lookup(struct parser_state *state, char *alternatives, size_t nb, char *c_out);

static void skip_whitespace(struct parser_state *state);
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// PARSING ROUTINES ------------------------------------------------------------
static i32 wavefront_parse_comment(struct parser_state *state, struct wavefront_obj *out_obj);
static i32 wavefront_parse_end_line(struct parser_state *state, struct wavefront_obj *out_obj);
static i32 wavefront_parse_end_of_obj(struct parser_state *state, struct wavefront_obj *out_obj);

static i32 wavefront_parse_obj_name(struct parser_state *state, struct wavefront_obj *out_obj);
static i32 wavefront_parse_obj_smoothing(struct parser_state *state, struct wavefront_obj *out_obj);
static i32 wavefront_parse_vertex(struct parser_state *state, struct wavefront_obj *out_obj);
static i32 wavefront_parse_vertex_pos(struct parser_state *state, struct wavefront_obj *out_obj);
static i32 wavefront_parse_vertex_normal(struct parser_state *state, struct wavefront_obj *out_obj);
static i32 wavefront_parse_face(struct parser_state *state, struct wavefront_obj *out_obj);
static i32 wavefront_parse_face_point(struct parser_state *state, i32 read_idx[3]);

static i32 wavefront_parse_value(struct parser_state *state, f32 *out_value);
static i32 wavefront_parse_value_int(struct parser_state *state, i32 *out_value);
static i32 wavefront_parse_value_uint(struct parser_state *state, i32 *out_value);

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
            .v  = array_create(make_system_allocator(), sizeof(*obj->v), 32),
            .vn = array_create(make_system_allocator(), sizeof(*obj->vn), 32),
            .f  = array_create(make_system_allocator(), sizeof(*obj->f), 32),
    };
}

/**
 * @brief
 *
 * @param obj
 */
void wavefront_obj_delete(struct wavefront_obj *obj)
{
    array_destroy(make_system_allocator(), (void **) &obj->v);
    array_destroy(make_system_allocator(), (void **) &obj->vn);
    array_destroy(make_system_allocator(), (void **) &obj->f);
}

/**
 * @brief
 *
 * @param obj
 * @param buffer
 */
void wavefront_obj_parse(struct wavefront_obj *obj, BUFFER *buffer)
{
    array_clear(obj->f);
    array_clear(obj->v);
    array_clear(obj->vn);

    struct parser_state state = { .buffer = buffer, 0 };

    while (!wavefront_parse_end_of_obj(&state, obj)) {
        skip_whitespace(&state);

        if (wavefront_parse_end_line(&state, obj)) {
            // NOP
        } else if (wavefront_parse_comment(&state, obj)) {
            // NOP
        } else if (wavefront_parse_obj_name(&state, obj)) {
            // NOP
        } else if (wavefront_parse_obj_smoothing(&state, obj)) {
            // NOP
        } else if (wavefront_parse_vertex(&state, obj)) {
            // NOP
        } else if (wavefront_parse_face(&state, obj)) {
            // NOP
        } else {
            fprintf(stderr, "at line %d:%d ; parsing error. The resulting geometry may be malformed.\n",
                    state.line+1, state.column+1);
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

    for (size_t i = 0 ; i < array_length(obj->f) ; i++) {

        face = obj->f[i];
        for (size_t j = 0 ; j < 3 ; j++) {
            geometry_push_vertex(geometry, &face_generated_indices[j]);

            geometry_vertex_pos(geometry, face_generated_indices[j], obj->v[face.v_idx[j]]);
            geometry_vertex_normal(geometry, face_generated_indices[j], obj->vn[face.vn_idx[j]]);
        }

        geometry_push_face(geometry, &idx_face);
        geometry_face_indices(geometry, idx_face, face_generated_indices);
    }
}

/**
 * @brief
 *
 * @param obj
 * @param file
 */
void wavefront_obj_dump(struct wavefront_obj *obj, FILE *file)
{
    for (size_t i = 0 ; i < array_length(obj->v) ; i++) {
        fprintf(file, "v %.6f %.6f %.6f\n", obj->v[i].x, obj->v[i].y, obj->v[i].z);
    }
    for (size_t i = 0 ; i < array_length(obj->vn) ; i++) {
        fprintf(file, "vn %.4f %.4f %.4f\n", obj->vn[i].x, obj->vn[i].y, obj->vn[i].z);
    }
    for (size_t i = 0 ; i < array_length(obj->f) ; i++) {
        fprintf(file, "f %d//%d %d//%d %d//%d\n", obj->f[i].v_idx[0]+1, obj->f[i].vn_idx[0]+1,
                obj->f[i].v_idx[1]+1, obj->f[i].vn_idx[1]+1, obj->f[i].v_idx[2]+1, obj->f[i].vn_idx[2]+1);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static i32 wavefront_parse_end_line(struct parser_state *state, struct wavefront_obj *out_obj)
{
    (void) out_obj;

    return accept(state, (char []) { '\n' }, 1, NULL);
}

static i32 wavefront_parse_comment(struct parser_state *state, struct wavefront_obj *out_obj)
{
    (void) out_obj;

    // detect comment character
    if (!accept(state, (char []) { '#' }, 1, NULL)) {
        return 0;
    }

    while (!lookup(state, (char []) { '\n' }, 1, NULL)) {
        parser_state_advance(state);
    }
    return 1;
}

static i32 wavefront_parse_obj_name(struct parser_state *state, struct wavefront_obj *out_obj)
{
    (void) out_obj;

    // detect 'o' starting letter
    if (!accept(state, (char []) { 'o' }, 1, NULL)) {
        return 0;
    }

    while (!lookup(state, (char []) { '\n' }, 1, NULL)) {
        parser_state_advance(state);
    }
    return 1;
}

static i32 wavefront_parse_obj_smoothing(struct parser_state *state, struct wavefront_obj *out_obj)
{
    (void) out_obj;

    // detect 'o' starting letter
    if (!accept(state, (char []) { 's' }, 1, NULL)) {
        return 0;
    }

    while (!lookup(state, (char []) { '\n' }, 1, NULL)) {
        parser_state_advance(state);
    }
    return 1;
}

static i32 wavefront_parse_vertex(struct parser_state *state, struct wavefront_obj *out_obj)
{
    skip_whitespace(state);

    if (!accept(state, (char []) { 'v' }, 1, NULL)) {
        return 0;
    }

    if (accept(state, (char []) { 'n' }, 1, NULL)) {
        return wavefront_parse_vertex_normal(state, out_obj);
    }

    return wavefront_parse_vertex_pos(state, out_obj);
}

static i32 wavefront_parse_vertex_pos(struct parser_state *state, struct wavefront_obj *out_obj)
{
    vector3 pos = { 0 };

    skip_whitespace(state);

    if (wavefront_parse_value(state, &pos.x)
            && wavefront_parse_value(state, &pos.y)
            && wavefront_parse_value(state, &pos.z)) {

        array_ensure_capacity(make_system_allocator(), (void **) &out_obj->v, 1);
        array_push(out_obj->v, &pos);

        return 1;
    }

    return 0;
}

static i32 wavefront_parse_vertex_normal(struct parser_state *state, struct wavefront_obj *out_obj)
{
    vector3 normal = { 0 };

    skip_whitespace(state);

    if (wavefront_parse_value(state, &normal.x)
            && wavefront_parse_value(state, &normal.y)
            && wavefront_parse_value(state, &normal.z)) {

        array_ensure_capacity(make_system_allocator(), (void **) &out_obj->vn, 1);
        array_push(out_obj->vn, &normal);
        return 1;
    }

    return 0;

}

static i32 wavefront_parse_face(struct parser_state *state, struct wavefront_obj *out_obj)
{
    struct wavefront_obj_face face = { 0 };
    i32 face_data[3][3] = { 0 };

    if (!accept(state, (char []) { 'f' }, 1, NULL)) {
        return 0;
    }

    if (!(wavefront_parse_face_point(state, face_data[0])
            && wavefront_parse_face_point(state, face_data[1])
            && wavefront_parse_face_point(state, face_data[2]))) {
        return 0;
    }

    for (size_t i = 0 ; i < 3 ; i++) {
        face.v_idx[i] = face_data[i][0] - 1;
        // if (face_data[i][1] > 0) face.vt_idx[i] = face_data[i][1] - 1;
        if (face_data[i][2] > 0) face.vn_idx[i] = face_data[i][2] - 1;
    }

    array_ensure_capacity(make_system_allocator(), (void **) &out_obj->f, 1);
    array_push(out_obj->f, &face);
    return 1;
}

static i32 wavefront_parse_face_point(struct parser_state *state, i32 read_idx[3])
{
    skip_whitespace(state);

    read_idx[0] = 0;
    read_idx[1] = 0;
    read_idx[2] = 0;

    if (!wavefront_parse_value_uint(state, &read_idx[0])) {
        return 0;
    }

    if (!accept(state, (char []) { '/' }, 1, NULL)) {
        return 1;
    }

    if (!accept(state, (char []) { '/' }, 1, NULL)) {
        wavefront_parse_value_uint(state, &read_idx[1]);
        if (accept(state, (char []) { '/' }, 1, NULL)) {
            return wavefront_parse_value_uint(state, &read_idx[2]);
        } else {
            return 1;
        }
    }

    return wavefront_parse_value_int(state, &read_idx[2]);
}

static i32 wavefront_parse_value(struct parser_state *state, f32 *out_value)
{
    i32 parsed_value = 0;
    f32 int_part = 0.f;
    f32 frac_part = 0.f;
    f32 sign = 1.f;
    size_t frac_part_start = 0;
    size_t frac_part_length = 0;

    skip_whitespace(state);

    if (accept(state, (char[]) { '-' }, 1, NULL)) {
        sign = -1.f;
    }

    if (!wavefront_parse_value_uint(state, &parsed_value)) {
        return 0;
    }
    int_part = parsed_value;

    if (accept(state, (char[]) { '.' }, 1, NULL)) {
        frac_part_start = state->buffer_idx;
        wavefront_parse_value_uint(state, &parsed_value);
        frac_part_length = state->buffer_idx - frac_part_start;
        frac_part = (f32) parsed_value;
    }

    for (size_t i = 0 ; i < frac_part_length ; i++) {
        frac_part /= 10.f;
    }

    *out_value = sign * (int_part + frac_part);

    return 1;
}

static i32 wavefront_parse_value_int(struct parser_state *state, i32 *out_value)
{
    i32 value = 0;
    i32 sign = 1;
    i32 valid = 0;
    char read_c = '+';

    skip_whitespace(state);

    if (lookup(state, (char[]) { '+', '-' }, 2, &read_c)) {
        if (read_c == '-') sign = -1;
        parser_state_advance(state);
    }

    valid = wavefront_parse_value_uint(state, &value);
    if (valid) {
        *out_value = sign * value;
    }

    return valid;
}

static i32 wavefront_parse_value_uint(struct parser_state *state, i32 *out_value)
{
    i32 value = 0;
    i32 valid = 0;
    char read_c = '\0';

    if (!expect(state, (char[]) { '0','1','2','3','4','5','6','7','8','9', }, 10, &read_c)) {
        return 0;
    }

    do {
        valid = 1;
        value *= 10;
        value += read_c - '0';
    } while (accept(state, (char[]) { '0','1','2','3','4','5','6','7','8','9', }, 10, &read_c));

    if (valid) {
        *out_value = value;
    }

    return valid;
}

static i32 wavefront_parse_end_of_obj(struct parser_state *state, struct wavefront_obj *out_obj)
{
    (void) out_obj;

    return ((state->buffer_idx+1) >= state->buffer->length);
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
static i32 accept(struct parser_state *state, char *alternatives, size_t nb, char *c_out)
{
    if (lookup(state, alternatives, nb, c_out)) {
        parser_state_advance(state);
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
static i32 expect(struct parser_state *state, char *alternatives, size_t nb, char *c_out)
{
    if (accept(state, alternatives, nb, c_out)) {
        return 1;
    }

    fprintf(stderr, "at line %d:%d ; ", state->line+1, state->column+1);

    fprintf(stderr, "expected ");
    if (nb == 0) {
        fprintf(stderr, "end of stream");
    } else {
        fprintf(stderr, "one of : ");
        for (size_t i = 0 ; i < nb ; i++) {
            fprintf(stderr, "%c ", alternatives[i]);
        }
    }

    fprintf(stderr, "\nbut found : ");
    if (state->buffer_idx >= state->buffer->length) fprintf(stderr, "end of stream.\n");
    else fprintf(stderr, "%c\n", state->buffer->data[state->buffer_idx]);

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
static i32 lookup(struct parser_state *state, char *alternatives, size_t nb, char *c_out)
{
    i32 found = 0;
    size_t alt_idx = 0;

    if (state->buffer_idx >= state->buffer->length) {
        return 0;
    }

    while (!found && (alt_idx < nb)) {
        found = state->buffer->data[state->buffer_idx] == alternatives[alt_idx];
        alt_idx += 1;
    }

    if (c_out) {
        *c_out = state->buffer->data[state->buffer_idx];
    }

    return found;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static void parser_state_advance(struct parser_state *state)
{
    if (state->buffer_idx >= state->buffer->length) {
        return;
    }

    if (state->buffer->data[state->buffer_idx] == '\n') {
        state->column = 0;
        state->line  += 1;
    } else {
        state->column += 1;
    }

    state->buffer_idx += 1;
}

/**
 * @brief
 *
 * @param buffer
 * @param buffer_idx
 */
static void skip_whitespace(struct parser_state *state)
{
    while (accept(state, (char[]) { ' ', '\t' }, 2, NULL));
}
