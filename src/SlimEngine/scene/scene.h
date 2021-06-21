#pragma once

#include "../core/base.h"
#include "../core/memory.h"
#include "./camera.h"
#include "./primitive.h"
#include "./box.h"
#include "./grid.h"

typedef struct SceneCounts {
    u32 cameras, primitives, curves, boxes, grids;
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
    Grid *grids;
    Box *boxes;
} Scene;

void initScene(Scene *scene, SceneCounts scene_counts, Memory *memory) {
    scene->counts = scene_counts;
    scene->primitives = null;
    scene->cameras    = null;
    scene->curves     = null;
    scene->boxes      = null;
    scene->grids      = null;

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

    if (scene_counts.boxes) {
        scene->boxes = allocateMemory(memory, sizeof(Box) * scene->counts.boxes);
        if (scene->boxes)
            for (u32 i = 0; i < scene_counts.boxes; i++)
                initBox(scene->boxes + i, getVec3Of(-1), getVec3Of(1));
    }

    if (scene_counts.grids) {
        scene->grids = allocateMemory(memory, sizeof(Grid) * scene->counts.grids);
        if (scene->grids)
            for (u32 i = 0; i < scene_counts.grids; i++)
                initGrid(scene->grids + i, -1, -1, 1, 1, 3, 3);
    }
}