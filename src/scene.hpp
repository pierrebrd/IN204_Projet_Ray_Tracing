#ifndef sceneHPP
#define sceneHPP

#include "objects.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <algorithm>
#include <cstdint>


#define DEPTH 4 // Profondeur de réflexion des rayons

std::optional<std::pair<Point_3D, Object*>> findNextIntersection(Ray myRay, std::vector<Object*>* objects) {
    // A partir d'un rayon donné (qui arrive à la caméra), lui associer un RGB en fonction des obstacles que rencontre le rayon.

    // Rayon d'ombre
    // On détermine l'intersection la plus proche de la caméra avec un objet
    std::vector<Object*>::iterator it = objects->begin();
    std::optional<Point_3D> closestIntersec = (*it)->find_intersection(myRay);
    Object* closestObjectPt = *it; // TODO : Vérifier que c'est un itérateur valide genre qu'on n'est pas à la fin du vecteur
    while (!closestIntersec.has_value() && it != objects->end()) {
        // On cherche la première intersection non vide
        closestIntersec = (*it)->find_intersection(myRay);
        closestObjectPt = *it;
        ++it;
    }

    // Le rayon n'intersecte avec aucun objet de la scène
    if (!closestIntersec.has_value()) {
        return std::nullopt;
    }

    // Le rayon intersecte avec au moins un objet
    Point_3D rayOrigin = myRay.get_origin();
    for (; it != objects->end(); it++) {
        std::optional<Point_3D> currentIntersec = (*it)->find_intersection(myRay);
        if (currentIntersec.has_value()) {
            if (rayOrigin.distanceTo(currentIntersec.value()) < rayOrigin.distanceTo(closestIntersec.value())) {
                closestIntersec = currentIntersec;
                closestObjectPt = *it;
            }
        }
    }
    return std::make_pair(closestIntersec.value(), closestObjectPt);
}



// We declare the function here to avoid circular dependencies
rgb determineLightIntensity(Point_3D intersection, Object* intersected_object, rgb ambient_light, std::vector<Light>* lights, std::vector<Object*>* objects, Ray incidentRay, int depth);



rgb determineLightIntensity_Diffuse(Point_3D intersection, Object* intersected_object, std::vector<Light>* lights, std::vector<Object*>* objects) {

    float diffuseCoeff = intersected_object->get_diffuseCoeff();
    if (diffuseCoeff == 0.f) { // Pas besoin de faire tous les calculs
        return { 0.f,0.f,0.f };
    }

    std::vector<Light>::iterator it = lights->begin();

    Vector_3D normal = intersected_object->get_normal(intersection);

    rgb  lightIntensityShadow = { 0.f,0.f,0.f }; // Initialisation
    // Rayons d'ombre
    for (;it != lights->end(); it++) { // Pour chaque source de lumière
        Ray nextRay(intersection, Vector_3D(intersection, it->get_position()));
        auto nextIntersection = findNextIntersection(nextRay, objects);
        rgb  currentIntensity;
        if (nextIntersection.has_value() && nextIntersection.value().first.distanceTo(intersection) > TRESHOLD && nextIntersection.value().first.distanceTo(intersection) < it->get_position().distanceTo(intersection)) { // arbitraire de fou
            // Si le rayon rencontre un objet avant la source de lumière
            currentIntensity = { 0.f,0.f,0.f };
        }
        else {
            currentIntensity = it->compute_PointLight(intersection, normal);
            lightIntensityShadow = lightIntensityShadow + currentIntensity;
        }
    }
    return diffuseCoeff * lightIntensityShadow; // On multiplie par le coefficient de réflexion diffuse
}

rgb determineLightIntensity_Specular(Point_3D intersection, Object* intersected_object, rgb ambient_light, std::vector<Light>* lights, std::vector<Object*>* objects, Ray incidentRay, int depth) {
    // Rayon réfléchi
    // On permet aux rayons de se réflechir une fois. 
    // A reflection ray is traced in the mirror-reflection direction. The closest object it intersects is what will be seen in the reflection

    if (depth == 0) { // On a atteint la profondeur maximale de calcul
        return { 0.f,0.f,0.f };
    }

    float specularCoeff = intersected_object->get_specularCoeff();

    if (specularCoeff == 0.f) { // Pas besoin de faire tous les calculs
        return { 0.f,0.f,0.f };
    }
    Vector_3D normal = intersected_object->get_normal(intersection); // Il sera normalisé dans la fonction determieReflectedRay
    Ray reflectedRayDirection = intersected_object->determineReflectedRay(incidentRay, intersection, normal);
    auto nextIntersectionPair = findNextIntersection(reflectedRayDirection, objects);

    if (!nextIntersectionPair.has_value()) {
        // Le rayon réfléchi ne recontre pas de nouvel objet
        return { 0.f,0.f,0.f };
    }

    Point_3D nextIntersection = nextIntersectionPair.value().first;
    Object* nextIntersectedObject = nextIntersectionPair.value().second;
    rgb nextIntersectedObjectColor = nextIntersectedObject->get_color();
    rgb lightIntensity = determineLightIntensity(nextIntersection, nextIntersectedObject, ambient_light, lights, objects, reflectedRayDirection, depth - 1);
    // On ne fait pas réfléchir le rayon une autre fois sur l'objet, donc on utilise bien la fonctino ..._Shadow
    return specularCoeff * lightIntensity * nextIntersectedObjectColor; // On multiplie par le coefficient de réflexion spéculaire
}



