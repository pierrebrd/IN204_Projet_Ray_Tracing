#include <iostream>
#include <vector>
#include <list> // cf td 6 ?

#include "scene.hpp"
#include "objects.hpp"


int main() {
    // Definition of the scene
    Camera cam(Point_3D(0, 0, 0), Vector_3D(0, 1, 0), 90, 400, 600);

    std::vector<Object*> objects;
    Sphere sphere1(Point_3D(0,5,0),1,{255,0,0});
    Sphere sphere2(Point_3D(0,3,1),1 ,{255,0,0});
    Plane plan1(Point_3D(0,0,-1),Vector_3D(0,0,1),{255,0,0});
    auto colors = sphere1.get_color();
    std::cout << static_cast<int>(std::get<0>(colors)) << " " << static_cast<int>(std::get<1>(colors)) << " " << static_cast<int>(std::get<2>(colors)) << std::endl;
    objects.push_back(&sphere2);
    //objects.push_back(&sphere1);
    objects.push_back(&plan1);

    std::vector<Light> lights;
    lights.push_back(Light(Point_3D(3,2,3),0.5f));
    lights.push_back(Light(Point_3D(-3,2,3),0.5f));
    // Ray emission from camera
    image_creator(cam,&objects,&lights,"scene_simple.ppm");
    
    // Affichage

    return 0;
}


