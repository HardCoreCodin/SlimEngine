#include "./projective_base.h"

void onButtonDown(MouseButton *mouse_button) {
    app->controls.mouse.pos_raw_diff = Vec2i(0, 0);

    if (show_secondary_viewport) {
        i32 w = secondary_viewport_frame_buffer.dimensions.width;
        i32 h = secondary_viewport_frame_buffer.dimensions.height;
        i32 x = secondary_viewport.settings.position.x;
        i32 y = secondary_viewport.settings.position.y;
        active_viewport = (
                inRange(app->controls.mouse.pos.x, w + x, x) &&
                inRange(app->controls.mouse.pos.y, h + y, y)
                ) ?
                        &secondary_viewport :
                        &app->viewport;
    }
}

void onDoubleClick(MouseButton *mouse_button) {
    if (mouse_button == &app->controls.mouse.left_button) {
        app->controls.mouse.is_captured = !app->controls.mouse.is_captured;
        app->platform.setCursorVisibility(!app->controls.mouse.is_captured);
        app->platform.setWindowCapture(    app->controls.mouse.is_captured);
        onButtonDown(mouse_button);
    }
}

void updateViewport(Viewport *viewport, Mouse *mouse) {
    if (mouse->is_captured) {
        navigateViewport(viewport, app->time.timers.update.delta_time);
        if (mouse->moved)         orientViewport(viewport, mouse);
        if (mouse->wheel_scrolled)  zoomViewport(viewport, mouse);
    } else {
        if (mouse->wheel_scrolled) {
            if (app->controls.is_pressed.alt)
                zoomViewport(&secondary_viewport, mouse);
            else
                dollyViewport(viewport, mouse);
        }
        if (mouse->moved) {
            if (mouse->middle_button.is_pressed)  panViewport(viewport, mouse);
            if (mouse->right_button.is_pressed) orbitViewport(viewport, mouse);
        }
    }
}

void renderIntro(Viewport *viewport, f32 delta_time, f32 elapsed_time) {
    rotatePrimitive(main_box_prim, delta_time / -3, 0, 0);

    drawGrid(viewport, Color(main_grid_prim->color), main_grid, main_grid_prim,0);
    drawGrid(viewport, Color(projection_plane_prim->color), projection_plane_grid, projection_plane_prim, 0);
    drawBox(viewport,  Color(main_box_prim->color), main_box, main_box_prim, BOX__ALL_SIDES, 1);

    edge_color = Color(Magenta);
    edge_color.A = MAX_COLOR_VALUE / 2;

    Edge edge;
    initBox(&projected_box);
    for (u8 i = 0; i < BOX__VERTEX_COUNT; i++) {
        edge.from = app->scene.cameras[1].transform.position;
        edge.to = convertPositionToWorldSpace(main_box->vertices.buffer[i], main_box_prim);

        transformEdge(&edge, &view_space_edge, &secondary_viewport.camera->transform);
        edge.from = scaleVec3(view_space_edge.to, secondary_viewport.settings.near_clipping_plane_distance/view_space_edge.to.z);
        edge.from = mulVec3Mat3(edge.from, secondary_viewport.camera->transform.rotation_matrix);
        edge.from = addVec3(edge.from, secondary_viewport.camera->transform.position);

        transformEdge(&edge, &view_space_edge, &viewport->camera->transform);
        projected_box.vertices.buffer[i] = view_space_edge.from;

        drawEdge(viewport, edge_color, &view_space_edge, 1);
    }

    edge_color = Color(Yellow);
    edge_color.A = MAX_COLOR_VALUE / 2;
    setBoxEdgesFromVertices(&projected_box.edges, &projected_box.vertices);
    for (u8 i = 0; i < BOX__EDGE_COUNT; i++)
        drawEdge(viewport, edge_color, projected_box.edges.buffer + i, 1);
}

