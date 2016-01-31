Multiplayer game using C++11. One of the objectives is to deploy latency compensation techniques, so that the player enjoys the experience even with high latencies.
This project is **old** and is not a **complete game**, but it still is functional and contains the aforementioned techniques.

# How to build this project

* First, you have to download and build my [modified version of SFML](https://github.com/Senzaki/SFML), which includes hardware scancodes support for international keyboard.
* Then, run CMake. Ensure that SFML-related variables point to the modified version of SFML if you did not install it, or if you have another version installed on your system. You might have to toggle CMake's advanced mode if you want to modify the paths of the SFML libraries manually.
* Build the project.

