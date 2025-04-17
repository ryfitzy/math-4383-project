#include "scene_loader.h"

std::vector<Patch> loadScene() {
    std::vector<Patch> patches;
    int resolution = 10;
    float size = 1.0f;
    float patchSize = size / resolution;

    for (int i = 0; i < resolution; ++i) {
        for (int j = 0; j < resolution; ++j) {
            Patch p;
            p.position = glm::vec3(
                -0.5f + patchSize * (i + 0.5f),
                -0.5f + patchSize * (j + 0.5f),
                0.0f
            );
            p.normal = glm::vec3(0.0f, 0.0f, 1.0f);
            p.area = patchSize * patchSize;
            p.color = glm::vec3(1.0f, 1.0f, 1.0f);
            p.reflectivity = 0.7f;
            p.emission = (i == resolution / 2 && j == resolution / 2) ? 10.0f : 0.0f;
            p.radiosity = 0.0f;
            patches.push_back(p);
        }
    }

    return patches;
}