void renderViewFrustum(Viewport *viewport, f32 delta_time, f32 elapsed_time) {
    drawArrow(viewport, Color(BrightRed),   &arrowX, 2);
    drawArrow(viewport, Color(BrightGreen), &arrowY, 2);
    drawArrow(viewport, Color(BrightBlue),  &arrowZ, 2);

    view_frustum_box.vertices.corners = getViewFrustumCorners(&secondary_viewport);
    setBoxEdgesFromVertices(&view_frustum_box.edges, &view_frustum_box.vertices);
    drawGrid(viewport, Color(main_grid_prim->color), main_grid, main_grid_prim,0);

    drawBox(viewport, Color(projection_plane_prim->color), &view_frustum_box, secondary_camera_prim, BOX__ALL_SIDES, 1);
    drawBox(viewport, Color(Grey), main_box, main_box_prim, BOX__ALL_SIDES, 0);

    transformGridVerticesFromObjectToViewSpace(&secondary_viewport, main_grid_prim, main_grid, &clipped_grid.vertices);
    transformBoxVerticesFromObjectToViewSpace(&secondary_viewport, main_box_prim, &main_box->vertices, &clipped_box.vertices);
    setGridEdgesFromVertices(clipped_grid.edges.uv.u, main_grid->u_segments, clipped_grid.vertices.uv.u.from, clipped_grid.vertices.uv.u.to);
    setGridEdgesFromVertices(clipped_grid.edges.uv.v, main_grid->v_segments, clipped_grid.vertices.uv.v.from, clipped_grid.vertices.uv.v.to);
    setBoxEdgesFromVertices(&clipped_box.edges, &clipped_box.vertices);
    Edge *clipped_edge = clipped_grid.edges.uv.u;
    for (u8 u = 0; u < main_grid->u_segments; u++, clipped_edge++) drawClippedEdge(viewport, clipped_edge, Color(main_grid_prim->color));
    clipped_edge = clipped_grid.edges.uv.v;
    for (u8 v = 0; v < main_grid->v_segments; v++, clipped_edge++) drawClippedEdge(viewport, clipped_edge, Color(main_grid_prim->color));

    transformBoxVerticesFromObjectToViewSpace(&secondary_viewport, main_box_prim, &main_box->vertices, &clipped_box.vertices);
    setBoxEdgesFromVertices(&clipped_box.edges, &clipped_box.vertices);
    clipped_edge = clipped_box.edges.buffer;
    for (u8 i = 0; i < BOX__EDGE_COUNT; i++, clipped_edge++) drawClippedEdge(viewport, clipped_edge, Color(main_box_prim->color));
}

void renderProjection(Viewport *viewport, f32 delta_time) {
    drawArrow(viewport, Color(BrightRed),   &arrowX, 2);
    drawArrow(viewport, Color(BrightGreen), &arrowY, 2);
    drawArrow(viewport, Color(BrightBlue),  &arrowZ, 2);

    drawBox(viewport, Color(Yellow), &NDC_box, secondary_camera_prim, BOX__ALL_SIDES, 1);
    drawBox(viewport, Color(projection_plane_prim->color), &view_frustum_box, secondary_camera_prim, BOX__ALL_SIDES, 0);
    drawBox(viewport, Color(Grey), main_box, main_box_prim, BOX__ALL_SIDES, 0);

    RGBA color, out_color = Color(BrightGrey);
    if (show_pre_projection) {
        drawBox(viewport, Color(Magenta), &pre_projected_view_frustum_box, secondary_camera_prim, BOX__ALL_SIDES, 0);

        if (transitions.pre_projection.active) {
            if (incTransition(&transitions.pre_projection, delta_time, true)) {
                transitionBox(&transitions.pre_projection, &view_frustum_box, &pre_projected_view_frustum_box);
                color = getColorInBetween(Color(Cyan), Color(Magenta), transitions.pre_projection.eased_t);
                drawBox(viewport, color, &transforming_view_frustum_box, secondary_camera_prim, BOX__ALL_SIDES, 1);
                color.A = LOCATOR_OPACITY;
                if (draw_locator_grids)
                    drawLocatorGrid(viewport, color, out_color, &transitions.pre_projection);
            } else {
                transitions.projection.active = true;
                transitions.projection.t = 0;
            }
        }

        if (transitions.projection.active) {
            if (incTransition(&transitions.projection, delta_time, true)) {
                transitionBox(&transitions.projection, &pre_projected_view_frustum_box, &NDC_box);
                color = getColorInBetween(Color(Magenta), Color(Yellow), transitions.projection.eased_t);
                drawBox(viewport, color, &transforming_view_frustum_box, secondary_camera_prim, BOX__ALL_SIDES, 1);
                color.A = LOCATOR_OPACITY;
                if (draw_locator_grids)
                    drawLocatorGrid(viewport, color, out_color, &transitions.projection);
            }
        }
    }

    if (transitions.full_projection.active) {
        if (incTransition(&transitions.full_projection, delta_time, true)) {
            transitionBox(&transitions.full_projection, &view_frustum_box, &NDC_box);
            color = getColorInBetween(Color(Cyan), Color(Yellow), transitions.full_projection.eased_t);
            drawBox(viewport, color, &transforming_view_frustum_box, secondary_camera_prim, BOX__ALL_SIDES, 1);
            color.A = LOCATOR_OPACITY;
            if (draw_locator_grids)
                drawLocatorGrid(viewport, color, out_color, &transitions.full_projection);
        }
    }
}

