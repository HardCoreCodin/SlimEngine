#pragma once

#include "../math/vec3.h"
#include "../math/quat.h"

INLINE f32 toneMappedBaked(f32 LinearColor) {
    f32 x = LinearColor - 0.004f;
    if (x < 0.0f) x = 0.0f;
    f32 x2_times_sholder_strength = x * x * 6.2f;
    return (x2_times_sholder_strength + x*0.5f)/(x2_times_sholder_strength + x*1.7f + 0.06f);
}

INLINE vec3 reflectWithDot(vec3 V, vec3 N, f32 NdotV) {
    return scaleAddVec3(N, -2 * NdotV, V);
}

INLINE vec3 shadePointOnSurface(Shaded *shaded, f32 NdotL) {
    if (shaded->has.specular) {
        vec3 half_vector, color;
        if (shaded->uses.blinn) {
            half_vector = normVec3(subVec3(shaded->light_direction, shaded->viewing_direction));
            color = scaleVec3(shaded->material->specular, powf(DotVec3(shaded->normal, half_vector), 16.0f * shaded->material->shininess));
        } else
            color = scaleVec3(shaded->material->specular, powf(DotVec3(shaded->reflected_direction, shaded->light_direction), 4.0f * shaded->material->shininess));

        if (shaded->has.diffuse)
            return scaleAddVec3(shaded->diffuse, clampValue(NdotL), color);
        else
            return color;
    } else
        return scaleVec3(shaded->diffuse, clampValue(NdotL));
}