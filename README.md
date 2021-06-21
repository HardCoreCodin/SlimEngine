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

* 3D Shapes: Box, Helix and Coil<br>
  <p float="left">
    <img src="src/examples/1_shapes3D.gif" alt="1_app" height="300">
    <img src="src/examples/1_shapes3D_c.png" alt="1_app_code" width="400">
  </p>
* Overridable window defaults: Initial dimensions and title.<br>
  <p float="left">
    <img src="src/examples/2_defaults.gif" alt="2_defaults" height="300">
    <img src="src/examples/2_defaults_c.png" alt="2_defaults_code" width="400">
  </p>
* Timers and window-title control:<br>
  <p float="left">
    <img src="src/examples/3_time.gif" alt="3_time" height="300">
    <img src="src/examples/3_time_c.png" alt="3_time_code" width="400">
  </p>
* Drawable window content: Basic shape drawing (Lines, rectangles, triangles and circles)<br>
  <p float="left">
    <img src="src/examples/4_shapes.gif" alt="4_shapes" height="300">
    <img src="src/examples/4_shapes_c.png" alt="4_shapes_code" width="400">
  </p>
* Text: String and number drawing:<br>
  <p float="left">
    <img src="src/examples/5_text.gif" alt="5_text" height="300">
    <img src="src/examples/5_text_c.png" alt="5_text_code" width="400">
  </p>
* Mouse: Buttons (+wheel), cursor position/movement and 'raw' mouse input (captured outside the window).<br>
  <p float="left">
    <img src="src/examples/6_mouse.gif" alt="6_mouse" height="300">
    <img src="src/examples/6_mouse_c.png" alt="6_mouse_code" width="400">
  </p>
* Keyboard: General key tracking + built-in tracking of control keys status (Ctrl, Alt, Shift, Space)<br>
  <p float="left">
    <img src="src/examples/7_keyboard.gif" alt="7_keyboard" height="300">
    <img src="src/examples/7_keyboard_c.png" alt="7_keyboard_code" width="400">
  </p>
* Memory: Arena allocator can be used for heap memory, then bound to the global app instance.<br>
  <p float="left">
    <img src="src/examples/8_memory.gif" alt="8_memory" height="300">
    <img src="src/examples/8_memory_c.png" alt="8_memory_code" width="400">
  </p>
* File: Opening/closing and reading/writing files in a platform-agnostic way (using os-specific API).<br>
  <p float="left">
    <img src="src/examples/9_files_io.gif" alt="9_files" height="300">
    <img src="src/examples/9_files_c.png" alt="9_files_code" width="400">
  </p>

<i>*All input capturing for mouse and keyboard is extendable through callbacks</i><br>