#pragma once

#include "../core/types.h"
#include "../math/vec3.h"
#include "../math/quat.h"
#include "../scene/primitive.h"
#include "../scene/box.h"

void setViewportProjectionPlane(Viewport *viewport) {
    Camera *camera = viewport->camera;
    ProjectionPlane *projection_plane = &viewport->projection_plane;
    Dimensions *dimensions = &viewport->dimensions;

    projection_plane->start = scaleVec3(*camera->transform.forward_direction, dimensions->f_height * camera->focal_length);
    projection_plane->right = scaleVec3(*camera->transform.right_direction, 1.0f - (f32)dimensions->width);
    projection_plane->down  = scaleVec3(*camera->transform.up_direction, dimensions->f_height - 1.0f);
    projection_plane->start = addVec3(projection_plane->start, projection_plane->right);
    projection_plane->start = addVec3(projection_plane->start, projection_plane->down);

    projection_plane->right = scaleVec3(*camera->transform.right_direction, 2);
    projection_plane->down  = scaleVec3(*camera->transform.up_direction, -2);
}

INLINE void setRayFromCoords(Ray *ray, vec2i coords, Viewport *viewport) {
    ray->origin = viewport->camera->transform.position;
    ray->direction = scaleAddVec3(viewport->projection_plane.right, (f32)coords.x, viewport->projection_plane.start);
    ray->direction = scaleAddVec3(viewport->projection_plane.down,  (f32)coords.y, ray->direction);
    ray->direction = normVec3(ray->direction);
}

INLINE bool hitPlane(vec3 plane_origin, vec3 plane_normal, vec3 *ray_origin, vec3 *ray_direction, RayHit *hit) {
    f32 Rd_dot_n = dotVec3(*ray_direction, plane_normal);
    if (Rd_dot_n == 0) // The ray is parallel to the plane
        return false;

    bool ray_is_facing_the_plane = Rd_dot_n < 0;

    vec3 RtoP = subVec3(*ray_origin, plane_origin);
    f32 Po_to_Ro_dot_n = dotVec3(RtoP, plane_normal);
    hit->from_behind = Po_to_Ro_dot_n < 0;
    if (hit->from_behind == ray_is_facing_the_plane) // The ray can't hit the plane
        return false;

    hit->distance = fabsf(Po_to_Ro_dot_n / Rd_dot_n);
    hit->position = scaleVec3(*ray_direction, hit->distance);
    hit->position = addVec3(*ray_origin,      hit->position);
    hit->normal   = plane_normal;

    return true;
}

INLINE BoxSide getBoxSide(vec3 octant, u8 axis) {
    switch (axis) {
        case 0 : return octant.x > 0 ? Right : Left;
        case 3 : return octant.x > 0 ? Left : Right;
        case 1 : return octant.y > 0 ? Top : Bottom;
        case 4 : return octant.y > 0 ? Bottom : Top;
        case 2 : return octant.z > 0 ? Front : Back;
        default: return octant.z > 0 ? Back : Front;
    }
}

INLINE BoxSide hitCube(RayHit *hit, vec3 *Ro, vec3 *Rd) {
    vec3 octant, RD_rcp = oneOverVec3(*Rd);
    octant.x = signbit(Rd->x) ? 1.0f : -1.0f;
    octant.y = signbit(Rd->y) ? 1.0f : -1.0f;
    octant.z = signbit(Rd->z) ? 1.0f : -1.0f;

    f32 t[6];
    t[0] = ( octant.x - Ro->x) * RD_rcp.x;
    t[1] = ( octant.y - Ro->y) * RD_rcp.y;
    t[2] = ( octant.z - Ro->z) * RD_rcp.z;
    t[3] = (-octant.x - Ro->x) * RD_rcp.x;
    t[4] = (-octant.y - Ro->y) * RD_rcp.y;
    t[5] = (-octant.z - Ro->z) * RD_rcp.z;

    u8 max_axis = t[3] < t[4] ? 3 : 4; if (t[5] < t[max_axis]) max_axis = 5;
    f32 maxt = t[max_axis];
    if (maxt < 0) // Further-away hit is behind the ray - intersection can not occur.
        return NoSide;

    u8 min_axis = t[0] > t[1] ? 0 : 1; if (t[2] > t[min_axis]) min_axis = 2;
    f32 mint = t[min_axis];
    if (maxt < (mint > 0 ? mint : 0))
        return NoSide;

    hit->from_behind = mint < 0; // Further-away hit is in front of the ray, closer one is behind it.
    if (hit->from_behind) {
        mint = maxt;
        min_axis = max_axis;
    }

    BoxSide side = getBoxSide(octant, min_axis);
    hit->position = scaleAddVec3(*Rd, mint, *Ro);
    hit->normal = getVec3Of(0);
    switch (side) {
        case Left:   hit->normal.x = hit->from_behind ? +1.0f : -1.0f; break;
        case Right:  hit->normal.x = hit->from_behind ? -1.0f : +1.0f; break;
        case Bottom: hit->normal.y = hit->from_behind ? +1.0f : -1.0f; break;
        case Top:    hit->normal.y = hit->from_behind ? -1.0f : +1.0f; break;
        case Back:   hit->normal.z = hit->from_behind ? +1.0f : -1.0f; break;
        case Front:  hit->normal.z = hit->from_behind ? -1.0f : +1.0f; break;
        default: return NoSide;
    }

    return side;
}

