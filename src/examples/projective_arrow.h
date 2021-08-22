#pragma once

#include "./projective_base.h"

typedef struct ArrowHead {
    Edge left, right;
    f32 length;
} ArrowHead;

typedef struct Arrow {
    ArrowHead head;
    Edge body;
    char* label;
} Arrow;

Arrow arrow1, arrowX, arrowY, arrowZ;

bool updateArrow(Arrow *arrow) {
    if (isEqualVec3(arrow->body.from, arrow->body.to)) return false;
    vec3 direction = scaleVec3(normVec3(subVec3(arrow->body.from, arrow->body.to)), arrow->head.length);
    arrow->head.left.to = arrow->head.right.to = arrow->body.to;
    arrow->head.left.from = arrow->head.right.from = addVec3(arrow->body.to, direction);
    direction = crossVec3(direction, crossVec3(direction, direction.x || direction.y ? Vec3(0, 0, 1) : Vec3(1, 0, 0)));
    arrow->head.left.from  = addVec3(arrow->head.left.from, direction);
    arrow->head.right.from = subVec3(arrow->head.right.from, direction);
    return true;
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

    if (arrow->label) {
        edge = arrow->body;
        edge.to = scaleAddVec3(subVec3(edge.to, edge.from), 1.2f, edge.from);
        transformEdge(&edge, &edge, xform);
        projectEdge(&edge, viewport);
        drawText(viewport->frame_buffer, color, arrow->label, (i32)edge.to.x, (i32)edge.to.y);
    }
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

void drawCoordinateArrowsToPoint(Viewport *viewport, RGBA x_color, RGBA y_color, RGBA z_color, vec3 location, u8 line_width) {
    arrowX.body.to = arrowY.body.to = arrowZ.body.to = getVec3Of(0);
    arrowX.body.to.x = arrowY.body.to.x = arrowZ.body.to.x = location.x;
    arrowY.body.to.y = arrowZ.body.to.y = location.y;

    arrowZ.body.to.z = location.z;
    arrowX.body.from = getVec3Of(0);
    arrowY.body.from = arrowX.body.to;
    arrowZ.body.from = arrowY.body.to;
    updateArrow(&arrowX);
    updateArrow(&arrowY);
    updateArrow(&arrowZ);

    drawArrow(viewport, x_color, &arrowX, line_width);
    drawArrow(viewport, y_color, &arrowY, line_width);
    drawArrow(viewport, z_color, &arrowZ, line_width);
}