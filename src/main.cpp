#include <iostream>
#include <fstream>
#include <memory>
#include <numeric>
#include "InputData.hpp"
#include "Algorithm.hpp"

/**
 * Solution to the problem statement of the HashCode online quallification round 2018.
 * \see https://hashcode.withgoogle.com/2018/tasks/hashcode2018_qualification_task.pdf
 */
int main(int argc, char *argv[]) {
    if(argc != 3) {
        std::cerr << "No input path and output path set." << std::endl;
        return 1;
    }

    std::shared_ptr<InputData> inputData;

    try { inputData = InputData::genFromFile(argv[1]); }
    catch(const std::fstream::failure e) {
        std::cerr << e.what() << "(Invalid file or path)" << std::endl;
        return 1;
    }

    std::string outputPath = argv[2];
    Algorithm algorithm(*inputData, outputPath);
    algorithm.run();

    return 0;
}