INLINE bool rayHitScene(Ray *ray, RayHit *local_hit, RayHit *hit, Scene *scene) {
    bool current_found, found = false;
    vec3 Ro, Rd;
    Primitive hit_primitive, primitive;
    for (u32 i = 0; i < scene->settings.primitives; i++) {
        primitive = scene->primitives[i];
        if (primitive.type == PrimitiveType_Mesh)
            primitive.scale = mulVec3(primitive.scale, scene->meshes[primitive.id].aabb.max);

        convertPositionAndDirectionToObjectSpace(ray->origin, ray->direction, &primitive, &Ro, &Rd);

        current_found = hitCube(local_hit, &Ro, &Rd);
        if (current_found) {
            local_hit->position       = convertPositionToWorldSpace(local_hit->position, &primitive);
            local_hit->distance_squared = squaredLengthVec3(subVec3(local_hit->position, ray->origin));
            if (local_hit->distance_squared < hit->distance_squared) {
                *hit = *local_hit;
                hit->object_type = primitive.type;
                hit->object_id = i;

                hit_primitive = primitive;
                found = true;
            }
        }
    }

    if (found) {
        hit->distance = sqrtf(hit->distance_squared);
        hit->normal = normVec3(convertDirectionToWorldSpace(hit->normal, &hit_primitive));
    }

    return found;
}

