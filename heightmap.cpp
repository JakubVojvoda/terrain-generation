/*
 * LoD terrain generation and displaying
 * Svoboda Petr, Vojvoda Jakub
 * 
 * 2014
 *
 */

#include "heightmap.h"

#include <iostream>

using namespace std;

HeightMap::~HeightMap()
{
    noise.free();
    for ( int i=0; i< width;i++)
        delete[] heightmap[i];
    delete[] heightmap;

}

HeightMap::HeightMap(int w, int h)
{
    width = w;
    height = h;

    heightmap = new float*[width];

    for (int i = 0; i < width; i++)
        heightmap[i] = new float[height];

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            heightmap[i][j] = -1;
        }
    }

    noise = PerlinNoise(width, height);
    noise.randomize();
}

float **HeightMap::getHeightMap()
{
    return heightmap;
}

float **HeightMap::generateHeightMap(float corners_weight, float diamond_weight)
{
    initSquate(corners_weight);
    diamondSquare(0, width - 1, 0, height - 1, diamond_weight);
    interpolate(INTERPOLATION_COS);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            if (heightmap[i][j] < 0.4)
                heightmap[i][j] = 0.0;

            heightmap[i][j] *= heightmap[i][j] * heightmap[i][j];

        }
    }

    smooth(4,1);
    interpolate(INTERPOLATION_COS);
    normalize();

    return getHeightMap();
}

unsigned char* HeightMap::convertTo1D(float** map)
{
    hMap = new unsigned char[width*width ];

    for ( int i = 0; i < width ; i++) {
        for ( int j = 0; j < width; j++) {
            hMap[i*width + j] = (unsigned char)(map[i][j]*25);
        }
    }

    for ( int i=0; i< width;i++) {
        delete[] map[i];
    }
    
    delete[] map;

    noise.free();
    return hMap;
}

float **HeightMap::enlargeHeightMap(int step)
{
    int norm_size = step * width;
    float **lmap = new float*[norm_size];

    for (int i = 0; i < norm_size; i++)
        lmap[i] = new float[norm_size];

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            for (int xstep = 0; xstep < step; xstep++) {
                for (int ystep = 0; ystep < step; ystep++) {

                    if ((2 * i + xstep) < norm_size && (2 * i + ystep) < norm_size)
                        lmap[2 * i + xstep][2 * j + ystep] = heightmap[i][j];
                }
            }
        }
    }

    for ( int i=0; i< width;i++) {
        delete[] heightmap[i];
    }
    
    delete[] heightmap;

    heightmap = lmap;
    
    interpolate(INTERPOLATION_COS);
    smooth(4.0, 1.0);
    addPerlinNoise(0.01, 0.25, 6, INTERPOLATION_COS);
    interpolate(INTERPOLATION_COS);

    return lmap;
}

void HeightMap::initSquate(float weight)
{
    heightmap[0][0] = weight * noise.randomValue();
    heightmap[0][height - 1] = weight * noise.randomValue();
    heightmap[width - 1][0] = weight * noise.randomValue();
    heightmap[width - 1][height - 1] = weight * noise.randomValue();
}

void HeightMap::diamondSquare(int x1, int x2, int y1, int y2, float weight)
{
    int cx = (x1 + x2) / 2;
    int cy = (y1 + y2) / 2;

    if (cx == x1 || cx == x2 || cy == y1 || cy == y2)
        return;

    float corners_average = (heightmap[x1][y1] + heightmap[x2][y1] + heightmap[x1][y2] + heightmap[x2][y2]) / 4;
    heightmap[cx][cy] = corners_average + weight * noise.randomValue();

    heightmap[cx][y1] = (heightmap[x1][y1] + heightmap[x2][y1] + heightmap[cx][cy]) / 3 + weight * noise.randomValue();
    heightmap[cx][y2] = (heightmap[x1][y2] + heightmap[x2][y2] + heightmap[cx][cy]) / 3 + weight * noise.randomValue();

    heightmap[x1][cy] = (heightmap[x1][y1] + heightmap[x1][y2] + heightmap[cx][cy]) / 3 + weight * noise.randomValue();
    heightmap[x2][cy] = (heightmap[x2][y1] + heightmap[x2][y2] + heightmap[cx][cy]) / 3 + weight * noise.randomValue();

    diamondSquare(x1, cx, y1, cy, weight);
    diamondSquare(cx, x2, y1, cy, weight);
    diamondSquare(x1, cx, cy, y2, weight);
    diamondSquare(cx, x2, cy, y2, weight);
}

void HeightMap::normalize()
{
    float maximum = 0.0;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            if (maximum < heightmap[i][j])
                maximum = heightmap[i][j];
        }
    }

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            heightmap[i][j] /= maximum;
        }
    }
}

void HeightMap::addPerlinNoise(float weight, float persistance, int octaves, int method)
{
    noise.setSize(width, height);
    noise.setInterpolationMethod(method);
    noise.setPerlinNoise(persistance, octaves);
    
    noise.generatePerlinNoiseMat();

    float **perlin_noise = noise.getPerlinNoise();

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            heightmap[i][j] += weight * perlin_noise[i][j];
        }
    }
}

