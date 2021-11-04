#pragma once

#include "../core/types.h"

INLINE vec4 sampleTextureMip(TextureMip *mip, vec2 UV, bool filter) {
    f32 u = UV.u;
    f32 v = UV.v;
    if (u > 1) u -= (f32)((u32)u);
    if (v > 1) v -= (f32)((u32)v);

    const f32 offset = filter ? 0.5f : -0.5f;

    const f32 U = u * (f32)mip->width  + offset;
    const f32 V = v * (f32)mip->height + offset;
    const u32 x = (u32)U;
    const u32 y = (u32)V;

    if (!filter) return mip->texels[mip->width * y + x];

    const f32 r = U - (f32)x;
    const f32 b = V - (f32)y;
    const f32 l = 1 - r;
    const f32 t = 1 - b;
    const f32 factors[4] = {
            t * l, t * r,
            b * l, b * r
    };
    const TexelQuad texel_quad = mip->texel_quad_lines[y][x];
    vec4 texel = getVec4Of(0);
    for (u8 i = 0; i < 4; i++) texel = scaleAddVec4(texel_quad.quadrants[i], factors[i], texel);

    return texel;
}

INLINE vec4 sampleTexture(Texture *texture, vec2 UV, vec2 dUV) {
    u8 mip_level = 0;
    if (texture->mipmap) {
        const f32 pixel_width  = dUV.u * (f32)texture->width;
        const f32 pixel_height = dUV.v * (f32)texture->height;
        f32 pixel_size = (f32)(pixel_width + pixel_height) * 0.5f;
        const u8 last_mip = texture->mip_count - 1;

        while (pixel_size > 1 && mip_level < last_mip) {
            pixel_size /= 2;
            mip_level += 1;
        }
    }

    return sampleTextureMip(texture->mips + mip_level, UV, texture->filter);
}