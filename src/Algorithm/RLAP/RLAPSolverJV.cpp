#include <algorithm>
#include <functional>
#include "RLAPSolverJV.hpp"

#ifndef NDEBUG
#include <iostream>

/** Prints contents of a row- or colsol of RLAPSolverJV */
#define DBG_PRINT_JV_SOL(type, sol) std::cout << type << std::endl; \
	for (const auto& i : sol) \
		std::cout << i << ' '; \
	std::cout << std::endl

/** Prints contents of a costmat of RLAPSolverJV */
#define DBG_PRINT_JV_COSTMAT(v) std::cout << "Costmat" << std::endl; \
	for (unsigned i = 0; i < costMat.size(); i++) { \
		for (unsigned j = 0; j < costMat[i].size(); j++) { \
			std::cout << costMat[i][j] << '\t'; \
		} \
		std::cout << std::endl; \
	} \
	std::cout << std::endl
#else
#define DBG_PRINT_JV_SOL(_, _)
#define DBG_PRINT_JV_COSTMAT(_)
#endif

/** Used to determine the solver size needed internally */
#define MAX_DIM(m) std::max(m.getDims()[0], m.getDims()[1])

RLAPSolverJV::RLAPSolverJV(const Tensor<int>& mat):
    rows(mat.getDims()[0]),
    cols(mat.getDims()[1]),
    size(MAX_DIM(mat)),
    costMat(MAX_DIM(mat), std::vector<double>(MAX_DIM(mat), 0)),
    rowsol(std::vector<int>(size)),
    colsol(std::vector<int>(size)) {
    // Fill costMat with inverted mat, since the maximum instead of the minimum value is needed.
    for(unsigned row = 0; row < rows; ++row) {
        for(unsigned col = 0; col < cols; ++col) {
            costMat[row][col] = -mat(row, col);
        }
    }
}

void RLAPSolverJV::solve(Tensor<unsigned>& assignments) {
    std::shared_ptr<std::vector<double>> u(new std::vector<double>(size));
    std::shared_ptr<std::vector<double>> v(new std::vector<double>(size));

    jvlap(size, costMat, rowsol, colsol, (*u), (*v));
    assignMatching(assignments);
}

void RLAPSolverJV::assignMatching(Tensor<unsigned>& assignments) {
    if(rows > cols) {
        for(unsigned i = 0; i < assignments.getDims()[0]; ++i) {
            assignments(i, 0) = colsol.at(i);
            assignments(i, 1) = i;
        }
    } else {
        for(unsigned i = 0; i < assignments.getDims()[0]; ++i) {
            assignments(i, 0) = i;
            assignments(i, 1) = rowsol.at(i);
        }
    }
}

