/*
Ce fichier contiendra les classes concernant les rayons et leur suivi
*/

#ifndef objectsHPP
#define objectsHPP

#include <cmath>
#include <stdexcept>
#include <iostream>
//#include "scene.hpp"
#include <optional>
#include <tuple>
#include <string>
#include <cstdint>

#define TRESHOLD 0.001f

using rgb = std::tuple<float, float, float>; // A color, a light, a pixel, etc. is represented by a tuple of 3 floats between 0 and 1

struct Point_3D
{
    // Represents a point in 3D space
    // Champs
    float x;
    float y;
    float z;

    // Constructeurs
    inline Point_3D(float x_coord, float y_coord, float z_coord) : x(x_coord), y(y_coord), z(z_coord) {}

    //Point_3D(const Vector_3D vector): x(vector.x), y(vector.y), z(vector.z) {}

    // Méthodes
    void print() const {
        std::cout << "Point: (" << x << ", " << y << ", " << z << ")\n";
    }

    float distanceTo(Point_3D anotherPoint);

    /* Opérateur de conversion ne fonctionne pas...
    explicit operator Vector_3D() const{
        // Conversion de Point_3D en Vector_3D
        return Vector_3D(x,y,z);
    }*/


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
    inline Vector_3D(float x_coord, float y_coord, float z_coord) : x(x_coord), y(y_coord), z(z_coord) {}

    // à partir de deux points Point_3D (P2-P1)
    inline Vector_3D(const Point_3D& P1, const Point_3D& P2) : x(P2.x - P1.x), y(P2.y - P1.y), z(P2.z - P1.z) {}

    inline Vector_3D(const Point_3D& point) : x(point.x), y(point.y), z(point.z) {}

    // Méthodes
    float norm() const {
        // Returns the norm of the vector
        return std::sqrt(x * x + y * y + z * z);
    }

    void print() const {
        std::cout << "Vector: (" << x << ", " << y << ", " << z << ")\n";
    }

    explicit operator Point_3D() const {
        // Conversion de Vector_3D en Point_3D
        return Point_3D(x, y, z);
    }

    Angles_Spherical to_angles() const; // We declare the method here to avoid circular dependencies
};

// Opérateurs globaux
inline float operator * (const Vector_3D& aLeftVector, const Vector_3D& aRightVector) {
    // Définition du produit scalaire
    return aRightVector.x * aLeftVector.x + aRightVector.y * aLeftVector.y + aRightVector.z * aLeftVector.z;
}
inline Vector_3D operator * (float aFloat, const Vector_3D& aRightVector) {
    // Multiplication par un scalaire
    return Vector_3D(aFloat * aRightVector.x, aFloat * aRightVector.y, aFloat * aRightVector.z);
}

inline Vector_3D operator - (const Vector_3D& aLeftPoint, const Vector_3D& aRightPoint) {
    // Définition d'un vecteur à partir de deux points
    Vector_3D vector = Vector_3D(aLeftPoint.x - aRightPoint.x, aLeftPoint.y - aRightPoint.y, aLeftPoint.z - aRightPoint.z);
    return vector;
}

inline Point_3D operator + (Point_3D aLeftPoint, const Vector_3D& aRightVector) {
    // Somme d'un point et d'un vecteur
    Point_3D resultPoint = Point_3D(aLeftPoint.x + aRightVector.x, aLeftPoint.y + aRightVector.y, aLeftPoint.z + aRightVector.z);
    return resultPoint;
}

// Fonction pour normaliser un vecteur 
Vector_3D normalize(Vector_3D vector) {
    float norm = vector.norm();
    if (norm == 0.f) {
        throw std::invalid_argument("The norm of the vector is null, it can not be normalized");
    }
    return Vector_3D(vector.x / norm, vector.y / norm, vector.z / norm);

}

