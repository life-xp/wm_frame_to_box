# wm_frame_to_box

## Description

ROS node receiving 2D bounding boxes and convert them to 3D bounding boxes using camera depthcloud

Tested to work with [ROS] kinetic and Ubuntu 16.04. This code may change in time and is licensed under [MIT license](LICENSE).

**Author(s): Philippe La Madeleine
Mainteneur: Philippe La Madeleine  
Affiliation: Club étudiant Walking Machine, ÉTS**

## Installation

### Buildé de la source

#### Dépendances

- [Robot Operating System (ROS)](http://wiki.ros.org) (middleware pour la robotique),
- [sara_msg](https://github.com/WalkingMachine/sara_msgs) (repo pour les messages utilisé par sara),
- [Openni] (camera 3d)
- [Open_cv] (vision artificielle)


#### Building

Pour build de la source, clonez la dernière version de ce repo dans votre workspace et compilez avec

	cd catkin_workspace/src
	git clone git@github.com:WalkingMachine/wm_frame_to_box.git
	cd ../
	catkin_make

## Utilisation

Lancer le launchfile avec

	roslaunch wm_frame_to_box wm_frame_to_box.launch


## Nodes

### frame_to_box

Reçoit le depth de la camera et les cadres 2D. Retourne des boites en 3D.


#### Topics Souscris*

* **`/head_xtion/depth/image`** ([sensor_msgs/Image])

	L'image de profondeur de la camera 3d.

* **`/darknet_ros/bounding_boxes`** ([darknet_ros_msgs/BoundingBoxes])

	Les cadres 2D

#### Topics Publiés*

* **`/frame_to_box/bounding_boxes`** ([wm_frame_to_box/BoundingBoxes3D])

	Les boites 3d obtenues

#### Service

* **`/frame_to_box/GetBoundingBoxes3D`** ([wm_frame_to_box/GetBoundingBoxes3D])

	Recois une depth Image et une liste de BoundingBoxes2D.
	- sara_msgs/BoundingBoxes2D boundingBoxes2D
	- sensor_msgs/Image image
	- string input_frame
	- string output_frame

	Retourne une liste des BoundingBoxes3D
	- sara_msgs/BoundingBoxes3D boundingBoxes3D

#### Paramètres
* **`auto_publisher`** (bool, default: true)

	Faut-il souscrire et publier automatiquement aux topic. Le service fonctionne quand même.

* **`camera_angle_width`** (float, default: 0.785398163397)

	Largeur en radians de l'ouverture de la camera

* **`camera_angle_height`** (float, default: 0.785398163397)

	Hauteur en radians de l'ouverture de la camera

* **`minimum_distance`** (float, default: 0.2)

	Distance minimale accepté en (m)

* **`maximum_distance`** (float, default: 50)

	Distance maximum accepté en (m)

* **`camera_topic`** (string, default: "/head_xtion/depth/image_raw")

	Topic où aller chercher le depth de la camera

* **`camera_frame`** (string, default: "head_xtion_depth_frame")

	Frame tf de la camera

* **`base_frame`** (string, default: "base_link")

	Frame tf de référence pour le message des boites 3d

* **`frame_ag`** (float, default: 0.0)

	Temps de décalage en (s) dans le passé où regarder pour les tf.

* **`yolo_topic`** (string, default: "/darknet_ros/bounding_boxes")

	Topic où aller chercher les cadres de darknet

* **`bounding_boxes_topic`** (string, default: "/frame_to_boxes/bounding_boxes")

	Topic où publier les boites 3d

* **`default_box_size`** (float, default: 0.1)

	Dimensions standards des boites
