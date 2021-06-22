#pragma once

#include "../scene/scene.h"


void manipulateSelection(Scene *scene, Mouse *mouse) {
    if (!mouse->is_captured) {
        // Left mouse button was pressed while in the bounds of the window, and not in WASD navigation mode:

        vec3 position;
        vec3 *cam_pos = &scene->camera.transform.position;
        mat3 *rot     = &scene->camera.transform.rotation_matrix;
        mat3 *inv_rot = &scene->camera.transform.rotation_matrix_inverted;
        RayHit *hit = &ray_tracer.trace.closest_hit;
        Ray ray, *local_ray = &ray_tracer.trace.local_space_ray;

        if (left_mouse_button.is_pressed) {
            if (!left_mouse_button.is_handled) { // This is the first frame after the left mouse button went down:
                left_mouse_button.is_handled = true;

                // Cast a ray onto the scene to find the closest object behind the hovered pixel:
                setRayFromCoords(&ray, mouse_pos);
                if (tracePrimaryRay(&ray, &ray_tracer.trace, scene, mouse_pos.x, mouse_pos.y)) {
                    // Detect if object selection has changed:
                    current_selection.changed = (
                            current_selection.object_type != hit->object_type ||
                            current_selection.object_id   != hit->object_id
                    );

                    // Track the object that is now selected:
                    current_selection.object_type = hit->object_type;
                    current_selection.object_id   = hit->object_id;
                    current_selection.primitive   = null;

                    // Capture a pointer to the selected object's position for later use in transformations:
                    switch (current_selection.object_type) {
                        case PointLightType:
                            current_selection.world_position = &scene->point_lights[current_selection.object_id].position_or_direction;
                            break;
                        case QuadLightType:
                            current_selection.world_position = &scene->quad_lights[ current_selection.object_id].position;
                            break;
                        default:
                            current_selection.primitive = scene->primitives + current_selection.object_id;
                            current_selection.world_position = &current_selection.primitive->position;
                            current_selection.transformation_plane_origin = hit->position;
                    }

                    current_selection.world_offset = subVec3(hit->position, *current_selection.world_position);

                    // Track how far away the hit position is from the camera along the z axis:
                    position = mulVec3Mat3(subVec3(hit->position, ray.origin), *inv_rot);
                    current_selection.object_distance = position.z;
                } else {
                    if (current_selection.object_type)
                        current_selection.changed = true;
                    current_selection.object_type = 0;
                }
            }
        }

        if (current_selection.object_type) {
            if (alt_is_pressed) {
                bool any_mouse_button_is_pressed = (left_mouse_button.is_pressed ||
                                                    middle_mouse_button.is_pressed ||
                                                    right_mouse_button.is_pressed);
                if (current_selection.primitive && !any_mouse_button_is_pressed) {
                    // Cast a ray onto the bounding box of the currently selected object:
                    setRayFromCoords(&ray, mouse_pos);
                    convertPositionAndDirectionToObjectSpace(ray.origin, ray.direction, current_selection.primitive, &local_ray->origin, &local_ray->direction);

                    if (current_selection.primitive->object_type == MeshType) {
                        vec3 inv_scale = oneOver(scene->meshes[current_selection.primitive->instance_id].aabb.max);
                        position = addVec3(local_ray->origin, local_ray->direction);
                        position = mulVec3(position, inv_scale);

                        local_ray->origin    = mulVec3(local_ray->origin, inv_scale);
                        local_ray->direction = norm(subVec3(position, local_ray->origin));
                    }

                    current_selection.box_side = hitCube(hit, &local_ray->origin, &local_ray->direction, 0);
                    if (current_selection.box_side) {
                        if (current_selection.primitive->object_type == MeshType)
                            hit->position = mulVec3(hit->position, scene->meshes[current_selection.primitive->instance_id].aabb.max);

                        current_selection.transformation_plane_center = convertPositionToWorldSpace(hit->normal,   current_selection.primitive);
                        current_selection.transformation_plane_origin = convertPositionToWorldSpace(hit->position, current_selection.primitive);
                        current_selection.transformation_plane_normal = convertDirectionToWorldSpace(hit->normal,  current_selection.primitive);
                        current_selection.transformation_plane_normal = norm(current_selection.transformation_plane_normal);
                        current_selection.world_offset = subVec3(current_selection.transformation_plane_origin, *current_selection.world_position);
                        current_selection.object_scale    = current_selection.primitive->scale;
                        current_selection.object_rotation = current_selection.primitive->rotation;
                    }
                }

                if (current_selection.box_side) {
                    if (current_selection.primitive) {
                        if (any_mouse_button_is_pressed) {
                            setRayFromCoords(&ray, mouse_pos);
                            if (hitPlane(current_selection.transformation_plane_origin,
                                         current_selection.transformation_plane_normal,
                                         &ray.origin,
                                         &ray.direction,
                                         hit)) {
                                if (left_mouse_button.is_pressed) {
                                    position = subVec3(hit->position, current_selection.world_offset);
                                    setSelectedPrimitivePosition(scene, position);
                                } else if (middle_mouse_button.is_pressed) {
                                    position      = current_selection.transformation_plane_origin;
                                    position      = convertPositionToObjectSpace(     position, current_selection.primitive);
                                    hit->position = convertPositionToObjectSpace(hit->position, current_selection.primitive);

                                    current_selection.primitive->scale = mulVec3(current_selection.object_scale,
                                                                                 mulVec3(hit->position, oneOver(position)));
                                    current_selection.primitive->flags |= IS_SCALED | IS_SCALED_NON_UNIFORMLY;
                                    updatePrimitiveSSB(scene, current_selection.primitive,
                                                       scene->ssb.bounds + current_selection.object_id,
                                                       scene->ssb.view_positions + current_selection.object_id);
                                } else if (right_mouse_button.is_pressed) {
                                    vec3 v1 = subVec3(hit->position,
                                                      current_selection.transformation_plane_center);
                                    vec3 v2 = subVec3(current_selection.transformation_plane_origin,
                                                      current_selection.transformation_plane_center);
                                    quat q;
                                    q.axis = cross(v2, v1);
                                    q.amount = sqrtf(squaredLengthVec3(v1) * squaredLengthVec3(v2)) + dot(v1, v2);
                                    q = normQuat(q);
                                    current_selection.primitive->rotation = mulQuat(q, current_selection.object_rotation);
                                    current_selection.primitive->rotation = normQuat(current_selection.primitive->rotation);
                                    current_selection.primitive->flags |= IS_ROTATED;
                                    updatePrimitiveSSB(scene, current_selection.primitive,
                                                       scene->ssb.bounds + current_selection.object_id,
                                                       scene->ssb.view_positions + current_selection.object_id);
                                }
                            }
                        }
                    }
                }
            } else {
                current_selection.box_side = NoSide;
                if (left_mouse_button.is_pressed && mouse_moved) {
                    // Back-project the new mouse position onto a quad at a distance of the selected-object away from the camera
                    position.z = current_selection.object_distance;

                    // Screen -> NDC:
                    position.x = (f32) mouse_pos.x / frame_buffer.dimensions.h_width - 1;
                    position.y = (f32) mouse_pos.y / frame_buffer.dimensions.h_height - 1;
                    position.y = -position.y;

                    // NDC -> View:
                    position.x *= position.z / scene->camera.focal_length;
                    position.y *= position.z / (scene->camera.focal_length * frame_buffer.dimensions.width_over_height);

                    // View -> World:
                    position = addVec3(mulVec3Mat3(position, *rot), *cam_pos);

                    // Back-track by the world offset (from the hit position back to the selected-object's center):
                    position = subVec3(position, current_selection.world_offset);

                    setSelectedPrimitivePosition(scene, position);
                }
            }
        }
    }
}