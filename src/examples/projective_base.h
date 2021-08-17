#pragma once

#include "../SlimEngine/app.h"
#include "../SlimEngine/core/time.h"
#include "../SlimEngine/scene/grid.h"
#include "../SlimEngine/scene/xform.h"
#include "../SlimEngine/viewport/navigation.h"
#include "../SlimEngine/viewport/manipulation.h"

typedef struct ArrowHead {
    Edge left, right;
    f32 length;
} ArrowHead;

typedef struct Arrow {
    ArrowHead head;
    Edge body;
} Arrow;

RGBA sides_color, near_color, far_color, NDC_color, X_color, Y_color, Z_color;
Arrow arrow1, arrowX, arrowY, arrowZ;
bool show_secondary_viewport = false;
PixelGrid secondary_viewport_frame_buffer;
Viewport secondary_viewport, *active_viewport;

Primitive *arrow_box_prim, *main_grid_prim, *projection_plane_prim, *projective_ref_plane_prim, *main_camera_prim, *secondary_camera_prim, *main_box_prim;
Grid *main_grid, *projection_plane_grid, *projective_ref_plane_grid, clipped_grid;
Box NDC_box, projected_box, pre_projected_view_frustum_box, transforming_view_frustum_box, view_frustum_box, clipped_box, *main_box, *arrow_box;
Edge view_space_edge;
RGBA edge_color;
RGBA camera_color;
bool draw_locator_grids = false;
bool show_pre_projection = false;
bool move_projective_point = true;

typedef enum VIZ {
    INTRO,
    VIEW_FRUSTUM,
    PROJECTION,
    PROJECTIVE_SPACE,
    VIEW_FRUSTUM_SLICE,
    VIS_COUNT
} VIZ;
VIZ current_viz = INTRO;

typedef struct Transition {
    bool active;
    f32 t, speed, eased_t;
} Transition;

bool incTransition(Transition *transition, f32 inc, bool reset_when_done) {
    transition->t += inc * transition->speed;
    if (transition->t > 1) {
        if (reset_when_done) {
            transition->t = 0;
            transition->active = false;
        } else transition->t = transition->eased_t = 1;
    } else
        transition->eased_t = smoothstep(0, 1, transition->t);

    return transition->active;
}

#define TRANSITION_COUNT 14

typedef union Transitions {
    struct {
        Transition pre_projection,
        projection,
        full_projection,
        projective_lines,
        view_frustom_slice,
        lift_up,
        translate_back,
        scale_back,
        shear_up,
        reveal_ref_plane,
        reveal_projective_point,
        reveal_normalizing_projective_point,
        lines_through_NDC,
        forcal_length_and_plane;
    };
    Transition states[TRANSITION_COUNT];
} Transitions;
Transitions transitions;

typedef union Locator {
    struct {
        Edge X, Y, Z;
    };
    Edge edges[3];
} Locator;

typedef union Quad {
    struct {
        vec3 top_left, top_right, bottom_right, bottom_left;
    };
    vec3 corners[4];
} Quad;

void setQuadFromBox(Quad *quad, Box *box) {
    quad->top_left     = box->vertices.corners.back_bottom_left;
    quad->top_right    = box->vertices.corners.back_bottom_right;
    quad->bottom_left  = box->vertices.corners.front_bottom_left;
    quad->bottom_right = box->vertices.corners.front_bottom_right;
    for (u8 i = 0; i < 4; i++) quad->corners[i].y = 0;
}


#define PROJECTION_LINES_COUNT 36
#define PROJECTION_LINES_OPACITY (MAX_COLOR_VALUE / 8)
#define PROJECTION_LINE_INC_SPEED 0.3f

#define LOCATOR_GRID_SIZE_X 41
#define LOCATOR_GRID_SIZE_Y 31
#define LOCATOR_GRID_SIZE_Z 21
#define LOCATOR_OPACITY (MAX_COLOR_VALUE / 8)
#define LOCATOR_SIZE 0.2f

