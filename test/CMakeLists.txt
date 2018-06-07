add_executable(TensorTest Numeric/TensorTest.cpp)
target_include_directories(TensorTest PRIVATE ${PROJECT_SOURCE_DIR}/src/Numeric)
target_link_libraries(TensorTest PRIVATE gtest gmock)
add_test(NAME TensorTest COMMAND TensorTest)

add_executable(InputDataTest DataHandlers/InputDataTest.cpp)
target_include_directories(InputDataTest PRIVATE DataHandlers)
target_link_libraries(InputDataTest PRIVATE gtest gmock InputData)
add_test(NAME InputDataTest COMMAND InputDataTest)

add_executable(SearchGraphNodeTest DataHandlers/SearchGraphNodeTest.cpp)
target_include_directories(SearchGraphNodeTest PRIVATE DataHandlers)
target_link_libraries(SearchGraphNodeTest PRIVATE gtest gmock SearchGraphNode)
add_test(NAME SearchGraphNodeTest COMMAND SearchGraphNodeTest)

add_executable(RLAPSolverTest Algorithm/RLAP/RLAPSolverTest.cpp)
target_include_directories(RLAPSolverTest
    PRIVATE
        Algorithm/RLAP
        ${PROJECT_SOURCE_DIR}/src/Algorithm/RLAP
        ${PROJECT_SOURCE_DIR}/src/Numeric
)
target_link_libraries(RLAPSolverTest PRIVATE gtest gmock RLAPSolverHungarian RLAPSolverJV)
add_test(NAME RLAPSolverTest COMMAND RLAPSolverTest)