void RLAPSolverJV::jvlap(int dim,
                         const std::vector<std::vector<double>>& assigncost,
                         std::vector<int>& rowsol,
                         std::vector<int>& colsol,
                         std::vector<double>& u,
                         std::vector<double>& v) {
    bool unassignedfound;
    int  i, imin, numfree = 0, prvnumfree, f, i0, k, freerow, *pred, *free_;
    int  j, j1, j2, endofpath, last, low, up, *collist, *matches;
    double min_, h, umin, usubmin, v2, *d;

    free_ = new int[dim];       // list of unassigned rows.
    collist = new int[dim];    // list of columns to be scanned in various ways.
    matches = new int[dim];    // counts how many times a row could be assigned.
    d = new double[dim];         // 'cost-distance' in augmenting path calculation.
    pred = new int[dim];       // row-predecessor of column in augmenting/alternating path.

    // init how many times a row will be assigned in the column reduction.
    for(i = 0; i < dim; i++)
    { matches[i] = 0; }

    // COLUMN REDUCTION
    for(j = dim; j--;) { // reverse order gives better results.
        // find minimum cost over rows.
        min_ = assigncost[0][j];
        imin = 0;

        for(i = 1; i < dim; i++)
            if(assigncost[i][j] < min_) {
                min_ = assigncost[i][j];
                imin = i;
            }

        v[j] = min_;

        if(++matches[imin] == 1) {
            // init assignment if minimum row assigned for first time.
            rowsol[imin] = j;
            colsol[j] = imin;
        } else if(v[j] < v[rowsol[imin]]) {
            int j1 = rowsol[imin];
            rowsol[imin] = j;
            colsol[j] = imin;
            colsol[j1] = -1;
        } else
        { colsol[j] = -1; }        // row already assigned, column not assigned.
    }

    // REDUCTION TRANSFER
    for(i = 0; i < dim; i++)
        if(matches[i] == 0)      // fill list of unassigned 'free' rows.
        { free_[numfree++] = i; }
        else if(matches[i] == 1) { // transfer reduction from rows that are assigned once.
            j1 = rowsol[i];
            min_ = BIG;

            for(j = 0; j < dim; j++)
                if(j != j1)
                    if(assigncost[i][j] - v[j] < min_)
                    { min_ = assigncost[i][j] - v[j]; }

            v[j1] = v[j1] - min_;
        }

    //   AUGMENTING ROW REDUCTION
    int loopcnt = 0;           // do-loop to be done twice.

    do {
        loopcnt++;

        //     scan all free rows.
        //     in some cases, a free row may be replaced with another one to be scanned next.
        k = 0;
        prvnumfree = numfree;
        numfree = 0;             // start list of rows still free after augmenting row reduction.

        while(k < prvnumfree) {
            i = free_[k];
            k++;

            //       find minimum and second minimum reduced cost over columns.
            umin = assigncost[i][0] - v[0];
            j1 = 0;
            usubmin = BIG;

            for(j = 1; j < dim; j++) {
                h = assigncost[i][j] - v[j];

                if(h < usubmin)
                    if(h >= umin) {
                        usubmin = h;
                        j2 = j;
                    } else {
                        usubmin = umin;
                        umin = h;
                        j2 = j1;
                        j1 = j;
                    }
            }

            i0 = colsol[j1];

            if(umin < usubmin)
                //         change the reduction of the minimum column to increase the minimum
                //         reduced cost in the row to the subminimum.
            { v[j1] = v[j1] - (usubmin - umin); }
            else                   // minimum and subminimum equal.
                if(i0 > -1) { // minimum column j1 is assigned.
                    //           swap columns j1 and j2, as j2 may be unassigned.
                    j1 = j2;
                    i0 = colsol[j2];
                }

            //       (re-)assign i to j1, possibly de-assigning an i0.
            rowsol[i] = j1;
            colsol[j1] = i;

            if(i0 > -1)   // minimum column j1 assigned earlier.
                if(umin < usubmin)
                    //           put in current k, and go back to that k.
                    //           continue augmenting path i - j1 with i0.
                { free_[--k] = i0; }
                else
                    //           no further augmenting reduction possible.
                    //           store i0 in list of free rows for next phase.
                { free_[numfree++] = i0; }
        }
    } while(loopcnt < 2);        // repeat once.

    // AUGMENT SOLUTION for each free row.
    for(f = 0; f < numfree; f++) {
        freerow = free_[f];       // start row of augmenting path.

        // Dijkstra shortest path algorithm.
        // runs until unassigned column added to shortest path tree.
        for(j = dim; j--;) {
            d[j] = assigncost[freerow][j] - v[j];
            pred[j] = freerow;
            collist[j] = j;        // init column list.
        }

        low = 0; // columns in 0..low-1 are ready, now none.
        up = 0;  // columns in low..up-1 are to be scanned for current minimum, now none.
        // columns in up..dim-1 are to be considered later to find new minimum,
        // at this stage the list simply contains all columns
        unassignedfound = false;

        do {
            if(up == low) {        // no more columns to be scanned for current minimum.
                last = low - 1;

                // scan columns for up..dim-1 to find all indices for which new minimum occurs.
                // store these indices between low..up-1 (increasing up).
                min_ = d[collist[up++]];

                for(k = up; k < dim; k++) {
                    j = collist[k];
                    h = d[j];

                    if(h <= min_) {
                        if(h < min_) {    // new minimum.
                            up = low;      // restart list at index low.
                            min_ = h;
                        }

                        // new index with same minimum, put on undex up, and extend list.
                        collist[k] = collist[up];
                        collist[up++] = j;
                    }
                }

                // check if any of the minimum columns happens to be unassigned.
                // if so, we have an augmenting path right away.
                for(k = low; k < up; k++)
                    if(colsol[collist[k]] < 0) {
                        endofpath = collist[k];
                        unassignedfound = true;
                        break;
                    }
            }

            if(!unassignedfound) {
                // update 'distances' between freerow and all unscanned columns, via next scanned column.
                j1 = collist[low];
                low++;
                i = colsol[j1];
                h = assigncost[i][j1] - v[j1] - min_;

                for(k = up; k < dim; k++) {
                    j = collist[k];
                    v2 = assigncost[i][j] - v[j] - h;

                    if(v2 < d[j]) {
                        pred[j] = i;

                        if(v2 == min_)    // new column found at same minimum value
                            if(colsol[j] < 0) {
                                // if unassigned, shortest augmenting path is complete.
                                endofpath = j;
                                unassignedfound = true;
                                break;
                            }
                        // else add to list to be scanned right away.
                            else {
                                collist[k] = collist[up];
                                collist[up++] = j;
                            }

                        d[j] = v2;
                    }
                }
            }
        } while(!unassignedfound);

        // update column prices.
        for(k = last + 1; k--;) {
            j1 = collist[k];
            v[j1] = v[j1] + d[j1] - min_;
        }

        // reset row and column assignments along the alternating path.
        do {
            i = pred[endofpath];
            colsol[endofpath] = i;
            j1 = endofpath;
            endofpath = rowsol[i];
            rowsol[i] = j1;
        } while(i != freerow);
    }

    // free reserved memory.
    delete[] pred;
    delete[] free_;
    delete[] collist;
    delete[] matches;
    delete[] d;
    return;
}

