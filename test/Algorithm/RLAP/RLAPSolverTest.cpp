#include <unordered_set>
#include "Tensor.hpp"
#include "RLAPSolverTest.hpp"
#include "RLAPSolverHungarian.hpp"
#include "RLAPSolverJV.hpp"

RLAPSolverTest::RLAPSolverTest() {
    solvers.push_back([](const Tensor<int>& m) -> RLAPSolver* {
        int maxEntry = 0;

        for(unsigned row = 0; row < m.getDims()[0]; ++row) {
            for(unsigned col = 0; col < m.getDims()[1]; ++col) {
                if(maxEntry < m(row, col)) {
                    maxEntry = m(row, col);
                }
            }
        }
        return new RLAPSolverHungarian(m, maxEntry);
    });
    solvers.push_back([](const Tensor<int>& m) -> RLAPSolver* {return new RLAPSolverJV(m); });
}

void RLAPSolverTest::uniqueEntries(const Tensor<unsigned>& assignments,
                                   unsigned& uniqueRows,
                                   unsigned& uniqueCols) {
    std::unordered_set<unsigned> rowSet, colSet;

    for(unsigned i = 0; i < assignments.getDims()[0]; ++i) {
        rowSet.insert(assignments(i, 0));
        colSet.insert(assignments(i, 1));
    }

    uniqueRows = rowSet.size();
    uniqueCols = colSet.size();
}

int RLAPSolverTest::assignmentSum(const Tensor<unsigned>& assignments,
                                  const Tensor<int>& inputMatrix) {
    int result = 0;

    for(unsigned row = 0; row < assignments.getDims()[0]; ++row) {
        result += inputMatrix(assignments(row, 0), assignments(row, 1));
    }

    return result;
}

std::list<RLAPTest> testParams = {
    {Tensor<int>({1, 1}, 42), 42},
    {Tensor<int>({2, 3}, 1), 2},
    {Tensor<int>({3, 2}, 1), 2},
    {Tensor<int>({1, 3}, {1, 2, 3}), 3},
    {Tensor<int>({3, 1}, {1, 2, 3}), 3},
    {Tensor<int>({3, 3}, {1, 1, 8, 19, 0, 9, 8, 13, 11}), 40},
    {Tensor<int>({4, 4}, {7, 9, 2, 13, 17, 5, 10, 4, 5, 12, 16, 15, 6, 18, 19, 6}), 64},
    {Tensor<int>({4, 6}, {1, 18, 15, 8, 18, 3, 19, 8, 12, 17, 8, 19, 13, 17, 11, 2, 17, 5, 6, 15, 12, 4, 8, 17}), 71},
    {Tensor<int>({6, 4}, {7, 4, 0, 9, 8, 17, 4, 12, 5, 14, 3, 15, 12, 5, 3, 13, 12, 2, 14, 4, 1, 12, 6, 9}), 58},
    {
        Tensor<int>({10, 10}, {
            5, 12, 17, 17,  9, 15, 15,  3, 19, 17,
            9, 14,  4,  3,  2, 15, 17,  0,  8, 16,
            2, 12,  4, 10,  9, 10,  6, 11,  7,  8,
            19, 18, 13, 12,  0, 16,  6,  6,  0, 13,
            11, 10,  5, 18,  2, 17, 10,  4,  8, 19,
            0,  5,  5, 15,  1, 12,  5,  8,  9,  0,
            5, 11, 17, 12, 11,  9, 11, 19,  3, 14,
            8, 13, 12,  8,  8, 15,  0, 13, 17,  9,
            10, 19, 13,  7,  6,  9, 11, 13, 10, 10,
            12,  0,  9, 12,  9, 13, 16,  2,  3,  1
        }), 165
    }
};

INSTANTIATE_TEST_CASE_P(RLAPTestCases, RLAPSolverTest, ::testing::ValuesIn(testParams));

/**
 * \test Parameterized test for all RLAPSolver implementations
 *       using various RLA Matrices with known results
 */
TEST_P(RLAPSolverTest, TestingWithRLAPParams) {
    const RLAPTest& param = GetParam();
    const Tensor<int>& inputMatrix = param.inputMatrix;
    const unsigned nAssignments = std::min(inputMatrix.getDims()[0],
                                           inputMatrix.getDims()[1]);
    int expectedSum = param.expectedSum;

    for(SolverCreator sc : solvers) {
        std::unique_ptr<RLAPSolver> solver(sc(param.inputMatrix));
        Tensor<unsigned> assignments({ nAssignments, 2 }, 0);

        solver->solve(assignments);
        unsigned uniqueRows, uniqueCols;
        uniqueEntries(assignments, uniqueRows, uniqueCols);

        EXPECT_EQ(nAssignments, uniqueRows);
        EXPECT_EQ(nAssignments, uniqueCols);
        EXPECT_EQ(expectedSum, assignmentSum(assignments, inputMatrix));
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}
