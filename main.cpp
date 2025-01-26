#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <iostream>
#include <tuple>
#include <sstream>

#include "src/scene.hpp"
#include "src/objects.hpp"


// A partir d'un fichier texte, crée les objets, les lumières et la caméra
int create_scene(std::string fileName, Camera* cam, std::vector<Object*>* objects, std::vector<Light*>* lights) {
    std::ifstream file(fileName);

    if (!file.is_open()) {
        std::cerr << "Erreur. Impossible d'ouvrir le fichier." << std::endl;
        return -1;
    }

    std::string objectShape; // Variable qui contiendra le type de l'objet étudié
    std::string line; // Variable qui contiendra les paramètres de l'objet
    while (std::getline(file, objectShape)) {
        if (objectShape == "" || objectShape[0] == '#') { // On peut mettre des commentaires en commençant la ligne par #
            continue;
        }

        std::getline(file, line);
        std::istringstream lineStream(line); // Permet de faire du parsing pour une string
        std::cout << objectShape << " : ";

        if (objectShape == "camera") {
            float x, y, z, dx, dy, dz, r, g, b;
            unsigned fov, width, height, depth;
            char braceL1, braceR1, braceL2, braceR2, braceL3, braceR3;
            if (lineStream >> braceL1 >> x >> y >> z >> braceL1 >> braceL2 >> dx >> dy >> dz >> braceR2 >> fov >> width >> height >> braceL3 >> r >> g >> b >> braceR3 >> depth) {
                *cam = Camera(Point_3D(x, y, z), Vector_3D(dx, dy, dz), fov, width, height, { r,g,b }, depth);
                std::cout << "pos (" << x << ", " << y << ", " << z << "), dir (" << dx << ", " << dy << ", " << dz << "), fov " << fov << ", resolution " << width << "x" << height << ", couleur ambiante (" << r << ", " << g << ", " << b << ")" << ", profondeur reflets " << depth << std::endl;
            }
            else {
                std::cerr << "Erreur. Camera : Format du texte erronée\n";
            }

        }
        else if (objectShape == "light") {
            float x, y, z, r, g, b;
            char braceL1, braceR1, braceL2, braceR2;
            if (lineStream >> braceL1 >> x >> y >> z >> braceR1 >> braceL2 >> r >> g >> b >> braceR2) {
                Light* aLight = new Light(Point_3D(x, y, z), { r,g,b });
                lights->push_back(aLight);
                std::cout << "pos (" << x << ", " << y << ", " << z << "), couleur (" << r << ", " << g << ", " << b << ")" << std::endl;
            }
            else {
                std::cerr << "Erreur. Light : Format du texte erronée\n";
            }
        }

        else if (objectShape == "sphere") {
            float x, y, z, radius, r, g, b, objectDiffuseReflection, objectSpecularReflection;
            char braceL1, braceR1, braceL2, braceR2;
            if (lineStream >> braceL1 >> x >> y >> z >> braceR1 >> radius >> braceL2 >> r >> g >> b >> braceR2 >> objectDiffuseReflection >> objectSpecularReflection) {
                Sphere* aSphere = new Sphere(Point_3D(x, y, z), radius, { r,g,b }, objectDiffuseReflection, objectSpecularReflection);
                objects->push_back(aSphere);
                std::cout << "centre (" << x << ", " << y << ", " << z << "), rayon " << radius << " couleur (" << r << ", " << g << ", " << b << ")" << " reflections : " << objectDiffuseReflection << " " << objectSpecularReflection << std::endl;
            }
            else {
                std::cerr << "Erreur. Sphere : Format du texte erronée\n";
            }
        }
        else if (objectShape == "plane") {
            float xp, yp, zp, xn, yn, zn, r, g, b, objectDiffuseReflection, objectSpecularReflection;
            char braceL1, braceL2, braceR1, braceR2, braceL3, braceR3;
            if (lineStream >> braceL1 >> xp >> yp >> zp >> braceR1 >> braceL2 >> xn >> yn >> zn >> braceR2 >> braceL3 >> r >> g >> b >> braceR3 >> objectDiffuseReflection >> objectSpecularReflection) {
                Plane* aPlane = new Plane(Point_3D(xp, yp, zp), Vector_3D(xn, yn, zn), { r,g,b }, objectDiffuseReflection, objectSpecularReflection);
                objects->push_back(aPlane);
                std::cout << "point (" << xp << "," << yp << "," << zp << "), normal : (" << xn << "," << yn << "," << zn << ")" << " couleur (" << r << ", " << g << ", " << b << ")" << " reflections : " << objectDiffuseReflection << " " << objectSpecularReflection << std::endl;
            }
            else {
                std::cerr << "Erreur. Plane : Format du texte erronée\n";
            }
        }
    }
    return 0;
}

int main(int argc, char** argv) {

    if (argc != 3) {
        std::cerr << "Erreur. Nombre d'arguments incorrect. Entrez le nom du fichier texte à lire et le nom de fichier de sortie" << std::endl;
        return -1;
    }

    // Intialisation
    Camera cam;
    std::vector<Object*> objects;
    std::vector<Light*> lights;

    // On crée la scène à partir du fichier texte
    create_scene(std::string(argv[1]), &cam, &objects, &lights);

    // On crée l'image
    image_creator(&cam, &objects, &lights, std::string(argv[2]));

    // On supprime les objets et lumières 
    for (auto it = objects.begin(); it != objects.end(); it++) {
        delete* it;
    }
    for (auto it = lights.begin(); it != lights.end(); it++) {
        delete* it;
    }


    return 0;
}