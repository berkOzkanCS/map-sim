#include "sim.hpp"


void CylinderStrategy::collisionCheck(Eigen::Vector3d v) {

}

void DomeStrategy::collisionCheck(Eigen::Vector3d v) {
    
}

Threat::Threat(Eigen::Vector3d c, int r, int h, std::unique_ptr<GeneratorStrategy> initStrategy)
    : center(c), R(r), H(h), strategy(std::move(initStrategy)) {}


void Threat::setStrategy(std::unique_ptr<GeneratorStrategy> newStrategy) {
    strategy = std::move(newStrategy);
}

Eigen::Vector3d Threat::getCenter() const {
    return center; 
}

void Threat::setCenter(const Eigen::Vector3d& c) { 
    center = c; 
}

Map::Map()
    : bounds(Eigen::Vector3d(0, 0, 0)),
      threats(),
      startPos(Eigen::Vector3d(0, 0, 0)),
      goals()
{}

Map::Map(Eigen::Vector3d b,
         std::vector<Threat> t,
         Eigen::Vector3d sP,
         std::vector<Eigen::Vector3d> g)
    : bounds(b),
      threats(std::move(t)),
      startPos(sP),       
      goals(std::move(g))
{}

Map::Map(Eigen::Vector3d b,
         Eigen::Vector3d sP,
         std::vector<Eigen::Vector3d> g,
         int numberOfThreats)
    : bounds(b),
      startPos(sP),      
      goals(std::move(g))
{
    std::random_device rd;
    std::mt19937 rng(rd());

    if (numberOfThreats <= 0) {
        std::uniform_int_distribution<int> dist(5, 10);
        numberOfThreats = dist(rng);  
    }

    std::uniform_real_distribution<double> ff(0.0, 1.0);
    std::uniform_real_distribution<double> centerX(0.0, bounds.x());
    std::uniform_real_distribution<double> centerY(0.0, bounds.y());
    std::uniform_int_distribution<int> Rdist(((bounds.x()+bounds.y())/2)*0.01, ((bounds.x()+bounds.y())/2)*0.05);
    std::uniform_int_distribution<int> Hdist(bounds.z()*0.05, bounds.z()*0.2);

    for (int i = 0; i < numberOfThreats; ++i) {
        Eigen::Vector3d pos(centerX(rng), centerY(rng), 0);
        int radius = Rdist(rng);
        int height = Hdist(rng);

        std::unique_ptr<GeneratorStrategy> strat;
        if (ff(rng) < 0.5)
            strat = std::make_unique<CylinderStrategy>();
        else
            strat = std::make_unique<DomeStrategy>();

        Threat t(pos, radius, height, std::move(strat));
        threats.push_back(std::move(t));
    }
}


void Map::readMap(const std::string& filePath, int mapWidth, int mapHeight) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open heightmap file: " + filePath);
    }

    std::vector<unsigned char> buffer(mapWidth * mapHeight, 0); 
    file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

    heightmap.assign(mapHeight, std::vector<int>(mapWidth, 0));
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            int idx = y * mapWidth + x;
            if (idx < buffer.size()) {
                heightmap[y][x] = buffer[idx];
            } else {
                heightmap[y][x] = 0;
            }
        }
    }

    // bounds = Eigen::Vector3d(mapWidth, mapHeight, 255);

    for (auto& threat : threats) {
        Eigen::Vector3d c = threat.getCenter();
        int xi = static_cast<int>(c.x());
        int yi = static_cast<int>(c.y());
        // std::cout << "hm[xy]: " << heightmap[yi][xi] << "cz " << c.z() << "\n";
        if (xi >= 0 && xi < mapWidth && yi >= 0 && yi < mapHeight) {
            c.z() = heightmap[yi][xi] + c.z() + threat.getH()/2;
        } else {
            c.z() = 0;
        }
        std::cout << "xi: " << xi << " yi: " << yi << " hm[xy]: " << heightmap[yi][xi] << " cz-h/2 " << c.z() - threat.getH()/2 << "\n";
        // std::cout << "Resulting H: " << c.z() << "\n";

        threat.setCenter(c);
    }
}

void Map::saveToFile(const std::string& filePath) const {
    std::ofstream out(filePath);
    if (!out) throw std::runtime_error("Cannot open file for writing: " + filePath);

    // Write bounds
    out << bounds.x() << " " << bounds.y() << " " << bounds.z() << "\n";

    // Write startPos
    out << startPos.x() << " " << startPos.y() << " " << startPos.z() << "\n";

    // Write goals
    out << goals.size() << "\n";
    for (const auto& g : goals)
        out << g.x() << " " << g.y() << " " << g.z() << "\n";

    // Write heightmap size
    // int rows = heightmap.size();
    // int cols = (rows > 0) ? heightmap[0].size() : 0;
    // out << rows << " " << cols << "\n";

    // Write heightmap
    // for (const auto& row : heightmap) {
    //     for (int h : row) out << h << " ";
    //     out << "\n";
    // }

    // Write threats
    out << threats.size() << "\n";
    for (const auto& t : threats) {
        Eigen::Vector3d c = t.getCenter();
        out << c.x() << " " << c.y() << " " << c.z() << " "
            << t.getR() << " " << t.getH() << "\n";
    }
}
