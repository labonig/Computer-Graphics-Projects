/****************************
Author: Laboni Goswami
Email: labonig1@umbc.edu
Class: CMSC 435, FALL 2025
Section: 01
Date: 9/12/25
Description: This is the driver for project 1.
*****************************/

#define _USE_MATH_DEFINES

#include <iostream>
#include <Eigen/Dense>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include "Entities.cpp"

using namespace std;
using Eigen::Vector3d;


//Function Prototypes
bool Parse(string filename, Viewpoint& v, Vector3d& b, vector<Light*>& l, vector<ObjFill*>& f,
	   vector<Shape*>& Shapes);
void SaveToPPM(string filename, vector<Vector3d*>& image, int resx, int resy);


//Function Definitions
int main(int argc, char**argv){
  const int EXPECTED_ARGS = 3;
  string inputFilename, outputFilename;

  vector<Vector3d*> image;
  Vector3d bgColor;

  Viewpoint view;
  vector<Light*> lights;
  vector<ObjFill*> fills;
  vector<Shape*> shapes;

  int resx, resy;
  Vector3d u,v,w; //dimensions for an orthonormal basis in R3
  double d, h, angle;
  Vector3d cameraPt; //Stores a point expressed in camera coordinates
  Vector3d DirectionOfPixel; //the vector from the eye to a pixel in camera coordinates
  Vector3d *pixelColor; //Stores the rgb values for a single pixel.
                        //Each element is in the interval [0, 255]
  Ray ray;
  HitRecord hr;
  
  //Exit gracefully if the program is run with the wrong number of arguments
  if(argc != EXPECTED_ARGS){
    cout << "Error: invalid argument(s)" << endl;
    cout << "Expected usage: ./hide <input.nff> <output.ppm>" << endl;
    return 0;
  }

  inputFilename = argv[1];
  outputFilename = argv[2];

  //Exit gracefully if the file extensions are incorrect
  if(inputFilename.substr(inputFilename.size()-4) != ".nff" ||
     outputFilename.substr(outputFilename.size()-4) != ".ppm"){
    cout << "Error: invalid argument(s)" << endl;
    cout << "Expected usage: ./hide <input.nff> <output.ppm>" << endl;
    return 0;
  }
  
  if(!Parse(inputFilename, view, bgColor, lights, fills, shapes)){
    //Exit gracefully if <input.nff> does not open
    return 0;
  }
  
  //If parsing succeeds:
  //set up the image dimensions
  resx = view.resolution[0];
  resy = view.resolution[1];
  angle = view.angle_in_rads;

  //construct an orthonormal basis
  w = (view.from - view.at).normalized();
  u = view.up.cross(w).normalized();
  v = w.cross(u).normalized();
  
  //calculate d: the length of the ray that goes from the eye to the center of the image
  //calculate h: the length per pixel across the the u axis
  d = (view.from - view.at).norm();
  h = 2 * d * tan(angle/2) / resx;

  //The rays we fire always start at "from"
  ray.first = view.from;
  
  //traverse the image row by row
  for(int i = 0; i < resy; i++){
    for(int j = 0; j < resx; j++){
      //First set the pixel to the background color
      //This will be changed later if we find an intersection
      pixelColor = new Vector3d;
      *pixelColor = 255 * bgColor;

      //Calculate the camera coordinates which describe this pixel
      cameraPt[0] = -d * tan(angle/2) + h * (j + 1/2);
      cameraPt[1] = d * tan(angle/2) * (resy/resx) - h * (i + 1/2);
      cameraPt[2] = -d;

      //Calculate the vector from -> cameraPt
      DirectionOfPixel = cameraPt[0] * u + cameraPt[1] * v + cameraPt[2] * w;
           
      ray.second = DirectionOfPixel;

      //loop through all the shapes in the image to see if our viewing ray
      //intersects with any of them. If so, color in the corresponding
      //pixel accordingly.
      for(Shape* currShape : shapes){
	if(currShape->intersect(ray, hr)){
	  *pixelColor = 255 * (fills.at(currShape->fill_index)->rgb);
	}
      }
            
      //Add this pixel to the image
      image.push_back(pixelColor);
    }
  }
  
  //Create a ppm image
  SaveToPPM(outputFilename, image, resx, resy);  
  
  //free allocated memory
  for(unsigned int i = 0; i < image.size(); i++){
    delete image.at(i);
  }
  for(unsigned int i = 0; i < lights.size(); i++){
    delete lights.at(i);
  }
  for(unsigned int i = 0; i < fills.size(); i++){
    delete fills.at(i);
  }
  for(unsigned int i = 0; i < shapes.size(); i++){
    delete shapes.at(i);
  }
  return 0;
}


