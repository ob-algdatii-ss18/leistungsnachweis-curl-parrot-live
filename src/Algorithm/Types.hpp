#ifndef TYPES_H
#define TYPES_H

#include "Tensor.hpp"

namespace Types {

/**
 * Position, point in time and gained points of each car.
 */
struct CarData {
    /**
     * Creates a zero initialized CarData object for a given amount of cars.
     *
     * \param [in] size Number of cars
     */
    CarData(const unsigned size) :
        x({size}), y({size}), t({size}), p({size}) {
        for(unsigned i = 0; i < size; ++i) {
            x(i) = 0;
            y(i) = 0;
            t(i) = 0;
            p(i) = 0;
        }
    };

    /**
     * Creates a copy of an existing CarData object.
     *
     * \param [in] other Object to copy
     */
    CarData(CarData& other):
        x(other.x), y(other.y), t(other.t), p(other.p) {};

    /** X position of each car. */
    Tensor<int> x;
    /** Y Position of each car. */
    Tensor<int> y;
    /** Point in time for each car. */
    Tensor<int> t;
    /** Gained points of each car. */
    Tensor<int> p;
};

}

#endif // TYPES_H
