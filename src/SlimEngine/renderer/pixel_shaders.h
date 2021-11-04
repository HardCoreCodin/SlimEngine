#pragma once

#include "../math/vec3.h"
#include "../scene/texture.h"
#include "./common.h"


INLINE u8 getCheckerBoardPixelValueByUV(vec2 UV, f32 half_step_count) {
    f32 s = UV.u * half_step_count;
    f32 t = UV.v * half_step_count;
    s -= floorf(s);
    t -= floorf(t);
    return (s > 0.5f ? (u8)1 : (u8)0) ^ (t < 0.5f ? (u8)1 : (u8)0);
}

void shadePixelTextured(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    outputs->color = sampleTexture(scene->textures + shaded->material->texture_ids[0], inputs->UV, inputs->dUV).v3;
}

void shadePixelDepth(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    outputs->color = getVec3Of(inputs->depth > 10 ? 1 : inputs->depth * 0.1f);
}

void shadePixelUV(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    outputs->color.v2 = inputs->UV;
}

void shadePixelPosition(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    outputs->color = scaleVec3(addVec3(shaded->position, getVec3Of(2.0f)), 0.5f);
}

void shadePixelNormal(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    outputs->color = scaleAddVec3(shaded->normal, 0.5f, getVec3Of(0.5f));
}

void shadePixelCheckerboard(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    outputs->color = getVec3Of(getCheckerBoardPixelValueByUV(inputs->UV, 4));
}

void shadePixelClassic(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    f32 NdotL, NdotRd, squared_distance;
    decodeMaterialSpec(shaded->material->flags, &shaded->has, &shaded->uses);

    shaded->diffuse = shaded->material->diffuse;

    if (shaded->material->texture_count) {
        shaded->diffuse = mulVec3(shaded->diffuse, sampleTexture(&scene->textures[shaded->material->texture_ids[0]], inputs->UV, inputs->dUV).v3);
        if (shaded->material->texture_count > 1) {
            vec3 normal = sampleTexture(&scene->textures[shaded->material->texture_ids[0]], inputs->UV, inputs->dUV).v3;
            normal = scaleAddVec3(normal, 0.5f, Vec3(-0.5f, -0.5f, -0.5f));
            shaded->normal = normVec3(addVec3(normal, shaded->normal));
        }
    }

    outputs->color = scene->ambient_light.color;
    shaded->viewing_direction = normVec3(subVec3(shaded->position, shaded->viewing_origin));
    if (shaded->uses.phong) {
        NdotRd = DotVec3(shaded->normal, shaded->viewing_direction);
        shaded->reflected_direction = reflectWithDot(shaded->viewing_direction, shaded->normal, NdotRd);
    }

    Light *light = scene->lights;
    for (u32 i = 0; i < scene->settings.lights; i++, light++) {
        shaded->light_direction = subVec3(light->position_or_direction, shaded->position);
        squared_distance = squaredLengthVec3(shaded->light_direction);
        shaded->light_direction = scaleVec3(shaded->light_direction, 1.0f / sqrtf(squared_distance));
        NdotL = dotVec3(shaded->normal, shaded->light_direction);
        if (NdotL > 0)
            outputs->color = mulAddVec3(
                    shadePointOnSurface(shaded, NdotL),
                    scaleVec3(light->color, light->intensity / squared_distance),
                    outputs->color);
    }

    outputs->color.x = toneMappedBaked(outputs->color.x);
    outputs->color.y = toneMappedBaked(outputs->color.y);
    outputs->color.z = toneMappedBaked(outputs->color.z);
}

void shadePixelClassicCheckerboard(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    shadePixelClassic(inputs, scene, shaded, outputs);

    if (!getCheckerBoardPixelValueByUV(inputs->UV, 4))
        outputs->color = scaleVec3(outputs->color, 0.5f);
}