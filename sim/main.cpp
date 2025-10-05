#include "sim.hpp"
#include <iostream>

int main() {

    Map world(Eigen::Vector3d(505, 505, 500), Eigen::Vector3d(10, 10, 10), {Eigen::Vector3d(480, 480, 480)}, 15);

    world.readMap("heightmap.bin", 505, 505);

    world.pathGenerator(15, ' ');

    // world.printPath();

    world.saveToFile("map_data.txt");

    return 0;
}