// Opérateurs globaux pour les tuples
inline rgb operator +(const rgb& aLeftTuple, const rgb& aRightTuple) {
    return { std::get<0>(aLeftTuple) + std::get<0>(aRightTuple),std::get<1>(aLeftTuple) + std::get<1>(aRightTuple),std::get<2>(aLeftTuple) + std::get<2>(aRightTuple) };
}

inline rgb operator *(float aFloat, const rgb& aRightTuple) {
    return { aFloat * std::get<0>(aRightTuple),aFloat * std::get<1>(aRightTuple),aFloat * std::get<2>(aRightTuple) };
}

inline rgb operator *(const rgb& aLeftTuple, const rgb& aRightTuple) {
    return { std::get<0>(aLeftTuple) * std::get<0>(aRightTuple),std::get<1>(aLeftTuple) * std::get<1>(aRightTuple),std::get<2>(aLeftTuple) * std::get<2>(aRightTuple) };
}

inline rgb minTuple(const rgb& tuple1, const rgb& tuple2) {
    return { std::min(std::get<0>(tuple1),std::get<0>(tuple2)),std::min(std::get<1>(tuple1),std::get<1>(tuple2)),std::min(std::get<2>(tuple1),std::get<2>(tuple2)) };
}

float Point_3D::distanceTo(Point_3D anotherPoint) {
    return Vector_3D(Point_3D(x, y, z), anotherPoint).norm();
}




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
    if (r == 0.f) {
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
    Point_3D cameraPosition;
    Vector_3D cameraDirection;
    unsigned cameraHfov; // Between 0 and 179, horizontal field of view in degrees
    unsigned resolution_height;
    unsigned resolution_width;
    rgb sceneAmbientLight; // 

public:
    // Constructor :
    Camera(Point_3D position = { 0.f, 0.f, 0.f }, Vector_3D direction = { 0.f, 1.f, 0.f }, unsigned fov = 90, unsigned res_height = 200, unsigned res_width = 300, rgb ambientLight = { 0.2f,0.2f,0.2f }) : cameraPosition(position), cameraDirection(direction), cameraHfov(fov), resolution_height(res_height), resolution_width(res_width), sceneAmbientLight(ambientLight) {
        if (cameraHfov > 179) {
            throw std::invalid_argument("horizontal fov must be less than 180 degrees");
        }
        if (resolution_height == 0 || resolution_width == 0) {
            throw std::invalid_argument("resolution can not be 0");
        }
        if (cameraHfov * resolution_height / resolution_width > 179) {
            throw std::invalid_argument("vertical fov must be less than 180 degrees");
        }
        if (direction.norm() == 0.f) {
            throw std::invalid_argument("The direction vector of the camera can not be null");
        }
    }

    // Destructor :
    ~Camera() {}; //default destructor

    // Getters :
    Point_3D get_position() const { return cameraPosition; }
    Vector_3D get_direction() const { return cameraDirection; }
    unsigned get_hfov() const { return cameraHfov; }
    unsigned get_resolution_height() const { return resolution_height; }
    unsigned get_resolution_width() const { return resolution_width; }
    rgb get_ambient_light() const { return sceneAmbientLight; }


    Ray ray_launcher(unsigned pixel_height, unsigned pixel_width) {
        // Launches a Ray from the camera to the given pixel coordinates. Coordinates start at 0, end at resolution_height-1 and resolution_width-1
        // TODO : Check that the pixel coordinates are within the resolution, otherwise return an exception

        // The coordinates starts on the bottom left corner
        Point_3D origin = cameraPosition;
        // camera::direction defines the center of the image, when pixel_height = resolution_height/2 and pixel_width = resolution_width/2
        // We consider that the camera is not rotated along the y axis (compared to the default viewing direction along the y axis)

        // We need to cast the unsigned to floats to avoid integer division
        float hfov_f = static_cast<float>(cameraHfov);
        float resolution_height_f = static_cast<float>(resolution_height);
        float resolution_width_f = static_cast<float>(resolution_width);
        float vfov_f = hfov_f * resolution_height_f / resolution_width_f; // Vertical field of view in degrees

        float horizontal_angle = ((hfov_f / 2) * (pixel_width - resolution_width_f / 2 + 0.5f) / (resolution_width / 2)) * M_PI / 180.0f; // In radians. The second resolution_width/2 is integer division
        float vertical_angle = ((vfov_f / 2) * (pixel_height - resolution_height_f / 2 + 0.5f) / (resolution_height / 2)) * M_PI / 180.0f; // In radians

        auto camera_direction_angles = cameraDirection.to_angles();
        float theta = camera_direction_angles.theta - vertical_angle;
        float phi = camera_direction_angles.phi - horizontal_angle;

        Vector_3D ray_direction = normalize(Angles_Spherical(theta, phi).to_vector());
        return Ray(origin, ray_direction);
    }
};