RGBA getColorInBetween(RGBA from, RGBA to, f32 t) {
    vec3 float_color = lerpVec3(Vec3((f32)from.R, (f32)from.G, (f32)from.B),
                                Vec3((f32)to.R,   (f32)to.G,   (f32)to.B), t);
    RGBA in_between;
    in_between.R = (u8)clampValueToBetween(float_color.x, 0, (f32)MAX_COLOR_VALUE);
    in_between.G = (u8)clampValueToBetween(float_color.y, 0, (f32)MAX_COLOR_VALUE);
    in_between.B = (u8)clampValueToBetween(float_color.z, 0, (f32)MAX_COLOR_VALUE);
    in_between.A = to.A;
    return in_between;
}

void setLocator(Locator *locator, vec3 location) {
    for (u8 i = 0; i < 3; i++) locator->edges[i].from = locator->edges[i].to = location;

    locator->X.from.x -= LOCATOR_SIZE;
    locator->X.to.x   += LOCATOR_SIZE;
    locator->Y.from.y -= LOCATOR_SIZE;
    locator->Y.to.y   += LOCATOR_SIZE;
    locator->Z.from.z -= LOCATOR_SIZE;
    locator->Z.to.z   += LOCATOR_SIZE;
}

void updateArrow(Arrow *arrow) {
    vec3 direction = scaleVec3(normVec3(subVec3(arrow->body.from, arrow->body.to)), arrow->head.length);
    arrow->head.left.to = arrow->head.right.to = arrow->body.to;
    arrow->head.left.from = arrow->head.right.from = addVec3(arrow->body.to, direction);
    direction = crossVec3(direction, crossVec3(direction, direction.x || direction.z ? Vec3(0, 1, 0) : Vec3(1, 0, 0)));
    arrow->head.left.from = addVec3(arrow->head.left.from, direction);
    arrow->head.right.from = subVec3(arrow->head.right.from, direction);
}

void transformEdge(Edge *in_edge, Edge *out_edge, xform3 *xform) {
    out_edge->from = subVec3(in_edge->from, xform->position);
    out_edge->from = mulVec3Quat(out_edge->from, xform->rotation_inverted);
    out_edge->to = subVec3(in_edge->to, xform->position);
    out_edge->to = mulVec3Quat(out_edge->to, xform->rotation_inverted);
}

void drawArrow(Viewport *viewport, RGBA color, Arrow *arrow, u8 line_width) {
    xform3 *xform = &viewport->camera->transform;
    Edge edge;
    transformEdge(&arrow->body, &edge, xform);
    drawEdge(viewport, color, &edge, line_width);

    transformEdge(&arrow->head.left, &edge, xform);
    drawEdge(viewport, color, &edge, line_width);

    transformEdge(&arrow->head.right, &edge, xform);
    drawEdge(viewport, color, &edge, line_width);
}

void drawSecondaryViewportToFrameBuffer(PixelGrid *frame_buffer) {
    Pixel *trg_pixels = frame_buffer->pixels;
    Pixel *src_pixels = secondary_viewport_frame_buffer.pixels;
    Dimensions *trg_dim = &frame_buffer->dimensions;
    Dimensions *src_dim = &secondary_viewport_frame_buffer.dimensions;
    i32 trg_index, src_index = 0;
    for (i32 y = 0; y < src_dim->height; y++) {
        for (i32 x = 0; x < src_dim->width; x++) {
            trg_index = secondary_viewport.settings.position.y + y;
            trg_index *= trg_dim->width;
            trg_index += secondary_viewport.settings.position.x + x;
            trg_pixels[trg_index] = src_pixels[src_index++];
        }
    }
    i32 x1 = secondary_viewport.settings.position.x;
    i32 x2 = secondary_viewport.frame_buffer->dimensions.width + x1;
    i32 y1 = secondary_viewport.settings.position.y;
    i32 y2 = secondary_viewport.frame_buffer->dimensions.height + y1;

    edge_color = Color(White);

    drawHLine2D(frame_buffer, edge_color, x1-1, x2+1, y1-1);
    drawHLine2D(frame_buffer, edge_color, x1-1, x2+1, y1+0);
    drawHLine2D(frame_buffer, edge_color, x1-1, x2+1, y1+1);

    drawHLine2D(frame_buffer, edge_color, x1-1, x2+1, y2-1);
    drawHLine2D(frame_buffer, edge_color, x1-1, x2+1, y2+0);
    drawHLine2D(frame_buffer, edge_color, x1-1, x2+1, y2+1);


    drawVLine2D(frame_buffer, edge_color, y1-1, y2+1, x1-1);
    drawVLine2D(frame_buffer, edge_color, y1-1, y2+1, x1+0);
    drawVLine2D(frame_buffer, edge_color, y1-1, y2+1, x1+1);

    drawVLine2D(frame_buffer, edge_color, y1-1, y2+1, x2-1);
    drawVLine2D(frame_buffer, edge_color, y1-1, y2+1, x2+0);
    drawVLine2D(frame_buffer, edge_color, y1-1, y2+1, x2+1);
}

