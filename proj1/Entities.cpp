/****************************
Author: Laboni Goswami
Email: labonig1@umbc.edu
Class: CMSC 435, FALL 2025
Section: 01
Date: 9/12/25
Description: This file contains structs for all nff entities. Additionally, it
  defines a Ray and a HitRecord. The nff entities cone, sphere, polygon, and
  polygonal patch are all derived from a more general "Shape" struct, and each
  has it's own "intersect" function. 
*****************************/

#define _USE_MATH_DEFINES

#include <iostream>
#include <Eigen/Dense>
#include <string>
#include <vector>
#include <cmath>
#include <limits>

using namespace std;
using Eigen::Vector3d;

//forward declarations
struct HitRecord;
struct Shape;

//A ray is a point and a direction, both of which are stored as Vector3d
typedef pair<Vector3d,Vector3d> Ray;

struct HitRecord{
  Shape* hitShape; //Pointer to the shape that was hit
  double t = numeric_limits<double>::infinity(); //point along the fired ray where the hit occurred
  Vector3d n; //surface normal at the hit point
};

//Struct Definitions
struct Viewpoint{
  Vector3d from; //eye location in XYZ
  Vector3d at;   //center of the image
  Vector3d up;
  double angle_in_degrees; //the nff file provides the angle in degrees
  double angle_in_rads;    //but it's useful to have it in radians
                           //(e.g. the tan function requires it)
  double hither;
  int resolution[2];
};

struct Light{
  double position[3];
  Vector3d rgb; //optional, irrelevant for proj 1
};

struct ObjFill{
  Vector3d rgb;   //Values are in the interval [0.0, 1.0]
  double Kd;      //Diffuse component
  double Ks;      //Specular component
  double Shine;   //Phong cosine power for highlights
  double T;       //T = transmittance
  double ior;     //ior = index_of_refraction
};

struct Shape{
  Shape(){}
  virtual bool intersect(Ray& r, HitRecord& hr) = 0;
  virtual ~Shape(){}
  string type;
  int fill_index; //index into a vector of ObjFills
};

struct Cone : Shape{
  //Technically, this is also for cylinders. Consider them a special cone.
  Cone() : Shape(){ type = "cone"; }
  ~Cone(){}

  Vector3d base;      //Point (XYZ) of the cone base
  double base_radius;
  Vector3d apex;      //Point (XYZ) of the cone apex
  double apex_radius; //if BOTH radii are negative, the inside is visible

  virtual bool intersect(Ray& r, HitRecord& hr){
    return false;
  }
};

struct Sphere : Shape{
  Sphere() : Shape(){ type = "sphere"; }
  ~Sphere(){}

  Vector3d center;
  double radius; //if the radius is negative, the inside is visible

  virtual bool intersect(Ray& r, HitRecord& hr){
    Vector3d e_sub_c = r.first - this->center;
    double d_dot_esc = r.second.dot(e_sub_c);
    double d_dot_d = r.second.dot(r.second);
    double discriminant = pow(d_dot_esc, 2) - (d_dot_d)*((e_sub_c).dot(e_sub_c) - pow(this->radius, 2));
    double solution;
    if(discriminant < 0){ return false; }
    if(discriminant == 0 || (discriminant > 0 && this->radius > 0) ){
      solution = (-1 * d_dot_esc - sqrt(discriminant))/(d_dot_d);
    }
    else{
      solution = (-1 * d_dot_esc + sqrt(discriminant))/(d_dot_d);
    }
    if(solution < hr.t){
      hr.hitShape = this;
      hr.t = solution;
    }
    
    return true;
  }
};

struct Polygon : Shape{
  Polygon() : Shape(){ type = "polygon"; }
  ~Polygon(){
    //Polygons need to delete vertices
    for(int j = 0; j < total_vertices; j++){      
      delete vertices.at(j);
    }
  }

  int total_vertices;
  vector<Vector3d*> vertices;
  
