/*
 * LoD terrain generation and displaying
 * Svoboda Petr, Vojvoda Jakub
 * 
 * 2014
 *
 */

#include "pgr.h"
#include "patch.h"
#include "landscape.h"


int         Landscape::m_NextTriNode;
TriTreeNode Landscape::m_TriPool[POOL_SIZE];

TriTreeNode *Landscape::AllocateTri()
{
    TriTreeNode *pTri;

    if ( m_NextTriNode >= POOL_SIZE )
        return NULL;

    pTri = &(m_TriPool[m_NextTriNode++]);
    pTri->LeftChild = pTri->RightChild = NULL;

    return pTri;
}

void Landscape::Init(float **hMap)
{
    if ( hMap == NULL ) {
        std::cerr<<"Landscape Init NULL pointer"<<std::endl;
    }
    Patch *patch;

    heightMap = new float*[MAP_SIZE+1];
    for ( int i=0;i < MAP_SIZE+1;i++) {
        heightMap[i] = new float[MAP_SIZE+1];
    }
    for ( int i=0; i < MAP_SIZE ; i++) {
        for ( int j = 0;j < MAP_SIZE ;j++) {
            heightMap[i][j] = hMap[i][j];
        }
        heightMap[i][MAP_SIZE] = hMap[i][MAP_SIZE -1 ];
        heightMap[MAP_SIZE][i] = hMap[MAP_SIZE-1][i];
    }

    for ( int Y=0; Y < NUM_PATCHES_PER_SIDE; Y++) {
        for ( int X=0; X < NUM_PATCHES_PER_SIDE; X++ ) {
            patch = &(m_Patches[Y][X]);
            patch->Init(X*(PATCH_SIZE), Y*(PATCH_SIZE), heightMap );
            sorted_distance.push_back(patch);
        }
    }
}

void Landscape::Reset()
{
    Patch *patch;

    SetNextTriNode(0);

    for ( int Y=0; Y < NUM_PATCHES_PER_SIDE; Y++ ) {
        for ( int X=0; X < NUM_PATCHES_PER_SIDE; X++) {
            patch = &(m_Patches[Y][X]);
            patch->Reset();

            if ( X > 0 )
                patch->GetBaseLeft()->LeftNeighbor = m_Patches[Y][X-1].GetBaseRight();
            else
                patch->GetBaseLeft()->LeftNeighbor = NULL;
            if ( X < (NUM_PATCHES_PER_SIDE-1) )
                patch->GetBaseRight()->LeftNeighbor = m_Patches[Y][X+1].GetBaseLeft();
            else
                patch->GetBaseRight()->LeftNeighbor = NULL;

            if ( Y > 0 )
                patch->GetBaseLeft()->RightNeighbor = m_Patches[Y-1][X].GetBaseRight();
            else
                patch->GetBaseLeft()->RightNeighbor = NULL;

            if ( Y < (NUM_PATCHES_PER_SIDE-1) )
                patch->GetBaseRight()->RightNeighbor = m_Patches[Y+1][X].GetBaseLeft();
            else
                patch->GetBaseRight()->RightNeighbor = NULL;
        }
    }
}

void Landscape::updateDistance()
{

    for (std::vector<Patch*>::iterator it=sorted_distance.begin(); it!=sorted_distance.end(); ++it) {
        (*it)->distance = glm::distance(glm::vec2(camera.z,camera.x),(*it)->getPosition());
    }

}

void Landscape::splitBasedOnDistance()
{

    int i = 0;
    int deep = 12;
    std::sort( sorted_distance.begin(),sorted_distance.end(), Patch::porovnani );
    for (std::vector<Patch*>::iterator it=sorted_distance.begin(); it!=sorted_distance.end(); ++it) {
        i++;
        if ( i == 5) { deep--; i=0; }
        (*it)->SplitPath(deep);
    }

}

void Landscape::renderBasedOnDistance()
{
    for (std::vector<Patch*>::iterator it=sorted_distance.begin(); it!=sorted_distance.end(); ++it) {
        (*it)->Render();
    }
}
