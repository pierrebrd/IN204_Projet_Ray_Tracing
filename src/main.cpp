#include <iostream>
#include <vector>
#include <list> // cf td 6 ?
#include <fstream>
#include <iostream>

#include "scene.hpp"
#include "objects.hpp"
#include <tuple>
#include <sstream>


// A partir d'un fichier texte, crée les objets
int create_scene(std::string fileName, Camera* cam, std::vector<Object*>* objects, std::vector<Light>* lights) {
    std::ifstream file(fileName);

    if (!file.is_open()) {
        std::cerr << "Erreur. Impossible d'ouvrir le fichier." << std::endl;
        return -1;
    }
    std::string objectShape;
    std::string line;
    while (std::getline(file, objectShape)) {
        if (objectShape == "" || objectShape[0] == '#') {
            continue;
        }

        std::getline(file, line);
        std::istringstream lineStream(line); // Permet de faire du parsing pour un string
        std::cout << objectShape << " : ";

        if (objectShape == "camera") {
            float x, y, z, dx, dy, dz, r, g, b;
            unsigned fov, width, height;
            char braceL1, braceR1, braceL2, braceR2, braceL3, braceR3;
            if (lineStream >> braceL1 >> x >> y >> z >> braceL1 >> braceL2 >> dx >> dy >> dz >> braceR2 >> fov >> width >> height >> braceL3 >> r >> g >> b >> braceR3) {
                *cam = Camera(Point_3D(x, y, z), Vector_3D(dx, dy, dz), fov, width, height, { r,g,b });
                std::cout << "pos (" << x << ", " << y << ", " << z << "), dir (" << dx << ", " << dy << ", " << dz << "), fov " << fov << ", resolution " << width << "x" << height << ", couleur ambiante (" << r << ", " << g << ", " << b << ")" << std::endl;
            }
            else {
                std::cerr << "Erreur. Camera : Format du fichier erronée";
            }

        }
        else if (objectShape == "light") {
            float x, y, z, r, g, b;
            char braceL1, braceR1, braceL2, braceR2;
            if (lineStream >> braceL1 >> x >> y >> z >> braceR1 >> braceL2 >> r >> g >> b >> braceR2) {
                Light aLight(Point_3D(x, y, z), { r,g,b });
                lights->push_back(aLight);
                std::cout << "pos (" << x << ", " << y << ", " << z << "), couleur (" << r << ", " << g << ", " << b << ")" << std::endl;
            }
            else {
                std::cerr << "Erreur. Light : Format du fichier erronée";
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
                // Il vaudrait mieux lancer une exception pour indiquer que le format du fichier n'est pas le bon.
                std::cerr << "Erreur. Sphere : Format du fichier erronée";
            }
        }
        else if (objectShape == "plane") {
            float xp, yp, zp, xn, yn, zn, r, g, b, objectDiffuseReflection, objectSpecularReflection;
            char braceL1, braceL2, braceR1, braceR2, braceL3, braceR3;
            if (lineStream >> braceL1 >> xp >> yp >> zp >> braceR1 >> braceL2 >> xn >> yn >> zn >> braceR2 >> braceL3 >> r >> g >> b >> braceR3 >> objectDiffuseReflection >> objectSpecularReflection
                && braceL1 == '{' && braceR1 == '}' && braceL2 == '{' && braceR2 == '}') {
                Plane* aPlane = new Plane(Point_3D(xp, yp, zp), Vector_3D(xn, yn, zn), { r,g,b }, objectDiffuseReflection, objectSpecularReflection);
                objects->push_back(aPlane);
                std::cout << "point (" << xp << "," << yp << "," << zp << "), normal : (" << xn << "," << yn << "," << zn << ")" << " couleur (" << r << ", " << g << ", " << b << ")" << " reflections : " << objectDiffuseReflection << " " << objectSpecularReflection << std::endl;
            }
            else {
                std::cerr << "Erreur. Plane : Format du fichier erronée";
            }
        }
    }
    return 0;
}

int main(int argc, char** argv) {
    // Definition of the scene

    //Camera cam(Point_3D(0, 0, 0), Vector_3D(0, 1, 0), 90, 400, 600, { 0,0,0 });

    Camera cam;
    std::vector<Object*> objects;
    std::vector<Light> lights;

    if (argc != 3) {
        std::cerr << "Erreur. Nombre d'arguments incorrect. Entrez le nom du fichier texte à lire et le nom de fichier de sortie" << std::endl;
        return -1;
    }
    std::cout << "Fichier : " << argv[1] << std::endl;
    std::cout << "Fichier de sortie : " << argv[2] << std::endl;

    create_scene(std::string(argv[1]), &cam, &objects, &lights);

    std::cout << "lights : " << lights.size() << std::endl;



    //Sphere sphere1(Point_3D(0, 5, 0), 1, { 1,0.2,0 }, 0.5, 0.5);
    //Sphere sphere2(Point_3D(0, 10, 1), 1, { 1,1,1 }, 0.5, 0.5);
    //Plane plan1(Point_3D(0, 0, -1), Vector_3D(0, 0, 1), { 0,0,0 }, 0, 1);
    //objects.push_back(&sphere2);
    //objects.push_back(&sphere1);
    //objects.push_back(&plan1);


    //lights.push_back(Light(Point_3D(3, 2, 3), { 0.5, 0, 0 }));
    //lights.push_back(Light(Point_3D(-3, 2, 3), { 0, 0, 0.5 }));
    // Ray emission from camera
    image_creator(&cam, &objects, &lights, std::string(argv[2]));

    // Affichage

    return 0;
}