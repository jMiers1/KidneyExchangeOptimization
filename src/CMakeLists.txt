# cmake_minimum_required(VERSION 3.10)
# project(KidneyProblem VERSION 1.0)

# set(CMAKE_CXX_COMPILER "clang++")

# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

# add_executable(KidneyProblem "main.cpp" 
#                              "Class_Problem.cpp" 
#                              "FindCycles_VFS.cpp"
#                              "FindCycles.cpp"
#                              "LagrangianRelaxation.cpp"
#                              "LongestPath.cpp"
#                              "ReadData.cpp")

# # Add the include directories for CPLEX and Concert
# target_include_directories(KidneyProblem PUBLIC
#     /Applications/CPLEX_Studio2211/cplex/include
#     /Applications/CPLEX_Studio2211/concert/include
# )


# # Add the library directories for CPLEX and Concert
# link_directories( 
#     /Applications/CPLEX_Studio2211/cplex/lib/arm64_osx/static_pic
#     /Applications/CPLEX_Studio2211/concert/lib/arm64_osx/static_pic
# )

# # Link the necessary libraries
# target_link_libraries(KidneyProblem 
#     cplex
#     concert
#     ilocplex
# )


# set_target_properties(KidneyProblem PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
#     ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#     LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
# )


cmake_minimum_required(VERSION 3.10)
project(KidneyProblem VERSION 1.0)

set(CMAKE_CXX_COMPILER "clang++")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(KidneyProblem "KidneyModel.cpp"
                             "ReadData.cpp"
                             "AugmentedNetwork.cpp"
                             "BBTree.cpp"
                             "Chains.cpp"
                             "Class_Problem.cpp"
                             "FindCycles.cpp" 
                             # "FindCycles_VFS.cpp"
                             "Logger.cpp"
                             "LagrangianRelaxation.cpp"
                             "LongestPath.cpp"
                             "main.cpp" )


                                                                                 
                                                                

# Add the include directories for CPLEX and Concert
target_include_directories(KidneyProblem PRIVATE
    /Applications/CPLEX_Studio2211/cplex/include
    /Applications/CPLEX_Studio2211/concert/include
)

# Add the library directories for CPLEX and Concert
target_link_directories(KidneyProblem PRIVATE
    /Applications/CPLEX_Studio2211/cplex/lib/arm64_osx/static_pic
    /Applications/CPLEX_Studio2211/concert/lib/arm64_osx/static_pic
)

# Link the necessary libraries
target_link_libraries(KidneyProblem PRIVATE
    cplex
    concert
    ilocplex
)

set_target_properties(KidneyProblem PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
