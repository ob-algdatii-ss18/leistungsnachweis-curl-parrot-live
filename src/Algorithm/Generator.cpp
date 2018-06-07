#include <cmath>
#include <numeric>
#include <algorithm>
#include <set>
#include "Generator.hpp"
#include "RLAPSolverHungarian.hpp"
#include "RLAPSolverJV.hpp"

void Generator::createSearchSpace(Tensor<int>& finishTimes,
                                  Tensor<int>& finishPoints,
                                  Tensor<int>& scores,
                                  std::vector<int>& unassigned,
                                  Types::CarData& cars) {
    // For each unassigned ride, calculate times and points for all cars
    auto ride = unassigned.begin();

    for(unsigned uIndex = 0; uIndex < unassigned.size(); ++uIndex, ++ride) {
        // distance of current ride
        int rideDistance = inputData.distances(*ride);
        // earliest possible start of current ride
        int earliestStart = inputData.startT(*ride);
        // latest possible finish of current ride
        int latestFinish = inputData.endT(*ride);

        for(unsigned car = 0; car < inputData.fleetSize; ++car) {

            // current time of the car
            int carTime = cars.t(car);
            // distance to ride starting point
            int ariveDistance = std::abs(cars.x(car) - inputData.startX(*ride)) +
                                std::abs(cars.y(car) - inputData.startY(*ride));
            // point in time when the ride can start (car arrived)
            int startTime = std::max(carTime + ariveDistance, earliestStart);

            finishTimes(car, uIndex) = startTime + rideDistance;
            finishPoints(car, uIndex) = cars.p(car);

            // ride ended in time (points gained)
            if(latestFinish >= finishTimes(car, uIndex)) {
                finishPoints(car, uIndex) += rideDistance;
            }

            // ride started in time (bonus gained)
            if(earliestStart == startTime) {
                finishPoints(car, uIndex) += inputData.bonus;
            }

            scores(car, uIndex) = std::max(static_cast<int>(inputData.maxTime) - finishTimes(car,
                                           uIndex), 0) + finishPoints(car, uIndex);
        }
    }
}

std::shared_ptr<SearchGraphNode> Generator::selectFromSearchSpace(Types::CarData& cars,
        std::vector<int>& unassigned,
        std::shared_ptr<SearchGraphNode>& prevNode,
        Tensor<int>& finishTimes,
        Tensor<int>& finishPoints,
        Tensor<int>& scores) {
    // no possible choices left
    const unsigned nAssignments = std::min(inputData.fleetSize,
                                           static_cast<unsigned>(unassigned.size()));

    if(nAssignments == 0) { return {}; }

    // solve RLAP to maximize gained points
    //RLAPSolverHungarian solver(scores, maxScore);
    RLAPSolverJV solver(scores);
    std::unique_ptr<Tensor<unsigned>> searchNodeValue(new Tensor<unsigned>({ nAssignments, 2 }));
    solver.solve(*searchNodeValue);

    // more efficient structure to remove entries from
    std::set<int> newUnassigned(unassigned.begin(), unassigned.end());
    float timeSum = 0;
    float pointSum = 0;

    // update cars
    for(unsigned i = 0; i < nAssignments; ++i) {
        const int car = (*searchNodeValue)(i, 0);
        const int uIndex = (*searchNodeValue)(i, 1);
        const int ride = unassigned[uIndex];
        newUnassigned.erase(ride);

        cars.x(car) = inputData.endX(ride);
        cars.y(car) = inputData.endY(ride);
        cars.t(car) = finishTimes(car, uIndex);
        cars.p(car) = finishPoints(car, uIndex);

        (*searchNodeValue)(i, 1) = ride;
    }

    // update unassigned vector (without rides that have been assigned)
    unassigned.assign(newUnassigned.begin(), newUnassigned.end());
    return std::make_shared<SearchGraphNode>(prevNode, searchNodeValue);
}

std::shared_ptr<SearchGraphNode> Generator::generate(std::vector<int>& unassigned,
        Types::CarData& cars,
        std::shared_ptr<SearchGraphNode>& prevNode) {
    // Time after each car was assigned to each unassigned ride (2D matrix)
    Tensor<int> finishTimes({ inputData.fleetSize, static_cast<unsigned>(unassigned.size()) });
    // Points after each car was assigned to each unassigned ride (2D matrix)
    Tensor<int> finishPoints({ inputData.fleetSize, static_cast<unsigned>(unassigned.size()) });
    // Scores of each car beeing assigned to each ride
    Tensor<int> scores({ inputData.fleetSize, static_cast<unsigned>(unassigned.size()) });

    createSearchSpace(finishTimes, finishPoints, scores, unassigned, cars);
    return selectFromSearchSpace(cars, unassigned, prevNode, finishTimes, finishPoints,
                                 scores);
};
