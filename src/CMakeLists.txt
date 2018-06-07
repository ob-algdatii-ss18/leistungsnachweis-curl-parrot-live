##################################################################
#                        SearchGraphNode                         #
##################################################################
add_library(SearchGraphNode DataHandlers/SearchGraphNode.cpp)
target_include_directories(SearchGraphNode
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/DataHandlers
        ${CMAKE_CURRENT_SOURCE_DIR}/Numeric
)

##################################################################
#                           InputData                            #
##################################################################
add_library(InputData DataHandlers/InputData.cpp)
target_include_directories(InputData
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/DataHandlers
        ${CMAKE_CURRENT_SOURCE_DIR}/Numeric
)

##################################################################
#                           Generator                            #
##################################################################
add_library(Generator Algorithm/Generator.cpp)
target_include_directories(Generator
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Algorithm
        ${CMAKE_CURRENT_SOURCE_DIR}/Numeric
)
target_link_libraries(Generator
    PUBLIC
        InputData
        SearchGraphNode
    PRIVATE
        RLAPSolverHungarian
        RLAPSolverJV
)

##################################################################
#                      RLAPSolverHungarian                       #
##################################################################
add_library(RLAPSolverHungarian Algorithm/RLAP/RLAPSolverHungarian.cpp)
target_include_directories(RLAPSolverHungarian
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Algorithm/RLAP
        ${CMAKE_CURRENT_SOURCE_DIR}/Numeric
)

##################################################################
#                          RLAPSolverJV                          #
##################################################################
add_library(RLAPSolverJV Algorithm/RLAP/RLAPSolverJV.cpp)
target_include_directories(RLAPSolverJV
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Algorithm/RLAP
        ${CMAKE_CURRENT_SOURCE_DIR}/Numeric
)

##################################################################
#                           Algorithm                            #
##################################################################
add_library(Algorithm Algorithm/Algorithm.cpp)
target_include_directories(Algorithm
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Algorithm
        ${CMAKE_CURRENT_SOURCE_DIR}/Numeric
)
target_link_libraries(Algorithm
    PUBLIC
        InputData
    PRIVATE
        SearchGraphNode
        Generator
)

##################################################################
#                        RideDistributor                         #
##################################################################
add_executable(RideDistributor main.cpp)
target_link_libraries(RideDistributor PRIVATE Algorithm SearchGraphNode InputData Generator)