void renderProjectiveSpace(Viewport *viewport, f32 delta_time) {
    drawGrid(viewport, Color(Magenta), projective_ref_plane_grid, projective_ref_plane_prim,0);
    drawBox(viewport, Color(Grey), main_box, main_box_prim, BOX__ALL_SIDES, 0);

    if (transitions.projective_lines.active) {
        if (incTransition(&transitions.projective_lines, delta_time, false))
            drawProjectiveSpace(viewport, &transitions.projective_lines, true);
    }
}

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


    if (transitions.view_frustom_slice.active) {
        if (incTransition(&transitions.view_frustom_slice, delta_time, false))
            camera_color.A = Y_color.A = (u8)((f32)camera_color.A * (1.0f - transitions.view_frustom_slice.eased_t));
    }
    if (camera_color.A) {
        sides_color.A = near_color.A = far_color.A = NDC_color.A = camera_color.A;
        drawBox(viewport, NDC_color, &NDC_box, secondary_camera_prim, BOX__ALL_SIDES, 1);
        drawFrustum(1);
        sides_color.A = near_color.A = far_color.A = NDC_color.A = MAX_COLOR_VALUE - camera_color.A;
    }

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
//    else if (transitions.lift_up.active) {
//        RGBA grid_color = Color(projective_ref_plane_prim->color);
//        if (incTransition(&transitions.lift_up, delta_time, false)) {
//            for (u8 i = 0; i < 4; i++) transforming_quad.corners[i].y = transitions.lift_up.eased_t;
//
//            grid_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.lift_up.eased_t);
//        }
//
//        if (transitions.translate_back.active) {
//            if (incTransition(&transitions.translate_back, delta_time, false)) {
//                f32 translation = -transitions.translate_back.eased_t * secondary_viewport.settings.near_clipping_plane_distance;
//                for (u8 i = 0; i < 4; i++) transforming_quad.corners[i].z += translation;
//
//                projective_ref_plane_prim->position.z = projective_ref_plane_prim->scale.z + secondary_viewport.settings.near_clipping_plane_distance + translation;
//                arrowY.body.to.z = translation;
//                updateArrow(&arrowY);
//            }
//
//            if (transitions.scale_back.active) {
//                vec3 scale_target = Vec3(pre_projection_matrix.X.x, 1.0f, pre_projection_matrix.Z.z);
//                vec3 scale = getVec3Of(1);
//                if (incTransition(&transitions.scale_back, delta_time, false)) {
//                    scale = lerpVec3(scale, scale_target, transitions.scale_back.eased_t);
//                    scale.y = 1.0f;
//                    for (u8 i = 0; i < 4; i++) transforming_quad.corners[i] = mulVec3(transforming_quad.corners[i], scale);
//                }
//
//                projective_ref_plane_prim->scale = scaleVec3(scale, 10);
//
//                arrowX.body.to.x = scale.x * 2;
//                updateArrow(&arrowX);
//
//                arrowZ.body.to.z = scale.z * 2;
//                updateArrow(&arrowZ);
//
//                if (transitions.shear_up.active) {
//                    if (incTransition(&transitions.shear_up, delta_time, false)) {
//
//                        arrowZ.body.to.y = transitions.shear_up.eased_t;
//                        updateArrow(&arrowZ);
//                    }
//                }
//            }
//        }
//
//        drawGrid(viewport, grid_color, projective_ref_plane_grid, projective_ref_plane_prim,0);
//    }

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
    if (NDC_color.A) {
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
            drawEdge(viewport, i % 2 ? sides_color : (i ? far_color : near_color), &edge, 2);
        }
    }

    if (transitions.forcal_length_and_plane.active) {
        viewport->settings.depth_sort = false;

        if (incTransition(&transitions.forcal_length_and_plane, delta_time, false))
            focal_length_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.forcal_length_and_plane.eased_t);

        Edge focal_length_edge;
        focal_length_edge.from = focal_length_edge.to = getVec3Of(0);
        focal_length_edge.to.z = secondary_viewport.camera->focal_length;
        focal_length_edge.from = convertPositionToObjectSpace(focal_length_edge.from, main_camera_prim);
        focal_length_edge.to   = convertPositionToObjectSpace(focal_length_edge.to,   main_camera_prim);
        drawEdge(viewport, focal_length_color, &focal_length_edge, 3);

        vec3 corner = Vec3(secondary_viewport.frame_buffer->dimensions.width_over_height, 1, secondary_viewport.camera->focal_length);

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

        X_color.A = focal_length_color.A;
        edge.from = edge.to = corner;
        edge.from.x = 0;
        edge.from.y = edge.to.y = 0;
        edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
        edge.to = convertPositionToObjectSpace(edge.to, main_camera_prim);
        drawEdge(viewport, X_color, &edge, 2);

        Y_color.A = focal_length_color.A;
        edge.from = edge.to = corner;
        edge.from.x = edge.to.x = 0;
        edge.from.y = 0;
        edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
        edge.to = convertPositionToObjectSpace(edge.to, main_camera_prim);
        drawEdge(viewport, Y_color, &edge, 2);

        viewport->settings.depth_sort = true;
        drawBox(viewport, Color(DarkGrey), main_box, main_box_prim, BOX__ALL_SIDES, 0);
        Edge *clipped_edge;
        transformGridVerticesFromObjectToViewSpace(&secondary_viewport, main_grid_prim, main_grid, &clipped_grid.vertices);
        setGridEdgesFromVertices(clipped_grid.edges.uv.u, main_grid->u_segments, clipped_grid.vertices.uv.u.from, clipped_grid.vertices.uv.u.to);
        setGridEdgesFromVertices(clipped_grid.edges.uv.v, main_grid->v_segments, clipped_grid.vertices.uv.v.from, clipped_grid.vertices.uv.v.to);
        clipped_edge = clipped_grid.edges.uv.u;
        for (u8 u = 0; u < main_grid->u_segments; u++, clipped_edge++) drawClippedEdge(viewport, clipped_edge, Color(main_grid_prim->color));
        clipped_edge = clipped_grid.edges.uv.v;
        for (u8 v = 0; v < main_grid->v_segments; v++, clipped_edge++) drawClippedEdge(viewport, clipped_edge, Color(main_grid_prim->color));

        transformBoxVerticesFromObjectToViewSpace(&secondary_viewport, main_box_prim, &main_box->vertices, &clipped_box.vertices);
        setBoxEdgesFromVertices(&clipped_box.edges, &clipped_box.vertices);
        clipped_edge = clipped_box.edges.buffer;
        for (u8 i = 0; i < BOX__EDGE_COUNT; i++, clipped_edge++) drawClippedEdge(viewport, clipped_edge, Color(main_box_prim->color));

        if (show_secondary_viewport) {
            fillPixelGrid(&secondary_viewport_frame_buffer, Color(Black));

            drawGrid(&secondary_viewport, Color(main_grid_prim->color), main_grid, main_grid_prim,0);
            drawBox(&secondary_viewport, Color(main_box_prim->color), main_box, main_box_prim, BOX__ALL_SIDES, 1);

            drawSecondaryViewportToFrameBuffer(viewport->frame_buffer);
        }
    }

    viewport->settings.depth_sort = false;
    updateCameraArrows(&secondary_viewport.camera->transform);
    drawArrow(viewport, X_color, &arrowX, 2);
    drawArrow(viewport, Y_color, &arrowY, 2);
    drawArrow(viewport, Z_color, &arrowZ, 2);
    viewport->settings.depth_sort = true;
}