BoxCorners getViewFrustumCorners(Viewport *viewport) {
    BoxCorners corners;
    corners.back_top_right.z = viewport->settings.near_clipping_plane_distance;
    corners.back_top_right.y = viewport->settings.near_clipping_plane_distance / viewport->camera->focal_length;
    corners.back_top_right.x = viewport->frame_buffer->dimensions.width_over_height * corners.back_top_right.y;
    corners.back_top_left = corners.back_top_right;
    corners.back_top_left.x *= -1;
    corners.back_bottom_left = corners.back_top_left;
    corners.back_bottom_left.y *= -1;
    corners.back_bottom_right = corners.back_top_right;
    corners.back_bottom_right.y *= -1;

    corners.front_top_right.z = viewport->settings.far_clipping_plane_distance;
    corners.front_top_right.y = viewport->settings.far_clipping_plane_distance / viewport->camera->focal_length;
    corners.front_top_right.x = viewport->frame_buffer->dimensions.width_over_height * corners.front_top_right.y;
    corners.front_top_left = corners.front_top_right;
    corners.front_top_left.x *= -1;
    corners.front_bottom_left = corners.front_top_left;
    corners.front_bottom_left.y *= -1;
    corners.front_bottom_right = corners.front_top_right;
    corners.front_bottom_right.y *= -1;

    return corners;
}

void drawFrustum(u8 line_width) {
    transformBoxVerticesFromObjectToViewSpace(&app->viewport, secondary_camera_prim, &view_frustum_box.vertices, &view_frustum_box.vertices);
    setBoxEdgesFromVertices(&view_frustum_box.edges, &view_frustum_box.vertices);

    drawEdge(&app->viewport, far_color,   &view_frustum_box.edges.sides.front_top,    line_width);
    drawEdge(&app->viewport, far_color,   &view_frustum_box.edges.sides.front_bottom, line_width);
    drawEdge(&app->viewport, far_color,   &view_frustum_box.edges.sides.front_left,   line_width);
    drawEdge(&app->viewport, far_color,   &view_frustum_box.edges.sides.front_right,  line_width);
    drawEdge(&app->viewport, near_color,  &view_frustum_box.edges.sides.back_top,     line_width);
    drawEdge(&app->viewport, near_color,  &view_frustum_box.edges.sides.back_bottom,  line_width);
    drawEdge(&app->viewport, near_color,  &view_frustum_box.edges.sides.back_left,    line_width);
    drawEdge(&app->viewport, near_color,  &view_frustum_box.edges.sides.back_right,   line_width);
    drawEdge(&app->viewport, sides_color, &view_frustum_box.edges.sides.left_top,     line_width);
    drawEdge(&app->viewport, sides_color, &view_frustum_box.edges.sides.left_bottom,  line_width);
    drawEdge(&app->viewport, sides_color, &view_frustum_box.edges.sides.right_top,    line_width);
    drawEdge(&app->viewport, sides_color, &view_frustum_box.edges.sides.right_bottom, line_width);
}

void convertEdgeFromSecondaryToMain(Edge *edge) {
    edge->from = convertPositionToWorldSpace(edge->from, secondary_camera_prim);
    edge->to   = convertPositionToWorldSpace(edge->to,   secondary_camera_prim);
    edge->from = convertPositionToObjectSpace(edge->from, main_camera_prim);
    edge->to   = convertPositionToObjectSpace(edge->to,   main_camera_prim);
}

