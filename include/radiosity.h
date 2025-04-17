#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Patch {
    glm::vec3 position;      // Center of patch
    glm::vec3 normal;        // Patch normal
    glm::vec3 color;         // Surface color
    float area;              // Patch area
    float emission;          // Emitted light
    float reflectivity;      // Reflectivity
    float radiosity = 0.0f;  // Computed radiosity
};

void computeRadiosity(std::vector<Patch>& patches, int width, int height);