void updateAndRender() {
    Timer *timer = &app->time.timers.update;
    Mouse *mouse = &app->controls.mouse;
    Viewport *viewport = &app->viewport;

    startFrameTimer(timer);
    fillPixelGrid(viewport->frame_buffer, Color(Black));

    static float elapsed = 0;
    elapsed += timer->delta_time;

    if (app->controls.is_pressed.ctrl) {
        if (mouse->wheel_scrolled) {
            f32 z = secondary_viewport.navigation.settings.speeds.zoom * mouse->wheel_scroll_amount;
            secondary_viewport.settings.near_clipping_plane_distance += z;
            mouse->wheel_scroll_handled = true;
        }
    } else if (app->controls.is_pressed.shift) {
        if (mouse->wheel_scrolled) {
            f32 z = secondary_viewport.navigation.settings.speeds.zoom * mouse->wheel_scroll_amount;
            secondary_viewport.settings.far_clipping_plane_distance += z;
            mouse->wheel_scroll_handled = true;
        }
    } else updateViewport(active_viewport, mouse);

    if (active_viewport->navigation.moved ||
        active_viewport->navigation.turned) {
        if (active_viewport == &secondary_viewport)
            updateCameraArrows(&secondary_viewport.camera->transform);
    }

    if (!mouse->is_captured) {
        manipulateSelection(&app->scene, active_viewport, &app->controls);

        arrow1.body.to = arrow_box_prim->position;
        updateArrow(&arrow1);
    }

    setPreProjectionMatrix(viewport);
    setPreProjectionMatrix(&secondary_viewport);

    f32 target_distance = viewport->camera->target_distance;
    viewport->camera->target_distance = lengthVec3(viewport->camera->transform.position);
    orbitCamera(viewport->camera, timer->delta_time / 30, 0);
    viewport->camera->target_distance = target_distance;

    viewport->settings.depth_sort = true;
    drawGrid(viewport, Color(DarkGrey), main_grid, main_grid_prim,0);

    secondary_camera_prim->position = secondary_viewport.camera->transform.position;
    secondary_camera_prim->rotation = secondary_viewport.camera->transform.rotation;
    main_camera_prim->position = viewport->camera->transform.position;
    main_camera_prim->rotation = viewport->camera->transform.rotation;
    updateProjectionBoxes(&secondary_viewport);

    sides_color = Color(Cyan);
    near_color = Color(BrightRed);
    far_color = Color(BrightBlue);
    NDC_color = Color(Yellow);

    X_color = Color(BrightRed);
    Y_color = Color(BrightGreen);
    Z_color = Color(BrightBlue);

    if (current_viz != VIEW_FRUSTUM_SLICE)
        drawCamera(viewport, camera_color, secondary_viewport.camera, 0);

    switch (current_viz) {
        case INTRO:              renderIntro(viewport, timer->delta_time, elapsed); break;
        case VIEW_FRUSTUM:       renderViewFrustum(viewport, timer->delta_time, elapsed); break;
        case PROJECTION:         renderProjection(viewport, timer->delta_time); break;
        case PROJECTIVE_SPACE:   renderProjectiveSpace(viewport, timer->delta_time); break;
        case VIEW_FRUSTUM_SLICE: renderViewSpaceFrustumSlice(viewport, timer->delta_time); break;
    }

    if (show_secondary_viewport) {
        fillPixelGrid(&secondary_viewport_frame_buffer, Color(Black));

        drawGrid(&secondary_viewport, Color(main_grid_prim->color), main_grid, main_grid_prim,0);
        drawBox(&secondary_viewport, Color(main_box_prim->color), main_box, main_box_prim, BOX__ALL_SIDES, 1);

        drawSecondaryViewportToFrameBuffer(viewport->frame_buffer);
    }

    active_viewport->navigation.moved  = false;
    active_viewport->navigation.turned = false;
    resetMouseChanges(mouse);
    endFrameTimer(timer);
}

