#include "objects.hpp"

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
    Vector_3D myVector = 3.0*Vector_3D(1,1,1);
    myVector.print();

    Plane aPlane(Point_3D(0,2,0), Vector_3D(0,1,0));
    Point_3D theIntersec = aPlane.find_intersection(ray);
    theIntersec.print();
    return 0;
}