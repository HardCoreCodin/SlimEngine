# SlimEngine

Minimalist, platform-agnostic, interactive render engine for writing graphical 2D/3D applications.<br>
Available as either a single header file or a directory of headers (a "unity build" setup).<br>
Can be complied in either C or C++.<br>

This project uses [SlimApp](https://github.com/HardCoreCodin/SlimApp) as it's base platform/application layer(s).

Architecture:
-
The platform layer only uses operating-system headers (no standard library used).<br>
The application layer itself has no dependencies, apart from the standard math header.<br>
It is just a library that the platform layer uses - it has no knowledge of the platform.<br>

More details on this architecture [here](https://youtu.be/Ev_TeQmus68).

Usage:
-
The single header file variant includes everything.<br>
Otherwise, specific headers can be included from the directory variant.<br>
The entry point needs to be defined (see [SlimApp](https://github.com/HardCoreCodin/SlimApp)). <br>

Features:
-
All features of SlimApp are available here as well.<br>
Additional features include facilities for interactive 3D application development:<br>
* A 3D viewport with wireframe rendering, a HUD and rich mouse/keyboard navigation.<br>
* A scene with cameras, meshes and parametric shapes.<br>
* Scene selection and interactive transformations (moving, rotating and scaling).

Well documented example applications cover the features:<br>

* Scene/camera setup and viewport drawing:<br>
  <p float="left">
    <img src="src/examples/1_scene.gif" alt="1_scene" height="300">
    <img src="src/examples/1_scene_and_camera_setup_c.png" alt="1_scene_and_camera_setup_code" height="300">
    <img src="src/examples/1_scene_setup_c.png" alt="1_scene_setup_code" width="400">
    <img src="src/examples/1_scene_initialization_and_viewport_drawing_c.png" alt="1_scene_initialization_and_viewport_drawing_code" width="400">
  </p>