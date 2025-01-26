#ifndef sceneHPP
#define sceneHPP

#include "objects.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include <algorithm>
#include <cstdint>


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

rgb determineLightIntensity_Shadow(Point_3D intersection, Object* intersected_object, std::vector<Light>* lights, std::vector<Object*>* objects) {
    std::vector<Light>::iterator it = lights->begin();

    Vector_3D normal = intersected_object->get_normal(intersection);

    rgb  lightIntensityShadow = { 0,0,0 }; // Initialisation
    // Rayons d'ombre
    for (;it != lights->end(); it++) { // Pour chaque source de lumière
        Ray nextRay(intersection, Vector_3D(intersection, it->get_position()));
        auto nextIntersection = findNextIntersection(nextRay, objects);
        rgb  currentIntensity;
        if (nextIntersection.has_value() && nextIntersection.value().first.distanceTo(intersection) > 0.001 && nextIntersection.value().first.distanceTo(intersection) < it->get_position().distanceTo(intersection)) { // arbitraire de fou
            // Si le rayon rencontre un objet avant la source de lumière
            currentIntensity = { 0,0,0 };
        }
        else {
            currentIntensity = it->compute_PointLight(intersection, normal);
            lightIntensityShadow = lightIntensityShadow + currentIntensity;
        }
    }
    return lightIntensityShadow;
}

rgb determineLightIntensity_Reflection(Point_3D intersection, Object* intersected_object, std::vector<Light>* lights, std::vector<Object*>* objects, Ray incidentRay) {
    // Rayon réfléchi
    // On permet aux rayons de se réflechir une fois. 
    // A reflection ray is traced in the mirror-reflection direction. The closest object it intersects is what will be seen in the reflection
    Vector_3D normal = intersected_object->get_normal(intersection);
    Ray reflectedRayDirection = intersected_object->determineReflectedRay(incidentRay, intersection, normal);
    auto nextIntersectionPair = findNextIntersection(reflectedRayDirection, objects);

    if (!nextIntersectionPair.has_value()) {
        // Le rayon réfléchi ne recontre pas de nouvel objet
        return { 0,0,0 };
    }

    float reflectionCoeff = intersected_object->get_reflectionCoeff();
    Point_3D nextIntersection = nextIntersectionPair.value().first;
    Object* nextIntersectedObject = nextIntersectionPair.value().second;
    rgb lightIntensity = determineLightIntensity_Shadow(nextIntersection, nextIntersectedObject, lights, objects);
    // On ne fait pas réfléchir le rayon une autre fois sur l'objet, donc on utilise bien la fonctino ..._Shadow
    return reflectionCoeff * lightIntensity;
}



// TODO : gros poblème, on prend ne comprte la lumière qui arrive sur l'objetsur lequel on rebondit mais pas sa couleur !!!!!!!!

rgb determineLightIntensity(Point_3D intersection, Object* intersected_object, Camera* cam, std::vector<Light>* lights, std::vector<Object*>* objects, Ray incidentRay) {
    rgb lightIntensityShadow = determineLightIntensity_Shadow(intersection, intersected_object, lights, objects);
    rgb lightIntensityReflection = determineLightIntensity_Reflection(intersection, intersected_object, lights, objects, incidentRay);
    rgb lightIntensityAmbient = cam->get_ambient_light();
    return minTuple(lightIntensityReflection + lightIntensityShadow + lightIntensityAmbient, { 1,1,1 }); // TODO : Enlever le min ?
}


rgb compute_color(Ray myRay, Camera* cam, std::vector<Object*>* objects, std::vector<Light>* lights) {
    auto nextIntersect = findNextIntersection(myRay, objects);
    if (!nextIntersect.has_value()) {
        return { 0,0,0 };
    }
    else {
        Point_3D intersection = nextIntersect.value().first;
        Object* intersected_object = nextIntersect.value().second;
        rgb lightIntensity = determineLightIntensity(intersection, intersected_object, cam, lights, objects, myRay);
        auto object_color = intersected_object->get_color();
        return { lightIntensity * object_color };

    }

}


void image_creator(Camera* cam, std::vector<Object*>* objects, std::vector<Light>* lights, std::string filename) {
    // This function creates an image of the scene and saves it as a ppm file

    // We open the ppm file as a stream
    std::ofstream file(filename); // We create the file

    if (!file.is_open()) {
        std::cerr << "Impossible to open the file " << filename << std::endl;
        return;
    }

    file << "P3" << std::endl;
    file << cam->get_resolution_width() << " " << cam->get_resolution_height() << std::endl;
    file << "255" << std::endl;
    for (unsigned i = 0; i < cam->get_resolution_height(); i++) {
        for (unsigned j = 0; j < cam->get_resolution_width(); j++) {
            // We lauch the ray from the camera
            Ray initial_Ray = cam->ray_launcher(cam->get_resolution_height() - i - 1, j);
            // We calculate the color of the pixel 
            rgb colors = compute_color(initial_Ray, cam, objects, lights);
            // We write the color in the file 
            file << static_cast<int>(std::get<0>(colors) * 255) << " " << static_cast<int>(std::get<1>(colors) * 255) << " " << static_cast<int>(std::get<2>(colors) * 255) << " ";
            file.flush();
        }
        file << std::endl;
        if (i % 100 == 0) {
            std::cout << "\rLine " << i << " done";
            std::cout.flush();
        }
    }
    file.close();
    std::cout << "\rThe image has been created and saved as " << filename << std::endl;
    return;
}





#endif