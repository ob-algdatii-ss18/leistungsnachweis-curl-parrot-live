#include <unordered_set>
#include <numeric>
#include <stdexcept>
#include "RLAPSolverHungarian.hpp"

#ifndef NDEBUG
#include <iostream>

/** Prints contents of a container of RLAPSolverHungarian::Zero human readable */
#define DBG_PRINT_ZEROS(v) std::cout << "zeros" << std::endl; \
    for(auto elm: v) \
        std::cout << '[' << elm.row \
        << ", " << elm.col << \
        ", " << elm.deleted << ']' << std::endl
#else
#define DBG_PRINT_ZEROS(_)
#endif

/** Used to determine the solver size needed internally */
#define MAX_DIM(m) std::max(m.getDims()[0], m.getDims()[1])

RLAPSolverHungarian::RLAPSolverHungarian(const Tensor<int>& mat, const int maxCost):
    rows(mat.getDims()[0]),
    cols(mat.getDims()[1]),
    size(MAX_DIM(mat)),
    costMat({MAX_DIM(mat), MAX_DIM(mat)}),
        rowMin({MAX_DIM(mat)}, maxCost),
        colMin({MAX_DIM(mat)}, maxCost),
solved(false) {
    // max cost changes when turning the minimize problem into a maximizing one
    int newMaxCost = 0;

    // turn cost matrix into profit matrix to find maxima
    // also find row and column minima for later use
    for(unsigned row = 0; row < rows; ++row) {
        for(unsigned col = 0; col < cols; ++col) {
            int& entry = costMat(row, col);
            entry = maxCost - mat(row, col);

            if(rowMin(row) > entry) {
                rowMin(row) = entry;
            }

            if(colMin(col) > entry) {
                colMin(col) = entry;
            }

            if(newMaxCost < entry) {
                newMaxCost = entry;
            }
        }
    }

    // fill dummy rows (in case input matrix is rectangular)
    if(rows != cols) {
        unsigned rowStart, colStart;

        if(rows > cols) {
            rowStart = 0;
            colStart = cols;

            for(unsigned col = colStart; col < size; ++col) {colMin(col) = newMaxCost;}
        } else {
            rowStart = rows;
            colStart = 0;

            for(unsigned row = rowStart; row < size; ++row) {rowMin(row) = newMaxCost;}
        }

        for(unsigned row = rowStart; row < size; ++row) {
            for(unsigned col = colStart; col < size; ++col) {
                costMat(row, col) = newMaxCost;
            }
        }
    }
}

void RLAPSolverHungarian::reduceRowsAndCols(Tensor<unsigned>& zeroCountRows,
        Tensor<unsigned>& zeroCountCols) {
    // used for row reduction
    std::unordered_set<unsigned> nonZeroCols;

    // initialize data structure
    for(unsigned i = 0; i < size; ++i) {
        zeroCountCols(i) = 0;
        zeroCountRows(i) = 0;
        nonZeroCols.insert(i);
    }

    // Reduce rows
    // update column minima
    // and store per row and per column zero counts
    for(unsigned row = 0; row < size; ++row) {
        for(unsigned col = 0; col < size; ++col) {
            int& entry = costMat(row, col);
            entry -= rowMin(row);

            if(entry == 0) {
                nonZeroCols.erase(col);
                ++zeroCountCols(col);
                ++zeroCountRows(row);
                zeros.push_back({row, col, false});
            } else if(colMin(col) > entry) {
                colMin(col) = entry;
            }
        }
    }

    // Reduce non zero columns
    // and store per row and per column zero counts
    for(unsigned col : nonZeroCols) {
        for(unsigned row = 0; row < size; ++row) {
            costMat(row, col) -= colMin(col);

            if(costMat(row, col) == 0) {
                ++zeroCountCols(col);
                ++zeroCountRows(row);
                zeros.push_back({row, col, false});
            }
        }
    }
}

unsigned RLAPSolverHungarian::coverZeros(Tensor<unsigned>& zeroCountRows,
        Tensor<unsigned>& zeroCountCols, Tensor<bool>& rowLines, Tensor<bool>& colLines) {
    // stores tracking markers for entries
    Tensor<bool> zeroCountRowsFlag({size}), zeroCountColsFlag({size});
    // value that marks entries to be tracked
    bool changedMarker = true;
    // tracked entries have changed
    bool changed;
    // number of lines
    unsigned lineCount = 0;

    // initialize data structure
    for(unsigned i = 0; i < size; ++i) {
        rowLines(i) = false;
        colLines(i) = false;
        zeroCountRowsFlag(i) = !changedMarker;
        zeroCountColsFlag(i) = !changedMarker;
    }

    do {
        changed = false;

        // for every zero that isn't deleted
        for(unsigned i = 0; i < zeros.size(); ++i) {
            if(zeros[i].deleted) {continue;}

            unsigned currentRow = zeros[i].row;
            unsigned currentCol = zeros[i].col;

            // check whether to draw a horizontal line
            // (more zeros in currentRow than in currentCol)
            // or a vertical line
            // (more zeros in currentCol than in currentRow)
            //
            // if the amount of zeros in currentCol and currentRow are equal,
            // mark currentRow and currentCol so that future changes on those
            // can trigger the changed flag
            if(zeroCountRows(currentRow) > zeroCountCols(currentCol)) {
                for(unsigned j = 0; j < zeros.size(); ++j) {
                    if(zeros[j].deleted) {continue;}
                    else if(zeros[j].row == currentRow) {
                        --zeroCountCols(zeros[j].col);
                        zeros[j].deleted = true;

                        if(zeroCountColsFlag(zeros[j].col) == changedMarker) {
                            changed = true;
                        }
                    }
                }

                ++lineCount;
                rowLines(currentRow) = true;

                zeroCountRows(currentRow) = 0;

                if(zeroCountRowsFlag(currentRow) == changedMarker) {
                    changed = true;
                }
            } else if(zeroCountRows(currentRow) < zeroCountCols(currentCol)) {
                for(unsigned j = 0; j < zeros.size(); ++j) {
                    if(zeros[j].deleted) {continue;}
                    else if(zeros[j].col == currentCol) {
                        --zeroCountRows(zeros[j].row);
                        zeros[j].deleted = true;

                        if(zeroCountRowsFlag(zeros[j].row) == changedMarker) {
                            changed = true;
                        }
                    }
                }

                ++lineCount;
                colLines(currentCol) = true;

                zeroCountCols(currentCol) = 0;

                if(zeroCountColsFlag(currentCol) == changedMarker) {
                    changed = true;
                }
            } else {
                zeroCountRowsFlag(currentRow) = changedMarker;
                zeroCountColsFlag(currentCol) = changedMarker;
            }
        }

        changedMarker = !changedMarker;
    } while(changed);

    // all zeros left create a symetrical structure so that minimum lines can be
    // drawn in a simple manner
    for(unsigned i = 0; i < zeros.size(); ++i) {
        if(!zeros[i].deleted) {
            if(!rowLines(zeros[i].row)) {
                ++lineCount;
                rowLines(zeros[i].row) = true;
            }
        } else {
            zeros[i].deleted = false;
        }

        zeroCountRows(zeros[i].row) = 0;
        zeroCountCols(zeros[i].col) = 0;
    }

    return lineCount;
}