// class Light {
// protected : 
//     std::string type = "Light";
// public :
//     // Function to compute the lightning on a point
//     virtual float compute_lighting(Point_3D point, Vector_3D normal_vector) const = 0;
// };

// Pour l'instant, nos lumières sont des points lumineux

class Light
{
    Point_3D position;
    rgb light_color; // Tuple de float entre 0 et 1
protected:
    std::string type = "PointLight";
public:
    // Constructor
    Light(Point_3D pos = { 0.f, 0.f, 0.f }, rgb i = { 1.0f,1.0f,1.0f }) : position(pos), light_color(i) {}
    // Getters
    Point_3D get_position() const { return position; }
    rgb  get_light_color() const { return light_color; }

    // Methods : 
    rgb compute_PointLight(Point_3D point, Vector_3D normal_vector) const {
        // We have a Pointlight and a point on an object and the normal vector at this point
        // We compute the lighting based on the dot product between the normal vector of the surface and the vector from the point to the light
        // It can be negative (part of a sphere in the shadow for example) 
        Vector_3D light_vector = Vector_3D(point, position);
        try {
            light_vector = normalize(light_vector);
        }
        catch (std::invalid_argument error) {
            return { 0.f,0.f,0.f };
        }
        try {
            normal_vector = normalize(normal_vector);
        }
        catch (std::invalid_argument error) {
            return { 0.f,0.f,0.f };
        }

        rgb lighting_intensity = (std::abs(light_vector * normal_vector)) * light_color;
        // if (lighting_intensity < 0){
        //     return 0.f;
        // }
        return lighting_intensity;
    }

};

// On pourra implémenter plus tard d'autres type de lumières (directionnelles, cônes, surface, sphère...)
// Carrément avoir des objets (genre une sphère) émetteurs de lumière ?

class Object
{
protected:
    std::string objectShape = "Object";
    rgb objectColor = { 1.f,0.f,0.f }; // RGB tuples de float entre 0 et 1
    float objectDiffuseReflection = 1.0f; // Réflection diffuse (dans toutes les directions de l'espace)
    float objectSpecularReflection = 0.2f; // Réflection spéculaire (effet miroir)
public:
    // Getters :
    std::string get_shape() const { return objectShape; }
    rgb get_color() const { return objectColor; }
    float get_diffuseCoeff() const { return objectDiffuseReflection; }
    float get_specularCoeff() const { return objectSpecularReflection; }



    // Methods

    // Method to find the intersection between the object and a ray
    virtual std::optional<Point_3D> find_intersection(const Ray& myRay) const = 0;

    // Method to compute the normal vector at a point
    virtual Vector_3D get_normal(Point_3D point) const = 0;

