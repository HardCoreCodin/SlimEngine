<img src="SlimEngine_logo.png" alt="SlimEngine_logo"><br>

Minimalist base-project for interactive graphical applications (2D/3D)<br>
Written in plain C (compatible with C++)<br>
Pure-software (no hardware acceleration of any kind)<br>
Focuses on simplicity, ease of use and brain-dead setup (single header file, no dependencies)<br>
<br>
Built on: [SlimApp](https://github.com/HardCoreCodin/SlimApp) <br>
Used in: [SlimTracin](https://github.com/HardCoreCodin/SlimTracin) and [SlimRaster](https://github.com/HardCoreCodin/SlimRaster) <br>

Architecture:
-
SlimEngine is platform-agnostic by design, though currently only supports Windows.<br>
The platform layer only uses operating-system headers - no standard library used.<br>
The application layer itself has no 3rd-party dependencies - only uses standard math headers.<br>
It is just a library that the platform layer uses - it has no knowledge of the platform.<br>

More details on this architecture [here](https://youtu.be/Ev_TeQmus68).

Usage:
-
The single header file variant includes everything.<br>
Otherwise, specific headers can be included from the directory of headers.<br>
The main entry point for the app needs to be defined explicitly (see [SlimApp](https://github.com/HardCoreCodin/SlimApp)). <br>

SlimEngine comes with pre-configured CMake targets for all examples.<br>
For manual builds on Windows, the typical system libraries need to be linked<br>
(winmm.lib, gdi32.lib, shell32.lib, user32.lib) and the SUBSYSTEM needs to be set to WINDOWS<br>

All examples were tested in all combinations of:<br>
Source: Single header file (SlimEngine.h), Directory of headers (specific includes) <br>
Compiler: MSVC, MinGW, CLang<br>
Language: C, C++<br>
CPU Arch: x86, x64<br>

Features:
-

SlimEngine extends SlimApp and so inherits all it's features<br>
Additional features include facilities for interactive 3D applications:<br>
* A scene with cameras, meshes and parametric curves<br>
* Scene selection and interactive transformations (moving, rotating and scaling)<br>
* 3D Viewport with a HUD and rich mouse/keyboard navigation<br>
* 3D Line drawing for wireframe rendering (optionally multi-sampled for very clean lines)<br>
<br>
<img src="src/examples/MSAA.gif" alt="MSAA" height="360"><br>

Well documented example applications cover the features:<br>
* <b><u>3D Viewports:</b></u><br>
  <img src="src/examples/1_viewport_HUD.gif" alt="1_viewport_HUD" height="360"><br>
  Viewports come with a HUD that can be toggled on/off<br>
  SlimEngine comes with an empty scene, apart from a default camera bound to a default viewport.<br>
  The window content is bound to the default viewport, so they resize together.<br>
  Customizing the scene/viewport can be done in callbacks that get invoked when they're ready.<br>
  The HUD is fully customizable, and can hold any number of lines set at viewport-initialization time.<br>
  <p float="left">
    <img src="src/examples/1_viewport_updateAndRender_c.png" alt="1_viewport_updateAndRender_code" width="360">
    <img src="src/examples/1_viewport_initialization_c.png" alt="1_viewport_initialization_code" height="300">
    <img src="src/examples/1_viewport_HUD_update_c.png" alt="1_viewport_HUD_update_code" width="360">
    <img src="src/examples/1_viewport_setup_c.png" alt="1_viewport_setup_code" height="300">  
  </p>
  
* <b><u>Viewport</b>:</u> Navigation<br>
  <img src="src/examples/2_navigation.gif" alt="1_scene" height="360"><br>
  Viewport navigation actions can be composed to set-up WASD(FPS)/DCC(orb) navigation style(s).<br>
  Keyboard navigation is agnostic to key-binding and is trivial to map keys to.<br>
  It supports moving forward, backwards, left, right, up and down and turning left or right.<br>
  Mouse navigation controls the viewport's camera and is more granular.<br>
  It can pan, zoom, dolly, orient or orbit the camera around a target.<br>
  <p float="left">
    <img src="src/examples/2_navigation_setup_c.png" alt="2_navigation_setup_code" width="350">
    <img src="src/examples/2_navigation_mouse_c.png" alt="2_navigation_mouse_code" height="300">
  </p>

* <b><u>Cameras</b>:</u> Viewport binding<br>
  <img src="src/examples/3_cameras.gif" alt="1_scene" height="360"><br>
  The scene can have any number of cameras which can be bound to any viewport dynamically.<br>
  The camera can be drawn as part of the drawing of the scene<br>
  <p float="left">
    <img src="src/examples/3_cameras_update_c.png" alt="3_cameras_update_code" height="260">
    <img src="src/examples/3_cameras_setup_c.png" alt="3_cameras_setup_code" height="260">
  </p>

* <b><u>Shapes</b>:</u> Setup and drawing parametric primitives (Grid, Box, Curves, etc.)<br>
  <img src="src/examples/4_shapes_rotating.gif" alt="1_scene" height="360"><br>
  SlimEngine allocates and initializes the scene based on quantities set at app-initialization time.<br>
  Wireframe drawing of the scene to the viewport can be set-up to occur on window-redraw.<br>  
  <p float="left">
    <img src="src/examples/4_shapes_initialization_c.png" alt="4_scene_initialization_and_viewport_drawing_code" width="350">
    <img src="src/examples/4_shapes_viewport_drawing_c.png" alt="4_scene_viewport_drawing_code" height="300">
  </p>

* <b><u>Scene</b>:</u> Object selection and manipulation (Translation, Rotation and Scale)<br>
  <img src="src/examples/5_manipulation.gif" alt="5_manipulation" height="360"><br>
  Scene objects can be dragged around the screen, moving them parallel to the viewing plane.<br>
  Selecting an object displays it's bouding box, enabling for per-axis translation, rotation and scaling.<br><br>
  <p float="left">
    <img src="src/examples/5_manipulation_update_c.png" alt="4_manipulation_update_code" width="350">
    <img src="src/examples/5_manipulation_scene_setup_c.png" alt="4_manipulation_scene_setup_code" width="350">
  </p>
  
* <b><u>Scene</b>:</u> Mesh loading from file, instancing and wire-frame drawing<br>
  <img src="src/examples/6_mesh_with_normals.gif" alt="6_mesh_with_normals" height="360"><br>
  Meshes can be loaded from binary files, with vertex positions, normals and texture coordinates.<br>
  They can be moves and transformed just like any other kind of primitive.<br>
  <img src="src/examples/6_mesh_instancing.gif" alt="6_mesh_instancing" height="360"><br>
  A mesh can have multiple instances, each with their own transformation and colors.<br>
  Simply have multiple mesh primitives all set with same mesh id.<br>
  Memory is allocated automatically for the meshes by reading their headers before loading.<br>
  <p float="left">
    <img src="src/examples/6_mesh_drawing_c.png" alt="6_mesh_drawing_code" width="350">
    <img src="src/examples/6_mesh_setup_c.png" alt="6_mesh_setup_code" width="350">
  </p>

* <b><u>Scene</b>:</u> Saving to and loading from `.scene` files<br>
  <img src="src/examples/7_scene.gif" alt="7_scene" height="360"><br>
  Scenes can be saved to a file and later loaded back in-place.
  <p float="left">
    <img src="src/examples/7_scene_setup_c.png" alt="7_scene_io_code" width="350">
    <img src="src/examples/7_scene_message_c.png" alt="7_scene_message_code" width="350">
  </p>

* <b><u>obj2mesh</b>:</u> Also privided is a separate CLI tool for converting `.obj` files to `.mesh` files.<br>
  It is also written in plain C (so is compatible with C++)<br>
  Usage: `./obj2mesh src.obj trg.mesh`<br>
  - invert_winding_order : Reverses the vertex ordering (for objs exported with clockwise order)<br>

<b>SlimEngine</b> does not come with any GUI functionality at this point.<br>
Some example apps have an optional HUD (heads up display) that shows additional information.<br>
It can be toggled on or off using the`tab` key.<br>

All examples are interactive using <b>SlimEngine</b>'s facilities having 2 interaction modes:
1. FPS navigation (WASD + mouse look + zooming)<br>
2. DCC application (default)<br>

Double clicking the `left mouse button` anywhere within the window toggles between these 2 modes.<btr>

Entering FPS mode captures the mouse movement for the window and hides the cursor.<br>
Navigation is then as in a typical first-person game (plus lateral movement and zooming):<br>

Move the `mouse` to freely look around (even if the cursor would leave the window border)<br>
Scroll the `mouse wheel` to zoom in and out (changes the field of view of the perspective)<br>
Hold `W` to move forward<br>
Hold `S` to move backward<br>
Hold `A` to move left<br>
Hold `D` to move right<br>
Hold `R` to move up<br>
Hold `F` to move down<br>

Exit this mode by double clicking the `left mouse button`.

The default interaction mode is similar to a typical DCC application (i.e: Maya):<br>
The mouse is not captured to the window and the cursor is visible.<br>
Holding the `right mouse button` and dragging the mouse orbits the camera around a target.<br>
Holding the `middle mouse button` and dragging the mouse pans the camera (left, right, up and down).<br>
Scrolling the `mouse wheel` dollys the camera forward and backward.<br>

Clicking the `left mouse button` selects an object in the scene that is under the cursor.<br>
Holding the `left mouse button` while hovering an object and then dragging the mouse,<br>
moves the object parallel to the screen.<br>

Holding `alt` highlights the currently selecte object by drawing a bounding box around it.<br>
While `alt` is still held, if the cursor hovers the selected object's bounding box,<br>
mouse interaction transforms the object along the plane of the bounding box that the cursor hovers on:<br>
Holding the `left mouse button` and dragging the mouse moves the object.<br>
Holding the `right mouse button` and dragging the mouse rotates the object.<br>
Holding the `middle mouse button` and dragging the mouse scales the object.<br>
<i>(`mouse wheel` interaction is disabled while `alt` is held)</i><br>


Perspective Projection Visualization :
======================================

In addition to examples of specific features, a more real-world application is also included here:<br>
It is the full source code used for all interactive visualizations in a [video about Perspective Projection](https://youtu.be/hdv_pnMVaVE) <br>
The video was a submission to 3Blue1Brown's [Summer of Math Explainers (a.k.a: SoME1)](https://www.3blue1brown.com/blog/some1) competition.<br>
It covers how perspective projection works when using graphics hardware, in its native 4D projective space.<br>

Since it's using SlimEngine it has no dependencies and is structures as a single compilation unit (Unity Build).<br>
It can therefore be easilly compiled with any C/C++ compiler by just compiling the 'perspective_projection.c' file.<br>
As with the other examples, a CMake target is already set up for it here.<br>

There are 5 separate visualizations:<br>
* Intro : A rotating cube being projected onto a projection plane.<br>
* Projective Space : A projective space centered on the secondary camera.<br>
* Space Warping : The volumetric warping of space and the transformations of the frustum-bounds.<br>
* View Frustum : Interactive view frustum with clipping planes and an NDC box centered on the secondary camera.<br> 
* Projection Matrix : The main section of the video deriving the transformation matrices geometrically.<br>

In all visualizations:<br>
- Camera controls are exactly as detailed above for the rest of the examples.<br>
- The `space` key cycles to the next visualization.<br>
- The `R` key toggles on/off a slow orbiting of the main camera about its target position.<br> 
- The `Tab` key toggles on/off a secondary viewport showing what the secondary camera sees, except for the last visualization where it toggles the Matrix HUD instead.<br>

Intro:
------

<img src="src/examples/visualizations/intro.gif" alt="intro" height="360"><br>
<br>

Projective Space:
-----------------

<img src="src/examples/visualizations/projective_space.gif" alt="projective_space" height="360"><br>

- The `3` key (re)starts the transition of 'lines through the origin' emanating from the secondary camera's origin.<br>
<br>


Space Warping:
--------------

<img src="src/examples/visualizations/space_warping.gif" alt="space_warping" height="360"><br>

- The `1` key transitions the frustum from view-space directly to NDC-space (box centered at the camera).<br>
- The `2` key transitions the frustum from view-space to clip-space and then to NDC-space (only works in 2-step mode).<br>
- The `3` toggles on/off the 2-step mode and the display of the frustum of the intermediary clip-space.<br>
- The `4` toggles on/off the volumetric visualization of space-warping transformation (using a 3D grid of locators).<br>
<br>


View Frustum:
-------------

<img src="src/examples/visualizations/view_frustum.gif" alt="view_frustum" height="360"><br>

- The `1` key transitions the frustum from view-space to NDC-space taking the clipped geometry with it.<br>
- Holding `ctrl` the mouse wheel moved the near clipping plane back and forth.<br>
- Holding `shift` the mouse wheel moved the far clipping plane back and forth.<br>
- Holding `alt` the mouse wheel zooms the camera in/out by increasing/decreasing the focal length.<br>
- Clicking anywhere within the bounds of the secondary viewport (when it's visible, toggled by `Tab`)<br>
  make the secondary viewport 'active' so that all camera controls affect the secondary camera.<br>
  Clicking anywhere outside the bounds makes the primary camera active again.
<br>


Projection Matrix:
------------------

<img src="src/examples/visualizations/projection_matrix.gif" alt="projection_matrix" height="360"><br>

- Holding `ctrl` the mouse wheel moved the near clipping plane back and forth.<br>
- Holding `shift` the mouse wheel moved the far clipping plane back and forth.<br>
- Holding `alt` the mouse wheel zooms the camera in/out by increasing/decreasing the focal length.<br>
- The X, Y/W and Z basis vectors can be manipulated (by moving their arrow-heads) thereby transforming the space.<br>
  This also correspondingly updates the current manual matrix (top-left corner of the matrix HUD, if visible).<br>
  Just click-dragging them moves them parallel to the view, but:
  holding `ctrl` constrains the movement to only be along the Z axis<br>
  holding `shift` constrains the movement to only be along the Y (or W) axis, and<br>
  holding `shift+ctrl` constrains the movement to only be along the X axis, and<br>
- The `T` key shows the transition of the next transformation step and adds its matrix to the HUD.<br>
  `shift+T` <i>adds</i> the matrix of the current 'manual' transformation instead.<br>
  `ctrl+T` <i>removed</i> the last-added transformation matrix.<br> 
- The `O` key toggles between working with an NDC space of a full-cube (OpenGL/Vulkan) of a half-cube (Direct3D).<br>
- The `V` key toggles the visibility of the geometry (yellow box and white grid).<br>
- The `L` key toggles the visibility of the NDC box's vertex coordinates (labels).<br>
- The `Z` key toggles the visibility of the auxiliary plane that controls the view-frustum's shape.<br>
- The `F` key toggles the visibility of the focal-length's ratio (labels).<br>
- The `A` key toggles the visibility of the aspect ratio's similar triangles (labels).<br>
- The `X` key toggles the visibility of the diagonals of the perfectly-diagonal pyramid.<br>
- The `S` key toggles the visibility of the arrows showing the X and Y scaling factors (labels).<br>
- The `G` key toggles the visibility of grids representing the XY and ZY planes.<br>
- The `1` key transitions the frustum from view-space to NDC-space taking the clipped geometry with it.<br>
- The `2` key slices the view of 4D space to XZW thereafter.<br>
- The `3` key reveals the projective space (centered at the origin).<br>
- The `4` key reveals a projective point (a line through the origin).<br>
- The `5` key reveals the projective reference plane at a W height of 1.<br>
- The `4` key (second time) moves a projective point (a line through the origin) along the ref. plane.<br>
- The `5` key (second time) shows a projective normalization, pushing a projective point down to the ref. plane.<br>
- The `6` key reveals the projective points (lines through the origin) of the NDC rectangular slice's corners.<br>
- The `7` key reveals the projective trajectories possible for the corners of the view frustum's slice's corners.<br>
- The `8` key reveals the wobbling quadrangle formed by the target corner-positions on the projective points.<br>
- The `9` key pauses the wobbling and sliding of the target projective points along their lines through the origin.<br>
- The `0` key drops the wobbled quadrangle by projectively normalizing the corners down to the ref. plane.<br>
- The `D` key reveals vectors showing the diagonal of the lines through the origin and the NDC corners.<br>
- The `C` key reveals the chosen trajectory for the view frustum's corners onto the NDC's projective points.<br>
- The `B` key reveals the labels of the chosen trajectory, showing the measures of transformations needed.<br>
- The `H` key reveals the top-view's labels for the Z-scaling (after the shear). <br>
- The `K` key toggles the 'alternative' Z-scale when in cube-NDC mode (OpenGL). <br>
- The `J` key toggles the Z-flip mode (OpenGL).<br>
- The `M` key toggles the 'final' matrix in the top-left corner of the matrix HUD (instead of the custum one).<br>
<br>