// TODO : gros poblème, on prend ne comprte la lumière qui arrive sur l'objetsur lequel on rebondit mais pas sa couleur !!!!!!!!

rgb determineLightIntensity(Point_3D intersection, Object* intersected_object, rgb ambient_light, std::vector<Light>* lights, std::vector<Object*>* objects, Ray incidentRay, int depth) {
    if (depth == 0) { // On a atteint la profondeur maximale de calcul
        return { 0.f,0.f,0.f };
    }

    // On calcule les intensités lumineuses dues aux différents phénomènes

    rgb lightIntensityShadow = determineLightIntensity_Diffuse(intersection, intersected_object, lights, objects);
    rgb lightIntensityReflection = determineLightIntensity_Specular(intersection, intersected_object, ambient_light, lights, objects, incidentRay, depth);
    rgb sum = lightIntensityReflection + lightIntensityShadow + ambient_light;
    //std::cout << std::get<0>(sum) << " " << std::get<1>(sum) << " " << std::get<2>(sum) << std::endl;
    //return minTuple(sum, { 10.f,10.f,10.f }); // TODO : Enlever le min ? en gros ça ferait qu'o, pourrait rentre l'objet plus lumin,eux qu'il ne l'est réellement, mais il faudrait capper dans la dernière fonction
    return sum;
}


rgb compute_color(Ray myRay, Camera* cam, std::vector<Object*>* objects, std::vector<Light>* lights, int depth) {
    // Function that looks for an intersection and computes the color by recursively calling light functions
    auto nextIntersect = findNextIntersection(myRay, objects);
    if (!nextIntersect.has_value()) {
        return { 0.f,0.f,0.f };
    }
    else {
        Point_3D intersection = nextIntersect.value().first;
        Object* intersected_object = nextIntersect.value().second;
        rgb lightIntensity = determineLightIntensity(intersection, intersected_object, cam->get_ambient_light(), lights, objects, myRay, depth); // On calcule l'intensité lumineuse reçue en considérant la profondeur de calcul
        auto object_color = intersected_object->get_color();
        //std::cout << "Object color : " << std::get<0>(object_color) << " " << std::get<1>(object_color) << " " << std::get<2>(object_color) << std::endl;
        auto value = lightIntensity * object_color;
        //std::cout << "Color product : " << std::get<0>(value) << " " << std::get<1>(value) << " " << std::get<2>(value) << std::endl;
        return { lightIntensity * object_color };

    }

}


void image_creator(Camera* cam, std::vector<Object*>* objects, std::vector<Light>* lights, std::string filename) {
    // This function creates an image of the scene and saves it as a ppm file

    std::ostringstream buffer; //Buffer pour accélerer

    buffer << "P3" << std::endl;
    buffer << cam->get_resolution_width() << " " << cam->get_resolution_height() << std::endl;
    buffer << "255" << std::endl;




    for (unsigned i = 0; i < cam->get_resolution_height(); i++) {
        for (unsigned j = 0; j < cam->get_resolution_width(); j++) {
            // We lauch the ray from the camera
            Ray initial_Ray = cam->ray_launcher(cam->get_resolution_height() - i - 1, j);
            // We calculate the color of the pixel 
            rgb colors = compute_color(initial_Ray, cam, objects, lights, DEPTH);
            colors = minTuple(colors, { 1.f,1.f,1.f }); // We cap the color to 1
            //std::cout << "Color : " << std::get<0>(colors) << " " << std::get<1>(colors) << " " << std::get<2>(colors) << std::endl;
            // We write the color in the file 
            buffer << static_cast<int>(std::get<0>(colors) * 255.) << " " << static_cast<int>(std::get<1>(colors) * 255.) << " " << static_cast<int>(std::get<2>(colors) * 255.) << " ";
        }
        buffer << std::endl;
        if (i % 100 == 0) {
            std::cout << "\rLine " << i << " done";
            std::cout.flush();
        }
    }

    // We open the ppm file as a stream
    std::ofstream file(filename); // We create the file

    if (!file.is_open()) {
        std::cerr << "Impossible to open the file " << filename << std::endl;
        return;
    }
    file << buffer.str();
    file.close();
    std::cout << "\rThe image has been created and saved as " << filename << std::endl;
    return;
}





#endif