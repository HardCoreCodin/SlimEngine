#pragma once

#include "./core.h"
#include "./arrow.h"
#include "./projective_space.h"
#include "./matrix.h"
#include "./locator.h"
#include "./transition.h"

Quad3 warping_ndc;

void renderViewSpaceFrustumSlice(Viewport *viewport, f32 delta_time) {
    static float elapsed = 0;
    elapsed += delta_time;
    arrow1.head.length = arrowX.head.length = arrowZ.head.length = arrowY.head.length = transitions.view_frustom_slice.active ? 0.5f : 0.25f;

    drawGrid(main_grid, main_grid_prim, Color(main_grid_prim->color), BG_GRID_OPACITY, BG_GRID_LINE_WIDTH, viewport);

    transitions.scale_arrows.active = true;
    incTransition(&transitions.scale_arrows, delta_time, true);
    if (transitions.scale_arrows.eased_t == 0) transitions.scale_arrows.eased_t = 0.001f;

    f32 L = secondary_viewport.camera->focal_length;
    f32 A = secondary_viewport.dimensions.width_over_height;
    f32 N = secondary_viewport.settings.near_clipping_plane_distance;
    f32 F = secondary_viewport.settings.far_clipping_plane_distance;

    rotatePrimitive(main_box_prim, delta_time / -3, 0, 0);

    if (matrix_count) {
        Matrix *matrix = matrices;
        if (!matrix->is_custom) {
            initMatrix(matrix);
            matrix->M.X.x = L / A;
            matrix->M.Y.y = L;
            copyToString(&matrix->components[0][0].string, (char*)"L/A", 0);
            copyToString(&matrix->components[1][1].string, (char*)"L", 0);
            matrix->component_colors[0][0] = Color(White);
            matrix->component_colors[1][1] = Color(White);

            if (matrix_count > 1) {
                matrix++;

                if (!matrix->is_custom) {
                    initMatrix(matrix);

                    matrix->M.Z.w = 1.0f;
                    matrix->M.W.w = 0;
                    matrix->M.W.z = -N;
                    copyToString(&matrix->components[2][3].string, (char*)"1", 0);
                    copyToString(&matrix->components[3][3].string, (char*)"0", 0);
                    copyToString(&matrix->components[3][2].string, (char*)"-N", 0);
                    matrix->component_colors[2][3] = Color(White);
                    matrix->component_colors[3][3] = Color(White);
                    matrix->component_colors[3][2] = Color(White);

                    if (matrix_count >= 3) {
                        matrix++;

                        if (!matrix->is_custom) {
                            initMatrix(matrix);

                            if (secondary_viewport.settings.use_cube_NDC) {
                                if (collapse_final_matrix) {
                                    matrix->M.Z.z = 2.0f*F / (F - N);
                                    copyToString(&matrix->components[2][2].string, (char*)"2F/(F-N)", 0);
                                    matrix->component_colors[2][2] = Color(White);
                                } else {
                                    matrix->M.W.z = -1;
                                    copyToString(&matrix->components[3][2].string, (char*)"-1", 0);
                                    matrix->component_colors[3][2] = Color(White);
                                }

                                if (matrix_count >= 4) {
                                    matrix++;

                                    if (!matrix->is_custom) {
                                        initMatrix(matrix);

                                        if (collapse_final_matrix) {
                                            matrix->M.W.z = -1;
                                            copyToString(&matrix->components[3][2].string, (char*)"-1", 0);
                                            matrix->component_colors[3][2] = Color(White);
                                        } else {
                                            matrix->M.Z.z = (F + N) / (F - N);
                                            copyToString(&matrix->components[2][2].string, (char*)"(F+N)/(F-N)", 0);
                                            matrix->component_colors[2][2] = Color(White);
                                        }

                                        if (secondary_viewport.settings.flip_z && matrix_count >= 5) {
                                            matrix++;
                                            initMatrix(matrix);
                                            matrix->M.W = invertedVec4(matrix->M.W);
                                            copyToString(&matrix->components[2][2].string, (char*)"-1", 0);
                                            matrix->component_colors[2][2] = Color(White);
                                        }
                                    }
                                }
                            } else {
                                matrix->M.Z.z = F/(F-N);
                                copyToString(&matrix->components[2][2].string, (char*)"F/(F-N)", 0);
                                matrix->component_colors[2][2] = Color(White);
                            }
                        }
                    }
                }
            }
        }
    }
    mat4 src = getMat4Identity();
    mat4 M = getMat4Identity();
    for (u8 i = 0; i < matrix_count; i++) {
        M = mulMat4(M, matrices[i].M);
        if (i < (matrix_count - 1))
            src = mulMat4(src, matrices[i].M);
    }
    M = mulMat4(M, main_matrix.M);

    Quad3 NDC_quad, view_frustum_quad, transforming_quad;
    setQuad3FromBox(&view_frustum_quad, &view_frustum_box);
    setQuad3FromBox(&NDC_quad, &NDC_box);

    if (transitions.lift_up.active) {
        incTransition(&transitions.lift_up, delta_time, false);
        for (u8 i = 0; i < 4; i++) NDC_quad.corners[i].y = transitions.lift_up.eased_t;
    }

    transforming_quad = view_frustum_quad;

    Edge projective_point_edge, edge;
    vec3 projective_point_color;
    projective_point_color.g = (f32)MAX_COLOR_VALUE / 2;
    projective_point_color.r = (f32)MAX_COLOR_VALUE / 4 + projective_point_color.g;
    projective_point_color.b = (f32)MAX_COLOR_VALUE / 4 + projective_point_color.g;

    vec3 corner = Vec3(secondary_viewport.dimensions.width_over_height, 1, L);

    if (transitions.show_NDC_corner_labels.active) {
        for (u8 i = 0; i < BOX__VERTEX_COUNT; i++) {
            edge.from = scaleVec3(NDC_box.vertices.buffer[i], 1.15f);
            edge.to   = scaleVec3(edge.from, 1.15f);
            drawLocalEdge(edge, Color(Grey), 1, 1);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);
            char *str;
            switch (i) {
                case 0: str = (char*)"(-1, 1, 1)"; break;
                case 1: str = (char*)"(1, 1, 1)"; break;
                case 2: str = (char*)"(-1,-1, 1)"; break;
                case 3: str = (char*)"(1,-1, 1)"; break;
                case 4: str = secondary_viewport.settings.use_cube_NDC ? (char*)"(-1, 1,-1)" : (char*)"(-1, 1, 0)"; break;
                case 5: str = secondary_viewport.settings.use_cube_NDC ? (char*)"(1, 1,-1)" : (char*)"(1, 1, 0)"; break;
                case 6: str = secondary_viewport.settings.use_cube_NDC ? (char*)"(-1,-1,-1)" : (char*)"(-1,-1, 0)"; break;
                case 7: str = secondary_viewport.settings.use_cube_NDC ? (char*)"(1,-1,-1)" : (char*)"(1,-1, 0)"; break;
            }
            addLabel(Color(White), str, (i32)edge.to.x - ((i%2) ? 0 : (FONT_WIDTH*10)), (i32)edge.to.y);
        }
    }
    if (transitions.show_transformation.active) incTransition(&transitions.show_transformation, delta_time, true);

    f32 opacity;
    vec3 up_color = Y_color;
    if (transitions.view_frustom_slice.active) {
        if (incTransition(&transitions.view_frustom_slice, delta_time, false))
            up_color = getColorInBetween(Y_color, W_color, transitions.view_frustom_slice.eased_t);

        for (u8 i = 0; i < 4; i++) transformedPosition(transforming_quad.corners + i, M, src);
    }
    if (transitions.view_frustom_slice.eased_t < 1) {
        opacity = 1.0f - transitions.view_frustom_slice.eased_t;
        if (!transitions.focal_length_and_plane.active)
            drawBox(&NDC_box, BOX__ALL_SIDES, secondary_camera_prim, NDC_color, opacity, 1, viewport);

        transformBoxVertices(&view_frustum_box, M, &transforming_view_frustum_box.vertices, src);
        drawFrustum(viewport, &transforming_view_frustum_box, near_color, far_color, sides_color, opacity, 1);
    }

    if (transitions.projective_lines.active) {
        if (incTransition(&transitions.projective_lines, delta_time, false))
            drawProjectiveSpace(viewport, &transitions.projective_lines, !transitions.reveal_projective_point.active);

        if (transitions.reveal_projective_point.active) {
            if (incTransition(&transitions.reveal_projective_point, delta_time, false))
                opacity = transitions.reveal_projective_point.eased_t;

            Locator projective_point;
            vec3 location;
            if (move_projective_point) {
                location.x = 2 * sinf(elapsed)      - 1;
                location.z = 2 * sinf(elapsed+1) - 1;
                location.y = 1;
                projective_point_edge.to = normVec3(location);
                projective_point_color.r = (f32)MAX_COLOR_VALUE / 4 + projective_point_color.g * projective_point_edge.to.x;
                projective_point_color.b = (f32)MAX_COLOR_VALUE / 4 + projective_point_color.g * projective_point_edge.to.z;
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
                projective_point_color.r = (f32)MAX_COLOR_VALUE / 4 + projective_point_color.g * projective_point_edge.to.x;
                projective_point_color.b = (f32)MAX_COLOR_VALUE / 4 + projective_point_color.g * projective_point_edge.to.z;
                transitions.reveal_normalizing_projective_point.active = true;
                arrow1.body.from = getVec3Of(0);
                arrow1.body.to   = location;
                if (updateArrow(&arrow1)) drawArrow(&arrow1, projective_point_color, opacity, COORDINATE_ARROW_LINE_WIDTH, viewport);
            }

            setLocator(&projective_point, location);
            Edge *edge_ptr = projective_point.edges;
            for (u8 i = 0; i < 3; i++, edge_ptr++) {
                edge_ptr->to   = convertPositionToObjectSpace(edge_ptr->to,   main_camera_prim);
                edge_ptr->from = convertPositionToObjectSpace(edge_ptr->from, main_camera_prim);
                edge_ptr->from.z = edge_ptr->to.z = 0;
                drawEdge(edge_ptr, projective_point_color, opacity, LOCATOR_LINE_WIDTH, viewport);
            }

            projective_point_edge.from = invertedVec3(projective_point_edge.to);
            projective_point_edge.from = convertPositionToObjectSpace(projective_point_edge.from, main_camera_prim);
            projective_point_edge.to   = convertPositionToObjectSpace(projective_point_edge.to, main_camera_prim);
            projective_point_edge.to.z = projective_point_edge.from.z = 0;
            drawEdge(&projective_point_edge, projective_point_color, opacity * 0.5f, FAT_LABEL_LINE_WIDTH, viewport);

            drawCoordinateArrowsToPoint(location, X_color, up_color, Z_color, opacity, COORDINATE_ARROW_LINE_WIDTH, viewport);
        }
    }

    if (transitions.reveal_ref_plane.active) {
        vec3 grid_color = Color(projective_ref_plane_prim->color);
        if (incTransition(&transitions.reveal_ref_plane, delta_time, false))
            opacity = transitions.reveal_ref_plane.eased_t;

        opacity *= 0.25f;
        drawGrid(projective_ref_plane_grid, projective_ref_plane_prim, grid_color, opacity, BG_GRID_LINE_WIDTH, viewport);
    }

    if (transitions.full_projection.active) {
        if (incTransition(&transitions.full_projection, delta_time, false)) {
            sides_color = getColorInBetween(sides_color, Color(Yellow), transitions.full_projection.eased_t);
            near_color  = getColorInBetween(near_color,  Color(Yellow), transitions.full_projection.eased_t);
            far_color   = getColorInBetween(far_color,   Color(Yellow), transitions.full_projection.eased_t);
        }
    }

    if (transitions.lines_through_NDC.active) {
        if (transitions.corner_trajectory.active) incTransition(&transitions.corner_trajectory, delta_time, false);

        if (incTransition(&transitions.lines_through_NDC, delta_time, false)) {
            projective_point_edge.from = getVec3Of(0);
            opacity = transitions.lines_through_NDC.eased_t;
            projective_point_color.g = (f32)MAX_COLOR_VALUE * 0.5f;

            for (u8 i = 0; i < 4; i++) {
                projective_point_edge.to = normVec3(NDC_quad.corners[i]);
                projective_point_color.r = (f32)MAX_COLOR_VALUE * 0.25f + projective_point_color.g * projective_point_edge.to.x;
                projective_point_color.b = (f32)MAX_COLOR_VALUE * 0.25f + projective_point_color.g * projective_point_edge.to.z;

                if (transitions.corner_trajectory.active) {
                    f32 f = 4 + 2 * sinf(elapsed + (f32)i);
                    arrow1.body.from = transforming_quad.corners[i];
                    arrow1.body.to   = warping_ndc.corners[i] = lerpVec3(arrow1.body.from, scaleVec3(projective_point_edge.to, f), transitions.corner_trajectory.eased_t);

                    if (updateArrow(&arrow1)) drawArrow(&arrow1, projective_point_color, opacity, COORDINATE_ARROW_LINE_WIDTH, viewport);
                }

                projective_point_edge.to = scaleVec3(projective_point_edge.to, 40);
                projective_point_edge.from = invertedVec3(projective_point_edge.to);

                projective_point_edge.from = convertPositionToObjectSpace(projective_point_edge.from, main_camera_prim);
                projective_point_edge.to   = convertPositionToObjectSpace(projective_point_edge.to, main_camera_prim);
                projective_point_edge.to.z = projective_point_edge.from.z = 0;
                drawEdge(&projective_point_edge, projective_point_color, opacity * 0.5f, COORDINATE_ARROW_LINE_WIDTH, viewport);
            }
            edge_color = NDC_color;
            if (transitions.corners_warping.active) {
                incTransition(&transitions.corners_warping, delta_time, false);
                opacity = transitions.corners_warping.eased_t;
            }
            if ((transitions.corners_warping.active ||
                 transitions.stop_warping_ndc.active) &&
                !transitions.drop_warping_ndc.active) {
                for (u8 i = 0; i < 4; i++) {
                    edge.from = warping_ndc.corners[i];
                    edge.to   = warping_ndc.corners[(i + 1) % 4];
                    edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
                    edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
                    edge.to.z = edge.from.z = 0;
                    drawEdge(&edge, edge_color, opacity, COORDINATE_ARROW_LINE_WIDTH, viewport);
                }
            }

            if (transitions.drop_warping_ndc.active) {
                incTransition(&transitions.drop_warping_ndc, delta_time, false);

                vec3 location;
                for (u8 i = 0; i < 4; i++) {
                    edge.from = warping_ndc.corners[i];
                    edge.to   = warping_ndc.corners[(i + 1) % 4];
                    edge.from = location = lerpVec3(edge.from,     NDC_quad.corners[i],          transitions.drop_warping_ndc.eased_t);
                    edge.to   = lerpVec3(edge.to,   NDC_quad.corners[(i + 1) % 4], transitions.drop_warping_ndc.eased_t);
                    edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
                    edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
                    edge.to.z = edge.from.z = 0;
                    drawEdge(&edge, NDC_color, NDC_BOX_OPACITY, NDC_BOX_LINE_WIDTH, viewport);

                    if (transitions.drop_warping_ndc.t < 1) {
                        projective_point_color.g = (f32)MAX_COLOR_VALUE * 0.5f;
                        projective_point_color.b = (f32)MAX_COLOR_VALUE * 0.25f + projective_point_color.g * projective_point_edge.to.x;
                        drawCoordinateArrowsToPoint(location, X_color, up_color, Z_color, COORDINATE_ARROW_OPACITY, COORDINATE_ARROW_LINE_WIDTH, viewport);
                    }
                }
            }

            if (transitions.show_diagonality.active) {
                if (incTransition(&transitions.show_diagonality, delta_time, false)) {
                    f32 f = 4 + 2 * sinf(elapsed);
                    edge.from.z = edge.to.z = edge.from.y = 0;
                    edge.from.x = edge.to.x = f;
                    edge.to.y = f;

                    arrow1.body.from = edge.from;
                    arrow1.body.to = edge.to;
                    if (updateArrow(&arrow1)) drawArrow(&arrow1, near_color, transitions.show_diagonality.eased_t, COORDINATE_ARROW_LINE_WIDTH, viewport);

                    arrow1.body.from = edge.from;
                    arrow1.body.to = getVec3Of(0);
                    if (updateArrow(&arrow1)) drawArrow(&arrow1, near_color, transitions.show_diagonality.eased_t, COORDINATE_ARROW_LINE_WIDTH, viewport);

                    f = 4 + 2 * sinf(elapsed + 1);
                    arrow1.body.from = getVec3Of(f);
                    arrow1.body.to = arrow1.body.from;
                    arrow1.body.to.x = 0;
                    if (updateArrow(&arrow1)) drawArrow(&arrow1, far_color, transitions.show_diagonality.eased_t, COORDINATE_ARROW_LINE_WIDTH, viewport);

                    arrow1.body.from = getVec3Of(f);
                    arrow1.body.to = arrow1.body.from;
                    arrow1.body.to.y = 0;
                    if (updateArrow(&arrow1)) drawArrow(&arrow1, far_color, transitions.show_diagonality.eased_t, COORDINATE_ARROW_LINE_WIDTH, viewport);

                    arrow1.body.from = getVec3Of(f);
                    arrow1.body.to = arrow1.body.from;
                    arrow1.body.to.z = 0;
                    if (updateArrow(&arrow1)) drawArrow(&arrow1, far_color, transitions.show_diagonality.eased_t, COORDINATE_ARROW_LINE_WIDTH, viewport);
                }
            }

        }
    }

    if (transitions.show_chosen_trajectory.active && incTransition(&transitions.show_chosen_trajectory, delta_time, false)) {
        near_color = default_near_color;
        far_color  = default_far_color;
        if (transitions.show_chosen_trajectory_labels.active)
            incTransition(&transitions.show_chosen_trajectory_labels, delta_time, false);

        Edge *edge_ptr;
        Locator locator;
        for (u8 i = 0; i < 4; i++) {
            setLocator(&locator, transforming_quad.corners[i]);
            edge_ptr = locator.edges;
            for (u8 j = 0; j < 3; j++, edge_ptr++) {
                edge_ptr->to   = convertPositionToObjectSpace(edge_ptr->to,   main_camera_prim);
                edge_ptr->from = convertPositionToObjectSpace(edge_ptr->from, main_camera_prim);
                drawEdge(edge_ptr, Color(BrightBlue), 1, TRAJECTORY_LINE_WIDTH, viewport);
            }
        }
        vec3 target_locations[4] = {
                Vec3(F, F, F),
                Vec3(-F, F, F),
                Vec3(N, N, secondary_viewport.settings.use_cube_NDC ? -N : 0),
                Vec3(-N, N, secondary_viewport.settings.use_cube_NDC ? -N : 0)
        };
        for (u8 i = 0; i < 4; i++) {
            setLocator(&locator, target_locations[i]);
            edge_ptr = locator.edges;
            for (u8 j = 0; j < 3; j++, edge_ptr++) {
                edge_ptr->to   = convertPositionToObjectSpace(edge_ptr->to,   main_camera_prim);
                edge_ptr->from = convertPositionToObjectSpace(edge_ptr->from, main_camera_prim);
                drawEdge(edge_ptr, Color(BrightYellow), 1, TRAJECTORY_LINE_WIDTH, viewport);
            }
        }

        edge.to = normVec3(NDC_quad.top_right);
        edge_color.g = (f32)MAX_COLOR_VALUE * 0.5f;
        edge_color.r = (f32)MAX_COLOR_VALUE * 0.25f + edge_color.g * edge.to.x;
        edge_color.b = (f32)MAX_COLOR_VALUE * 0.25f + edge_color.b * edge.to.z;

        arrow1.body.from = Vec3(N, 0, N);
        arrow1.body.to = arrow1.body.from;
        arrow1.body.to.y = arrow1.body.to.x;
        edge.from = lerpVec3(arrow1.body.from, arrow1.body.to, 0.5f);
        arrow1.body.to = lerpVec3(arrow1.body.from, arrow1.body.to, transitions.show_chosen_trajectory.eased_t);
        if (transitions.show_chosen_trajectory_labels.active) {
            edge.from.x += 0.05f;
            edge.to = edge.from;
            edge.to.x += 0.25f;
            edge.to.y += 0.15f;

            drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);

            addLabel(near_color, (char*)"N", (i32)edge.to.x, (i32)edge.to.y);
        }
        if (updateArrow(&arrow1)) drawArrow(&arrow1, edge_color, transitions.show_chosen_trajectory.eased_t, ARROW_LINE_WIDTH, viewport);


        arrow1.body.from = Vec3(N, 0, N);
        arrow1.body.from.y = arrow1.body.from.x;
        arrow1.body.to = arrow1.body.from;
        arrow1.body.to.z = 0;
        edge.from = lerpVec3(arrow1.body.from, arrow1.body.to, 0.5f);
        arrow1.body.to = lerpVec3(arrow1.body.from, arrow1.body.to, transitions.show_chosen_trajectory.eased_t);
        if (updateArrow(&arrow1)) drawArrow(&arrow1, edge_color, transitions.show_chosen_trajectory.eased_t, ARROW_LINE_WIDTH, viewport);
        if (transitions.show_chosen_trajectory_labels.active) {
            edge.from.x += 0.05f;
            edge.to = edge.from;
            edge.to.x += 0.25f;
            edge.to.y += 0.15f;

            drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);

            addLabel(near_color, (char*)"N", (i32)edge.to.x, (i32)edge.to.y);

            arrow1.body.from = Vec3(N, 0, N);
            arrow1.body.from.y = 0;
            arrow1.body.to = arrow1.body.from;
            arrow1.body.to.z = 0;
            edge.from = lerpVec3(arrow1.body.from, arrow1.body.to, 0.5f);
            arrow1.body.to = lerpVec3(arrow1.body.from, arrow1.body.to, transitions.show_chosen_trajectory.eased_t);

            edge.from.x += 0.05f;
            edge.to = edge.from;
            edge.to.x += 0.25f;
            edge.to.y += 0.15f;

            drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);

            addLabel(near_color, (char*)"N", (i32)edge.to.x, (i32)edge.to.y);

            drawLocalEdge(arrow1.body, near_color, transitions.show_chosen_trajectory_labels.eased_t, LABEL_LINE_WIDTH);
        }
        if (secondary_viewport.settings.use_cube_NDC) {
            arrow1.body.from = Vec3(N, 0, 0);
            arrow1.body.from.y = arrow1.body.from.x;
            arrow1.body.to = arrow1.body.from;
            arrow1.body.to.z = -N;
            edge.from = lerpVec3(arrow1.body.from, arrow1.body.to, 0.5f);
            arrow1.body.to = lerpVec3(arrow1.body.from, arrow1.body.to, transitions.show_chosen_trajectory.eased_t);
            if (updateArrow(&arrow1)) drawArrow(&arrow1, edge_color, transitions.show_chosen_trajectory.eased_t, ARROW_LINE_WIDTH, viewport);

            if (transitions.show_chosen_trajectory_labels.active) {
                edge.from.x += 0.05f;
                edge.to = edge.from;
                edge.to.x += 0.25f;
                edge.to.y += 0.15f;

                drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);

                edge.to = lerpVec3(edge.from, edge.to, 1.15f);

                edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
                edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
                projectEdge(&edge, viewport);

                addLabel(near_color, (char*)"N", (i32)edge.to.x, (i32)edge.to.y);
            }
        }

        arrow1.body.from = Vec3(N, 0, N);
        arrow1.body.to = arrow1.body.from;
        arrow1.body.to.y = arrow1.body.to.z;
        arrow1.body.to.z = secondary_viewport.settings.use_cube_NDC ? -N : 0;
        arrow1.body.to = lerpVec3(arrow1.body.from, arrow1.body.to, transitions.scale_arrows.eased_t);
        if (updateArrow(&arrow1)) drawArrow(&arrow1, edge_color, transitions.show_chosen_trajectory.eased_t, ARROW_LINE_WIDTH, viewport);

        edge.to = normVec3(NDC_quad.bottom_right);
        edge_color.r = (f32)MAX_COLOR_VALUE * 0.25f + edge_color.g * edge.to.x;
        edge_color.b = (f32)MAX_COLOR_VALUE * 0.25f + edge_color.g * edge.to.z;
        arrow1.body.from = Vec3(F, 0, F);
        arrow1.body.to = arrow1.body.from;
        arrow1.body.to.y = arrow1.body.to.x;
        edge.from = lerpVec3(arrow1.body.from, arrow1.body.to, 0.5f);
        arrow1.body.to = lerpVec3(arrow1.body.from, arrow1.body.to, transitions.scale_arrows.eased_t);
        if (transitions.show_chosen_trajectory_labels.active) {
            edge.from.x += 0.05f;
            edge.to = edge.from;
            edge.to.x += 0.25f;
            edge.to.y += 0.15f;

            drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);

            addLabel(far_color, (char*)"F", (i32)edge.to.x, (i32)edge.to.y);
        }
        if (updateArrow(&arrow1)) drawArrow(&arrow1, edge_color, transitions.show_chosen_trajectory.eased_t, ARROW_LINE_WIDTH, viewport);

        arrow1.body.from = Vec3(F, 0, F);
        arrow1.body.to = arrow1.body.from;
        arrow1.body.to.z = 0;
        edge.from = lerpVec3(arrow1.body.from, arrow1.body.to, 0.5f);
        arrow1.body.to = lerpVec3(arrow1.body.from, arrow1.body.to, transitions.show_chosen_trajectory.eased_t);
        if (transitions.show_chosen_trajectory_labels.active) {
            edge.from.x += 0.05f;
            edge.to = edge.from;
            edge.to.x += 0.25f;
            edge.to.y += 0.15f;

            drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);

            addLabel(far_color, (char*)"F", (i32)edge.to.x, (i32)edge.to.y);
            drawLocalEdge(arrow1.body, far_color, transitions.show_chosen_trajectory_labels.eased_t, LABEL_LINE_WIDTH);
        }

        arrow1.body.from = Vec3(N, 0, N);
        arrow1.body.to = arrow1.body.from;
        arrow1.body.to.z = 0;
        edge.from = lerpVec3(arrow1.body.from, arrow1.body.to, 0.5f);
        arrow1.body.to = lerpVec3(arrow1.body.from, arrow1.body.to, transitions.show_chosen_trajectory.eased_t);
        if (transitions.show_chosen_trajectory_labels.active) {
            edge.from.x += 0.05f;
            edge.to = edge.from;
            edge.to.x += 0.25f;
            edge.to.y += 0.15f;

            drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);

            addLabel(near_color, (char*)"N", (i32)edge.to.x, (i32)edge.to.y);
        }
    }

    if (transitions.focal_length_and_plane.active) {
        focal_length_color = default_focal_length_color;
        incTransition(&transitions.focal_length_and_plane, delta_time, false);

        // Touching diagonals:
        edge.from = getVec3Of(0);
        edge.to = scaleVec3(Vec3(0, 1, L), 30);
        edge.to = lerpVec3(edge.from, edge.to, transitions.focal_length_and_plane.eased_t);
        drawLocalEdge(edge, focal_length_color, transitions.focal_length_and_plane.eased_t, LABEL_LINE_WIDTH);

        edge.to = scaleVec3(Vec3(A, 0, L), 30);
        edge.to = lerpVec3(edge.from, edge.to, transitions.focal_length_and_plane.eased_t);
        drawLocalEdge(edge, focal_length_color, transitions.focal_length_and_plane.eased_t, LABEL_LINE_WIDTH);

        // Labels
        edge.from = Vec3(0.05f, 0.5f, L);
        edge.to   = Vec3(0.35f, 0.6f, L);
        drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);

        edge.to = lerpVec3(edge.from, edge.to, 1.15f);

        edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
        edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
        projectEdge(&edge, viewport);

        addLabel(Y_color, (char*)"1", (i32)edge.to.x, (i32)edge.to.y);

        edge.from = Vec3(0.05f, 0, L*0.75f);
        edge.to   = Vec3(0.35f, 0, L*0.75f + 0.25f);
        drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);

        edge.to = lerpVec3(edge.from, edge.to, 1.15f);

        edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
        edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
        projectEdge(&edge, viewport);
        addLabel(focal_length_color, (char*)"L", (i32)edge.to.x, (i32)edge.to.y);

        if (transitions.show_focal_ratio.active) {
            edge.from = Vec3(0, 0, L/2);
            edge.to   = Vec3(0, 0.5f, L);

            arrow1.body = edge;
            if (updateArrow(&arrow1)) drawArrow(&arrow1, Color(BrightGrey), ARROW_OPACITY, ARROW_LINE_WIDTH, viewport);

            arrow1.body.from = edge.to;
            arrow1.body.to = edge.from;
            if (updateArrow(&arrow1)) drawArrow(&arrow1, Color(BrightGrey), ARROW_OPACITY, ARROW_LINE_WIDTH, viewport);

            vec3 offset_dir = Vec3(0, edge.to.z - edge.from.z, edge.from.y - edge.to.y);

            edge.from = scaleAddVec3(offset_dir, 0.05f, lerpVec3(edge.from, edge.to, 0.5f));
            edge.to   = scaleAddVec3(offset_dir, 0.5f, edge.from);
            drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);

            edge.to   = scaleAddVec3(offset_dir, 0.15f, edge.to);

            edge.to   = convertPositionToObjectSpace(edge.to,   main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);

            addLabel(Color(White), (char*)"  /   =", (i32)edge.to.x - 30, (i32)edge.to.y);
            addLabel(focal_length_color, (char*)"L", (i32)edge.to.x - 30, (i32)edge.to.y);
            addLabel(Y_color, (char*)"1", (i32)edge.to.x - 30 + 4 * FONT_WIDTH, (i32)edge.to.y);
            addLabel(focal_length_color, (char*)"L", (i32)edge.to.x - 30 + 8 * FONT_WIDTH, (i32)edge.to.y);

            offset_dir.z = -offset_dir.z;

            edge.from = Vec3(0, 0, L/4.0f);
            edge.to   = Vec3(0, 1.0f/6.0f, L/6.0f);
            drawLocalEdge(edge, Color(BrightBlue), 1, 1);

            edge.from.y = 0.1f;
            edge.from.z = 0.3f;
            edge.to = scaleAddVec3(offset_dir, 0.5f, edge.from);
            drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);

            edge.to   = scaleAddVec3(offset_dir, 0.05f, edge.to);

            edge.to = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);
            addLabel(Color(White), (char*)"cot(     )", (i32)edge.to.x - 60, (i32)edge.to.y);
            addLabel(Color(BrightBlue), (char*)"angle", (i32)edge.to.x - 60 + 4 * FONT_WIDTH, (i32)edge.to.y);
        }

        Edge focal_length_edge;
        focal_length_edge.from = focal_length_edge.to = getVec3Of(0);
        focal_length_edge.to.z = L;
        focal_length_edge.from = convertPositionToObjectSpace(focal_length_edge.from, main_camera_prim);
        focal_length_edge.to   = convertPositionToObjectSpace(focal_length_edge.to,   main_camera_prim);
        drawEdge(&focal_length_edge, focal_length_color, transitions.focal_length_and_plane.eased_t,3, viewport);

        Quad3 ref_quad;
        for (u8 i = 0; i < 4; i++)
            ref_quad.corners[i] = corner;

        ref_quad.top_left.x    = ref_quad.bottom_left.x  = -ref_quad.top_left.x;
        ref_quad.bottom_left.y = ref_quad.bottom_right.y = -ref_quad.top_left.y;

        for (u8 i = 0; i < 4; i++)
            ref_quad.corners[i] = convertPositionToObjectSpace(ref_quad.corners[i], main_camera_prim);

        edge_color = Color(Grey);
        for (u8 i = 0; i < 4; i++) {
            edge.from = ref_quad.corners[i];
            edge.to   = ref_quad.corners[(i + 1) % 4];
            drawEdge(&edge, edge_color, transitions.focal_length_and_plane.eased_t, FAT_LABEL_LINE_WIDTH, viewport);
        }

        edge.from = edge.to = corner;
        edge.to.y = -1;
        edge.from.x = edge.to.x = 1;
        drawLocalEdge(edge, edge_color, transitions.focal_length_and_plane.eased_t, BG_SHAPE_LINE_WIDTH);
        edge.from.x = edge.to.x = -1;
        drawLocalEdge(edge, edge_color, transitions.focal_length_and_plane.eased_t, BG_SHAPE_LINE_WIDTH);

        edge_color = default_aspect_ratio_color;
        edge.from = edge.to = corner;
        edge.from.x = 0;
        edge.from.y = edge.to.y = 0;
        drawLocalEdge(edge, edge_color, transitions.focal_length_and_plane.eased_t, FAT_LABEL_LINE_WIDTH);

        edge_color = Y_color;
        edge.from = edge.to = corner;
        edge.from.x = edge.to.x = 0;
        edge.from.y = 0;
        drawLocalEdge(edge, edge_color, transitions.focal_length_and_plane.eased_t, FAT_LABEL_LINE_WIDTH);

        if (transitions.aspect_ratio.active) {
            aspect_ratio_color = default_aspect_ratio_color;
            incTransition(&transitions.aspect_ratio, delta_time, false);

            // Diagonal:
            edge.from = edge.to = corner;
            edge.from.x = -edge.to.x;
            edge.from.y = -edge.to.y;
            edge.to = lerpVec3(edge.from, edge.to, transitions.aspect_ratio.eased_t);
            drawLocalEdge(edge, Color(Grey), 1, FG_SHAPE_LINE_WIDTH);

            // Bottom:
            edge.from = edge.to = corner;
            edge.from.y = -edge.from.y;
            edge.to = edge.from;
            edge.to.x = -edge.to.x;
            edge.to = lerpVec3(edge.from, edge.to, transitions.aspect_ratio.eased_t);
            drawLocalEdge(edge, default_aspect_ratio_color, transitions.aspect_ratio.eased_t, FG_SHAPE_LINE_WIDTH);

            // Side
            edge.from = edge.to = corner;
            edge.from.y = -edge.from.y;
            edge.to = lerpVec3(edge.from, edge.to, transitions.aspect_ratio.eased_t);
            drawLocalEdge(edge, Y_color, 1, FG_SHAPE_LINE_WIDTH);

            // Labels
            edge.from = Vec3(A * 0.85f, 0.05f, L);
            edge.to   = Vec3(A * 0.85f + 0.4f, 0.25f, L);
            drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);
            addLabel(default_aspect_ratio_color, (char*)"A", (i32)edge.to.x, (i32)edge.to.y);
            addLabel(Color(White), (char*)" / ", (i32)edge.to.x + FONT_WIDTH, (i32)edge.to.y);
            addLabel(Y_color, (char*)"1", (i32)edge.to.x + FONT_WIDTH * 4, (i32)edge.to.y);
            addLabel(Color(White), (char*)" = ", (i32)edge.to.x + FONT_WIDTH * 5, (i32)edge.to.y);
            addLabel(default_aspect_ratio_color, (char*)"W", (i32)edge.to.x + FONT_WIDTH * 8, (i32)edge.to.y);
            addLabel(Color(White), (char*)" / ", (i32)edge.to.x + FONT_WIDTH * 9, (i32)edge.to.y);
            addLabel(Y_color, (char*)"H", (i32)edge.to.x + FONT_WIDTH * 12, (i32)edge.to.y);

            edge.from = Vec3(0.05f + A, 0.5f, L);
            edge.to   = Vec3(0.35f + A, 0.6f, L);
            drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);

            addLabel(Y_color, (char*)"1", (i32)edge.to.x, (i32)edge.to.y);
        } else {
            edge.from = Vec3(A * 0.85f, 0.05f, L);
            edge.to   = Vec3(A * 0.85f + 0.4f, 0.25f, L);
            drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);
            addLabel(default_aspect_ratio_color, (char*)"A", (i32)edge.to.x, (i32)edge.to.y);
        }

        if (transitions.scale_out.active && incTransition(&transitions.scale_out, delta_time, true)) {
            edge.from = Vec3(0, 1, L);
            edge.to   = Vec3(0, L, L);
            edge.to   = lerpVec3(edge.from, edge.to, transitions.scale_out.eased_t);
            arrow1.body = edge;
            if (updateArrow(&arrow1)) drawArrow(&arrow1, Y_color, 0.75f, COORDINATE_ARROW_LINE_WIDTH, viewport);

            edge.from = edge.to;
            edge.from.x -= 0.05f;
            edge.from.y -= 0.05f;
            edge.to.y += 0.15f;
            edge.to.x -= 0.25f;
            drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);
            addLabel(focal_length_color, (char*)"L", (i32)edge.to.x, (i32)edge.to.y);
            addLabel(Color(White), (char*)" / ", (i32)edge.to.x + FONT_WIDTH, (i32)edge.to.y);
            addLabel(Y_color, (char*)"1", (i32)edge.to.x + FONT_WIDTH * 3, (i32)edge.to.y);


            edge.from = Vec3(A, 0, L);
            edge.to   = Vec3(L, 0, L);
            edge.to   = lerpVec3(edge.from, edge.to, transitions.scale_out.eased_t);
            arrow1.body = edge;
            if (updateArrow(&arrow1)) drawArrow(&arrow1, X_color, 0.75f, COORDINATE_ARROW_LINE_WIDTH, viewport);

            edge.from = edge.to;
            edge.from.x += 0.05f;
            edge.from.y += 0.05f;
            edge.to.y += 0.25f;
            edge.to.x += 0.15f;
            drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);
            addLabel(focal_length_color, (char*)"L", (i32)edge.to.x, (i32)edge.to.y);
            addLabel(Color(White), (char*)" / ", (i32)edge.to.x + FONT_WIDTH, (i32)edge.to.y);
            addLabel(default_aspect_ratio_color, (char*)"A", (i32)edge.to.x + FONT_WIDTH * 3, (i32)edge.to.y);
        }
    }

    opacity = 1.0f;
    if (transitions.view_scene.active) {
        if (incTransition(&transitions.view_scene, delta_time, false))
            opacity = transitions.view_scene.eased_t;

        if (!transitions.view_frustom_slice.active)
            drawBox(main_box, BOX__ALL_SIDES, main_box_prim, Color(DarkYellow), opacity, BG_SHAPE_LINE_WIDTH, viewport);

        Edge *clipped_edge;
        transformGridVerticesFromObjectToViewSpace(main_grid, &clipped_grid.vertices, main_grid_prim, &secondary_viewport);
        setGridEdgesFromVertices(clipped_grid.edges.uv.u, main_grid->u_segments, clipped_grid.vertices.uv.u.from, clipped_grid.vertices.uv.u.to);
        setGridEdgesFromVertices(clipped_grid.edges.uv.v, main_grid->v_segments, clipped_grid.vertices.uv.v.from, clipped_grid.vertices.uv.v.to);

        edge_color = Color(main_grid_prim->color);
        clipped_edge = clipped_grid.edges.uv.u;
        for (u8 u = 0; u < main_grid->u_segments; u++, clipped_edge++) {
            if (cullAndClipEdge(clipped_edge, &secondary_viewport)) {
                drawClippedEdge(viewport, transformedEdge(clipped_edge, M, src), edge_color, opacity, FG_GRID_LINE_WIDTH);
            }
        }

        clipped_edge = clipped_grid.edges.uv.v;
        for (u8 v = 0; v < main_grid->v_segments; v++, clipped_edge++) {
            if (cullAndClipEdge(clipped_edge, &secondary_viewport)) {
                drawClippedEdge(viewport, transformedEdge(clipped_edge, M, src), edge_color, opacity, FG_GRID_LINE_WIDTH);
            }
        }

        transformBoxVerticesFromObjectToViewSpace(&main_box->vertices, &clipped_box.vertices, main_box_prim, &secondary_viewport);
        setBoxEdgesFromVertices(&clipped_box.edges, &clipped_box.vertices);
        clipped_edge = clipped_box.edges.buffer;
        edge_color = Color(main_box_prim->color);
        for (u8 i = 0; i < BOX__EDGE_COUNT; i++, clipped_edge++) {
            if (transitions.view_frustom_slice.active && !(
                    clipped_edge == &clipped_box.edges.sides.left_bottom ||
                    clipped_edge == &clipped_box.edges.sides.right_bottom ||
                    clipped_edge == &clipped_box.edges.sides.front_bottom ||
                    clipped_edge == &clipped_box.edges.sides.back_bottom))
                continue;

            if (cullAndClipEdge(clipped_edge, &secondary_viewport))
                drawClippedEdge(viewport, transformedEdge(clipped_edge, M, src), edge_color, opacity, FG_SHAPE_LINE_WIDTH);
        }
    }

    if (transitions.grid_XW.active) {
        edge_color = Color(DarkGrey);
        if (incTransition(&transitions.grid_XW, delta_time, false))
            opacity = transitions.grid_XW.eased_t * 0.5f;

        vec3 P3;
        vec4 P4;
        for (u8 side = 0; side < 2; side++) {
            for (u8 axis = 0; axis < 2; axis++) {
                u8 segment_count = axis ? main_grid->v_segments : main_grid->u_segments;
                for (u8 segment = 0; segment < segment_count; segment++) {
                    P3 = main_grid->vertices.buffer[axis][side][segment];
                    if (transitions.view_frustom_slice.active) {
                        P4.x = 0;
                        P4.y = 0;
                        P4.z = P3.z;
                        P4.w = P3.x;
                        P4 = mulVec4Mat4(P4, M);
                        P3.x = 0;
                        P3.y = P4.w;
                        P3.z = P4.z;
                    } else {
                        P3.y = P3.x;
                        P3.x = 0;
                        mulVec3Mat4(P3, 1.0f, M, &P3);
                    }
                    transformed_grid.vertices.buffer[axis][side][segment] = P3;
                }
            }
        }
        setGridEdgesFromVertices(transformed_grid.edges.uv.u, main_grid->u_segments, transformed_grid.vertices.uv.u.from, transformed_grid.vertices.uv.u.to);
        setGridEdgesFromVertices(transformed_grid.edges.uv.v, main_grid->v_segments, transformed_grid.vertices.uv.v.from, transformed_grid.vertices.uv.v.to);

        drawGrid(&transformed_grid, main_grid_prim, edge_color, opacity, BG_GRID_LINE_WIDTH, viewport);

        for (u8 side = 0; side < 2; side++) {
            for (u8 axis = 0; axis < 2; axis++) {
                u8 segment_count = axis ? main_grid->v_segments : main_grid->u_segments;
                for (u8 segment = 0; segment < segment_count; segment++) {
                    P3 = main_grid->vertices.buffer[axis][side][segment];
                    if (transitions.view_frustom_slice.active) {
                        P4.z = 0;
                        P4.y = 0;
                        P4.x = P3.x;
                        P4.w = P3.z;
                        P4 = mulVec4Mat4(P4, M);
                        P3.x = P4.x;
                        P3.y = P4.w;
                        P3.z = 0;
                    } else {
                        P3.y = P3.z;
                        P3.z = 0;
                        mulVec3Mat4(P3, 1.0f, M, &P3);
                    }
                    transformed_grid.vertices.buffer[axis][side][segment] = P3;
                }
            }
        }
        setGridEdgesFromVertices(transformed_grid.edges.uv.u, main_grid->u_segments, transformed_grid.vertices.uv.u.from, transformed_grid.vertices.uv.u.to);
        setGridEdgesFromVertices(transformed_grid.edges.uv.v, main_grid->v_segments, transformed_grid.vertices.uv.v.from, transformed_grid.vertices.uv.v.to);

        drawGrid(&transformed_grid, main_grid_prim, edge_color, opacity, BG_GRID_LINE_WIDTH, viewport);
    }

    draw_in_2D = true;
    if (transitions.ground_diagonal.active) {
        incTransition(&transitions.ground_diagonal, delta_time, false);

        edge_color = Color(Grey);
        opacity = 0.75f;
        edge.from = getVec3Of(0);

        edge.to = Vec3(50, 0, 50);
        edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
        drawLocalEdge(edge, edge_color, opacity, LABEL_LINE_WIDTH);

        edge.to = Vec3(-50, 0, 50);
        edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
        drawLocalEdge(edge, edge_color, opacity, LABEL_LINE_WIDTH);

        if (!transitions.view_frustom_slice.active) {
            edge.to = Vec3(0, 50, 50);
            edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
            drawLocalEdge(edge, edge_color, opacity, LABEL_LINE_WIDTH);

            edge.to = Vec3(0, -50, 50);
            edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
            drawLocalEdge(edge, edge_color, opacity, LABEL_LINE_WIDTH);

            edge.to = Vec3(50, 50, 50);
            edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
            drawLocalEdge(edge, edge_color, opacity, LABEL_LINE_WIDTH);

            edge.to = Vec3(-50, 50, 50);
            edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
            drawLocalEdge(edge, edge_color, opacity, LABEL_LINE_WIDTH);

            edge.to = Vec3(50, -50, 50);
            edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
            drawLocalEdge(edge, edge_color, opacity, LABEL_LINE_WIDTH);

            edge.to = Vec3(-50, -50, 50);
            edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
            drawLocalEdge(edge, edge_color, opacity, LABEL_LINE_WIDTH);

            if (transitions.focal_length_and_plane.active) {
                edge.from = Vec3(0, 0, L);
                edge.to = Vec3(0, L, L);
                drawLocalEdge(edge, default_focal_length_color, LABEL_OPACITY, LABEL_LINE_WIDTH);

                edge.from = Vec3(0, 0, L);
                edge.to = Vec3(L, 0, L);
                drawLocalEdge(edge, default_focal_length_color, LABEL_OPACITY, LABEL_LINE_WIDTH);
            }
        }
    }

    if (transitions.view_frustom_slice.active) {
        vec3 *NDC = NDC_quad.corners, *frs = transforming_quad.corners;
        for (u8 i = 0; i < 4; i++, frs++, NDC++) {
            *frs = convertPositionToObjectSpace(*frs, main_camera_prim);
            *NDC = convertPositionToObjectSpace(*NDC, main_camera_prim);
        }
        for (u8 i = 0; i < 4; i++) {
            edge.from = NDC_quad.corners[i];
            edge.to   = NDC_quad.corners[(i + 1) % 4];
            edge.to.z = edge.from.z = 0;
            drawEdge(&edge, NDC_color, transitions.view_frustom_slice.eased_t, NDC_BOX_LINE_WIDTH, viewport);

            edge.from = transforming_quad.corners[i];
            edge.to   = transforming_quad.corners[(i + 1) % 4];
            edge.to.z = edge.from.z = 0;
            edge_color = i % 2 ? sides_color : (i ? far_color : near_color);
            drawEdge(&edge, edge_color, transitions.view_frustom_slice.eased_t, NDC_BOX_LINE_WIDTH, viewport);
        }
    }

    if (transitions.show_final_scale.active) {
        incTransition(&transitions.show_final_scale, delta_time, false);

        near_color = default_near_color;
        far_color  = default_far_color;

        edge.from = Vec3(-0.8f, F, -N);
        edge.to = Vec3(-0.8f, F, 0);
        drawLocalEdge(edge, near_color, transitions.show_final_scale.eased_t, LABEL_LINE_WIDTH);
        edge.from = lerpVec3(edge.from, edge.to, 0.5f);
        edge.from.x += 0.1f;
        edge.to = edge.from;
        edge.to.x += 0.5f;
        edge.to.y += 1.25f;
        drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);
        edge.to = lerpVec3(edge.from, edge.to, 1.25f);
        edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
        edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
        projectEdge(&edge, viewport);
        addLabel(near_color, (char*)"N", (i32)edge.to.x, (i32)edge.to.y);

        edge.from = Vec3(0, F, -N);
        edge.to = Vec3(0, F, F - N);
        drawLocalEdge(edge, far_color, transitions.show_final_scale.eased_t, LABEL_LINE_WIDTH);
        edge.from = lerpVec3(edge.from, edge.to, 0.5f);
        edge.from.x += 0.1f;
        edge.to = edge.from;
        edge.to.x += 1.5f;
        edge.to.y += 1.25f;
        drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);
        edge.to = lerpVec3(edge.from, edge.to, 1.25f);
        edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
        edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
        projectEdge(&edge, viewport);
        addLabel(far_color, (char*)"F", (i32)edge.to.x, (i32)edge.to.y);

        edge.from = Vec3(-0.8f, F, 0);
        edge.to = Vec3(-0.8f, F, F);
        drawLocalEdge(edge, far_color, transitions.show_final_scale.eased_t, LABEL_LINE_WIDTH);
        edge.from = lerpVec3(edge.from, edge.to, 0.5f);
        edge.from.x -= 0.2f;
        edge.to = edge.from;
        edge.to.x -= 1.5f;
        edge.to.y += 1.25f;
        drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);
        edge.to = lerpVec3(edge.from, edge.to, 1.25f);
        edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
        edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
        projectEdge(&edge, viewport);
        addLabel(far_color, (char*)"F", (i32)edge.to.x, (i32)edge.to.y);

        if (collapse_final_matrix) {
            edge.from = Vec3(+0.16f, F, F);
            edge.to = Vec3(+0.16f, F, 2*F);
            drawLocalEdge(edge, far_color, transitions.show_final_scale.eased_t, LABEL_LINE_WIDTH);
            edge.from = lerpVec3(edge.from, edge.to, 0.5f);
            edge.from.x -= 0.2f;
            edge.to = edge.from;
            edge.to.x -= 1.5f;
            edge.to.y += 1.25f;
            drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);
            edge.to = lerpVec3(edge.from, edge.to, 1.25f);
            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);
            addLabel(far_color, (char*)"F", (i32)edge.to.x, (i32)edge.to.y);
        }

        arrow1.body.from = Vec3(0.8f, F, F - N);
        arrow1.body.to = lerpVec3(arrow1.body.from, Vec3(0.8f, F, collapse_final_matrix ? (2*F) : F), transitions.scale_arrows.eased_t);
        if (updateArrow(&arrow1)) drawArrow(&arrow1, Z_color, ARROW_OPACITY, ARROW_LINE_WIDTH, viewport);
        edge.from = Vec3(0.8f, F, F);
        edge.from.x += 0.2f;
        edge.to = edge.from;
        edge.to.x += 1.5f;
        edge.to.y += 1.25f;
        drawLocalEdge(edge, Color(BrightGrey), LABEL_OPACITY, LABEL_LINE_WIDTH);
        edge.to = lerpVec3(edge.from, edge.to, 1.25f);
        edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
        edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
        projectEdge(&edge, viewport);
        if (secondary_viewport.settings.use_cube_NDC) {
            if (collapse_final_matrix) {
                addLabel(far_color, (char*)"2F", (i32)edge.to.x, (i32)edge.to.y);
                addLabel(Color(White), (char*)" / (", (i32)edge.to.x + FONT_WIDTH*2, (i32)edge.to.y);
                addLabel(far_color, (char*)"F", (i32)edge.to.x + FONT_WIDTH * 6, (i32)edge.to.y);
                addLabel(Color(White), (char*)" - ", (i32)edge.to.x + FONT_WIDTH * 7, (i32)edge.to.y);
                addLabel(near_color, (char*)"N", (i32)edge.to.x + FONT_WIDTH * 10, (i32)edge.to.y);
                addLabel(Color(White), (char*)")", (i32)edge.to.x + FONT_WIDTH * 11, (i32)edge.to.y);
            } else {
                addLabel(Color(White), (char*)"(", (i32)edge.to.x, (i32)edge.to.y);
                addLabel(far_color, (char*)"F", (i32)edge.to.x + FONT_WIDTH, (i32)edge.to.y);
                addLabel(Color(White), (char*)" + ", (i32)edge.to.x + FONT_WIDTH * 2, (i32)edge.to.y);
                addLabel(near_color, (char*)"N", (i32)edge.to.x + FONT_WIDTH * 5, (i32)edge.to.y);
                addLabel(Color(White), (char*)") / (", (i32)edge.to.x + FONT_WIDTH * 6, (i32)edge.to.y);
                addLabel(far_color, (char*)"F", (i32)edge.to.x + FONT_WIDTH * 11, (i32)edge.to.y);
                addLabel(Color(White), (char*)" - ", (i32)edge.to.x + FONT_WIDTH * 12, (i32)edge.to.y);
                addLabel(near_color, (char*)"N", (i32)edge.to.x + FONT_WIDTH * 15, (i32)edge.to.y);
                addLabel(Color(White), (char*)")", (i32)edge.to.x + FONT_WIDTH * 16, (i32)edge.to.y);
            }
        } else {
            addLabel(far_color, (char*)"F", (i32)edge.to.x, (i32)edge.to.y);
            addLabel(Color(White), (char*)" / (", (i32)edge.to.x + FONT_WIDTH, (i32)edge.to.y);
            addLabel(far_color, (char*)"F", (i32)edge.to.x + FONT_WIDTH * 5, (i32)edge.to.y);
            addLabel(Color(White), (char*)" - ", (i32)edge.to.x + FONT_WIDTH * 6, (i32)edge.to.y);
            addLabel(near_color, (char*)"N", (i32)edge.to.x + FONT_WIDTH * 9, (i32)edge.to.y);
            addLabel(Color(White), (char*)")", (i32)edge.to.x + FONT_WIDTH * 10, (i32)edge.to.y);
        }
    }

    arrowX.body.from = arrowY.body.from = arrowZ.body.from = getVec3Of(0);
    arrowX.body.to = arrowX_box_prim->position;
    arrowY.body.to = arrowY_box_prim->position;
    arrowZ.body.to = arrowZ_box_prim->position;

    arrowX.label = (char*)"X";
    arrowY.label = (char*)"Y";
    arrowZ.label = (char*)"Z";
    if (transitions.view_frustom_slice.active) {
        main_matrix.M.X = Vec4(arrowX_box_prim->position.x, main_matrix.M.X.y, arrowX_box_prim->position.z, arrowX_box_prim->position.y);
        main_matrix.M.Z = Vec4(arrowZ_box_prim->position.x, main_matrix.M.Z.y, arrowZ_box_prim->position.z, arrowZ_box_prim->position.y);
        main_matrix.M.W = Vec4(arrowY_box_prim->position.x, main_matrix.M.W.y, arrowY_box_prim->position.z, arrowY_box_prim->position.y);
        arrowY.label = (char*)"W";
    } else {
        main_matrix.M.X = Vec4fromVec3(arrowX_box_prim->position, main_matrix.M.X.w);
        main_matrix.M.Y = Vec4fromVec3(arrowY_box_prim->position, main_matrix.M.Y.w);
        main_matrix.M.Z = Vec4fromVec3(arrowZ_box_prim->position, main_matrix.M.Z.w);
    }
    if (updateArrow(&arrowX)) drawArrow(&arrowX, X_color, COORDINATE_ARROW_OPACITY, COORDINATE_ARROW_LINE_WIDTH, viewport);
    if (updateArrow(&arrowY)) drawArrow(&arrowY, transitions.view_frustom_slice.active ? W_color : Y_color, COORDINATE_ARROW_OPACITY, COORDINATE_ARROW_LINE_WIDTH, viewport);
    if (updateArrow(&arrowZ)) drawArrow(&arrowZ, Z_color, COORDINATE_ARROW_OPACITY, COORDINATE_ARROW_LINE_WIDTH, viewport);
    draw_in_2D = false;
}