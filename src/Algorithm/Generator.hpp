#ifndef GENERATOR_H
#define GENERATOR_H

#include <memory>
#include <vector>
#include "Types.hpp"
#include "Tensor.hpp"
#include "InputData.hpp"
#include "SearchGraphNode.hpp"

/**
 * Finds the best possible way to distribute one ride to each car given
 * a list of unassigned rides and a list of cars with their positions gained points and points in time.
 */
class Generator {
public:

    /**
     * Creates a generator of choices on given input data.
     *
     * \param [in] inputData used internally to get specific information about rides
     */
    Generator(InputData& inputData): inputData(inputData) {};

    /**
     * Given the current state of the search process this generates the next optimal choice to take.
     *
     * \note This method should be called repeatedly using its result until the searchpath is complete
     *
     * \param [in,out] unassigned a list of indices of all unassigned rides
     * \param [in,out] cars represents the current state of all cars
     * \param [in] prevNode is referenced by the new SearchGraphNode of the returned Choice
     * \return std::unique_ptr<Types::Choice> optimal way to assign one ride to each car
     */
    std::shared_ptr<SearchGraphNode> generate(std::vector<int>& unassigned,
            Types::CarData& cars,
            std::shared_ptr<SearchGraphNode>& prevNode);

private:

    /**
     * Given a list of unassigned rides and current state of each car, this method
     * calculates finish times and finish points for each car taking each unassigned ride
     *
     * \param [out] finishTimes CxR matrix (C:=numer of cars, R:=number of unassigned rides)
     * \param [out] finishPoints CxR matrix (C:=numer of cars, R:=number of unassigned rides)
     * \param [out] scores CxR RLA Matrix to be solved to assign rides optimally (C:=numer of cars, R:=number of unassigned rides)
     * \param [in] a list of indices of all unassigned rides
     * \param [in] represents the current state of all cars
     * \return int maximum value in finishPoints (used for hungarian algorithm)
     */
    void createSearchSpace(Tensor<int>& finishTimes,
                           Tensor<int>& finishPoints,
                           Tensor<int>& scores,
                           std::vector<int>& unassigned,
                           Types::CarData& cars);

    /**
     * Solves the rectangular linear assignment problem of choosing the maximal scores and
     * generates a choice out of it
     *
     * \param [out] cars represents the current state of all cars
     * \param [in,out] unassigned a list of indices of all unassigned rides
     * \param [in] prevNode is referenced by the new SearchGraphNode of the returned Choice
     * \param [in] finishTimes finish times os all cars taking all rides
     * \param [in] finishPoints gained points for all cars taking all rides
     * \param [in] scores RLA Matrix to be solved to assign rides optimally
     * \return std::shared_ptr<SearchGraphNode> optimal way to assign one ride to each car
     */
    std::shared_ptr<SearchGraphNode> selectFromSearchSpace(Types::CarData& cars,
            std::vector<int>& unassigned,
            std::shared_ptr<SearchGraphNode>& prevNode,
            Tensor<int>& finishTimes,
            Tensor<int>& finishPoints,
            Tensor<int>& scores);

    /** Used internally to get specific information about rides. */
    InputData& inputData;
};

#endif // GENERATOR_H
