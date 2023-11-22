#include <iostream>
#include <chrono>
#include <fstream>
#include <opencv2/opencv.hpp>

void load_intrinsic_params(float * intrinsic_matrix, float * distortion_coeffs){
    std::ifstream inFile("../intrinsic_test_0.txt");
    if (!inFile.is_open()) {
        std::cerr << "Error: Couldn't open the camera parameter file." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.find("K:") != std::string::npos) {
            std::istringstream valueStream(line.substr(2)); // Remove "K:"

            for (int x = 0; x < 9; x++){
                valueStream >> intrinsic_matrix[x];
                std::cout << intrinsic_matrix[x] << ", ";
            }
            std::cout << std::endl;
        } else if (line.find("D:") != std::string::npos) {
            std::istringstream valueStream(line.substr(2)); // Remove "D:"

            for (int x = 0; x < 5; x++){
                valueStream >> distortion_coeffs[x];
                std::cout << distortion_coeffs[x] << ", ";
            }
            std::cout << std::endl;
        }
    }

    inFile.close();
}