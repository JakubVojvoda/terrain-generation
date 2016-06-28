/*
 * LoD terrain generation and displaying
 * Svoboda Petr, Vojvoda Jakub
 * 
 * 2014
 *
 */

#include "trianglemesh.h"
#include "perlinnoise.h"
#include <iostream>
#include <iterator>

using namespace std;

TriangleMesh::TriangleMesh(double **map, int w, int h)
{
    width = w;
    height = h;

    no_points = w * h;
    no_raw_triangles = 6 * (w - 1) * (h - 1);

    heightmap = map;

    points = new float*[no_points];
    colors = new float*[no_points];

    terrain = new Point[no_points];

    for (int i = 0; i < no_points; i++) {
        points[i] = new float[3];
        colors[i] = new float[3];
    }

    indeces = new unsigned int[no_raw_triangles];

}

void TriangleMesh::trianglesFromHeightMap()
{
    int index = 0;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            points[j + i * width][0] = i;
            points[j + i * width][1] = heightmap[i][j];
            points[j + i * width][2] = j;

            colors[j + i * width][0] = heightmap[i][j];
            colors[j + i * width][1] = heightmap[i][j];
            colors[j + i * width][2] = heightmap[i][j];

            if ((i != height - 1) && (j != height - 1)) {

                indeces[index] = j + i * width;
                indeces[index + 1] = j + (i + 1) * width;
                indeces[index + 2] = j + (i + 1) * width + 1;

                indeces[index + 3] = j + i * width;
                indeces[index + 4] = j + i * width + 1;
                indeces[index + 5] = j + (i + 1) * width + 1;

                index += 6;
            }

        }
    }
}

float **TriangleMesh::getPoints()
{
    return points;
}

float **TriangleMesh::getColors()
{
    return colors;
}

Point *TriangleMesh::getPointsData()
{
    return terrain;
}

void TriangleMesh::buildTerrain(float shift, float scale, float randomness)
{
    PerlinNoise noise = PerlinNoise();

    for (int i = 0; i < no_points; i++) {

        float point_value = points[i][1] + randomness * noise.randomValue();

        if (point_value < 0.15 - shift) {
            terrain[i].color[0] = 0;
            terrain[i].color[1] = 0.5 * colors[i][1];
            terrain[i].color[2] = colors[i][1];
        }

        else if (point_value < 0.3 - shift) {
            terrain[i].color[0] = 0.9 * colors[i][1];
            terrain[i].color[1] = 0.9 * colors[i][1];
            terrain[i].color[2] = 0.5 * colors[i][1];
        }

        else if (point_value < 0.75 - shift) {
            terrain[i].color[0] = 0;
            terrain[i].color[1] = colors[i][1];
            terrain[i].color[2] = 0;
        }

        else if (point_value < 0.85 - shift) {
            terrain[i].color[0] = colors[i][1];
            terrain[i].color[1] = colors[i][1];
            terrain[i].color[2] = colors[i][1];
        }

        else if (point_value < 0.9 - shift) {
            terrain[i].color[0] = 0.4 * colors[i][1];
            terrain[i].color[1] = 0.5 * colors[i][1];
            terrain[i].color[2] = 0.4 * colors[i][1];
        }

        else {
            terrain[i].color[0] = 0.6 * colors[i][1];
            terrain[i].color[1] = 0.7 * colors[i][1];
            terrain[i].color[2] = 0.6 * colors[i][1];
        }

        terrain[i].position[0] = scale * points[i][0];
        terrain[i].position[1] = points[i][1];
        terrain[i].position[2] = scale * points[i][2];
    }
}

unsigned int *TriangleMesh::getIndeces()
{
    return indeces;
}

int TriangleMesh::getPointsSize()
{
    return no_points;
}

int TriangleMesh::getIndecesSize()
{
    return no_raw_triangles;
}
