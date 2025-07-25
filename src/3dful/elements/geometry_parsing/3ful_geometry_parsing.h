/**
 * @file 3ful_geometry_parsing.h
 * @author Gabriel Bédat
 * @brief Provides an interface used to parse (a subset) of Wavefront Object
 * files, that describe 3d models.
*
 * @version 0.1
 * @date 2025-07-25
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef GEOMETRY_PARSING_3DFUL_H__
#define GEOMETRY_PARSING_3DFUL_H__

#include <ustd/math2d.h>
#include <ustd/math3d.h>

#include "../../inout/file_operations.h"
#include "../3dful_core.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/** Face definition. Holds indices for vertices, normals and texture UVs. */
struct wavefront_obj_face { u32 v_idx[3], vn_idx[3], vt_idx[3]; };

/**
 * @brief Mirrors the content of a .obj file. This is a parser object that
 * serves as an intermediate between a bufferized file and an usable geometry.
 */
struct wavefront_obj {
    /** Vertices. */
    struct vector3 *v_array;
    /** Normals. */
    struct vector3 *vn_array;
    /** Texture UVs. */
    struct vector2 *vt_array;
    /** Faces. */
    struct wavefront_obj_face *f_array;

    /** True if the model should be rendered smooth. */
    bool smooth;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// Allocates memory for a parsing object.
void wavefront_obj_create(struct wavefront_obj *obj);
// Releases memory from a parsing object.
void wavefront_obj_delete(struct wavefront_obj *obj);
// Loads an obj file (already in a buffer) to a parsing object.
void wavefront_obj_parse(struct wavefront_obj *obj, const byte *buffer_array);
// Builds a geometry from parsed data.
void wavefront_obj_to(const struct wavefront_obj *obj,
        struct geometry *geometry);
// Writes data parsed to a stream, in a form compatible with the .obj format.
void wavefront_obj_dump(struct wavefront_obj *obj, FILE *file);

#endif