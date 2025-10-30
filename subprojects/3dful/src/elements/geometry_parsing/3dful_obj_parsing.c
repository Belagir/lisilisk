/**
 * @file 3dful_obj_parsing.c
 * @author Gabriel Bédat
 * @brief Implementation of the Wavefront OBJ parser.
 * @version 0.1
 * @date 2025-07-25
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "3ful_geometry_parsing.h"

#include <stdio.h>

#include <ustd/array.h>
#include <ustd/parsing.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static i32 wavefront_parse_comment(struct parser_state *state,
        struct wavefront_obj *out_obj);
static i32 wavefront_parse_obj_name(struct parser_state *state,
        struct wavefront_obj *out_obj);
static i32 wavefront_parse_obj_smoothing(struct parser_state *state,
        struct wavefront_obj *out_obj);
static i32 wavefront_parse_vertex(struct parser_state *state,
        struct wavefront_obj *out_obj);
static i32 wavefront_parse_vertex_pos(struct parser_state *state,
        struct wavefront_obj *out_obj);
static i32 wavefront_parse_vertex_normal(struct parser_state *state,
        struct wavefront_obj *out_obj);
static i32 wavefront_parse_vertex_texture(struct parser_state *state,
        struct wavefront_obj *out_obj);
static i32 wavefront_parse_face(struct parser_state *state,
        struct wavefront_obj *out_obj);
static i32 wavefront_parse_face_point(struct parser_state *state,
        i32 read_idx[3]);


// -----------------------------------------------------------------------------

static void geometry_add_face_from_parsed(i32 face_data[3][3],
        struct wavefront_obj *out_obj);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Allocates memory needed to parse an .obj file.
 *
 * @param[out] obj Initialized parser object.
 */
void wavefront_obj_create(struct wavefront_obj *obj)
{
    *obj = (struct wavefront_obj) {
            .v_array  = array_create(make_system_allocator(),
                    sizeof(*obj->v_array), 32),
            .vn_array = array_create(make_system_allocator(),
                    sizeof(*obj->vn_array), 32),
            .vt_array = array_create(make_system_allocator(),
                    sizeof(*obj->vt_array), 32),
            .f_array  = array_create(make_system_allocator(),
                    sizeof(*obj->f_array), 32),
            .smooth = false,
    };
}

/**
 * @brief Releases memory taken by a parser object.
 *
 * @param[inout] obj Destroyed object.
 */
void wavefront_obj_delete(struct wavefront_obj *obj)
{
    array_destroy(make_system_allocator(), (ARRAY_ANY *) &obj->v_array);
    array_destroy(make_system_allocator(), (ARRAY_ANY *) &obj->vn_array);
    array_destroy(make_system_allocator(), (ARRAY_ANY *) &obj->vt_array);
    array_destroy(make_system_allocator(), (ARRAY_ANY *) &obj->f_array);

    *obj = (struct wavefront_obj) { 0 };
}

/**
 * @brief Parses an .obj file, stored in a buffer. The parser object is cleared
 * before reading the buffer.
 *
 * @warning The buffer is expected to have been created with ustd/array.h.
 *
 * @param[inout] obj Parser object.
 * @param[in] buffer Parsed buffer.
 */
void wavefront_obj_parse(struct wavefront_obj *obj, const byte *buffer)
{
    array_clear(obj->f_array);
    array_clear(obj->v_array);
    array_clear(obj->vn_array);
    array_clear(obj->vt_array);
    obj->smooth = 0;

    struct parser_state state = { .buffer_array = buffer, 0 };

    while (!parser_parse_end_of_file(&state)) {
        parser_skip_whitespace(&state);

        if (parser_parse_end_line(&state)) {
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
            fprintf(stderr, "at line %d:%d ; parsing error. The resulting "
                    "geometry may be malformed.\n", state.line+1,
                    state.column+1);
            break;
        }
    }
}

/**
 * @brief Builds a geometry object from the data contained within a parser
 * object. The geometry is not cleared ; the contents of the parsed data is
 * appended to the contents already present.
 *
 * @param[in] obj Parser object.
 * @param[inout] geometry Target geometry object.
 */
