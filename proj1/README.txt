Name: Laboni Goswami
Email: labonig1@umbc.edu


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


Instructions
--------------
In this directory, there should be files named "hide.cpp", "tetra-3.nff", "gears.nff", "balls.nff", and a Makefile.
The driver is hide.cpp. To compile the program, use the command "make hide" or just "make."
To run the program, use the command "./hide input.nff output.ppm" where input.nff one of the three .nff files specified above, and
output.ppm is a .ppm file of your choosing. The Makefile also provides 3 commands:
1. make tetra = ./hide tetra-3.nff tetra-3.ppm
2. make gears = ./hide gears.nff gears.ppm
3. make balls = ./hide balls.nff balls.ppm
Note, even though all three commands are given, not all of them will produce the correct output. The Extra Credit section will clarify.


Extra Credit Implemented
--------------------------
This program works with spheres. The "make balls" command will produce the correct output.


Extra Credit Unimplemented
----------------------------
handle concave polygons. Splitting polygons into a fan of triangles only works for convex polygons. Instead, use the test-ray method to handle arbitrary concave polygons. Try the "gears" SPD program to generate a scene with some pretty complex polygons.
Key words to search: "Ear clipping"

