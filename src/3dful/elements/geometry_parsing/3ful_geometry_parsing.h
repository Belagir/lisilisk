
#ifndef GEOMETRY_PARSING_3DFUL_H__
#define GEOMETRY_PARSING_3DFUL_H__

#include <ustd/math2d.h>
#include <ustd/math3d.h>

#include "../../inout/file_operations.h"
#include "../3dful_core.h"

// -------------------------------------------------------------------------------------------------

struct wavefront_obj_face { u32 v_idx[3], vn_idx[3], vt_idx[3]; };

struct wavefront_obj {
    struct vector3 *v_array;
    struct vector3 *vn_array;
    struct vector2 *vt_array;
    struct wavefront_obj_face *f_array;
};

void wavefront_obj_create(struct wavefront_obj *obj);
void wavefront_obj_delete(struct wavefront_obj *obj);
void wavefront_obj_parse(struct wavefront_obj *obj, byte *buffer);
void wavefront_obj_to(struct wavefront_obj *obj, struct geometry *geometry);
void wavefront_obj_dump(struct wavefront_obj *obj, FILE *file);

#endif