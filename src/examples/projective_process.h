#pragma once

#include "./projective_base.h"
#include "./projective_arrow.h"
#include "./projective_space.h"
#include "./projective_matrix.h"
#include "./projective_locator.h"
#include "./projective_transition.h"

void renderViewSpaceFrustumSlice(Viewport *viewport, f32 delta_time) {
    rotatePrimitive(main_box_prim, delta_time / -3, 0, 0);

    Quad view_frustum_quad, NDC_quad, transforming_quad;
    setQuadFromBox(&view_frustum_quad, &view_frustum_box);
    setQuadFromBox(&NDC_quad, &NDC_box);
    transforming_quad = view_frustum_quad;

    Edge projective_point_edge;
    RGBA focal_length_color = Color(BrightGrey);
    RGBA projective_point_color;
    projective_point_color.G = MAX_COLOR_VALUE / 2;
    projective_point_color.R = MAX_COLOR_VALUE / 4 + (u8)((f32)projective_point_color.G);
    projective_point_color.B = MAX_COLOR_VALUE / 4 + (u8)((f32)projective_point_color.G);

    if (transitions.projective_lines.active) {
        Y_color = Color(Magenta);

        if (incTransition(&transitions.projective_lines, delta_time, false)) {
            Y_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.projective_lines.eased_t);
            bool colorize = !(transitions.reveal_projective_point.active || transitions.lift_up.active);
            drawProjectiveSpace(viewport, &transitions.projective_lines, colorize);
        }

        if (transitions.reveal_projective_point.active) {
            if (incTransition(&transitions.reveal_projective_point, delta_time, false))
                projective_point_color.A = X_color.A = Y_color.A = Z_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.reveal_projective_point.eased_t);

            static float elapsed = 0;
            elapsed += delta_time;

            Locator projective_point;
            Edge *edge = projective_point.edges;
            vec3 location;
            if (move_projective_point) {
                location.x = 2 * sinf(elapsed)       - 1;
                location.z = 2 * sinf(elapsed+1) - 1;
                location.y = 1;
                projective_point_edge.to = normVec3(location);
                projective_point_color.R = MAX_COLOR_VALUE / 4 + (u8)((f32)projective_point_color.G * projective_point_edge.to.x);
                projective_point_color.B = MAX_COLOR_VALUE / 4 + (u8)((f32)projective_point_color.G * projective_point_edge.to.z);
                projective_point_edge.to = scaleVec3(location, 5);
            } else {
                location = getVec3Of(5 * sinf(elapsed));
                location.z = -location.z;
                projective_point_edge.to = getVec3Of(10);
                projective_point_edge.to.z = -projective_point_edge.to.z;
            }
            if (transitions.reveal_normalizing_projective_point.active) {
                if (incTransition(&transitions.reveal_normalizing_projective_point, delta_time, false))
                    location = lerpVec3(Vec3(-4, 2, -8), Vec3(-2, 1, -4), transitions.reveal_normalizing_projective_point.eased_t);
                if (transitions.reveal_normalizing_projective_point.t >= 1)
                    transitions.reveal_normalizing_projective_point.t = 0;

                projective_point_edge.to = scaleVec3(normVec3(location), 5);
                projective_point_color.R = MAX_COLOR_VALUE / 4 + (u8)((f32)projective_point_color.G * projective_point_edge.to.x);
                projective_point_color.B = MAX_COLOR_VALUE / 4 + (u8)((f32)projective_point_color.G * projective_point_edge.to.z);
                transitions.reveal_normalizing_projective_point.active = true;
                arrow1.body.from = getVec3Of(0);
                arrow1.body.to   = location;
                arrow1.head.length = 0.5f;
                updateArrow(&arrow1);

                viewport->settings.depth_sort = false;
                drawArrow(viewport, projective_point_color, &arrow1, 2);
                viewport->settings.depth_sort = true;
            }

            setLocator(&projective_point, location);
            for (u8 i = 0; i < 3; i++, edge++) {
                edge->to   = convertPositionToObjectSpace(edge->to,   main_camera_prim);
                edge->from = convertPositionToObjectSpace(edge->from, main_camera_prim);
                drawEdge(viewport, projective_point_color, edge, 3);
            }

            projective_point_color.A /= 2;
            projective_point_edge.from = invertedVec3(projective_point_edge.to);
            projective_point_edge.from = convertPositionToObjectSpace(projective_point_edge.from, main_camera_prim);
            projective_point_edge.to   = convertPositionToObjectSpace(projective_point_edge.to, main_camera_prim);
            drawEdge(viewport, projective_point_color, &projective_point_edge, 2);


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
            viewport->settings.depth_sort = false;
            drawArrow(viewport, X_color, &arrowX, 2);
            drawArrow(viewport, Y_color, &arrowY, 2);
            drawArrow(viewport, Z_color, &arrowZ, 2);
            viewport->settings.depth_sort = true;

            X_color.A = Y_color.A = Z_color.A = MAX_COLOR_VALUE;
        }

        if (transitions.reveal_ref_plane.active) {
            RGBA grid_color = Color(projective_ref_plane_prim->color);
            if (incTransition(&transitions.reveal_ref_plane, delta_time, false))
                grid_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.reveal_ref_plane.eased_t);

            grid_color.A /= 4;
            drawGrid(viewport, grid_color, projective_ref_plane_grid, projective_ref_plane_prim,0);
        }

        if (transitions.lift_up.active) {
            if (incTransition(&transitions.lift_up, delta_time, false))
                for (u8 i = 0; i < 4; i++)
                    transforming_quad.corners[i].y = view_frustum_quad.corners[i].y = NDC_quad.corners[i].y = transitions.lift_up.eased_t;
        }
    }

    if (transitions.full_projection.active) {
        if (incTransition(&transitions.full_projection, delta_time, true)) {
            for (u8 i = 0; i < 4; i++)
                transforming_quad.corners[i] = lerpVec3(view_frustum_quad.corners[i], NDC_quad.corners[i], transitions.full_projection.eased_t);

            sides_color = getColorInBetween(sides_color, Color(Yellow), transitions.full_projection.eased_t);
            near_color  = getColorInBetween(near_color,  Color(Yellow), transitions.full_projection.eased_t);
            far_color   = getColorInBetween(far_color,   Color(Yellow), transitions.full_projection.eased_t);
        }
    }

    if (transitions.lines_through_NDC.active) {
        viewport->settings.depth_sort = true;
        if (incTransition(&transitions.lines_through_NDC, delta_time, false)) {
            projective_point_edge.from = getVec3Of(0);
            projective_point_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.lines_through_NDC.eased_t);
            projective_point_color.G = MAX_COLOR_VALUE / 2;
            for (u8 i = 0; i < 4; i++) {
                projective_point_edge.to = normVec3(NDC_quad.corners[i]);
                projective_point_color.R = MAX_COLOR_VALUE / 4 + (u8)((f32)projective_point_color.G * projective_point_edge.to.x);
                projective_point_color.B = MAX_COLOR_VALUE / 4 + (u8)((f32)projective_point_color.G * projective_point_edge.to.z);
                projective_point_edge.to = scaleVec3(projective_point_edge.to, 10);

                projective_point_edge.from = invertedVec3(projective_point_edge.to);
                projective_point_edge.from = convertPositionToObjectSpace(projective_point_edge.from, main_camera_prim);
                projective_point_edge.to   = convertPositionToObjectSpace(projective_point_edge.to, main_camera_prim);
                drawEdge(viewport, projective_point_color, &projective_point_edge, 2);
            }
        }
    }
    Edge edge;
    vec3 corner = Vec3(secondary_viewport.frame_buffer->dimensions.width_over_height, 1, secondary_viewport.camera->focal_length);

    if (transitions.focal_length_and_plane.active) {
        viewport->settings.depth_sort = false;

        if (incTransition(&transitions.focal_length_and_plane, delta_time, false))
            focal_length_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.focal_length_and_plane.eased_t);

        Edge focal_length_edge;
        focal_length_edge.from = focal_length_edge.to = getVec3Of(0);
        focal_length_edge.to.z = secondary_viewport.camera->focal_length;
        focal_length_edge.from = convertPositionToObjectSpace(focal_length_edge.from, main_camera_prim);
        focal_length_edge.to   = convertPositionToObjectSpace(focal_length_edge.to,   main_camera_prim);
        drawEdge(viewport, focal_length_color, &focal_length_edge, 3);

        Quad ref_quad;
        for (u8 i = 0; i < 4; i++)
            ref_quad.corners[i] = corner;

        ref_quad.top_left.x    = ref_quad.bottom_left.x  = -ref_quad.top_left.x;
        ref_quad.bottom_left.y = ref_quad.bottom_right.y = -ref_quad.top_left.y;

        for (u8 i = 0; i < 4; i++)
            ref_quad.corners[i] = convertPositionToObjectSpace(ref_quad.corners[i], main_camera_prim);

        focal_length_color.R = focal_length_color.G = focal_length_color.B = (u8)((f32)MAX_COLOR_VALUE * 0.5f);
        for (u8 i = 0; i < 4; i++) {
            edge.from = ref_quad.corners[i];
            edge.to   = ref_quad.corners[(i + 1) % 4];
            drawEdge(viewport, focal_length_color, &edge, 2);
        }

        edge.from = edge.to = corner;
        edge.to.y = -1;
        edge.from.x = edge.to.x = 1;
        drawLocalEdge(edge, focal_length_color, 0);
        edge.from.x = edge.to.x = -1;
        drawLocalEdge(edge, focal_length_color, 0);

        edge_color = X_color;
        edge_color.A = focal_length_color.A;
        edge.from = edge.to = corner;
        edge.from.x = 0;
        edge.from.y = edge.to.y = 0;
        drawLocalEdge(edge, edge_color, 2);

        edge_color = Y_color;
        edge_color.A = focal_length_color.A;
        edge.from = edge.to = corner;
        edge.from.x = edge.to.x = 0;
        edge.from.y = 0;
        drawLocalEdge(edge, edge_color, 2);

        viewport->settings.depth_sort = true;
    }

    u8 A = MAX_COLOR_VALUE;
    if (transitions.view_scene.active) {
        if (incTransition(&transitions.view_scene, delta_time, false))
            A = (u8)((f32)MAX_COLOR_VALUE * transitions.view_scene.eased_t);

        edge_color = Color(DarkGrey);
        edge_color.A = A;
        drawBox(viewport, edge_color, main_box, main_box_prim, BOX__ALL_SIDES, 0);
        Edge *clipped_edge;
        transformGridVerticesFromObjectToViewSpace(&secondary_viewport, main_grid_prim, main_grid, &clipped_grid.vertices);
        setGridEdgesFromVertices(clipped_grid.edges.uv.u, main_grid->u_segments, clipped_grid.vertices.uv.u.from, clipped_grid.vertices.uv.u.to);
        setGridEdgesFromVertices(clipped_grid.edges.uv.v, main_grid->v_segments, clipped_grid.vertices.uv.v.from, clipped_grid.vertices.uv.v.to);

        edge_color = Color(main_grid_prim->color);
        edge_color.A = A;
        clipped_edge = clipped_grid.edges.uv.u;
        for (u8 u = 0; u < main_grid->u_segments; u++, clipped_edge++) drawClippedEdge(viewport, clipped_edge, edge_color);
        clipped_edge = clipped_grid.edges.uv.v;
        for (u8 v = 0; v < main_grid->v_segments; v++, clipped_edge++) drawClippedEdge(viewport, clipped_edge, edge_color);

        transformBoxVerticesFromObjectToViewSpace(&secondary_viewport, main_box_prim, &main_box->vertices, &clipped_box.vertices);
        setBoxEdgesFromVertices(&clipped_box.edges, &clipped_box.vertices);
        clipped_edge = clipped_box.edges.buffer;
        edge_color = Color(main_box_prim->color);
        edge_color.A = A;
        for (u8 i = 0; i < BOX__EDGE_COUNT; i++, clipped_edge++) drawClippedEdge(viewport, clipped_edge, edge_color);

        if (show_secondary_viewport) {
            fillPixelGrid(&secondary_viewport_frame_buffer, Color(Black));

            drawGrid(&secondary_viewport, Color(main_grid_prim->color), main_grid, main_grid_prim,0);
            drawBox(&secondary_viewport, Color(main_box_prim->color), main_box, main_box_prim, BOX__ALL_SIDES, 1);

            drawSecondaryViewportToFrameBuffer(viewport->frame_buffer);
        }
    }

    if (transitions.view_frustom_slice.active) {
        if (incTransition(&transitions.view_frustom_slice, delta_time, false))
            NDC_color.A = Y_color.A = (u8)((f32)MAX_COLOR_VALUE * (1.0f - transitions.view_frustom_slice.eased_t));

        for (u8 i = 0; i < 4; i++) {
            NDC_quad.corners[i]          = convertPositionToObjectSpace(NDC_quad.corners[i], main_camera_prim);
            transforming_quad.corners[i] = convertPositionToObjectSpace(transforming_quad.corners[i], main_camera_prim);
        }
        for (u8 i = 0; i < 4; i++) {
            edge.from = NDC_quad.corners[i];
            edge.to   = NDC_quad.corners[(i + 1) % 4];
            drawEdge(viewport, NDC_color, &edge, 2);

            edge.from = transforming_quad.corners[i];
            edge.to   = transforming_quad.corners[(i + 1) % 4];
            edge_color = i % 2 ? sides_color : (i ? far_color : near_color);
            edge_color.A = NDC_color.A;
            drawEdge(viewport, edge_color, &edge, 2);
        }
    }
    if (NDC_color.A) {
        sides_color.A = near_color.A = far_color.A = NDC_color.A;
        drawBox(viewport, NDC_color, &NDC_box, secondary_camera_prim, BOX__ALL_SIDES, 1);
        drawFrustum(viewport, &view_frustum_box, near_color, far_color, sides_color, 1);
    }

    viewport->settings.depth_sort = false;


    if (transitions.aspect_ratio.active) {
        edge_color = Color(White);

        if (incTransition(&transitions.aspect_ratio, delta_time, false))
            A = edge_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.aspect_ratio.eased_t);

        // Diagonal:
        edge.from = edge.to = corner;
        edge.from.x = -edge.to.x;
        edge.from.y = -edge.to.y;
        edge.to = lerpVec3(edge.from, edge.to, transitions.aspect_ratio.eased_t);
        drawLocalEdge(edge, edge_color, 1);

        // Bottom:
        edge.from = edge.to = corner;
        edge.from.y = -edge.from.y;
        edge.to = edge.from;
        edge.to.x = -edge.to.x;
        edge.to = lerpVec3(edge.from, edge.to, transitions.aspect_ratio.eased_t);
        drawLocalEdge(edge, X_color, 1);

        // Side
        edge.from = edge.to = corner;
        edge.from.y = -edge.from.y;
        edge.to = lerpVec3(edge.from, edge.to, transitions.aspect_ratio.eased_t);
        drawLocalEdge(edge, Y_color, 1);
    }

    if (transitions.ground_diagonal.active) {
        incTransition(&transitions.ground_diagonal, delta_time, false);
        edge_color.G = edge_color.R = MAX_COLOR_VALUE;
        edge_color.B = MAX_COLOR_VALUE / 2;
        edge_color.A = MAX_COLOR_VALUE;
        edge.from = getVec3Of(0);

        edge.to = Vec3(50, 0, 50);
        edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
        drawLocalEdge(edge, edge_color, 1);

        edge.to = Vec3(-50, 0, 50);
        edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
        drawLocalEdge(edge, edge_color, 1);

        if (!transitions.view_frustom_slice.active) {
            edge.to = Vec3(0, 50, 50);
            edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
            drawLocalEdge(edge, edge_color, 1);

            edge.to = Vec3(0, -50, 50);
            edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
            drawLocalEdge(edge, edge_color, 1);

            edge_color.R /= 2;

            edge.to = Vec3(50, 50, 50);
            edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
            drawLocalEdge(edge, edge_color, 1);

            edge.to = Vec3(-50, 50, 50);
            edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
            drawLocalEdge(edge, edge_color, 1);

            edge.to = Vec3(50, -50, 50);
            edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
            drawLocalEdge(edge, edge_color, 1);

            edge.to = Vec3(-50, -50, 50);
            edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
            drawLocalEdge(edge, edge_color, 1);
        }
    }

    arrowX.label = "X";
    arrowY.label = "Y";
    arrowZ.label = "Z";
    arrowX.head.length = arrowZ.head.length = arrowY.head.length = 0.25f;
    arrowX.body.from = arrowY.body.from = arrowZ.body.from = getVec3Of(0);
    arrowX.body.to = main_matrix.M.X = arrowX_box_prim->position;
    arrowY.body.to = main_matrix.M.Y = arrowY_box_prim->position;
    arrowZ.body.to = main_matrix.M.Z = arrowZ_box_prim->position;
    updateArrow(&arrowX);
    updateArrow(&arrowY);
    updateArrow(&arrowZ);
    drawArrow(viewport, X_color, &arrowX, 2);
    drawArrow(viewport, Y_color, &arrowY, 2);
    drawArrow(viewport, Z_color, &arrowZ, 2);
    viewport->settings.depth_sort = true;

    matrix_count = 3;
    if (show_matrix_hud) {
        updateMatrixStrings(&main_matrix);
        drawMatrixHUD(viewport->frame_buffer);
    }
}