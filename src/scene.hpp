#ifndef sceneHPP
#define sceneHPP

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <algorithm>
#include <cstdint>

#include "objects.hpp"



std::optional<std::pair<Point_3D, Object*>> findNextIntersection(const Ray& myRay, const std::vector<Object*>* objects) {
    // A partir d'un rayon donné (qui arrive à la caméra), lui associer un RGB en fonction des obstacles que rencontre le rayon.

    // Rayon d'ombre
    // On détermine l'intersection la plus proche de la caméra avec un objet
    std::vector<Object*>::const_iterator it = objects->begin();
    std::optional<Point_3D> closestIntersec = (*it)->find_intersection(myRay);
    Object* closestObjectPt = *it;
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



// On la déclare ici pour éviter les dépendances circulaires
rgb determineLightIntensity(const Point_3D& intersection, const Object* intersected_object, const rgb& ambient_light, const std::vector<Light*>* lights, const std::vector<Object*>* objects, const Ray& incidentRay, int depth);



rgb determineLightIntensity_Diffuse(const Point_3D& intersection, const Object* intersected_object, const std::vector<Light*>* lights, const std::vector<Object*>* objects) {

    float diffuseCoeff = intersected_object->get_diffuseCoeff();
    if (diffuseCoeff == 0.f) { // In s'arrête là, pas besoin de faire tous les calculs
        return { 0.f,0.f,0.f };
    }

    std::vector<Light*>::const_iterator it = lights->begin();

    Vector_3D normal = intersected_object->get_normal(intersection);

    rgb  lightIntensityShadow = { 0.f,0.f,0.f }; // Initialisation
    // Rayons d'ombre
    for (;it != lights->end(); it++) { // Pour chaque source de lumière
        Ray nextRay(intersection, Vector_3D(intersection, (*it)->get_position()));
        auto nextIntersection = findNextIntersection(nextRay, objects);
        rgb  currentIntensity;
        if (nextIntersection.has_value() && nextIntersection.value().first.distanceTo(intersection) > THRESHOLD && nextIntersection.value().first.distanceTo(intersection) < (*it)->get_position().distanceTo(intersection)) {
            // Si le rayon rencontre un objet avant la source de lumière
            currentIntensity = { 0.f,0.f,0.f };
        }
        else {
            currentIntensity = (*it)->compute_PointLight(intersection, normal);
            lightIntensityShadow = lightIntensityShadow + currentIntensity;
        }
    }
    return diffuseCoeff * lightIntensityShadow; // On multiplie par le coefficient de réflexion diffuse
}

rgb determineLightIntensity_Specular(const Point_3D& intersection, const Object* intersected_object, const rgb& ambient_light, const std::vector<Light*>* lights, const std::vector<Object*>* objects, const Ray& incidentRay, int depth) {
    // Rayon réfléchi comme sur un miroir

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


rgb determineLightIntensity(const Point_3D& intersection, const Object* intersected_object, const rgb& ambient_light, const std::vector<Light*>* lights, const std::vector<Object*>* objects, const Ray& incidentRay, int depth) {
    if (depth == 0) { // On a atteint la profondeur maximale de calcul
        return { 0.f,0.f,0.f };
    }

    // On calcule les intensités lumineuses dues aux différents phénomènes

    rgb lightIntensityShadow = determineLightIntensity_Diffuse(intersection, intersected_object, lights, objects);
    rgb lightIntensityReflection = determineLightIntensity_Specular(intersection, intersected_object, ambient_light, lights, objects, incidentRay, depth);
    return (lightIntensityReflection + lightIntensityShadow + ambient_light);
}


rgb compute_color(const Ray& myRay, const Camera* cam, const std::vector<Object*>* objects, const std::vector<Light*>* lights) {
    // Fonction qui cherche une intersection et calcule la couleur en appelant récursivement les fonctions de lumière
    auto nextIntersect = findNextIntersection(myRay, objects);
    if (!nextIntersect.has_value()) {
        return { 0.f,0.f,0.f };
    }
    else {
        Point_3D intersection = nextIntersect.value().first;
        Object* intersected_object = nextIntersect.value().second;
        rgb lightIntensity = determineLightIntensity(intersection, intersected_object, cam->get_ambient_light(), lights, objects, myRay, cam->get_reflection_depth() + 1); // On calcule l'intensité lumineuse reçue en considérant la profondeur de calcul
        auto object_color = intersected_object->get_color();
        auto value = lightIntensity * object_color;
        return { lightIntensity * object_color };

    }

}


void image_creator(const Camera* cam, const std::vector<Object*>* objects, const std::vector<Light*>* lights, const std::string& filename) {
    // Cette fonction crée une image de la scène et la sauvegarde en tant que fichier ppm

    std::ostringstream buffer; // Buffer pour ne pas faire trop d'I/O

    // Header du fichier ppm
    buffer << "P3" << std::endl;
    buffer << cam->get_resolution_width() << " " << cam->get_resolution_height() << std::endl;
    buffer << "255" << std::endl;

    for (unsigned i = 0; i < cam->get_resolution_height(); i++) {
        for (unsigned j = 0; j < cam->get_resolution_width(); j++) {

            // On calcule un rayon depuis la caméra
            Ray initial_Ray = cam->ray_launcher(cam->get_resolution_height() - i - 1, j);

            // On cherche la couleur associée à ce pixel
            rgb colors = compute_color(initial_Ray, cam, objects, lights);
            colors = minTuple(colors, { 1.f,1.f,1.f }); // On cappe les valeurs à 1

            // On écrit les valeurs dans le buffer
            buffer << static_cast<int>(std::get<0>(colors) * 255.) << " " << static_cast<int>(std::get<1>(colors) * 255.) << " " << static_cast<int>(std::get<2>(colors) * 255.) << " ";
        }

        buffer << std::endl;
        if (i % 100 == 0) {
            std::cout << "\rLigne " << i << "/" << cam->get_resolution_height() << " calculée";
            std::cout.flush();
        }
    }

    // On ouvre le fichier et on écrit le buffer dedans
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Impossible d'ouvrir le fichier " << filename << std::endl;
        return;
    }
    file << buffer.str();
    file.close();
    std::cout << "\rImage sauvegardée : " << filename << std::endl;
    return;
}


#endif