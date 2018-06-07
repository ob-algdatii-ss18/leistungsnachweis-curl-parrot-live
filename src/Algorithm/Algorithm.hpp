#ifndef ALGORITHM_H
#define ALGORHITM_H

#include <string>
#include <vector>
#include "InputData.hpp"
#include "Generator.hpp"

/**
 * Algorithm solving the problem statement of the HashCode online quallification round 2018
 * by build a search graph containing available choices.
 * Only the best choices are taken into consideration until there all rides has been assigned or no new ones can be assigned anymore.
 */
class Algorithm {
public:

    /**
     * Prepare the algorithm by intalizing its parameter.
     *
     * \param [in]	inputData	InputData representing the simulation enviroment.
     * \param [out]	outputPath	Path of output file containing the calculated solution.
     */
    Algorithm(InputData& inputData, std::string outputPath) :
        inputData(inputData), outputPath(outputPath), generator(inputData) {};

    /**
     * Run the algorithm, until all rides have been assigned or no new ones can be assigned anymore.
     * Best choice is writen to the outputPath.
     */
    void run();

private:

    /**
     * Check if carData is still within a given simulation time.
     *
     * \param [in]	carData	CarData to check.
     * \param [in]	T		Max. valid simulation time.
     * \return bool true if carData is valid, otherwise false.
     */
    bool isCarDataValid(Types::CarData& carData, unsigned T);

    /**
     * Prints the percentage of assigned rides
     *
     * \param unassigned indices of all unassigned rides
     */
    void printStatus(const std::vector<int>& unassigned);

    /** InputData representing the simulation enviroment. */
    InputData& inputData;
    /** Path of output file. */
    std::string outputPath;
    /** Generator trying to find the best choices. */
    Generator generator;
};

#endif // ALGORITHM_H
