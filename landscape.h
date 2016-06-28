/*
 * LoD terrain generation and displaying
 * Svoboda Petr, Vojvoda Jakub
 * 
 * 2014
 *
 */

#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include "patch.h"
#include "pgr.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <exception>

#define MAP_SIZE (513)
#define NUM_PATCHES_PER_SIDE (8)

#define POOL_SIZE (150000)

#define PATCH_SIZE (MAP_SIZE/NUM_PATCHES_PER_SIDE)
#define MAX(a,b) ((a < b) ? (b) : (a))

extern glm::vec3 camera;

class Landscape
{
protected:
    Patch m_Patches[NUM_PATCHES_PER_SIDE][NUM_PATCHES_PER_SIDE];	
    std::vector<Patch*> sorted_distance;                            

    static int	m_NextTriNode;										
    static TriTreeNode m_TriPool[POOL_SIZE];				

    static int GetNextTriNode() { return m_NextTriNode; }
    static void SetNextTriNode( int nNextNode ) { m_NextTriNode = nNextNode; }

public:
    float** heightMap;                                  
    static TriTreeNode *AllocateTri();

    void updateDistance();
    void splitBasedOnDistance();
    void renderBasedOnDistance();

    void Init(float **hMap);
    void Reset();

};


#endif