    Ray determineReflectedRay(Ray myRay, Point_3D pointOfReflection, Vector_3D normal) {
        // La méthode ne dépend pas de la forme de l'objet mais du vecteur normal à l'objet
        // En notant u le vecteur du rayon incident, w le vecteur du rayon réfléchi et n le vecteur normal à la sphère
        // w = u - 2*(u.n)n
        // Hypothèse : u et n de norme 1
        Point_3D rayOrigin = myRay.get_origin();
        Vector_3D rayDirection = normalize(myRay.get_direction()); // u
        normal = normalize(normal); // n
        Vector_3D reflectedRayDirection = rayDirection - 2 * (rayDirection * normal) * normal;
        return Ray(pointOfReflection, reflectedRayDirection);
    }
};



class Sphere : public Object
{
    Point_3D center;
    float radius;
public:

    // Constructor
    Sphere(Point_3D c, float r, rgb color = { 1.f,0.f,0.f }, float diffuseCoeff = 1.f, float specularCoeff = 0.2f) : center(c), radius(r) {
        objectShape = "Sphere";
        objectColor = color;
        objectDiffuseReflection = diffuseCoeff;
        objectSpecularReflection = specularCoeff;
    };

    // Methods
    std::optional<Point_3D> find_intersection(const Ray& myRay) const {
        Point_3D rayOrigin = myRay.get_origin();
        Vector_3D rayDirection = myRay.get_direction();

        Vector_3D originToSphere = Vector_3D(center, rayOrigin);

        // Equation du 2nd degré à résoudre du type a*t^2 + b*t + c = 0
        float a = rayDirection * rayDirection;
        float b = 2.0f * (rayDirection * originToSphere);
        float c = (originToSphere * originToSphere) - radius * radius;
        float delta = b * b - 4.0f * a * c; // discriminant

        if (delta < 0.f) {
            // Pas d'intersection
            return std::nullopt;
        }
        float t;
        if (delta == 0.f) {
            t = -b / (2.f * a);
        }
        else {
            float sqrt_delta = std::sqrt(delta);
            float t1 = (-b + sqrt_delta) / (2.f * a);
            float t2 = (-b - sqrt_delta) / (2.f * a);
            // Rq : t2 < t1
            if (t1 <= 0.f && t2 <= 0.f) { // On est du mauvais côté du rayon si les deux valeurs négatives
                return std::nullopt;
            }
            else if (t2 > TRESHOLD) { // Sinon, on prend la positive la plus petite.
                t = t2;
            }
            // t2 < TRESHOLD
            else if (t1 > TRESHOLD) {
                t = t1;
            }
            else {
                return std::nullopt;
            }
        }
        Point_3D intersection = rayOrigin + t * rayDirection;
        return intersection;
    }

    Vector_3D get_normal(Point_3D point) const {
        return Vector_3D(center, point);
    }


};

class Plane : public Object
{
    Point_3D origin;
    Vector_3D normalVector;
public:
    // Constructor
    Plane(Point_3D o, Vector_3D nv, rgb color = { 1.f,0.f,0.f }, float diffuseCoeff = 1.f, float specularCoeff = 0.2f) : origin(o), normalVector(nv) {
        objectShape = "Plane";
        objectColor = color;
        objectDiffuseReflection = diffuseCoeff;
        objectSpecularReflection = specularCoeff;
    };
    // Methods
    std::optional<Point_3D> find_intersection(const Ray& myRay) const {
        Point_3D rayOrigin = myRay.get_origin();
        Vector_3D rayDirection = myRay.get_direction();

        if (normalVector * rayDirection == 0.f) {
            // La droite est parallèle au plan
            // On exclut le cas où la droite du rayon appartient au plan
            // Cas où il n'y a pas d'intersection
            return std::nullopt;
        }

        float t = (normalVector * (Vector_3D(rayOrigin, origin))) / (normalVector * rayDirection);
        if (t < TRESHOLD) {
            return std::nullopt;
        }
        Point_3D intersection = rayOrigin + t * rayDirection;
        return intersection;
    }

    Vector_3D get_normal(Point_3D point) const {
        return normalVector;
    }
};

#endif