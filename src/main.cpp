#include <iostream>
#include <vector>
#include <list> // cf td 6 ?

#include "scene.hpp"
#include "objects.hpp"


int main() {
    // Definition of the scene
    Camera cam(Point_3D(0, 0, 0), Vector_3D(0, 1, 0), 90, 20, 30);

    std::vector<Object> objects;
    objects.push_back(Sphere(Point_3D(0,5,0),1));

    std::vector<Light> lights;
    lights.push_back(Light(Point_3D(2,3,0),1.0f));
    // Ray emission from camera
    image_creator(cam,&objects,&lights,"scene_simple.ppm");
    
    // Affichage

    return 0;
}


