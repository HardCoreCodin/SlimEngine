#pragma once

#include "./projective_base.h"
#include "./projective_transition.h"

#define LOCATOR_GRID_SIZE_X 41
#define LOCATOR_GRID_SIZE_Y 31
#define LOCATOR_GRID_SIZE_Z 21
#define LOCATOR_OPACITY (MAX_COLOR_VALUE / 8)
#define LOCATOR_SIZE 0.2f

typedef union Locator {
    struct {
        Edge X, Y, Z;
    };
    Edge edges[3];
} Locator;

void setLocator(Locator *locator, vec3 location) {
    for (u8 i = 0; i < 3; i++) locator->edges[i].from = locator->edges[i].to = location;

    locator->X.from.x -= LOCATOR_SIZE;
    locator->X.to.x   += LOCATOR_SIZE;
    locator->Y.from.y -= LOCATOR_SIZE;
    locator->Y.to.y   += LOCATOR_SIZE;
    locator->Z.from.z -= LOCATOR_SIZE;
    locator->Z.to.z   += LOCATOR_SIZE;
}

void drawLocatorGrid(Viewport *viewport, RGBA color, RGBA out_color, Transition *transition) {
    f32 from_w, to_w;
    f32 n = secondary_viewport.settings.near_clipping_plane_distance;
    f32 f = secondary_viewport.settings.far_clipping_plane_distance;

    Edge trg_edge, *edge;
    f32 x_step = (view_frustum_box.vertices.corners.front_bottom_right.x * 2.0f) / ((f32)LOCATOR_GRID_SIZE_X);
    f32 y_step = (view_frustum_box.vertices.corners.front_top_right.y    * 2.0f) / ((f32)LOCATOR_GRID_SIZE_Y);
    f32 z_step = (n - f) / ((f32)LOCATOR_GRID_SIZE_Z);
    vec3 start = view_frustum_box.vertices.corners.front_bottom_left;
    vec3 location = start;
    Locator locator;
    out_color.A /= 8;
    bool is_out;

    for (i32 z = 0; z < LOCATOR_GRID_SIZE_Z; z++) {
        location.y = start.y;
        for (i32 y = 0; y < LOCATOR_GRID_SIZE_Y; y++) {
            location.x = start.x;
            for (i32 x = 0; x < LOCATOR_GRID_SIZE_X; x++) {
                setLocator(&locator, location);

                edge = locator.edges;

                for (u8 i = 0; i < 3; i++, edge++) {
                    from_w = mulVec3Mat4(edge->from, 1.0f, secondary_viewport.pre_projection_matrix, &trg_edge.from);
                    to_w   = mulVec3Mat4(edge->to,   1.0f, secondary_viewport.pre_projection_matrix, &trg_edge.to);

                    is_out = fabsf(trg_edge.from.x) > from_w ||
                             fabsf(trg_edge.from.y) > from_w ||
                             fabsf(trg_edge.from.z) > from_w ||
                             fabsf(trg_edge.to.x) > from_w ||
                             fabsf(trg_edge.to.y) > from_w ||
                             fabsf(trg_edge.to.z) > from_w;

                    if (transition == &transitions.projection) *edge = trg_edge;
                    if (transition != &transitions.pre_projection) {
                        trg_edge.from = scaleVec3(trg_edge.from, 1.0f / from_w);
                        trg_edge.to   = scaleVec3(trg_edge.to,   1.0f / to_w);
                    }

                    edge->from = lerpVec3(edge->from, trg_edge.from, transition->eased_t);
                    edge->to   = lerpVec3(edge->to,   trg_edge.to,   transition->eased_t);

                    convertEdgeFromSecondaryToMain(edge);
                    drawEdge(viewport, is_out ? out_color : color, edge, 4);
                }
                location.x += x_step;
            }
            location.y += y_step;
        }
        location.z += z_step;
    }
}

