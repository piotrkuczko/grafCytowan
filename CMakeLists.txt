cmake_minimum_required(VERSION 3.10)
project(grafCytowan)

set(CMAKE_CXX_STANDARD 17)

add_executable(grafCytowan citation_graph_example.cc citation_graph.h)

add_executable(grafCytowanTrivial citation_graph_trivial.cc citation_graph.h)