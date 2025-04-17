#include <GL/glut.h>
#include <chrono>
#include <iostream>
#include "scene_loader.h"
#include "radiosity.h"
#include "monte_carlo.h"

std::vector<Patch> patches;

bool useMonteCarlo = false; // set true to render MCRT instead of Radiosity
std::vector<glm::vec3> monteCarloImage;
const int width = 800, height = 600;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (useMonteCarlo) {
        // Render MCRT image as points
        glBegin(GL_POINTS);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const glm::vec3& color = monteCarloImage[y * width + x];
                glColor3f(color.r, color.g, color.b);
                float fx = (x / (float)width) * 2.0f - 1.0f;
                float fy = (y / (float)height) * 2.0f - 1.0f;
                glVertex2f(fx, fy);
            }
        }
        glEnd();
    } else {
        // Render Radiosity patches
        glBegin(GL_QUADS);
        for (const auto& p : patches) {
            float brightness = p.radiosity / 10.0f;  // normalize for visibility
            glColor3f(brightness, brightness, brightness);

            float halfSize = sqrt(p.area) / 2.0f;
            glVertex3f(p.position.x - halfSize, p.position.y - halfSize, p.position.z);
            glVertex3f(p.position.x + halfSize, p.position.y - halfSize, p.position.z);
            glVertex3f(p.position.x + halfSize, p.position.y + halfSize, p.position.z);
            glVertex3f(p.position.x - halfSize, p.position.y + halfSize, p.position.z);
        }
        glEnd();
    }

    glFlush();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Radiosity vs Monte Carlo Ray Tracing");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    auto start = std::chrono::high_resolution_clock::now();

    if (useMonteCarlo) {
        std::cout << "Rendering with Monte Carlo Ray Tracing...\n";
        monteCarloImage = renderWithMonteCarlo(width, height);  // Render and save MCRT image
    } else {
        std::cout << "Rendering with Radiosity...\n";
        patches = loadScene();
        computeRadiosity(patches, width, height);  // Perform Radiosity and save to PPM
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Rendering time: " << duration.count() << " seconds\n";

    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}

