#include "monte_carlo.h"
#include <random>
#include <algorithm>
#include <fstream>

static glm::vec3 randomUnitHemisphere(const glm::vec3& normal) {
    static std::default_random_engine gen;
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    float u = dist(gen);
    float v = dist(gen);

    float theta = 2.0f * M_PI * u;
    float phi = acos(2.0f * v - 1.0f);

    glm::vec3 dir;
    dir.x = sin(phi) * cos(theta);
    dir.y = sin(phi) * sin(theta);
    dir.z = cos(phi);

    return glm::normalize(dir * glm::sign(glm::dot(dir, normal)));
}

HitInfo intersectPlane(const Ray& ray) {
    glm::vec3 planeNormal = glm::vec3(0, 0, 1);
    glm::vec3 planePoint = glm::vec3(0, 0, 0);
    float denom = glm::dot(ray.direction, planeNormal);

    HitInfo info = {false};
    if (abs(denom) > 1e-6) {
        float t = glm::dot(planePoint - ray.origin, planeNormal) / denom;
        if (t > 0.001f) {
            info.hit = true;
            info.distance = t;
            info.normal = planeNormal;
            info.color = glm::vec3(0.7f);  // gray plane
        }
    }
    return info;
}

glm::vec3 traceRay(const Ray& ray, int depth) {
    if (depth == 0)
        return glm::vec3(0.0f);

    HitInfo hit = intersectPlane(ray);
    if (!hit.hit)
        return glm::vec3(0.2f, 0.2f, 0.5f); // sky color

    glm::vec3 indirect = glm::vec3(0.0f);
    for (int i = 0; i < 8; ++i) {
        glm::vec3 newDir = randomUnitHemisphere(hit.normal);
        Ray newRay = {ray.origin + ray.direction * hit.distance + newDir * 0.001f, newDir};
        indirect += traceRay(newRay, depth - 1);
    }
    indirect /= 8.0f;

    // Add direct light contribution (simple directional light)
    glm::vec3 lightDir = glm::normalize(glm::vec3(1, 1, 1));
    float ndotl = std::max(0.0f, glm::dot(hit.normal, lightDir));
    glm::vec3 direct = glm::vec3(1.0f) * ndotl;

    return 0.2f * direct + 0.8f * indirect * hit.color;
}

std::vector<glm::vec3> renderWithMonteCarlo(int width, int height) {
    std::vector<glm::vec3> framebuffer(width * height);

    glm::vec3 camPos(0, 0, 1.0f);
    float aspect = (float)width / height;
    float scale = 1.0f;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float u = (2.0f * (x + 0.5f) / width - 1.0f) * aspect * scale;
            float v = (1.0f - 2.0f * (y + 0.5f) / height) * scale;

            glm::vec3 dir = glm::normalize(glm::vec3(u, v, -1.0f));
            Ray ray = {camPos, dir};

            framebuffer[y * width + x] = traceRay(ray, 2);  // max 2 bounces
        }
    }

    // Save to PPM file
    std::ofstream ppmFile("monte_carlo.ppm");
    ppmFile << "P3\n" << width << " " << height << "\n255\n";
    for (const auto& pixel : framebuffer) {
        ppmFile << (int)(pixel.r * 255) << " "
                << (int)(pixel.g * 255) << " "
                << (int)(pixel.b * 255) << "\n";
    }
    ppmFile.close();

    return framebuffer;
}
