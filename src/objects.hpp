/*
Ce fichier contiendra les classes concernant les rayons et leur suivi
*/

#ifndef objectsHPP
#define objectsHPP

#include <cmath>
#include <stdexcept>
#include <iostream>
#include "scene.hpp"

struct Point_3D
{
    // Represents a point in 3D space
    float x;
    float y;
    float z;

    Point_3D(float x_coord, float y_coord, float z_coord) : x(x_coord), y(y_coord), z(z_coord) {}

};

struct Angles_Spherical; // We declare the class here to avoid circular dependencies


struct Vector_3D
{
    // Represents a vector in 3D space
    // Champs
    float x;
    float y;
    float z;

    // Constructeurs
        // à partir des coordonnées cartésiennes
    Vector_3D(float x_coord, float y_coord, float z_coord) : x(x_coord), y(y_coord), z(z_coord) {}
    
        // à partir de deux points Point_3D (P2-P1)
    Vector_3D(Point_3D P1, Point_3D P2) : x(P2.x-P1.x), y(P2.y-P1.y), z(P2.z-P1.z) {};

    // Méthodes
    float norm() const {
        // Returns the norm of the vector
        return std::sqrt(x * x + y * y + z * z);
    }

    float operator * (Vector_3D aRightVector){
        // Définition du produit scalaire
        Vector_3D aLeftVector = *this;
        return aRightVector.x*aLeftVector.x + aRightVector.y*aLeftVector.y + aRightVector.z*aLeftVector.z;
    }

    void print() const {
        std::cout << "Vector: (" << x << ", " << y << ", " << z << ")\n";
    }

    Angles_Spherical to_angles() const; // We declare the method here to avoid circular dependencies
};


struct Angles_Spherical
{
    // Represents the 2 angles between 2 vectors in 3D space using the spherical coordinates
    float theta; // Angle with the z axis
    float phi; // Angle with the x axis in the x-y plane

    Angles_Spherical(float t, float p) : theta(t), phi(p) {}

    void print() const {
        std::cout << "Angles: (" << theta << ", " << phi << ")\n";
    }

    Vector_3D to_vector() const; // We declare the method here to avoid circular dependencies
};

// Methods of Vector_3D and Angles_Spherical :

Vector_3D Angles_Spherical::to_vector() const {
    // Returns the vector corresponding to the angles in spherical coordinates
    // We don't specify r : the norm of the vector is 1
    float x = std::sin(theta) * std::cos(phi);
    float y = std::sin(theta) * std::sin(phi);
    float z = std::cos(theta);
    Vector_3D vector(x, y, z);
    return vector;
}

Angles_Spherical Vector_3D::to_angles() const {
    // Returns the angles of the vector in spherical coordinates
    auto r = norm();
    if (r == 0) {
        throw std::invalid_argument("The vector is null, it has no angles");
    }
    float theta = std::acos(z / r); // Angle with the z axis
    float phi = std::atan2(y, x); // Angle with the x axis in the x-y plane
    Angles_Spherical angles(theta, phi);
    return angles;
}



class Ray
{
private:
    Point_3D origin;
    Vector_3D direction;

public:
    // Constructors
    Ray(float xo, float yo, float zo, float xd, float yd, float zd) : origin(xo, yo, zo), direction(xd, yd, zd) {}
    Ray(Point_3D ori, Vector_3D dir) : origin(ori), direction(dir) {};

    // Getters
    Point_3D get_origin() const { return origin; }
    Vector_3D get_direction() const { return direction; }

};

class Camera
{
private:
    Point_3D position;
    Vector_3D direction;
    unsigned hfov; // Between 0 and 179, horizontal field of view in degrees
    unsigned resolution_height;
    unsigned resolution_width;

public:
    // Constructor :
    Camera(Point_3D pos = { 0, 0, 0 }, Vector_3D dir = { 0, 1, 0 }, unsigned f = 90, unsigned res_h = 400, unsigned res_w = 600) : position(pos), direction(dir), hfov(f), resolution_height(res_h), resolution_width(res_w) {
        if (hfov > 179) {
            throw std::invalid_argument("horizontal fov must be less than 180 degrees");
        }
        if (resolution_height == 0 || resolution_width == 0) {
            throw std::invalid_argument("resolution can not be 0");
        }
        if (hfov * resolution_height / resolution_width > 179) {
            throw std::invalid_argument("vertical fov must be less than 180 degrees");
        }
        if (direction.norm() == 0) {
            throw std::invalid_argument("The direction vector of the camera can not be null");
        }
    }

    // Destructor :
    ~Camera() {}; //default destructor

    // Getters :
    Point_3D get_position() const { return position; }
    Vector_3D get_direction() const { return direction; }
    unsigned get_hfov() const { return hfov; }
    unsigned get_resolution_height() const { return resolution_height; }
    unsigned get_resolution_width() const { return resolution_width; }


    Ray ray_launcher(unsigned pixel_height, unsigned pixel_width) {
        // Launches a Ray from the camera to the given pixel coordinates. Coordinates start at 0, end at resolution_height-1 and resolution_width-1
        // TODO : Check that the pixel coordinates are within the resolution, otherwise return an exception

        // The coordinates starts on the bottom left corner
        Point_3D origin = Camera::position;
        // camera::direction defines the center of the image, when pixel_height = resolution_height/2 and pixel_width = resolution_width/2
        // We consider that the camera is not rotated along the y axis (compared to the default viewing direction along the y axis)

        // We need to cast the unsigned to floats to avoid integer division
        float hfov_f = static_cast<float>(hfov);
        float resolution_height_f = static_cast<float>(resolution_height);
        float resolution_width_f = static_cast<float>(resolution_width);
        float vfov_f = hfov_f * resolution_height_f / resolution_width_f; // Vertical field of view in degrees

        float horizontal_angle = ((hfov_f / 2) * (pixel_width - resolution_width_f / 2 + 0.5) / (resolution_width / 2)) * M_PI / 180.0; // In radians. The second resolution_width/2 is integer division
        float vertical_angle = ((vfov_f / 2) * (pixel_height - resolution_height_f / 2 + 0.5) / (resolution_height / 2)) * M_PI / 180.0; // In radians

        auto camera_direction_angles = direction.to_angles();
        float theta = camera_direction_angles.theta - vertical_angle;
        float phi = camera_direction_angles.phi - horizontal_angle;

        Vector_3D ray_direction = Angles_Spherical(theta, phi).to_vector();
        return Ray(origin, ray_direction);
    }
};

class Object
{
    class Shape
    {};
    class Texture
    {

    };
};

class Sphere : public Object
{
    Point_3D center;
    float radius;

public:
    // Constructor
    Sphere(Point_3D c, float r) : center(c), radius(r) {};


};

class Plane : public Object
{
    Point_3D origin;
    Vector_3D normalVector;

public:
    // Constructor
    Plane(Point_3D  o, Vector_3D nv) : origin(o), normalVector(nv) {};

    //Methods
    Point_3D find_intersection(Ray myRay) const {
        Point_3D rayOrigin = myRay.get_origin();
        Vector_3D rayDirection = myRay.get_direction();
        float t = (normalVector*(origin-rayOrigin)) / (normalVector*rayDirection);
    }
};

#endif