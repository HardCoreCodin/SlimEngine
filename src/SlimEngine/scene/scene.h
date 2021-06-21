#pragma once

#include "../core/base.h"
#include "../core/memory.h"
#include "./camera.h"
#include "./primitive.h"

typedef struct SceneCounts {
    u32 cameras, primitives, curves;
} SceneCounts;

SceneCounts getDefaultSceneCounts() {
    SceneCounts default_scene_counts;

    default_scene_counts.cameras = 1;
    default_scene_counts.primitives = 0;

    return default_scene_counts;
}

typedef struct Scene {
    SceneCounts counts;
    Camera *cameras;
    Primitive *primitives;
    Curve *curves;
} Scene;

void initScene(Scene *scene, SceneCounts scene_counts, Memory *memory) {
    scene->counts = scene_counts;
    scene->primitives = null;
    scene->cameras    = null;
    scene->curves     = null;

    if (scene_counts.cameras) {
        scene->cameras = allocateMemory(memory, sizeof(Camera) * scene->counts.cameras);
        if (scene->cameras)
            for (u32 i = 0; i < scene_counts.cameras; i++)
                initCamera(scene->cameras + i);
    }

    if (scene_counts.primitives) {
        scene->primitives = allocateMemory(memory, sizeof(Primitive) * scene->counts.primitives);
        if (scene->primitives)
            for (u32 i = 0; i < scene_counts.primitives; i++)
                initPrimitive(scene->primitives + i);
    }

    if (scene_counts.curves) {
        scene->curves = allocateMemory(memory, sizeof(Curve) * scene->counts.curves);
        if (scene->curves)
            for (u32 i = 0; i < scene_counts.curves; i++)
                initCurve(scene->curves + i);
    }
}