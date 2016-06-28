/*
 * LoD terrain generation and displaying
 * Svoboda Petr, Vojvoda Jakub
 * 
 * 2014
 *
 */

#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include "perlinnoise.h"

class HeightMap
{
public:
    HeightMap(int w, int h);
    ~HeightMap();

    int width, height;
    
    float **heightmap;
    unsigned char* hMap;

    unsigned char* get1DHeightMap(){ return hMap;}

    float **getHeightMap();
    float **generateHeightMap(float corners_weight, float diamond_weight);
    float **enlargeHeightMap(int step);

    void initSquate(float weight);
    void diamondSquare(int x1, int x2, int y1, int y2, float weight);

    void normalize();

    void addPerlinNoise(float weight, float persistance, int octaves, int method);
    void interpolate(int method);
    void smooth(float w1, float w2);

    unsigned char *convertTo1D(float **map);

private:
    PerlinNoise noise;
};

#endif // HEIGHTMAP_H
