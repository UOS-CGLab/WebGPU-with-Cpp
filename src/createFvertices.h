//
// Created by Spoof_UoS on 8/4/24.
//

#ifndef WEBGPU_WITH_CPP_CREATEFVERTICES_H
#define WEBGPU_WITH_CPP_CREATEFVERTICES_H

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>


std::vector<float> parseFloatArray(const std::string& data);
std::vector<uint32_t> parseUint32Array(const std::string& data);
std::string readFile(const std::string& filePath);

struct VertexData {
    std::vector<std::vector<uint32_t>> connectivitys;
    std::vector<std::vector<uint32_t>> OrdinaryPointData;
};

VertexData createFVertices(const std::string& folderName, int depth);

#endif //WEBGPU_WITH_CPP_CREATEFVERTICES_H
