#include "../src/objects.hpp"

int main() {
    Camera cam(Point_3D(0, 0, 0), Vector_3D(0, 1, 0), 90, 5, 7);
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 7; j++) {
            Ray ray = cam.ray_launcher(i, j);
            std::cout << "Ray " << i << " " << j << std::endl;
            ray.get_direction().print();
            ray.get_direction().to_angles().print();
        }
    }
    

    Ray ray = Ray(Point_3D(0,0,0),Vector_3D(0,1,0));

    Sphere aSphere = Sphere(Point_3D(0,-3,0),1);
    std::optional<Point_3D> theIntersec = aSphere.find_intersection(ray);
    if (!theIntersec.has_value()){
        printf("Intersection vide");
    }
    else{
        theIntersec.value().print();
    }    
    return 0;
}