// Parse()
// Preconditions: Param "filename" is a .nff file.
// Postconditions: Returns false if the file did not open, returns true otherwise.
// Description: 
bool Parse(string filename, Viewpoint& v, Vector3d& b, vector<Light*>& l, vector<ObjFill*>& f,
	   vector<Shape*>& Shapes){
  ifstream input(filename);
  string entity;
  int curr_fill_index = -1;
  
  //Error check: file exists
  if(!input.is_open()){
    cout << "Error: Could not open " << filename << "." << endl;
    return false;
  }

  //Begin reading file. Store all entities into their corresponding structs
  while(input >> entity){
    //Possible Entities:
    //v = viewpoint location
    if(entity == "v"){
      string discard;
      
      input >> discard >> v.from[0] >> v.from[1] >> v.from[2];
      input >> discard >> v.at[0] >> v.at[1] >> v.at[2];
      input >> discard >> v.up[0] >> v.up[1] >> v.up[2];
      input >> discard >> v.angle_in_degrees;
      input >> discard >> v.hither;
      input >> discard >> v.resolution[0] >> v.resolution[1];
      
      v.angle_in_rads = v.angle_in_degrees / 180 * M_PI;
    }
    //b = background color
    if(entity == "b"){
      input >> b[0] >> b[1] >> b[2];
    }
    //l = positional light location
    if(entity == "l"){
      Light *newLight = new Light;
      input >> newLight->position[0] >> newLight->position[1] >> newLight->position[2];
      l.push_back(newLight);
    }
    //f = object material properties
    if(entity == "f"){
      ObjFill *newFill = new ObjFill;
      input >> newFill->rgb[0] >> newFill->rgb[1] >> newFill->rgb[2]
	    >> newFill->Kd >> newFill->Ks
	    >> newFill->Shine >> newFill->T >> newFill->ior;
      f.push_back(newFill);
      curr_fill_index++;
    }
    //c = cone or cylinder primitive
    if(entity == "c"){
      Cone *newCone = new Cone();
      input >> newCone->base[0] >> newCone->base[1] >> newCone->base[2] >> newCone->base_radius
	    >> newCone->apex[0] >> newCone->apex[1] >> newCone->apex[2] >> newCone->apex_radius;
      newCone->fill_index = curr_fill_index;
      Shapes.push_back(newCone);
    }
    //s = sphere primitive
    if(entity == "s"){
      Sphere *newSphere = new Sphere();
      input >> newSphere->center[0] >> newSphere->center[1] >> newSphere->center[2]
	    >> newSphere->radius;
      newSphere->fill_index = curr_fill_index;
      Shapes.push_back(newSphere);
    }
    //p = polygon primitive
    if(entity == "p"){
      Polygon *newPoly = new Polygon();
      input >> newPoly->total_vertices;

      //Dynamically allocate each vertex, and store them in the "vertices" vector.
      Vector3d* newVertex;
      for(int i = 0; i < newPoly->total_vertices; i++){
	newVertex = new Vector3d;
      	input >> (*newVertex)[0] >> (*newVertex)[1] >> (*newVertex)[2];	  
	newPoly->vertices.push_back(newVertex);
      }
      newPoly->fill_index = curr_fill_index;
      Shapes.push_back(newPoly);
    }
    //pp = polygonal patch primitive
    if(entity == "pp"){
      PolygonalPatch *newPP = new PolygonalPatch();
      input >> newPP->total_vertices;

      //Dynamically allocate each vertex and norm.
      //Store them in the "vertices" vector and "norms" vector.
      Vector3d* newVertex;
      Vector3d* newNorm;
      for(int i = 0; i < newPP->total_vertices; i++){
	newVertex = new Vector3d;
	newNorm = new Vector3d;
	input >> (*newVertex)[0] >> (*newVertex)[1] >> (*newVertex)[2]
	      >> (*newNorm)[0] >> (*newNorm)[1] >> (*newNorm)[2];
	newPP->vertices.push_back(newVertex);
	newPP->norms.push_back(newNorm);
      }
      newPP->fill_index = curr_fill_index;
      Shapes.push_back(newPP);
    }
  }//end while

  input.close();
  
  return true;
}

/*
  SaveToPPM
  Preconditions: The image vector has been filled with resx * resy Vector3ds representing
                 pixel colors. The filename is a .ppm file.
  Postconditions: A new .ppm file named filename is created. It accurately depicts the scene
                  described in the input nff file.
  Description: This function creates and writes to an output ppm file, formatting the file
               as a P6 ppm.
*/
void SaveToPPM(string filename, vector<Vector3d*>& image, int resx, int resy){
  const int MAX_COLOR_VAL = 255;
  ofstream out(filename);

  if(!out.is_open()){
    cout << "Error: Failed to open " << filename << "." << endl;
    return;
  }

  out << "P6" << endl
      << resx << endl
      << resy << endl
      << MAX_COLOR_VAL << endl;
  
  for(int i = 0; i < resx*resy; i++){
    for(int j = 0; j < 3; j++){
      out << char((*image.at(i))[j]);
    }
  }

  out.close();
}

