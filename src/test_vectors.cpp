#include "objects.hpp"

int main(){
    auto cam = Camera();
    auto direction = cam.get_direction();
    auto angles = direction.to_angles();
    direction.print();
    angles.print();
    return 0;
}