Multiplayer game using C++11.

This project is **old** and is not a **complete game**, but it still is functional and contains the following features:
* Multiplayer games, with latency compensation techniques so that user experience remains smooth even with high latencies.
* A map editor, to create & modify the isometric maps easily (not very user friendly yet).
* Collisions and shadowcasting (a player cannot see another player if there is an obstacle between them). The original goal of this project was to create a game in which stealth was the key to success.
* Spells (well, a single spell for now, press 1 then click in the desired direction).
* A custom UI based on widgets. No external dependency required.

# How to build this project

* First, you have to download and build my [modified version of SFML](https://github.com/Senzaki/SFML), which includes hardware scancodes support for international keyboard.
* Then, run CMake. Ensure that SFML-related variables point to the modified version of SFML if you did not install it, or if you have another version installed on your system. You might have to toggle CMake's advanced mode if you want to modify the paths of the SFML libraries manually.
* Build the project.

