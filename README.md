# CSCE 645 / VIZA 675: Geometric Modeling

## Assignment 2: Diffusion Curves [100 pts]

## Due 3/21

### Dependencies:

- [SFML](http://www.sfml-dev.org/)
- [OpenGL](http://www.opengl.org/)
- [libSOIL](http://www.lonesock.net/soil.html)

### Building

*NOTE: Only tested on Ubuntu 12.10, and Crunchbang 11. All libraries are multiplatform, so
there should be no issues, but I cannot guarantee it.*

~~~
make
~~~

### Running

~~~
./DiffusionCurves
~~~

### How to Use

- **Left Mouse Click:**
  - *In normal mode:* adds a point to the point buffer at the mouse location; prompts the user to enter the left and right color for the newly placed point
  - *In edit mode:* moves a selected point to current mouse location
- **Right Mouse Click:**
  - *In normal mode:* creates a new Bezier curve from the current points in the point buffer, then clears the point buffer.
  - *In edit mode:* click and drag to select a point to manipulate (*NOTE: must be dragging from top-left to bottom-right*)

- **Escape:** closes the application
- **C:** clears the canvas
- **E:** toggles edit mode
- **F:** finalized the canvas, sending to the be processed into diffusion curves
- **Q:** *In edit mode:* prompts the user to pick new colors for the selected point
- **V:** toggles control point visibility

#### Setting colors

When prompted *(by creating a new point or pressing Q when a point is currently selected)*, the color values follow the OpenGL standard for float-based RGB values. Each value must be between 0.0f and 1.0f.

### Purpose:

This assignment will introduce you to a simple form of barycentric coordinates, a hierarchical solver, and demonstrate how other types of data can be used with smooth curves.

### Description

In this assignment you will implement a diffusion curve editor and renderer. The lecture on diffusion curves has a lot of detail about the suggested method for rendering diffusion curves. I suggest you review that lecture for details about the hierarchical solver.

In terms of the interface, you should allow the user to select the color of both the left and right sides of the curve on a per control point level. You should allow the user to select from at least seven different colors (the binary expansion represented as RGB for 1-7 is acceptable). For the curves, you should use Bezier curves. You should allow the user to input multiple Bezier curves (perhaps starting a new curve when the user hits a key such as 'b'). You should also support Bezier curves up to degree 6. Furthermore, the user should be able to drag existing control points around and change their left/right colors. To facilitate this interaction, you may add an editing mode that does not insert new curves/control points, but you should be able to transition between editing control points and adding new control points.

To render the curves, you should draw the curves as a set of quadrilateral polygons about 6 pixels wide. Vertices should be placed at evaluation points along the curve and offset in the normal direction of curve by 3 pixels in both the left and right directions. For information about how to compute normals, you may use the Frenet frame lecture or, because the curve is only in 2D, simply take the tangent vector and rotate it by 90 degrees to . OpenGL allows you to specify color on a per vertex basis, and you should do so setting the color of the vertices on the left part of the curve to the left color and the vertices on the right part of the curve to the right color.

You should also allow the user to transition to a rendering mode to draw the image defined by the diffusion curves (for example, hitting the 'd' key). In this case you should render the curve without drawing any control points, read the image from the framebuffer, and use non-black pixels as constraints. MAKE SURE YOU TURN OFF ANTI-ALIASING IN YOUR GRAPHICS CARD! Now solve for a harmonic function over the black pixels that satisfies those constraints. I strongly suggest you implement the hierarchical solver discussed in class. Note that, depending on your CPU and number of iterations you chose, it may take a few seconds to render the image. You may write your own code, but I have provided code that will use OpenGL to pull the current image from the framebuffer. This code also contains code to save a BMP file of an image, which you will likely find invaluable when debugging your solver.

Note that implementing the assignment as specified will only provide 90 out of 100 points. To receive full credit, you must go above and beyond the basic requirements. You might add a progressive display that shows the current highest resolution image in the hierarchical computation while computing the next highest resolution to provide immediate feedback from a slow solver. You could incorporate different types of curves as modeling primitives. You could add code to create a more full-function curve editor. You might use your program to create your own piece of art (hopefully with save/load features). These are only some suggestions, and points will be given based on difficulty/creativity.

### Grading

- [5] Ability to add control points
- [5] Ability to select left/right colors of control points
- [5] Ability to insert multiple curves
- [5] Ability to edit positions and colors of control points
- [20] Correct rendering of curves as thick quads with correct left/right colors
- [50] Correct rendering of diffusion curve image
- [10] Extensions to basic requirements
