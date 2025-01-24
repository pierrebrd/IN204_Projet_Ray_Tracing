#include "../src/scene.hpp"
#include "../src/objects.hpp"
#include <string>

int main(){
    Camera cam;
    std::vector<Object> objects;
    std::vector<Light> lights;
    image_creator(cam, &objects, &lights, "test_image.ppm");
    return 0;
}
