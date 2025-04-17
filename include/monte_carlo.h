#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

struct HitInfo {
    bool hit;
    float distance;
    glm::vec3 normal;
    glm::vec3 color;
};

std::vector<glm::vec3> renderWithMonteCarlo(int width, int height);
