A rendre :
- PDF de la présentation (diapositives)
- code source (lien GitHub)

Diapos :
1. présentation du projet
2. architecture du SW (logiciel)
3. démo

Etape 1 :
- afficher objet simple (sphère)
- surface simple (non texturée)
- générer rayon réfléchi et/ou diffracté à chaque surface


Etape 1 
Phase A
Différentes méthodes :
- ray tracing
- les rayons partent ducaméra/oeil pour aller vers la source lumineuse (principe de retour inverse)
- source lumineuse : ce sont des points dans l'espace, la plupart des rayons partant de la source se retrouveraient hors de la source. Il y aurait trop de calculs inutiles.

Calculs vectoriels :
- intersection d'une droite et d'un plan : utilisation des équations paramétriques/cartésiennes de la droite et du plan

Algorithmes :
- Ray-tracing 
- Ray casting algorithm
- Volume ray casting algorithm
- SDF ray marching algorithm
- Recursive ray tracing algorithm

Rayons :
- réflexion
- réfraction (transparence)
- ombre

Représentation d'une scène d'objet :
- sphère
- plan
- cube
- intersection et union d'objets
- objet défini par maillages

Objet :
- forme
- surface

Surface :
- niveau de transparence
- niveau de réflexion
- texture

XML :
- structured text file used to store and transport data in a format that is both human-readable and machine-readable

Blender is a free and open-source 3D computer graphics software tool.

### Analyse d'usage




Contexte d'utilisation :
- L'utilisateur précise dans un fichier texte la position de la caméra et des objets.
- Le programme génère un fichier image PPM que l'utilisateur peut lire grâce à un outil externe.

Choix d'utiliser PPM :
- bien qu'inefficace comme format (contient bcp d'informations que l'oeil ne discerne pas)
- it is very easy to write and analyze programs to process this format
- plus d'informations : https://netpbm.sourceforge.net/doc/ppm.html

### Analyse fonctionnelle

Fonctions :
- Une fonction qui lance les rayons initiaux en partant de la caméra, pour chacun des pixels de l'image. Cette fonction peut être parallélisée (plusieurs threads qui s'occupent chacun d'une partie différente de l'image).
- Une fonction qui suit un rayon jusqu'à l'intersection avec un objet. Quand on atteint un objet, il faut lancer les rayons de réflection, de réfraction, et les rayons d'ombre. Les rayons d'ombre sont des rayons que l'on envoie vers chaque source de lumière pour savoir si l'objet est éclairé ou non.
- Pour chacun des objets différents (sphère, plan, etc), il faudra des fonctions permettant de calculer les conditions d'intersection et les directions des rayons réfléchis et réfractés. Il faudra aussi une fonction qui calcule la couleur finale du pixel, en fonction des propriétés du matériel (couleur, transparence, etc)

Interactions : 
- La fonction qui lance les rayons à partir de la caméra doit interagir avec la fonction de suivi de rayon, qui renvoie la couleur du pixel
- la fonction de suivi de rayon interagit avec les fonctions de calcul d'intersection. On doit définir l'ensemble des objets que le rayon est susceptible de rencontrer afin de pouvoir tester les intersections au fur et à mesure. Nous utiliserons des scènes simples avec peu d'objets au début, donc nous considérerons l'ensemble des objets pour le suivi du rayon.

### Architecture gros grains

Différentes classes :
- Image : contient les informations sur l'image (taille, résolution, liste des pixels)
- Camera : information sur la caméra (position, direction)
- Scene : ensemble des objets de la scène
- LightSource : objet représentant une source de lumière. On pourra prendre une source de lumière ponctuelle
- Object : classe mère de tous les objets; forme de la surface, texture de la surface
- Des classes spécifiques à chaque objet
- Ray : classe qui contient les informations sur les rayons (origine, direction, etc)
- Une classe vecteur ????

Où mettre les informations relatives à l'écran ? elles sont implicites dans les données de la caméra ???? Ça me parait pas mal, genre la caméra a une position, une direction d'observation, une résolution genre 600x400, et un angle de vue. On peut alors en déduire l'ensemble des rayons à lancer.




#### Rayons et vecteurs

Il faut faire attention aux unités (gradients, degrés pour les angles)

#### Caméra 

Dans `ray_launcher`, on doit faire attention à ne pas faire de division d'entiers unsigned, qui fausseraient les calculs. On doit donc faire des cast vers des flottants pour la résolution et le champ de vision.

#### Plan : calcul de l'intersection avec un rayon
Héritage ? Nécessité des opérateurs * (produit scalaire) et - entre deux Point_3D pour créer un vecteur.
