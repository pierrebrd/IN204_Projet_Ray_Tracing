#include <iostream>
#include <vector>
#include <list> // cf td 6 ?
#include <fstream>

#include "src/scene.hpp"
#include "src/objects.hpp"
#include <tuple>



int main() {
    // Definition of the scene
    Camera cam(Point_3D(0, 0, 0), Vector_3D(0, 1, 0), 60, 400, 600, { 0,0,0 });

    std::vector<Object*> objects;

    Sphere sphere2(Point_3D(0, 5, 0), 0.3, { 1,1,1 }, 0, 1);
    // Adding spheres to form the word "doris"
    std::vector<Point_3D> doris_points = {
        // D
        { -4.5, 10, 0 }, { -4.5, 10, 0.5 }, { -4.5, 10, 1 }, { -4.5, 10, 1.5 }, { -4.5, 10, 2 },
        { -4, 10, 0 }, { -4, 10, 2 }, { -3.5, 10, 0.5 }, { -3.5, 10, 1.5 },{ -3.5, 10, 1 },
        // O
        { -2.5, 10, 0 }, { -2.5, 10, 2 }, { -2, 10, 0 }, { -2, 10, 2 }, { -1.5, 10, 0.5 }, { -1.5, 10, 1.5 }, { -1.5, 10, 1 },{ -3, 10, 0.5 }, { -3, 10, 1.5 }, { -3, 10, 1 },
        // R
        { -0.5, 10, 0 }, { -0.5, 10, 0.5 }, { -0.5, 10, 1 }, { -0.5, 10, 1.5 }, { -0.5, 10, 2 },
         { 0, 10, 1 }, { 0, 10, 2 }, { 0.5, 10, 1.5 }, { 0.5, 10, 2 }, { 0.5, 10, 0 },{ 0.5, 10, 0.5 },
         // I
         { 1.5, 10, 0 }, { 1.5, 10, 0.5 }, { 1.5, 10, 1 }, { 1.5, 10, 1.5 }, { 1.5, 10, 2 },
         // S
         { 2.5, 10, 0 }, { 2.5, 10, 1 }, { 2.5, 10, 2 }, { 3, 10, 0 }, { 3, 10, 1 }, { 3, 10, 2 }, { 3.5, 10, 1 }, { 3.5, 10, 0 }, { 3.5, 10, 2 } ,  { 3.5, 10, 0.5 } ,  { 2.5, 10, 1.5 }
    };

    for (const auto& point : doris_points) {
        Sphere* sphere = new Sphere(point, 0.2, { 1, 1, 1 }, 0.3, 1);
        objects.push_back(sphere);
    }

    Plane plan1(Point_3D(0, 0, -1), Vector_3D(0, 0, -1), { 1,1,1 }, 1, 0.5);
    //objects.push_back(&sphere2);
    objects.push_back(&plan1);

    std::vector<Light> lights;
    lights.push_back(Light(Point_3D(3, 2, 3), { 0.8, 0, 0 }));
    lights.push_back(Light(Point_3D(-3, 2, 3), { 0, 0, 0.8 }));
    // Ray emission from camera
    image_creator(&cam, &objects, &lights, "scene_simple.ppm");

    // Affichage

    return 0;
}