#include <numeric>
#include <cmath>
#include "Algorithm.hpp"

bool Algorithm::isCarDataValid(Types::CarData& carData, unsigned T) {
    bool result = true;

    for(int i = 0; i < carData.t.getSize(); i++) {
        if(carData.t(i) > T) {
            result = false;
            break;
        }
    }

    return result;
}

void Algorithm::printStatus(const std::vector<int>& unassigned) {
    std::cout << std::round((inputData.nRides - unassigned.size()) * 100.0 / inputData.nRides)
              << "% of rides are assigned." << std::endl;
}

void Algorithm::run() {
    std::vector<int> unassigned(inputData.nRides);
    std::iota(unassigned.begin(), unassigned.end(), 0);
    Types::CarData cars(inputData.fleetSize);
    std::shared_ptr<SearchGraphNode> root;

    std::shared_ptr<SearchGraphNode> currentNode = generator.generate(unassigned, cars, root);

    printStatus(unassigned);

    while(unassigned.size() > 0 && isCarDataValid(cars, inputData.maxTime)) {
        currentNode = generator.generate(unassigned, cars, currentNode);
        printStatus(unassigned);
    }

    unsigned score = 0;

    for(int i = 0; i < inputData.fleetSize; ++i) {
        score += cars.p(i);
    }

    std::cout << "Final score: " << score << std::endl;

    std::cout << "Writing output to: " << outputPath << std::endl;
    currentNode->writeToFile(outputPath, inputData.fleetSize);
}
