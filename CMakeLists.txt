cmake_minimum_required(VERSION 3.8)

project(SlimEngine_1_viewport)
add_executable(SlimEngine_1_viewport WIN32 src/examples/1_viewport.c)

project(SlimEngine_2_navigation)
add_executable(SlimEngine_2_navigation WIN32 src/examples/2_navigation.c)

project(SlimEngine_3_cameras)
add_executable(SlimEngine_3_cameras WIN32 src/examples/3_cameras.c)

project(SlimEngine_4_shapes)
add_executable(SlimEngine_4_shapes WIN32 src/examples/4_shapes.c)

project(SlimEngine_5_manipulation)
add_executable(SlimEngine_5_manipulation WIN32 src/examples/5_manipulation.c)

project(SlimEngine_6_mesh)
add_executable(SlimEngine_6_mesh WIN32 src/examples/6_mesh.c)

project(SlimEngine_7_scene)
add_executable(SlimEngine_7_scene WIN32 src/examples/7_scene.c)

project(PerspectiveProjection)
add_executable(PerspectiveProjection WIN32 src/examples/visualizations/perspective_projection.c)

project(Obj2mesh)
add_executable(Obj2mesh src/obj2mesh.c)