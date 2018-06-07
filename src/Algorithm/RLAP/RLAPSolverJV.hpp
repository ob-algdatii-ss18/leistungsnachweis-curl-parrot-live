#ifndef RLAP_SOLVER_JV_H
#define RLAP_SOLVER_JV_H

#include <list>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include "Tensor.hpp"
#include "RLAPSolver.hpp"

#define BIG 100000

/**
 * Implementation for solving the rectangular linear assignment problem
 * ([more info here](http://www.optimization-online.org/DB_FILE/2008/10/2115.pdf)).
 * The solver variant used here is called [Jonker-Volgenant algorithm](https://link.springer.com/article/10.1007/BF02278710).
 *
 * This implementation is based on [hrldcpr/pyLAPJV](https://github.com/hrldcpr/pyLAPJV),
 * which itself is based on a [C implementation by Roy Jonker](https://web.archive.org/web/20051030203159/http://www.magiclogic.com/assignment/lap_cpp.zip).
 */
class RLAPSolverJV : public RLAPSolver {
public:

    /**
    * Constructor for RLAPSolverJV.
    * \param [in] mat 2D-Matrix of cars and rides with earned points
    */
    RLAPSolverJV(const Tensor<int>& mat);

    void solve(Tensor<unsigned>& assignments) override;

private:

    /** Amount of matrix rows. */
    const unsigned rows;

    /** Amount of matrix columns. */
    const unsigned cols;

    /** Dimension of the RLAP. */
    const unsigned size;

    /** Cost matrix with inverted values and dummy rows and columns. */
    std::vector<std::vector<double>> costMat;

    /** Column assigned to row in solution. */
    std::vector<int> rowsol;

    /** Row assigned to column in solution. */
    std::vector<int> colsol;

    /**
     * JV shortest augmenting path algorithm to solve the assignment problem.
     * \param [in] dim			problem size
     * \param [in] assigncost	cost matrix
     * \param [in] rowsol		column assigned to row in solution
     * \param [in] colsol		row assigned to column in solution
     * \param [in] u			dual variables, row reduction numbers
     * \param [in] v			dual variables, column reduction numbers
     */
    void jvlap(int dim, const std::vector<std::vector<double>>& assigncost,
               std::vector<int>& rowsol,
               std::vector<int>& colsol, std::vector<double>& u, std::vector<double>& v);

    /**
    * Fill assignments with the found RLAP solution using rowsol and colsol.
    * \param [in] assignments	Tensor for the RLAP solution (must have the correct size)
    */
    void assignMatching(Tensor<unsigned>& assignments);
};

#endif // RLAP_SOLVER_JV_H
