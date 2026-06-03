Name: Laboni Goswami
Email: laboni.goswami@hotmail.com


References / Help Received
----------------------------
- https://www.fileformat.info/format/nff/egff.htm
- https://netpbm.sourceforge.net/doc/ppm.html
- https://community.lambdatest.com/t/how-to-get-pi-value-in-c-without-defining-it-manually/30445
- https://en.cppreference.com/w/cpp/types/numeric_limits/infinity.html


Description
--------------
This is a C++ program for rendering triangles using ray tracing and the barycentric coordinate intersection method.
The program reads a .nff file, specified on the command line, which describes a single image and produces a .ppm file.
A single color is defined for rays that hit an object, and a background color is defined for rays that don't.
Advanced features like shadows, reflection, refraction, etc. are not included in this project.


Directory Structure
---------------------
proj1
|---- hide.cpp
|---- tetra-3.nff
|---- balls.nff
|---- gears.nff
|---- Makefile


Instructions
--------------
This project makes use of the Eigen library for linear algebra. First, download the source code:
https://libeigen.gitlab.io/eigen/docs-5.0/

The driver is hide.cpp. To compile the program, use the command "make hide" or just "make."
To run the program, use the command "./hide input.nff output.ppm" where input.nff is one of the three .nff files specified above, and
output.ppm is a .ppm file of your choosing. The Makefile also provides 3 commands:
1. make tetra = ./hide tetra-3.nff tetra-3.ppm
2. make gears = ./hide gears.nff gears.ppm
3. make balls = ./hide balls.nff balls.ppm
Note, not all of these commands will produce the correct output; see the Extra Credit section.


Extra Credit Implemented
--------------------------
This program works with spheres. The "make balls" command will produce the correct output.


Extra Credit Unimplemented
----------------------------
This program does not handle:
- Concave polygons. The "make gears" command will not produce the correct output.