void setupViewportCore(Viewport *viewport) {
    Scene *scene = &app->scene;

    for (u32 i = 0; i < scene->settings.cameras; i++) initCamera(scene->cameras + i);
    for (u32 i = 0; i < scene->settings.primitives; i++) { initPrimitive(scene->primitives + i); scene->primitives[i].id = i; }
    for (u32 i = 0; i < scene->settings.boxes; i++) initBox(scene->boxes + i);
    for (u32 i = 0; i < scene->settings.grids; i++) initGrid(scene->grids + i, 3, 3);

    main_box_prim->color = Yellow;
    main_grid_prim->color = White;
    projection_plane_prim->color = Cyan;
    camera_color = Color(projection_plane_prim->color);
    camera_color.A = MAX_COLOR_VALUE / 2;

    xform3 *camera_xform = &scene->cameras[0].transform;
    camera_xform->position = Vec3(0, 13, -30);
    rotateXform3(camera_xform, 0, -0.15f, 0);

    draw_locator_grids = false;
    show_pre_projection = false;
    move_projective_point = true;

    camera_xform = &scene->cameras[1].transform;

    if (current_viz == VIEW_FRUSTUM_SLICE) {
        //    camera_xform->position = Vec3(0, 0.05f, 0.05f);

        initGrid(main_grid,41, 41);
        main_grid_prim->scale    = Vec3(20, 1, 20);
        //    main_grid_prim->position = Vec3(0, 0, 10);

        initGrid(projective_ref_plane_grid,41, 41);
        projective_ref_plane_prim->color = Magenta;
        projective_ref_plane_prim->scale    = Vec3(20, 1, 20);
        projective_ref_plane_prim->position = Vec3(0, 1, 0);

        main_box_prim->position = Vec3(0, 0, 7);
        rotatePrimitive(main_box_prim, 0.23f, 0.34f, 0);
    } else {
        rotatePrimitive(main_grid_prim, 0.5f, 0, 0);
        camera_xform->position = Vec3(0, 9, -10);
        rotateXform3(camera_xform, 0, -0.2f, 0);

        initGrid(main_grid,11, 11);
        main_grid_prim->scale    = Vec3(5, 1, 5);
        main_grid_prim->position = Vec3(0, 0, 5);

        vec2 sin_cos = Vec2(SQRT2_OVER_2, SQRT2_OVER_2);
        vec3 x_axis = Vec3(1, 0, 0);

        projection_plane_prim->id = 1;
        projection_plane_prim->scale = Vec3(6, 1, 4);
        projection_plane_prim->position = scaleAddVec3(*camera_xform->forward_direction,
                                                       secondary_viewport.settings.near_clipping_plane_distance,
                                                       camera_xform->position);
        initGrid(projection_plane_grid, 2, 2);
        projection_plane_prim->rotation = rotateAroundAxisBySinCos(camera_xform->rotation, x_axis, sin_cos);

        projective_ref_plane_prim->id = 2;
        //    projective_ref_plane_prim->scale = Vec3(10, 1, 10);
        projective_ref_plane_prim->position = scaleAddVec3(*camera_xform->forward_direction, 1, camera_xform->position);
        initGrid(projective_ref_plane_grid, 11, 11);
        projective_ref_plane_prim->rotation = rotateAroundAxisBySinCos(camera_xform->rotation, x_axis, sin_cos);

        main_box_prim->position = Vec3(0, 3, 5);
        main_box_prim->scale = getVec3Of(2);
    }

    arrow1.body.from = Vec3(2, 0, 3);
    arrow1.body.to = addVec3(arrow1.body.from, getVec3Of(10));
    arrow1.head.length = 1;
    updateArrow(&arrow1);

    arrow_box_prim->position = arrow1.body.to;
    scene->selection->primitive = scene->primitives + 1;

    initBox(&NDC_box);
    for (u8 i = 4; i < 8; i++) NDC_box.vertices.buffer[i].z = 0;
    setBoxEdgesFromVertices(&NDC_box.edges, &NDC_box.vertices);

    secondary_camera_prim->position = secondary_viewport.camera->transform.position;
    secondary_camera_prim->rotation = secondary_viewport.camera->transform.rotation;
    main_camera_prim->position = viewport->camera->transform.position;
    main_camera_prim->rotation = viewport->camera->transform.rotation;

    updateProjectionBoxes(&secondary_viewport);
    updateCameraArrows(&secondary_viewport.camera->transform);

    for (u8 i = 0; i < TRANSITION_COUNT; i++) {
        transitions.states[i].active = false;
        transitions.states[i].t = 0;
        transitions.states[i].speed = i == 2 ? 0.25f : (i == 3 ? PROJECTION_LINE_INC_SPEED : 0.5f);
    }
    viewport->camera->target_distance = lengthVec3(viewport->camera->transform.position);
}