  virtual bool intersect(Ray& r, HitRecord& hr){
    bool foundHit = false;
    Vector3d triangle[3]; 
    Eigen::Matrix3d A;
    Vector3d solution; //gives us values for beta, gamma, and t where
                       // e + dt = a + beta(b-a) + gamma(c-a),
                       // 0 < beta, 0 < gamma, beta + gamma < 1

    //break up the polygon into a fan of triangles
    //e.g. for a polygon with 5 vertices can be broken into triangles
    //(v0, v1, v2), (v0, v2, v3), (v0, v3, v4)
    triangle[0] = (*(this->vertices.at(0)));
    
    for(int i = 1; i < this->total_vertices - 1; i++){
      triangle[1] = (*(this->vertices.at(i)));
      triangle[2] = (*(this->vertices.at(i+1)));
      
      //determine if the ray and triangle intersect
      A(0,0) = triangle[1][0] - triangle[0][0];
      A(1,0) = triangle[1][1] - triangle[0][1];
      A(2,0) = triangle[1][2] - triangle[0][2];
      
      A(0,1) = triangle[2][0] - triangle[0][0];
      A(1,1) = triangle[2][1] - triangle[0][1];
      A(2,1) = triangle[2][2] - triangle[0][2];
      
      A(0,2) = -((r.second)[0]);
      A(1,2) = -((r.second)[1]);
      A(2,2) = -((r.second)[2]);
	
      solution = A.inverse() * (r.first - triangle[0]);

      //If the ray and shape intersect, check if this intersection is closer
      //than the one currently recorded in the hit record. If it is, update
      //the hit record.
      if(solution[0] > 0 && solution[1] > 0 && solution[0] + solution[1] < 1){
      	foundHit = true;
	if(solution[2] < hr.t){
	    hr.hitShape = this;
	    hr.t = solution[2];
	}
      }
    } //end for
    //Return whether we found an intersection or not
    return foundHit;
  }
};

struct PolygonalPatch : Shape{
  PolygonalPatch() : Shape(){ type = "polygonal patch"; }
  ~PolygonalPatch(){
    //Polygonal Patches need to delete vertices and norms
    for(int j = 0; j < total_vertices; j++){
      delete vertices[j];
      delete norms[j];
    }
  }

  int total_vertices = 0;
  vector<Vector3d*> vertices;
  vector<Vector3d*> norms;

  virtual bool intersect(Ray& r, HitRecord& hr){
    return false;
  }
};

/*

bool Polygon::intersect(Ray& r, HitRecord& hr){
  bool foundHit = false;
  
  
  Vector3d triangle[3];
  double beta, gamma;
  Eigen::Matrix3d A;
  Vector3d solution; //gives us values for beta, gamma, and t where
  // e + dt = a + beta(b-a) + gamma(c-a),
  // 0 < beta, 0 < gamma, beta + gamma < 1
  //break up the polygon into a fan of triangles
  //e.g. for a polygon with 5 vertices can be broken into triangles
  //(v0, v1, v2), (v0, v2, v3), (v0, v3, v4)
  triangle[0] = (*(this->vertices.at(0)));

  for(int i = 1; i < this->total_vertices - 1; i++){
    triangle[1] = (*(this->vertices.at(i)));
    triangle[2] = (*(this->vertices.at(i+1)));

    //determine if the ray and triangle intersect
    A(0,0) = (*triangle[1])[0] - (*triangle[0])[0];
    A(1,0) = (*triangle[1])[1] - (*triangle[0])[1];
    A(2,0) = (*triangle[1])[2] - (*triangle[0])[2];

    A(0,1) = (*triangle[2])[0] - (*triangle[0])[0];
    A(1,1) = (*triangle[2])[1] - (*triangle[0])[1];
    A(2,1) = (*triangle[2])[2] - (*triangle[0])[2];

    A(0,2) = -((r.second)[0]);
    A(1,2) = -((r.second)[1]);
    A(2,2) = -((r.second)[2]);

    solution = A.inverse() * (r.first - (*triangle[0]));

    //If the ray and shape intersect, check if this intersection is closer
    //than the one currently recorded in the hit record. If it is, update
    //the hit record.
    if(beta > 0 && gamma > 0 && beta + gamma < 1){
      foundHit = true;
      if(solution[2] < hr.t){
	hr.hitShape = this;
	hr.t = solution[2];
      }
    }
  } //end for
  //Return whether we found an intersection or not

  
  return foundHit;
}
*/
