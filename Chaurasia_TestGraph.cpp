/*

Author: Darsh Chaurasia

Date: 2024-04-07

Description: This file contains the main function to test the functionalities of the MatrixGraph class. It includes loading graphs from files, user interaction to perform graph operations, and saving the results to files.

*/

#include "MatrixGraph_Chaurasia.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iomanip>

// Function to display the user menu; optionally suppresses the user prompt if desired
void printMenu(bool promptUser = true);
// Loads a graph from a specified file; allows for directed/undirected and weighted/unweighted configurations
MatrixGraph* loadGraphFromFile(const std::string& filepath, bool isDirected, bool isWeighted);

int main(int argc, char* argv[]) {
    // Validate command line arguments for correct usage
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " {-u|-w} <file> [-ud]" << std::endl;
        return 1;
    }

    // Process command line arguments to determine graph characteristics
    bool isDirected = true; // Default to directed graph
    bool isWeighted = (std::string(argv[1]) == "-w"); // Determine if the graph is weighted
    std::string graphFile = argv[2]; // Graph file path
    if (argc == 4 && std::string(argv[3]) == "-ud") {
        isDirected = false; // Undirected graph if specified
    }

    // Load the graph from the specified file
    MatrixGraph* graph = loadGraphFromFile(graphFile, isDirected, isWeighted);
    if (graph == nullptr) {
        return 1; // Exit if the graph could not be loaded
    }

    // Main interaction loop for the program
    std::string outputFile; // Store the output file path
    int choice; // User's menu selection
    do {
        printMenu(); // Display the menu options
        std::cin >> choice; // Read user choice
        switch (choice) {
            case 1: {
                // Print the graph's adjacency matrix representation
                std::cout << graph->toString();
                break;
            }
            case 2: {
                // Find and print the shortest path between two vertices
                int start, end;
                std::cin >> start >> end; // Read start and end vertices
                try {
                    std::vector<int> path = graph->getBFSPath(start - 1, end - 1); // Adjust for zero-based indexing
                    if (!path.empty()) {
                        // If a path exists, print it along with the cumulative weight
                        std::cout << "[" <<std::setw(2) <<path[0] + 1 << ":  0.00]==>"; // Start vertex
                        float cumulativeWeight = 0.0;
                        for (size_t i = 1; i < path.size(); ++i) {
                            cumulativeWeight += graph->getEdgeWeight(path[i - 1], path[i]);
                            std::cout << "[" <<std::setw(2)<< path[i] + 1 << ":" << std::fixed << std::setprecision(2) << std::setw(6) << cumulativeWeight << "]";
                            if (i < path.size() - 1) {
                                std::cout << "==>";
                            }
                        }
                        std::cout << std::endl;
                    } else {
                        // If no path exists, notify the user
                        std::cout << "No path from " << start << " to " << end << "." << std::endl;
                    }
                } catch (const std::exception& e) {
                    // Handle exceptions, such as invalid vertex indices
                    std::cerr << "Error: " << e.what() << std::endl;
                }
                break;
            }
            case 3: {
                // Start writing to a new file
                std::cin >> outputFile; // Read the output file name
                std::ofstream outFile(outputFile);
                if (outFile.is_open()) {
                    // If file opens successfully, write the graph's data
                    // First, count the number of edges in the graph
                    int numEdges = 0;
                    for (int i = 0; i < graph->getVertexCount(); ++i) {
                        for (int j = 0; j < graph->getVertexCount(); ++j) {
                            if (graph->adjacent(i, j)) {
                                ++numEdges;
                            }
                        }
                    }

                    // Write the number of vertices and edges to the file
                    outFile << graph->getVertexCount() << " " << numEdges << std::endl;

                    // Write each edge to the file, including the weight
                    for (int i = 0; i < graph->getVertexCount(); ++i) {
                        for (int j = 0; j < graph->getVertexCount(); ++j) {
                            if (graph->adjacent(i, j)) {
                                float weight = graph->getEdgeWeight(i, j);
                                outFile << i + 1 << " " << j + 1 << " " << std::fixed << std::setprecision(6) << weight << std::endl;
                            }
                        }
                    }

                    outFile.close(); // Close the file after writing
                } else {
                    std::cerr << "Could not open file for writing." << std::endl;
                }
                break;
            }
            case 4: {
                // Append a path to the previously created file
                if (outputFile.empty()) {
                    std::cout << "No file has been created yet." << std::endl;
                } else {
                    int start, end;
                    std::cin >> start >> end; // Read start and end vertices
                    try {
                        std::vector<int> path = graph->getBFSPath(start - 1, end - 1); // Adjust for zero-based indexing
                        std::ofstream outFile(outputFile, std::ios_base::app); // Open file in append mode
                        if (outFile.is_open()) {
                            // Write the path or a message indicating no path was found
                            if (!path.empty()) {
                                outFile << "[ " << path[0] + 1 << ":  0.00]==>"; // Start vertex
                                float cumulativeWeight = 0.0;
                                for (size_t i = 1; i < path.size(); ++i) {
                                    cumulativeWeight += graph->getEdgeWeight(path[i - 1], path[i]);
                                    outFile << "[ " << path[i] + 1 << ":" << std::fixed << std::setprecision(2) << std::setw(6) << cumulativeWeight << "]";
                                    if (i < path.size() - 1) {
                                        outFile << "==>";
                                    }
                                }
                                outFile << std::endl;
                            } else {
                                outFile << "No path from " << start << " to " << end <<"."<< std::endl;
                            }
                            outFile.close(); // Close the file after appending
                        } else {
                            std::cerr << "Could not open file for appending." << std::endl;
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Error: " << e.what() << std::endl;
                    }
                }
                break;
            }
            case 9999: {
                // Debug option to print the raw adjacency matrix
                graph->printRaw();
                break;
            }
            case 0: {
                // Exit the loop and end the program
                break;
            }
            default: {
                // Handle invalid menu selections
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
            }
        }
    } while (choice != 0); // Repeat until the user chooses to quit

    delete graph; // Clean up dynamically allocated graph before exiting
    return 0;
}

// Prints the user interaction menu to the console
void printMenu(bool promptUser) {
    std::cout << "Welcome to the Graph tester!" << std::endl;
    std::cout << "1) Print the graph" << std::endl;
    std::cout << "2) Find a path" << std::endl;
    std::cout << "3) Start a file" << std::endl;
    std::cout << "4) Add a path to the file" << std::endl;
    std::cout << "0) Quit" << std::endl;
    if (promptUser) {
    }
}

// Loads a graph from a file, supporting directed/undirected and weighted/unweighted graphs
MatrixGraph* loadGraphFromFile(const std::string& filepath, bool isDirected, bool isWeighted) {
    std::ifstream inFile(filepath);
    if (!inFile) {
        // If the file cannot be opened, return a null pointer
        std::cerr << "Could not open the file." << std::endl;
        return nullptr;
    }

    int vertices, edges;
    inFile >> vertices >> edges; // Read the number of vertices and edges
    MatrixGraph* graph = new MatrixGraph(vertices, isDirected); // Create a new graph

    int u, v;
    float weight;
    for (int i = 0; i < edges; i++) {
        inFile >> u >> v; // Read an edge
        if (isWeighted) {
            inFile >> weight; // Read the weight for weighted graphs
        } else {
            weight = 1.0f;  // Use default weight for unweighted graphs
        }
        graph->addEdge(u - 1, v - 1, weight);  // Add the edge to the graph, adjusting for zero-based indexing
    }

    return graph; // Return the loaded graph
}
