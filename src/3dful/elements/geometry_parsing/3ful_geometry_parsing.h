
#ifndef GEOMETRY_PARSING_3DFUL_H__
#define GEOMETRY_PARSING_3DFUL_H__

#include <ustd/range.h>
#include <ustd/math3d.h>

#include "../../inout/file_operations.h"
#include "../3dful_core.h"

// -------------------------------------------------------------------------------------------------

struct wavefront_obj_face { u32 v_idx[3], vn_idx[3]; };

struct wavefront_obj {
    RANGE(struct vector3) *v;
    RANGE(struct vector3) *vn;
    RANGE(struct wavefront_obj_face) *f;
};

void wavefront_obj_create(struct wavefront_obj *obj);
void wavefront_obj_delete(struct wavefront_obj *obj);
void wavefront_obj_parse(struct wavefront_obj *obj, BUFFER *buffer);
void wavefront_obj_to(struct wavefront_obj *obj, struct geometry *geometry);

#endif