int RLAPSolverHungarian::recalculateCosts(const int minVal,
        Tensor<unsigned>& zeroCountRows, Tensor<unsigned>& zeroCountCols, Tensor<bool>& rowLines,
        Tensor<bool>& colLines) {
    int resultMinVal, newMinVal = minVal;

    // determine minimum entry (not covered by lines)
    // and initialize data structure
    if(newMinVal == 0) {
        for(unsigned row = 0; row < size; ++row) {
            if(rowLines(row)) {continue;}

            for(unsigned col = 0; col < size; ++col) {
                if(colLines(col)) {continue;}
                else if(newMinVal > costMat(row, col) || newMinVal == 0) {
                    newMinVal = costMat(row, col);
                }
            }
        }
    }

    // add minimum entry to twice covered entries an subtract it from not covered ones
    // recalculate per row and per column zero counts and minimum entry
    resultMinVal = newMinVal;

    for(unsigned row = 0; row < size; ++row) {
        for(unsigned col = 0; col < size; ++col) {
            int& entry = costMat(row, col);

            if(rowLines(row) && colLines(col)) {
                entry += newMinVal;
            } else if(!rowLines(row) && !colLines(col)) {
                entry -= newMinVal;
            }

            if(entry == 0) {
                ++zeroCountRows(row);
                ++zeroCountCols(col);
                zeros.push_back({row, col, false});
            } else if(resultMinVal > entry) {
                resultMinVal = entry;
            }
        }
    }

    return resultMinVal;
}

void RLAPSolverHungarian::assignMatching(Tensor<unsigned>& assignments,
        Tensor<unsigned>& zeroCountRows, Tensor<unsigned>& zeroCountCols) {
    for(const Zero& elm : zeros) {
        ++zeroCountRows(elm.row);
        ++zeroCountCols(elm.col);
    }

    const unsigned maxAssignments = std::min(rows, cols);
    unsigned assignmentsInd = 0;
    unsigned minZeros = 1, newMinZeros = 0;

    while(assignmentsInd < maxAssignments) {
        for(Zero& elm : zeros) {
            if(elm.deleted) {continue;}

            unsigned currentMinZeros = std::min(zeroCountRows(elm.row), zeroCountCols(elm.col));

            if(currentMinZeros <= minZeros) {
                if(elm.row < rows && elm.col < cols) {
                    assignments(assignmentsInd, 0) = elm.row;
                    assignments(assignmentsInd, 1) = elm.col;
                    ++assignmentsInd;
                }

                elm.deleted = true;

                for(Zero& compElm : zeros) {
                    if(compElm.deleted) {continue;}

                    if(elm.row == compElm.row || elm.col == compElm.col) {
                        --zeroCountRows(compElm.row);
                        --zeroCountCols(compElm.col);
                        compElm.deleted = true;

                        currentMinZeros = std::min(zeroCountRows(elm.row), zeroCountCols(elm.col));

                        if(newMinZeros == 0 || newMinZeros > currentMinZeros) {
                            newMinZeros = currentMinZeros;
                        }
                    }
                }
            } else if(newMinZeros == 0 || newMinZeros > currentMinZeros) {
                newMinZeros = currentMinZeros;
            }
        }

        minZeros = newMinZeros;
        newMinZeros = 0;
    }
}

void RLAPSolverHungarian::solve(Tensor<unsigned>& assignments) {
    if(solved) {throw std::runtime_error("RLAP solved already");}

    Tensor<unsigned> zeroCountRows({size}), zeroCountCols({size});
    Tensor<bool> rowLines({size}), colLines({size});

    reduceRowsAndCols(zeroCountRows, zeroCountCols);
    unsigned lineCount = coverZeros(zeroCountRows, zeroCountCols, rowLines, colLines);

    int minVal = 0;

    while(lineCount < size) {
        zeros.clear();
        minVal = recalculateCosts(minVal, zeroCountRows, zeroCountCols, rowLines, colLines);
        lineCount = coverZeros(zeroCountRows, zeroCountCols, rowLines, colLines);
    }

    assignMatching(assignments, zeroCountRows, zeroCountCols);
    solved = true;
}
