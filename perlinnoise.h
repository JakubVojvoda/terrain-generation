/*
 * LoD terrain generation and displaying
 * Svoboda Petr, Vojvoda Jakub
 * 
 * 2014
 *
 */

#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#define PNOISE_WIDTH 513
#define PNOISE_HEIGHT 513

#define PI 3.141593

#define INTERPOLATION_LINEAR 1
#define INTERPOLATION_COS    2

class PerlinNoise
{
public:
    PerlinNoise();
    PerlinNoise(int w, int h);
    PerlinNoise(int w, int h, float pers, int n_octs, int method);

    void free();
    void setInterpolationMethod(int method);
    void setPerlinNoise(float pers, int n_octs);
    void setSize(int w, int h);

    float **getRandomNoise();
    float **getSmoothNoise();
    float **getInterpolatedNoise();
    float **getPerlinNoise();

    void randomize();
    float randomValue();
    float randomNoise(int x, int y);
    void generateRandomNoiseMat();

    float linearInterpolation(float a, float b, float x);
    float cosinInterpolation(float a, float b, float x);
    float interpolate(float x, float y);
    void generateInterpNoiseMat();

    float smoothing(int x, int y);
    float smoothing(int x, int y, int w1, int w2, int w3);
    void generateSmoothNoiseMat();

    float perlinNoise(float x, float y);
    void generatePerlinNoiseMat();

protected:
    int width, height;

    int interpolation_method;

    float persistance;
    int octaves;

    float **random_noise;
    float **smooth_noise;
    float **interp_noise;
    float **perlin_noise;

private:
    unsigned int seed;

    void allocate();
};

#endif // PERLINNOISE_H