void drawClippedEdge(Viewport *viewport, Edge *clipped_edge, RGBA color) {
    if (!cullAndClipEdge(clipped_edge, &secondary_viewport))
        return;

    convertEdgeFromSecondaryToMain(clipped_edge);
    projectEdge(clipped_edge, viewport);
    clipped_edge->from.z -= 0.1f;
    clipped_edge->to.z -= 0.1f;
    drawLine3D(viewport->frame_buffer, color, clipped_edge->from, clipped_edge->to, 1);
}

void updateCameraArrows(xform3 *camera_xform) {
    vec3 P = camera_xform->position;
    vec3 R = *camera_xform->right_direction;
    vec3 U = *camera_xform->up_direction;
    vec3 F = *camera_xform->forward_direction;
    arrowX.body.from =  arrowY.body.from = arrowZ.body.from = P;
    arrowX.body.to = addVec3(R, P);
    arrowY.body.to = addVec3(U, P);
    arrowZ.body.to = addVec3(F, P);
    arrowX.head.length = arrowZ.head.length = arrowY.head.length = 0.25f;
    updateArrow(&arrowX);
    updateArrow(&arrowY);
    updateArrow(&arrowZ);
}

void updateProjectionBoxes(Viewport *viewport) {
    setPreProjectionMatrix(viewport);
    view_frustum_box.vertices.corners = getViewFrustumCorners(viewport);
    setBoxEdgesFromVertices(&view_frustum_box.edges, &view_frustum_box.vertices);

    for (u8 i = 0; i < BOX__VERTEX_COUNT; i++)
        mulVec3Mat4(view_frustum_box.vertices.buffer[i], 1.0f, viewport->pre_projection_matrix, pre_projected_view_frustum_box.vertices.buffer + i);

    setBoxEdgesFromVertices(&pre_projected_view_frustum_box.edges, &pre_projected_view_frustum_box.vertices);
}

void transitionBox(Transition *transition, Box *from_box, Box *to_box) {
    for (u8 i = 0; i < BOX__VERTEX_COUNT; i++)
        transforming_view_frustum_box.vertices.buffer[i] = lerpVec3(from_box->vertices.buffer[i], to_box->vertices.buffer[i], transition->eased_t);

    setBoxEdgesFromVertices(&transforming_view_frustum_box.edges, &transforming_view_frustum_box.vertices);
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

void drawProjectiveSpace(Viewport *viewport, Transition *transition, bool colorize) {
    Edge edge;
    f32 step = TAU / (PROJECTION_LINES_COUNT * 2);
    vec2 sin_cos = Vec2(cosf(step), sinf(step));

    RGBA color;
    color.A = PROJECTION_LINES_OPACITY;
    color.R = color.G = color.B = MAX_COLOR_VALUE / 2;
    vec3 x_axis = Vec3(1, 0, 0);
    vec3 y_axis = Vec3(0, 1, 0);
    quat azimuth_rotation;
    quat altitude_rotation = getIdentityQuaternion();
    quat altitude_rotation_step = getRotationAroundAxisBySinCon(x_axis, sin_cos);
    quat azimuth_rotation_step  = getRotationAroundAxisBySinCon(y_axis, sin_cos);

    for (i32 z = 0; z < PROJECTION_LINES_COUNT; z++) {
        azimuth_rotation = getIdentityQuaternion();
        for (i32 x = 0; x < PROJECTION_LINES_COUNT; x++) {
            edge.from = getVec3Of(0);
            edge.to = mulVec3Quat(Vec3(1, 0, 0), mulQuat(altitude_rotation, azimuth_rotation));
            if (colorize) {
                color.R = MAX_COLOR_VALUE / 4 + (u8)((f32)color.G * edge.to.x);
                color.B = MAX_COLOR_VALUE / 4 + (u8)((f32)color.G * edge.to.z);
            }

            edge.to = scaleVec3(edge.to, 15 * transition->eased_t);
            convertEdgeFromSecondaryToMain(&edge);

            drawEdge(viewport, color, &edge, 0);

            azimuth_rotation = mulQuat(azimuth_rotation, azimuth_rotation_step);
        }
        altitude_rotation = mulQuat(altitude_rotation, altitude_rotation_step);
    }
}