void onKeyChanged(u8 key, bool is_pressed) {
    NavigationMove *move = &active_viewport->navigation.move;
    NavigationTurn *turn = &active_viewport->navigation.turn;
    if (key == 'Q') turn->left     = is_pressed;
    if (key == 'E') turn->right    = is_pressed;
    if (key == 'R') move->up       = is_pressed;
    if (key == 'F') move->down     = is_pressed;
    if (key == 'W') move->forward  = is_pressed;
    if (key == 'S') move->backward = is_pressed;
    if (key == 'A') move->left     = is_pressed;
    if (key == 'D') move->right    = is_pressed;

    if (!is_pressed) {
        if (key == app->controls.key_map.space) {
            current_viz = (VIZ)(((u8)(current_viz) + 1) % (u8)VIS_COUNT);
            setupViewportCore(&app->viewport);
        }

        if (key == app->controls.key_map.tab) show_secondary_viewport = !show_secondary_viewport;
        if (key == '2') {
            transitions.pre_projection.active = true;
            transitions.pre_projection.t = 0;
            transitions.view_frustom_slice.active = true;
            transitions.view_frustom_slice.t = 0;
        }
        if (key == '1') {
            transitions.full_projection.active = true;
            transitions.full_projection.t = 0;
        }
        if (key == '4') {
//            transitions.translate_back.active = true;
//            transitions.translate_back.t = 0;
            transitions.reveal_projective_point.active = true;
            transitions.reveal_projective_point.t = 0;
            move_projective_point = !move_projective_point;
            draw_locator_grids = !draw_locator_grids;
        }
        if (key == '3') {
            transitions.projective_lines.active = true;
            transitions.projective_lines.t = 0;
            show_pre_projection = !show_pre_projection;
        }
        if (key == '5') {
//            transitions.scale_back.active = true;
//            transitions.scale_back.t = 0;
            transitions.reveal_ref_plane.active = true;
            if (transitions.reveal_projective_point.active && move_projective_point) {
                transitions.reveal_normalizing_projective_point.active = true;
                transitions.reveal_normalizing_projective_point.t = 0;
            } else
                transitions.reveal_ref_plane.t = 0;
        }
        if (key == '6') {
            transitions.reveal_projective_point.active = false;
            transitions.lift_up.active = true;
            transitions.lift_up.t = 0;
//            transitions.shear_up.active = true;
//            transitions.shear_up.t = 0;
        }
        if (key == '7') {
            transitions.lines_through_NDC.active = true;
            transitions.lines_through_NDC.t = 0;
        }
        if (key == '8') {
//            alpha = !alpha;
        }
        if (key == 'Z') {
            transitions.forcal_length_and_plane.active = !transitions.forcal_length_and_plane.active;
            transitions.forcal_length_and_plane.t = 0;
        }
    }
}

