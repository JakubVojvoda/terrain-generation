/*
 * LoD terrain generation and displaying
 * Svoboda Petr, Vojvoda Jakub
 * 
 * 2014
 *
 */

#include "perlinnoise.h"
#include <cmath>
#include <climits>
#include <iostream>
#include <stdlib.h>
#include <ctime>

using namespace std;

void PerlinNoise::free()
{
    for ( int i=0; i< width;i++)
    {
        delete[] random_noise[i];
        delete[] smooth_noise[i];
        delete[] interp_noise[i];
        delete[] perlin_noise[i];
    }
    delete[] random_noise;
    delete[] smooth_noise;
    delete[] interp_noise;
    delete[] perlin_noise;

}

PerlinNoise::PerlinNoise()
{
    width = PNOISE_WIDTH;
    height = PNOISE_HEIGHT;

    persistance = 1.0/4;
    octaves = 6;

    allocate();
}

PerlinNoise::PerlinNoise(int w, int h)
{
    width = w;
    height = h;

    persistance = 1.0/4;
    octaves = 6;

    allocate();
}

PerlinNoise::PerlinNoise(int w, int h, float pers, int n_octs, int method)
{
    width = w;
    height = h;

    persistance = pers;
    octaves = n_octs;
    interpolation_method = method;

    allocate();
}

void PerlinNoise::setInterpolationMethod(int method)
{
    interpolation_method = method;
}

void PerlinNoise::setPerlinNoise(float pers, int n_octs)
{
    persistance = pers;
    octaves = n_octs;
}

void PerlinNoise::setSize(int w, int h)
{
    width = w;
    height = h;
}

float **PerlinNoise::getRandomNoise()
{
    return random_noise;
}

float **PerlinNoise::getSmoothNoise()
{
    return smooth_noise;
}

float **PerlinNoise::getInterpolatedNoise()
{
    return interp_noise;
}

float **PerlinNoise::getPerlinNoise()
{
    return perlin_noise;
}

void PerlinNoise::randomize()
{
    srand(time(NULL));
    seed = rand();
}

float PerlinNoise::randomValue()
{
    seed = seed * 69069L + 1;
    return seed / ((float) UINT_MAX + 1);
}

void PerlinNoise::generateRandomNoiseMat()
{
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            random_noise[i][j] = randomNoise(i, j);
        }
    }
}

void PerlinNoise::allocate()
{
    random_noise = new float*[width];
    smooth_noise = new float*[width];
    interp_noise = new float*[width];
    perlin_noise = new float*[width];

    for (int i = 0; i < width; i++) {
        random_noise[i] = new float[height];
        smooth_noise[i] = new float[height];
        interp_noise[i] = new float[height];
        perlin_noise[i] = new float[height];
    }
}

float PerlinNoise::randomNoise(int x, int y)
{
    int s = 69069L * (x) * (x + 1) + 34537L * (y + 1) * (y + 2) + 1;
    s = s * s * 69069L + 1;
    return s / ((float) INT_MAX + 1);
}

float PerlinNoise::linearInterpolation(float a, float b, float x)
{
    return a * (1 - x) + b * x;
}

float PerlinNoise::cosinInterpolation(float a, float b, float x)
{
    float y = (1 - cos(x * PI)) / 2;
    return  (a * (1 - y) + b * x);
}

float PerlinNoise::interpolate(float x, float y)
{
    int ix = int(x);
    int iy = int(y);

    float fx = x - ix;
    float fy = y - iy;

    float v1 = smoothing(ix,     iy);
    float v2 = smoothing(ix,     iy + 1);
    float v3 = smoothing(ix + 1, iy);
    float v4 = smoothing(ix + 1, iy + 1);

    if (interpolation_method == INTERPOLATION_COS) {

        return cosinInterpolation(cosinInterpolation(v1, v2, fx),
                                  cosinInterpolation(v3, v4, fx),
                                  fy);
    }
    else {
        return linearInterpolation(linearInterpolation(v1, v2, fx),
                                   linearInterpolation(v3, v4, fx),
                                   fy);
    }
}

void PerlinNoise::generateInterpNoiseMat()
{
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            interp_noise[i][j] = interpolate(i, j);
        }
    }
}

float PerlinNoise::smoothing(int x, int y)
{   
    return smoothing(x, y, 4, 2, 1);
}

float PerlinNoise::smoothing(int x, int y, int w1, int w2, int w3)
{
    float smoothed_noise = w1 * randomNoise(x, y);

    smoothed_noise += w2 * randomNoise(x - 1,y - 1);
    smoothed_noise += w2 * randomNoise(x + 1,y - 1);
    smoothed_noise += w2 * randomNoise(x - 1,y + 1);
    smoothed_noise += w2 * randomNoise(x + 1,y + 1);

    smoothed_noise += w3 * randomNoise(x,y - 1);
    smoothed_noise += w3 * randomNoise(x - 1,y);
    smoothed_noise += w3 * randomNoise(x + 1,y);
    smoothed_noise += w3 * randomNoise(x,y + 1);

    return smoothed_noise / (w1 + 4 * w2 + 4 * w3);
}

void PerlinNoise::generateSmoothNoiseMat()
{
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            smooth_noise[i][j] = smoothing(i, j);
        }
    }
}

float PerlinNoise::perlinNoise(float x, float y)
{
    float noise = 0.0;
    float normalization = 0.0;

    for (int i = 0; i < octaves; i++) {

        normalization += pow(persistance, i);
        noise += interpolate(x * pow(2, i) , y * pow(2, i)) * pow(persistance, i);
    }

    return noise / normalization;
}

void PerlinNoise::generatePerlinNoiseMat()
{
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            perlin_noise[i][j] = perlinNoise(i, j);
        }
    }
}