void manipulateSelection(Scene *scene, Viewport *viewport, Controls *controls) {
    Mouse *mouse = &controls->mouse;
    Camera *camera = viewport->camera;
    Dimensions *dimensions = &viewport->dimensions;
    Selection *selection = scene->selection;

    setProjectionMatrix(viewport);
    setViewportProjectionPlane(viewport);

    vec3 position;
    vec3 *cam_pos = &camera->transform.position;
    mat3 *rot     = &camera->transform.rotation_matrix;
    mat3 *inv_rot = &camera->transform.rotation_matrix_inverted;
    RayHit *hit = &selection->hit;
    Ray ray, *local_ray = &selection->local_ray;
    Primitive primitive;
    vec2i mouse_pos = Vec2i(mouse->pos.x - viewport->position.x,
                            mouse->pos.y - viewport->position.y);

    if (mouse->left_button.is_pressed) {
        if (!mouse->left_button.is_handled) { // This is the first frame after the left mouse button went down:
            mouse->left_button.is_handled = true;

            // Cast a ray onto the scene to find the closest object behind the hovered pixel:
            setRayFromCoords(&ray, mouse_pos, viewport);

            hit->distance_squared = INFINITY;
            if (rayHitScene(&ray, &selection->local_hit, hit, scene)) {
                // Detect if object scene->selection has changed:
                selection->changed = (
                        selection->object_type != hit->object_type ||
                        selection->object_id   != hit->object_id
                );

                // Track the object that is now selected:
                selection->object_type = hit->object_type;
                selection->object_id   = hit->object_id;
                selection->primitive   = null;

                // Capture a pointer to the selected object's position for later use in transformations:
                selection->primitive = scene->primitives + selection->object_id;
                selection->world_position = &selection->primitive->position;
                selection->transformation_plane_origin = hit->position;

                selection->world_offset = subVec3(hit->position, *selection->world_position);

                // Track how far away the hit position is from the camera along the z axis:
                position = mulVec3Mat3(subVec3(hit->position, ray.origin), *inv_rot);
                selection->object_distance = position.z;
            } else {
                if (selection->object_type)
                    selection->changed = true;
                selection->object_type = 0;
            }
        }
    }

    if (selection->object_type) {
        if (controls->is_pressed.alt) {
            bool any_mouse_button_is_pressed = (
                    mouse->left_button.is_pressed ||
                    mouse->middle_button.is_pressed ||
                    mouse->right_button.is_pressed);
            if (selection->primitive && !any_mouse_button_is_pressed) {
                // Cast a ray onto the bounding box of the currently selected object:
                setRayFromCoords(&ray, mouse_pos, viewport);
                primitive = *selection->primitive;
                if (primitive.type == PrimitiveType_Mesh)
                    primitive.scale = mulVec3(primitive.scale, scene->meshes[primitive.id].aabb.max);

                convertPositionAndDirectionToObjectSpace(ray.origin, ray.direction, &primitive, &local_ray->origin, &local_ray->direction);

                selection->box_side = hitCube(hit, &local_ray->origin, &local_ray->direction);
                if (selection->box_side) {
                    selection->transformation_plane_center = convertPositionToWorldSpace(hit->normal,   &primitive);
                    selection->transformation_plane_origin = convertPositionToWorldSpace(hit->position, &primitive);
                    selection->transformation_plane_normal = convertDirectionToWorldSpace(hit->normal,  &primitive);
                    selection->transformation_plane_normal = normVec3(selection->transformation_plane_normal);
                    selection->world_offset = subVec3(selection->transformation_plane_origin, *selection->world_position);
                    selection->object_scale    = selection->primitive->scale;
                    selection->object_rotation = selection->primitive->rotation;
                }
            }

            if (selection->box_side) {
                if (selection->primitive) {
                    if (any_mouse_button_is_pressed) {
                        setRayFromCoords(&ray, mouse_pos, viewport);
                        if (hitPlane(selection->transformation_plane_origin,
                                     selection->transformation_plane_normal,
                                     &ray.origin,
                                     &ray.direction,
                                     hit)) {

                            primitive = *selection->primitive;
                            if (primitive.type == PrimitiveType_Mesh)
                                primitive.scale = mulVec3(primitive.scale, scene->meshes[primitive.id].aabb.max);
                            if (mouse->left_button.is_pressed) {
                                position = subVec3(hit->position, selection->world_offset);
                                *selection->world_position = position;
                                if (selection->primitive)
                                    selection->primitive->flags |= IS_TRANSLATED;
                            } else if (mouse->middle_button.is_pressed) {
                                position      = selection->transformation_plane_origin;
                                position      = convertPositionToObjectSpace(     position, &primitive);
                                hit->position = convertPositionToObjectSpace(hit->position, &primitive);

                                selection->primitive->scale = mulVec3(selection->object_scale,
                                                                             mulVec3(hit->position, oneOverVec3(position)));
                                selection->primitive->flags |= IS_SCALED | IS_SCALED_NON_UNIFORMLY;
                            } else if (mouse->right_button.is_pressed) {
                                vec3 v1 = subVec3(hit->position,
                                                  selection->transformation_plane_center);
                                vec3 v2 = subVec3(selection->transformation_plane_origin,
                                                  selection->transformation_plane_center);
                                quat q;
                                q.axis = crossVec3(v2, v1);
                                q.amount = sqrtf(squaredLengthVec3(v1) * squaredLengthVec3(v2)) + dotVec3(v1, v2);
                                q = normQuat(q);
                                selection->primitive->rotation = mulQuat(q, selection->object_rotation);
                                selection->primitive->rotation = normQuat(selection->primitive->rotation);
                                selection->primitive->flags |= IS_ROTATED;
                            }
                        }
                    }
                }
            }
        } else {
            selection->box_side = NoSide;
            if (mouse->left_button.is_pressed && mouse->moved) {
                // Back-project the new mouse position onto a quad at a distance of the selected-object away from the camera
                position.z = selection->object_distance;

                // Screen -> NDC:
                position.x = ((f32)mouse_pos.x + 0.5f) / dimensions->h_width  - 1;
                position.y = ((f32)mouse_pos.y + 0.5f) / dimensions->h_height - 1;
                position.y = -position.y;

                // NDC -> View:
                position.x *= position.z / (camera->focal_length * dimensions->height_over_width);
                position.y *= position.z / camera->focal_length;

                // View -> World:
                position = addVec3(mulVec3Mat3(position, *rot), *cam_pos);

                // Back-track by the world offset (from the hit position back to the selected-object's center):
                position = subVec3(position, selection->world_offset);
                *selection->world_position = position;
                if (selection->primitive)
                    selection->primitive->flags |= IS_TRANSLATED;
            }
        }
    }
}

void drawSelection(Scene *scene, Viewport *viewport, Controls *controls) {
    Mouse *mouse = &controls->mouse;
    Selection *selection = scene->selection;
    Box *box = &selection->box;

    if (controls->is_pressed.alt &&
        !mouse->is_captured &&
        selection->object_type &&
        selection->primitive) {
        Primitive primitive = *selection->primitive;
        if (primitive.type == PrimitiveType_Mesh)
            primitive.scale = mulVec3(primitive.scale, scene->meshes[primitive.id].aabb.max);

        initBox(box);
        drawBox(box, BOX__ALL_SIDES, &primitive, Color(Yellow), 0.5f, 0, viewport);
        if (selection->box_side) {
            vec3 color = Color(White);
            switch (selection->box_side) {
                case Left:  case Right:  color = Color(Red);   break;
                case Top:   case Bottom: color = Color(Green); break;
                case Front: case Back:   color = Color(Blue);  break;
                case NoSide: break;
            }
            drawBox(box, selection->box_side, &primitive, color, 0.5f, 1, viewport);
        }
    }
}