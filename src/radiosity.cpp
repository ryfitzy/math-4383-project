#include "radiosity.h"
#include <cmath>
#include <iostream>
#include <fstream>

float computeFormFactor(const Patch& pi, const Patch& pj) {
    glm::vec3 rij = pj.position - pi.position;
    float r2 = glm::dot(rij, rij);
    if (r2 < 1e-4f) return 0.0f;

    glm::vec3 rij_dir = glm::normalize(rij);
    float cosThetaI = glm::dot(pi.normal, rij_dir);
    float cosThetaJ = glm::dot(pj.normal, -rij_dir);

    if (cosThetaI <= 0 || cosThetaJ <= 0) return 0.0f;

    return (cosThetaI * cosThetaJ * pj.area) / (float)(M_PI * r2);
}

void computeRadiosity(std::vector<Patch>& patches, int width, int height) {
    int n = patches.size();
    std::vector<std::vector<float>> F(n, std::vector<float>(n, 0.0f));

    // Compute form factor matrix
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (i != j)
                F[i][j] = computeFormFactor(patches[i], patches[j]);

    // Normalize each row (sum of Fij = 1 ideally)
    for (int i = 0; i < n; ++i) {
        float sumF = 0.0f;
        for (int j = 0; j < n; ++j) sumF += F[i][j];
        if (sumF > 0.0f) for (int j = 0; j < n; ++j) F[i][j] /= sumF;
    }

    // Iteratively solve radiosity using Gauss-Seidel
    for (int iter = 0; iter < 100; ++iter) {
        for (int i = 0; i < n; ++i) {
            float B = patches[i].emission;
            for (int j = 0; j < n; ++j)
                B += patches[i].reflectivity * F[i][j] * patches[j].radiosity;
            patches[i].radiosity = B;
        }
    }

    // Example: Normalize the radiosity for display purposes
    std::vector<glm::vec3> image(width * height);
    for (size_t i = 0; i < patches.size(); ++i) {
        const Patch& p = patches[i];
        int x = static_cast<int>((p.position.x + 1.0f) / 2.0f * (width - 1));
        int y = static_cast<int>((p.position.y + 1.0f) / 2.0f * (height - 1));
        image[y * width + x] = glm::vec3(p.radiosity / 10.0f);  // Just a placeholder normalization
    }

    // Save the Radiosity image to a PPM file
    std::ofstream ppmFile("radiosity.ppm");
    ppmFile << "P3\n" << width << " " << height << "\n255\n";
    for (const auto& pixel : image) {
        ppmFile << (int)(pixel.r * 255) << " "
                << (int)(pixel.g * 255) << " "
                << (int)(pixel.b * 255) << "\n";
    }
    ppmFile.close();

}
