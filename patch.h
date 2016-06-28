/*
 * LoD terrain generation and displaying
 * Svoboda Petr, Vojvoda Jakub
 * 
 * 2014
 *
 */

#ifndef PATCH_H
#define PATCH_H

#include "pgr.h"
#include <iostream>
#include <vector>
#include "glm/glm.hpp"
#include "trianglemesh.h"
#include "perlinnoise.h"

const float HIGH = 25;

#define R 0
#define G 1
#define B 2

class Landscape;
extern GLuint positionAttrib,colorAttrib;
extern bool draw_triangles;

struct TriTreeNode
{
    TriTreeNode *LeftChild;
    TriTreeNode *RightChild;
    TriTreeNode *BaseNeighbor;
    TriTreeNode *LeftNeighbor;
    TriTreeNode *RightNeighbor;
};

class Patch
{
protected:
    GLuint buff_data;                     
    std::vector<struct Point> data;       

    float **m_HeightMap;									
    int m_WorldX, m_WorldY;									

    TriTreeNode m_BaseLeft;                 
    TriTreeNode m_BaseRight;								
public:
    float distance;                                         
    TriTreeNode *GetBaseLeft()  { return &m_BaseLeft; }
    TriTreeNode *GetBaseRight() { return &m_BaseRight; }
    glm::vec2 getPosition(){return glm::vec2(m_WorldX,m_WorldY);}
    void RecurseSplitPath( TriTreeNode *tri,
                                 int leftX,  int leftY,
                                 int rightX, int rightY,
                                 int apexX,  int apexY,
                                 int depth );
    void SplitPath(int depth);

    static bool porovnani(Patch* uno, Patch* dos){ return (uno->distance < dos->distance);}

    void Init(int worldX, int worldY, float **hMap );
    void Reset();
    void Render();

    void Split( TriTreeNode *tri);

    void RecursRender( TriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY );

    void fillColor(float color[], float nheight);
};


#endif