void wavefront_obj_to(const struct wavefront_obj *obj,
        struct geometry *geometry)
{
    u32 idx_face = 0;
    struct wavefront_obj_face face = { };
    u32 face_generated_indices[3] = { 0 };

    for (size_t i = 0 ; i < array_length(obj->f_array) ; i++) {

        face = obj->f_array[i];
        for (size_t j = 0 ; j < 3 ; j++) {
            geometry_push_vertex(geometry, &face_generated_indices[j]);

            geometry_vertex_pos(geometry, face_generated_indices[j],
                    obj->v_array[face.v_idx[j]]);
            geometry_vertex_normal(geometry, face_generated_indices[j],
                    obj->vn_array[face.vn_idx[j]]);
            geometry_vertex_uv(geometry, face_generated_indices[j],
                    obj->vt_array[face.vt_idx[j]]);
        }

        geometry_push_face(geometry, &idx_face);
        geometry_face_indices(geometry, idx_face, face_generated_indices);
    }

    geometry_set_smoothing(geometry, obj->smooth);
}

/**
 * @brief Writes the .obj file back to some stream. Useful for debugging.
 *
 * @param[in] obj Parser object.
 * @param[in] file Target stream.
 */
void wavefront_obj_dump(struct wavefront_obj *obj, FILE *file)
{
    for (size_t i = 0 ; i < array_length(obj->v_array) ; i++) {
        fprintf(file, "v %.6f %.6f %.6f\n", obj->v_array[i].x,
                obj->v_array[i].y, obj->v_array[i].z);
    }
    for (size_t i = 0 ; i < array_length(obj->vn_array) ; i++) {
        fprintf(file, "vn %.4f %.4f %.4f\n", obj->vn_array[i].x,
                obj->vn_array[i].y, obj->vn_array[i].z);
    }
    for (size_t i = 0 ; i < array_length(obj->vt_array) ; i++) {
        fprintf(file, "vt %.4f %.4f\n", obj->vt_array[i].x,
                obj->vt_array[i].y);
    }

    fprintf(file, "s %c\n", obj->smooth? '1' : '0');

    for (size_t i = 0 ; i < array_length(obj->f_array) ; i++) {
        fprintf(file, "f %d//%d %d//%d %d//%d\n",
                obj->f_array[i].v_idx[0]+1, obj->f_array[i].vn_idx[0]+1,
                obj->f_array[i].v_idx[1]+1, obj->f_array[i].vn_idx[1]+1,
                obj->f_array[i].v_idx[2]+1, obj->f_array[i].vn_idx[2]+1);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static i32 wavefront_parse_comment(struct parser_state *state,
        struct wavefront_obj *out_obj)
{
    (void) out_obj;

    // detect comment character
    if (!parser_accept(state, (char []) { '#' }, 1, NULL)) {
        return 0;
    }

    while (!parser_lookup(state, (char []) { '\n' }, 1, NULL)) {
        parser_state_advance(state);
    }
    return 1;
}

static i32 wavefront_parse_obj_name(struct parser_state *state,
        struct wavefront_obj *out_obj)
{
    (void) out_obj;

    // detect 'o' starting letter
    if (!parser_accept(state, (char []) { 'o' }, 1, NULL)) {
        return 0;
    }

    while (!parser_lookup(state, (char []) { '\n' }, 1, NULL)) {
        parser_state_advance(state);
    }
    return 1;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static i32 wavefront_parse_obj_smoothing(struct parser_state *state,
        struct wavefront_obj *out_obj)
{
    (void) out_obj;
    char is_smooth = '0';

    // detect 'o' starting letter
    if (!parser_accept(state, (char []) { 's' }, 1, NULL)) {
        return 0;
    }

    parser_skip_whitespace(state);

    if (parser_expect(state, (char []) { '0', '1' }, 2, &is_smooth)) {
        out_obj->smooth = (is_smooth == '1');
    }

    return 1;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static i32 wavefront_parse_vertex(struct parser_state *state,
        struct wavefront_obj *out_obj)
{
    parser_skip_whitespace(state);

    if (!parser_accept(state, (char []) { 'v' }, 1, NULL)) {
        return 0;
    }

    if (parser_accept(state, (char []) { 'n' }, 1, NULL)) {
        return wavefront_parse_vertex_normal(state, out_obj);
    }

    if (parser_accept(state, (char []) { 't' }, 1, NULL)) {
        return wavefront_parse_vertex_texture(state, out_obj);
    }

    return wavefront_parse_vertex_pos(state, out_obj);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static i32 wavefront_parse_vertex_pos(struct parser_state *state,
        struct wavefront_obj *out_obj)
{
    vector3 pos = { 0 };

    parser_skip_whitespace(state);

    if (parser_parse_value_float(state, &pos.x)
            && parser_parse_value_float(state, &pos.y)
            && parser_parse_value_float(state, &pos.z)) {

        array_ensure_capacity(make_system_allocator(),
                (void **) &out_obj->v_array, 1);
        array_push(out_obj->v_array, &pos);

        return 1;
    }

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static i32 wavefront_parse_vertex_normal(struct parser_state *state,
        struct wavefront_obj *out_obj)
{
    vector3 normal = { 0 };

    parser_skip_whitespace(state);

    if (parser_parse_value_float(state, &normal.x)
            && parser_parse_value_float(state, &normal.y)
            && parser_parse_value_float(state, &normal.z)) {

        array_ensure_capacity(make_system_allocator(),
                (void **) &out_obj->vn_array, 1);
        array_push(out_obj->vn_array, &normal);
        return 1;
    }

    return 0;

}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static i32 wavefront_parse_vertex_texture(struct parser_state *state,
        struct wavefront_obj *out_obj)
{
    vector3 uv = { 0 };

    parser_skip_whitespace(state);

    if (parser_parse_value_float(state, &uv.x)
            && parser_parse_value_float(state, &uv.y)) {

        array_ensure_capacity(make_system_allocator(),
                (void **) &out_obj->vt_array, 1);
        array_push(out_obj->vt_array, &uv);
        return 1;
    }

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static i32 wavefront_parse_face(struct parser_state *state,
        struct wavefront_obj *out_obj)
{
    i32 face_data[3][3] = { 0 };
    i32 additional_face[3] =  { 0 };

    if (!parser_accept(state, (char []) { 'f' }, 1, NULL)) {
        return 0;
    }

    if (!(wavefront_parse_face_point(state, face_data[0])
            && wavefront_parse_face_point(state, face_data[1])
            && wavefront_parse_face_point(state, face_data[2]))) {
        return 0;
    }

    geometry_add_face_from_parsed(face_data, out_obj);

    while (!parser_parse_end_line(state)) {
        wavefront_parse_face_point(state, additional_face);

        bytewise_copy(face_data[1], face_data[2], sizeof(*face_data));
        bytewise_copy(face_data[2], additional_face, sizeof(*face_data));

        geometry_add_face_from_parsed(face_data, out_obj);
    }

    return 1;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static i32 wavefront_parse_face_point(struct parser_state *state,
        i32 read_idx[3])
{
    parser_skip_whitespace(state);

    read_idx[0] = 0;
    read_idx[1] = 0;
    read_idx[2] = 0;

    if (!parser_parse_value_int(state, &read_idx[0])) {
        return 0;
    }

    if (!parser_accept(state, (char []) { '/' }, 1, NULL)) {
        return 1;
    }

    if (!parser_accept(state, (char []) { '/' }, 1, NULL)) {
        parser_parse_value_int(state, &read_idx[1]);
        if (parser_accept(state, (char []) { '/' }, 1, NULL)) {
            return parser_parse_value_int(state, &read_idx[2]);
        } else {
            return 1;
        }
    }

    return parser_parse_value_int(state, &read_idx[2]);
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static void geometry_add_face_from_parsed(i32 face_data[3][3],
        struct wavefront_obj *out_obj)
{
    struct wavefront_obj_face face = { 0 };

    for (size_t i = 0 ; i < 3 ; i++) {
        face.v_idx[i] = face_data[i][0] - 1; // vertex index
        if (face_data[i][1] > 0) face.vt_idx[i] = face_data[i][1] - 1; // texture uv index
        if (face_data[i][2] > 0) face.vn_idx[i] = face_data[i][2] - 1; // normal vector index
    }

    array_ensure_capacity(make_system_allocator(),
            (void **) &out_obj->f_array, 1);
    array_push(out_obj->f_array, &face);
}