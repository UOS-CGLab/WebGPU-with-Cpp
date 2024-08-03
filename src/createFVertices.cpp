#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>

std::vector<float> parseFloatArray(const std::string& data) {
    std::vector<float> result;
    std::stringstream ss(data);
    std::string token;

    while (std::getline(ss, token, ',')) {
        result.push_back(std::stof(token));
    }

    return result;
}

std::vector<uint32_t> parseUint32Array(const std::string& data) {
    std::vector<uint32_t> result;
    std::stringstream ss(data);
    std::string token;

    while (std::getline(ss, token, ',')) {
        result.push_back(static_cast<uint32_t>(std::stoul(token)));
    }

    return result;
}

std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Error opening file: " + filePath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

struct VertexData {
    std::vector<std::vector<uint32_t>> connectivitys;
    std::vector<std::vector<uint32_t>> OrdinaryPointData;
};

VertexData createFVertices(const std::string& folderName, int depth) {
    if (folderName.empty()) {
        throw std::runtime_error("Error: folderName is undefined or empty.");
    }

    const std::string basePath = "./" + folderName;

    std::vector<std::vector<uint32_t>> preConnectivityData;
    try {
        std::string patchData = readFile(basePath + "/patch.txt");
        std::stringstream ss(patchData);
        std::string subArray;

        while (std::getline(ss, subArray, '-')) {
            preConnectivityData.push_back(parseUint32Array(subArray));
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Error fetching patch.txt: " + std::string(e.what()));
    }

    std::vector<std::vector<uint32_t>> preOrdinaryPointData;
    try {
        for (int i = 0; i <= depth; ++i) {
            std::string extraOrdinaryData = readFile(basePath + "/extra_ordinary" + std::to_string(i) + ".txt");
            preOrdinaryPointData.push_back(parseUint32Array(extraOrdinaryData));
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Error fetching extra_ordinary.txt: " + std::string(e.what()));
    }

    return { preConnectivityData, preOrdinaryPointData };
}
