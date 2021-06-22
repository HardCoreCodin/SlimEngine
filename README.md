# SlimEngine

Minimalist, platform-agnostic, interactive render engine for writing graphical 2D/3D applications.<br>
Available as either a single header file or a directory of headers (a "unity build" setup).<br>
Can be complied in either C or C++.<br>

This project uses [SlimApp](https://github.com/HardCoreCodin/SlimApp) as it's base platform/application layer(s).

<i>(*A bare-bone application executable on Windows measures at around 17 KB(!))<br></i>

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
* Viewport: HUD (Head-UP Display)<br>
  <p float="left">
    <img src="src/examples/1_viewport.gif" alt="1_scene" height="300">
    <img src="src/examples/1_viewport_updateAndRender_c.png" alt="1_viewport_updateAndRender_code" width="360">
    <img src="src/examples/1_viewport_scene_setup_c.png" alt="1_viewport_scene_setup_code" width="350">
    <img src="src/examples/1_viewport_HUD_setup_c.png" alt="1_viewport_HUD_setup_code" height="300">
  </p>
  Viewports come with a HUD that can be toggled on/off<br>
  SlimEngine comes with an empty scene, apart from a default camera bound to a default viewport.<br>
  The window content is bound to the default viewport so they resize together.<br>

* Viewport: Navigation<br>
  <p float="left">
    <img src="src/examples/2_navigation.gif" alt="1_scene" height="300">
    <img src="src/examples/2_navigation_update_c.png" alt="2_navigation_update_code" width="360">
    <img src="src/examples/2_navigation_setup_c.png" alt="2_navigation_setup_code" width="350">
    <img src="src/examples/2_navigation_mouse_c.png" alt="2_navigation_mouse_code" height="300">
  </p>
  Viewport navigation actions can be composed to set-up WASD(FPS)/DCC(orb) navigation style(s).<br>
  Keyboard navigation is agnostic to key-binding and is trivial to map keys to.<br>
  It supports moving forward, backwards, left, right, up and down and turning left or right.<br>
  Mouse navigation controls the viewport's camera and is more granular.<br>
  It can pan, zoom, dolly, orient or orbit the camera around a target.<br>
  
* Scene: Setup and drawing parametric primitives (Grid, Box, Curves, etc.)<br>
  <p float="left">
    <img src="src/examples/3_scene.gif" alt="1_scene" height="300">
    <img src="src/examples/3_scene_setup_c.png" alt="3_scene_setup_code" width="360">
    <img src="src/examples/3_scene_initialization_and_viewport_drawing_c.png" alt="3_scene_initialization_and_viewport_drawing_code" width="350">
    <img src="src/examples/3_scene_viewport_drawing_c.png" alt="3_scene_viewport_drawing_code" height="300">
  </p>
  SlimEngine allocates and initializes the scene based on quantities set at app-initialization time.<br>
  Customizing the scene/viewport can be done in callbacks that get invoked when they're ready.<br>
  Wireframe drawing of the scene to the viewport can be invoked manually on window-redraw.<br>  

