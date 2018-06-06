#include <numeric>
#include <cmath>
#include "Algorithm.hpp"

#ifndef NDEBUG
#include <iostream>
#define DBG_PRINT_CHOICE(choice) std::cout << "Choice:" << std::endl; \
        DBG_PRINT_1D_TENSOR("x", choice->cars.x); \
        DBG_PRINT_1D_TENSOR("y", choice->cars.y); \
        DBG_PRINT_1D_TENSOR("t", choice->cars.t); \
        DBG_PRINT_1D_TENSOR("p", choice->cars.p); \
        std::cout << "Score: " << choice->score << std::endl
#else
#define DBG_PRINT_CHOICE(_)
#endif

bool Algorithm::isCarDataValid(Types::CarData carData, unsigned T) {
	bool result = false;

	for (int i = 0; i < carData.t.getSize(); i++) {
		if (carData.t(i) < T) {
			result = true;
			break;
		}
	}

	return result;
}

void Algorithm::run() {

	std::vector<int> unassigned(inputData.nRides);
	std::iota(unassigned.begin(), unassigned.end(), 0);
	std::unique_ptr<Types::CarData> cars(new Types::CarData(inputData.fleetSize));
	std::shared_ptr<SearchGraphNode> parentNode;

	std::unique_ptr<Types::Choice> choice = generator.generate(parentNode, unassigned, std::move(cars));

	std::cout << std::round((inputData.nRides - choice->unassigned.size()) * 100.0 /
		inputData.nRides) << "% of rides are assigned." << std::endl;

	while (choice->unassigned.size() > 0 && isCarDataValid(choice->cars, inputData.maxTime)) {
		std::unique_ptr<Types::CarData> newCars(new Types::CarData(choice->cars));
		choice = generator.generate(choice->searchGraphNode, choice->unassigned, std::move(newCars));
		std::cout << std::round((inputData.nRides - choice->unassigned.size()) * 100.0 /
			inputData.nRides) << "% of rides are assigned." << std::endl;
	}

	std::cout << "All rides got assigned or all cars reached the end time." << std::endl;
	std::cout << "Printing end state:" << std::endl;
	DBG_PRINT_CHOICE(choice);

	unsigned googleScore = 0;
	for (int i = 0; i < inputData.fleetSize; ++i) {
		googleScore += choice->cars.p(i);
	}
	std::cout << "Final score: " << googleScore << std::endl;

	std::cout << "Writing output file.." << std::endl;
	choice->searchGraphNode->writeToFile(outputPath, inputData.fleetSize);
}