void HeightMap::interpolate(int method)
{
    for (int i = 0; i < width - 1; i++) {
        for (int j = 0; j < height - 1; j++) {

            int fc = heightmap[i][j] - int(heightmap[i][j]);

            if (method == INTERPOLATION_LINEAR) {
                
                float v1 = noise.linearInterpolation(heightmap[i][j], heightmap[i][j+1], fc);
                float v2 = noise.linearInterpolation(heightmap[i+1][j], heightmap[i+1][j+1], fc);

                heightmap[i][j] = noise.linearInterpolation(v1, v2, fc);
            }
            
            else {

                float v1 = noise.cosinInterpolation(heightmap[i][j], heightmap[i][j+1], fc);
                float v2 = noise.cosinInterpolation(heightmap[i+1][j], heightmap[i+1][j+1], fc);

                heightmap[i][j] = noise.cosinInterpolation(v1, v2, fc);
            }

        }
    }

    normalize();
}

void HeightMap::smooth(float w1, float w2)
{
    int w = width - 1;
    int h = height - 1;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            float average_value = 0.0;

            if (i == 0 && j == 0) {
                average_value += w1 * heightmap[i][j];

                average_value += w2 * heightmap[i][j+1];
                average_value += w2 * heightmap[i+1][j];
                average_value += w2 * heightmap[i+1][j+1];

                heightmap[i][j] = average_value / (3 * (w1 + 3 * w2));
            }
            
            else if (i == w && j == 0) {
                average_value += w1 * heightmap[i][j];

                average_value += w2 * heightmap[i-1][j];
                average_value += w2 * heightmap[i-1][j+1];
                average_value += w2 * heightmap[i][j+1];

                heightmap[i][j] = average_value / (3 * (w1 + 3 * w2));
            }
            
            else if (i == 0 && j == h) {
                average_value += w1 * heightmap[i][j];

                average_value += w2 * heightmap[i][j-1];
                average_value += w2 * heightmap[i+1][j-1];
                average_value += w2 * heightmap[i+1][j];

                heightmap[i][j] = average_value / (3 * (w1 + 3 * w2));
            }
            
            else if (i == w && j == h) {
                average_value += w1 * heightmap[i][j];

                average_value += w2 * heightmap[i-1][j-1];
                average_value += w2 * heightmap[i-1][j];
                average_value += w2 * heightmap[i][j-1];

                heightmap[i][j] = average_value / (w1 + 3 * w2);
            }

            else if (i == 0 && j > 0 && j < h) {
                average_value += w1 * heightmap[i][j];

                average_value += w2 * heightmap[i][j-1];
                average_value += w2 * heightmap[i][j+1];
                average_value += w2 * heightmap[i+1][j-1];
                average_value += w2 * heightmap[i+1][j];
                average_value += w2 * heightmap[i+1][j+1];

                heightmap[i][j] = average_value / (w1 + 5 * w2);
            }

            else if (i == w && j > 0 && j < h) {
                average_value += w1 * heightmap[i][j];

                average_value += w2 * heightmap[i-1][j-1];
                average_value += w2 * heightmap[i-1][j];
                average_value += w2 * heightmap[i-1][j+1];
                average_value += w2 * heightmap[i][j-1];
                average_value += w2 * heightmap[i][j+1];

                heightmap[i][j] = average_value / (w1 + 5 * w2);
            }

            else if (i > 0 && i < h && j == 0) {
                average_value += w1 * heightmap[i][j];

                average_value += w2 * heightmap[i-1][j];
                average_value += w2 * heightmap[i-1][j+1];
                average_value += w2 * heightmap[i][j+1];
                average_value += w2 * heightmap[i+1][j];
                average_value += w2 * heightmap[i+1][j+1];

                heightmap[i][j] = average_value / (w1 + 5 * w2);
            }

            else if (i > 0 && i < h && j == h) {
                average_value += w1 * heightmap[i][j];

                average_value += w2 * heightmap[i-1][j-1];
                average_value += w2 * heightmap[i-1][j];
                average_value += w2 * heightmap[i][j-1];
                average_value += w2 * heightmap[i+1][j-1];
                average_value += w2 * heightmap[i+1][j];

                heightmap[i][j] = average_value / (w1 + 5 * w2);
            }

            else {
                average_value += w1 * heightmap[i][j];

                average_value += w2 * heightmap[i-1][j-1];
                average_value += w2 * heightmap[i-1][j];
                average_value += w2 * heightmap[i-1][j+1];

                average_value += w2 * heightmap[i][j-1];
                average_value += w2 * heightmap[i][j+1];

                average_value += w2 * heightmap[i+1][j-1];
                average_value += w2 * heightmap[i+1][j];
                average_value += w2 * heightmap[i+1][j+1];

                heightmap[i][j] = average_value / (w1 + 8 * w2);
            }

        }
    }
}