void setupViewport(Viewport *viewport) {
    active_viewport = viewport;
    viewport->settings.antialias = viewport->settings.depth_sort = true;
    initPixelGrid(&secondary_viewport_frame_buffer, allocateAppMemory(RENDER_SIZE));
    initViewport(&secondary_viewport,
                 &viewport->settings,
                 &viewport->navigation.settings,
                 app->scene.cameras + 1,
                 &secondary_viewport_frame_buffer);
    updateDimensions(&secondary_viewport_frame_buffer.dimensions, 600,400);
    secondary_viewport.settings.position.x = 20;
    secondary_viewport.settings.position.y = 20;
    secondary_viewport.settings.far_clipping_plane_distance = 20;
    secondary_viewport.settings.near_clipping_plane_distance = 4;

    Scene *scene = &app->scene;

    arrow_box_prim = scene->primitives;
    main_box_prim = arrow_box_prim + 1;
    main_grid_prim = main_box_prim + 1;
    projection_plane_prim = main_grid_prim + 1;
    projective_ref_plane_prim = projection_plane_prim + 1;
    main_camera_prim = projective_ref_plane_prim + 1;
    secondary_camera_prim = main_camera_prim + 1;

    main_grid = scene->grids;
    projection_plane_grid = main_grid + 1;
    projective_ref_plane_grid = projection_plane_grid + 1;

    main_box = scene->boxes;
    arrow_box = main_box + 1;

    main_box_prim->type = arrow_box_prim->type = PrimitiveType_Box;
    main_grid_prim->type = projection_plane_prim->type = projective_ref_plane_prim->type = PrimitiveType_Grid;

    setupViewportCore(viewport);
}

void initApp(Defaults *defaults) {
    app->on.viewportReady = setupViewport;
    app->on.windowRedraw  = updateAndRender;
    app->on.keyChanged               = onKeyChanged;
    app->on.mouseButtonDown          = onButtonDown;
    app->on.mouseButtonDoubleClicked = onDoubleClick;
    defaults->settings.viewport.hud_default_color = Green;
    defaults->settings.scene.grids      = 3;
    defaults->settings.scene.boxes      = 2;
    defaults->settings.scene.primitives = 7;
    defaults->settings.scene.cameras    = 2;
    defaults->additional_memory_size = RENDER_SIZE;
}