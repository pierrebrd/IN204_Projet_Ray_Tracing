/*
Ce fichier contiendra les classes concernant les rayons et leur suivi
*/



#ifndef objectsHPP
#define objectsHPP

#include <vector>
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

struct Vector_3D;

struct Angles_Spherical {
    // Represents the 2 angles between 2 vectors in 3D space using the spherical coordinates
    float theta; // Angle with the z axis
    float phi; // Angle with the x axis in the x-y plane

    Angles_Spherical(float t, float p) : theta(t), phi(p) {}

    Vector_3D to_vector(){
        // Returns the vector corresponding to the angles in spherical coordinates
        float x = std::sin(theta) * std::cos(phi);
        float y = std::sin(theta) * std::sin(phi);
        float z = std::cos(theta);
        Vector_3D vector(x, y, z);
        return vector;
    }

    void print() const {
        std::cout << "Angles: (" << theta << ", " << phi << ")\n";
    }
};

struct Vector_3D
{
    // Represents a vector in 3D space
    float x;
    float y;
    float z;

    Vector_3D(float x_coord, float y_coord, float z_coord) : x(x_coord), y(y_coord), z(z_coord) {}

    float norm() const {
        // Returns the norm of the vector
        return std::sqrt(x * x + y * y + z * z);
    }

    Angles_Spherical to_angles(){
        // Returns the angles of the vector in spherical coordinates
        auto r = norm();
        if (r == 0){
            throw std::invalid_argument("The vector is null, it has no angles");
        }
        float theta = std::acos(z/r); // Angle with the z axis
        float phi = std::atan2(y, x); // Angle with the x axis in the x-y plane
        Angles_Spherical angles(theta, phi);
        return angles;
    }

    void print() const {
        std::cout << "Vector: (" << x << ", " << y << ", " << z << ")\n";
    }



};



class Ray
{
private:
    Point_3D origin;
    Vector_3D direction;

public:
    Ray(float xo, float yo, float zo, float xd, float yd, float zd) : origin(xo, yo, zo), direction(xd, yd, zd) {}
    Ray(Point_3D ori, Vector_3D dir) : origin(ori),direction(dir){};

    // Getters
    Point_3D get_origin() const {return origin;}
    Vector_3D get_direction() const {return direction;}
    


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
    Camera(Point_3D pos = {0, 0, 0}, Vector_3D dir = {0, 1, 0}, unsigned f = 90, unsigned res_h = 400, unsigned res_w = 600) : position(pos), direction(dir), hfov(f), resolution_height(res_h), resolution_width(res_w) {}
    // TODO : il faudrait vérifier que les valeurs de fov sont bien comprises entre 0 et 179 : genre si on a 179 en hfov mais que l'image est plus haute que large, il y aura un pb ???
    // Destructor :
    ~Camera();

    // Getters :
    Point_3D get_position() const {return position;}
    Vector_3D get_direction() const {return direction;}
    unsigned get_hfov() const {return hfov;}
    unsigned get_resolution_height() const {return resolution_height;}
    unsigned get_resolution_width() const {return resolution_width;}
    

    Ray ray_launcher(unsigned pixel_height,unsigned pixel_width){
        // Launches a Ray from the camera to the given pixel coordinates
        // TODO : Check that the pixel coordinates are within the resolution, otherwise return an exception
        
        // The coordinates starts on the bottom left corner
        Point_3D origin = Camera::position;
        // camera::direction defines the center of the image, when pixel_height = resolution_height/2 and pixel_width = resolution_width/2
        // We consider that the camera is not rotated along the y axis (compared to the default viewing direction along the y axis)
        // Pixel 0,0 is the bottom left corner, so the direction is
        float horizontal_angle = (hfov * (pixel_width - resolution_width/2) / (resolution_width/2)) * M_PI / 180.0; // In radians
        float vfov = hfov * resolution_height / resolution_width; // Vertical field of view in degrees
        float vertical_angle = (vfov * (pixel_height - resolution_height/2) / (resolution_height/2)) * M_PI / 180.0; // In radians
        
        auto camera_direction_angles = direction.to_angles();
        float theta = camera_direction_angles.theta - vertical_angle;
        float phi = camera_direction_angles.phi + horizontal_angle;

        Vector_3D ray_direction = Angles_Spherical(theta, phi).to_vector();
        return Ray(origin, ray_direction);
    }
};

class Object
{
    class Shape
    {
    };
    class Texture{

    };
};

class Sphere : public Object
{
    Point_3D center;
    float radius;

    public:
        // Constructor
        Sphere(Point_3D c, float r): center(c), radius(r){};


};

class Plane : public Object
{
    Point_3D origin;
    Vector_3D vector1;
    Vector_3D vector2;

    public:
        // Constructor
        Plane(Point_3D  o, Vector_3D v1, Vector_3D v2): origin(o), vector1(v1), vector2(v2){};    

        // Methods
        Point_3D find_intersection(Ray myRay) const{
            myRay.get_direction();
            Vector_3D normal= Vector_3D()
        }
};

#endif