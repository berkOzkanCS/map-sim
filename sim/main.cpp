#include "sim.hpp"
#include <iostream>

int main() {

    Map world(Eigen::Vector3d(500, 500, 500), Eigen::Vector3d(10, 10, 10), {Eigen::Vector3d(480, 480, 480)}, 5);

    world.readMap("heightmap.bin", 500, 500);
    world.saveToFile("map_data.txt");

    return 0;
}