/*
 * LoD terrain generation and displaying
 * Svoboda Petr, Vojvoda Jakub
 * 
 * 2014
 *
 */

#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

struct Point {
    float color[3];
    float position[3];
};

class TriangleMesh
{
public:
    TriangleMesh(double **map, int w, int h);

    void trianglesFromHeightMap();

    float **getPoints();
    float **getColors();

    Point *getPointsData();
    void buildTerrain(float shift, float scale, float randomness);

    unsigned int *getIndeces();

    int getPointsSize();
    int getIndecesSize();

    double **heightmap;
    Point *terrain;

    int width, height;

    int no_points;
    int no_raw_triangles;

    float **colors;
    float **points;

    unsigned int *indeces;
};

#endif // TRIANGLEMESH_H
