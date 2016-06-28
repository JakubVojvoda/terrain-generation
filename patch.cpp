/*
 * ROAM Simplistic Implementation
 * All code copyright Bryan Turner (Jan, 2000)
 * brturn@bellsouth.net
 * 
 * Based on the Tread Marks engine by Longbow Digital Arts
 * (www.LongbowDigitalArts.com)
 * Much help and hints provided by Seumas McNally, LDA.
 * 
 * Edited and rewritten by Petr Svoboda and Jakub Vojvoda
 * 
 */

#include "patch.h"
#include "landscape.h"

#include <iostream>
using namespace std;


void Patch::Split(TriTreeNode *tri)
{
    if ( tri == NULL ) return;
    // We are already split, no need to do it again.
    if (tri->LeftChild)
        return;

    // If this triangle is not in a proper diamond, force split our base neighbor
    if ( tri->BaseNeighbor && (tri->BaseNeighbor->BaseNeighbor != tri) )
        Split(tri->BaseNeighbor);

    // Create children and link into mesh
    tri->LeftChild  = Landscape::AllocateTri();
    tri->RightChild = Landscape::AllocateTri();

    // If creation failed, just exit.
    if ( !tri->LeftChild )
    {
        return;
    }

    // Fill in the information we can get from the parent (neighbor pointers)
    tri->LeftChild->BaseNeighbor  = tri->LeftNeighbor;
    tri->LeftChild->LeftNeighbor  = tri->RightChild;

    tri->RightChild->BaseNeighbor  = tri->RightNeighbor;
    tri->RightChild->RightNeighbor = tri->LeftChild;

    // Link our Left Neighbor to the new children
    if (tri->LeftNeighbor != NULL)
    {
        if (tri->LeftNeighbor->BaseNeighbor == tri)
            tri->LeftNeighbor->BaseNeighbor = tri->LeftChild;
        else if (tri->LeftNeighbor->LeftNeighbor == tri)
            tri->LeftNeighbor->LeftNeighbor = tri->LeftChild;
        else if (tri->LeftNeighbor->RightNeighbor == tri)
            tri->LeftNeighbor->RightNeighbor = tri->LeftChild;
        else
        {
        }
    }

    // Link our Right Neighbor to the new children
    if (tri->RightNeighbor != NULL)
    {
        if (tri->RightNeighbor->BaseNeighbor == tri)
            tri->RightNeighbor->BaseNeighbor = tri->RightChild;
        else if (tri->RightNeighbor->RightNeighbor == tri)
            tri->RightNeighbor->RightNeighbor = tri->RightChild;
        else if (tri->RightNeighbor->LeftNeighbor == tri)
            tri->RightNeighbor->LeftNeighbor = tri->RightChild;
        else{}
    }

    // Link our Base Neighbor to the new children
    if (tri->BaseNeighbor != NULL)
    {
        if ( tri->BaseNeighbor->LeftChild )
        {
            tri->BaseNeighbor->LeftChild->RightNeighbor = tri->RightChild;
            tri->BaseNeighbor->RightChild->LeftNeighbor = tri->LeftChild;
            tri->LeftChild->RightNeighbor = tri->BaseNeighbor->RightChild;
            tri->RightChild->LeftNeighbor = tri->BaseNeighbor->LeftChild;
        }
        else
            Split( tri->BaseNeighbor);
    }
    else
    {
        // An edge triangle, trivial case.
        tri->LeftChild->RightNeighbor = NULL;
        tri->RightChild->LeftNeighbor = NULL;
    }
}

void Patch::SplitPath( int recurse_depth)
{
        RecurseSplitPath (	&m_BaseLeft,
                            m_WorldX,				m_WorldY+PATCH_SIZE,
                            m_WorldX+PATCH_SIZE,	m_WorldY,
                            m_WorldX,				m_WorldY,
                            recurse_depth );


        RecurseSplitPath(	&m_BaseRight,
                            m_WorldX+PATCH_SIZE,	m_WorldY,
                            m_WorldX,				m_WorldY+PATCH_SIZE,
                            m_WorldX+PATCH_SIZE,	m_WorldY+PATCH_SIZE,
                            recurse_depth );
}

void Patch::RecurseSplitPath( TriTreeNode *tri,
                             int leftX,  int leftY,
                             int rightX, int rightY,
                             int apexX,  int apexY,
                             int depth )
{
    int centerX = (leftX + rightX)>>1; 
    int centerY = (leftY + rightY)>>1; 

    if ( depth > 0) {
        --depth;
        Split(tri);
        if ( tri->LeftChild ==  NULL ) return;
        RecurseSplitPath( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY, depth  );
        RecurseSplitPath( tri->LeftChild,   apexX,  apexY, leftX, leftY, centerX, centerY, depth  );
    }
}

void Patch::RecursRender( TriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY )
{

    if ( tri->LeftChild ) {
        int centerX = (leftX + rightX)>>1;
        int centerY = (leftY + rightY)>>1;

        RecursRender( tri->LeftChild,  apexX,   apexY, leftX, leftY, centerX, centerY );
        RecursRender( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY );
    }
    else {

        float leftZ  = m_HeightMap[m_WorldY + leftY ][m_WorldX+leftX] * HIGH;
        float rightZ = m_HeightMap[m_WorldY + rightY][m_WorldX+rightX] * HIGH;
        float apexZ  = m_HeightMap[m_WorldY + apexY ][m_WorldX+apexX] * HIGH;

        float norm_left = leftZ / HIGH;
        float norm_right = rightZ / HIGH;
        float norm_apex = apexZ / HIGH;

        float color_left[3], color_right[3], color_apex[3];

        fillColor(color_left, norm_left);
        fillColor(color_right, norm_right);
        fillColor(color_apex, norm_apex);

        Point jedna = { {color_left[R], color_left[G], color_left[B]}, {(float)leftY + m_WorldY,(float)leftZ,(float)leftX + m_WorldX}};
        Point dva = { {color_right[R], color_right[G], color_right[B]}, {(float)rightY + m_WorldY,(float)rightZ,(float)rightX + m_WorldX}};
        Point tri = { {color_apex[R], color_apex[G], color_apex[B]}, {(float)apexY + m_WorldY,(float)apexZ,(float)apexX + m_WorldX}};

        data.push_back(jedna);
        data.push_back(dva);
        data.push_back(tri);
    }
}

void Patch::fillColor(float color[3], float nheight)
{
    if (nheight < 0.09) {
        color[R] = 0.0;
        color[G] = 0.5 * nheight;
        color[B] = nheight;
    }

    else if (nheight < 0.18) {
        color[R] = 0.9 * nheight;
        color[G] = 0.9 * nheight;
        color[B] = 0.5 * nheight;
    }

    else if (nheight < 0.60) {
        color[R] = 0.0;
        color[G] = 0.6 * nheight;
        color[B] = 0.0;
    }

    else if (nheight < 0.72) {
        color[R] = 0.4 * nheight;
        color[G] = 0.6 * nheight;
        color[B] = 0.4 * nheight;
    }

    else if (nheight < 0.8) {
        color[R] = nheight;
        color[G] = nheight;
        color[B] = nheight;
    }

    else {
        color[R] = 0.5 * nheight;
        color[G] = 0.6 * nheight;
        color[B] = 0.5 * nheight;
    }
}

void Patch::Init(int worldX, int worldY, float **hMap )
{
    // Clear all the relationships
    m_BaseLeft.RightNeighbor = m_BaseLeft.LeftNeighbor = m_BaseRight.RightNeighbor = m_BaseRight.LeftNeighbor = NULL;
    m_BaseLeft.LeftChild = m_BaseLeft.RightChild = m_BaseRight.LeftChild = m_BaseRight.RightChild = NULL;

    // Attach the two m_Base triangles together
    m_BaseLeft.BaseNeighbor = &m_BaseRight;
    m_BaseRight.BaseNeighbor = &m_BaseLeft;

    // Store pointer to first byte of the height data for this patch.
    m_HeightMap = hMap;

    // Store Patch offsets for the world and heightmap.
    m_WorldX = worldX;
    m_WorldY = worldY;

    // Create buffer for rendering
    glGenBuffers(1, &buff_data);
}

void Patch::Reset()
{
    // Reset the important relationships
    m_BaseLeft.LeftChild = m_BaseLeft.RightChild = m_BaseRight.LeftChild = m_BaseRight.RightChild = NULL;

    // Attach the two m_Base triangles together
    m_BaseLeft.BaseNeighbor = &m_BaseRight;
    m_BaseRight.BaseNeighbor = &m_BaseLeft;

    // Clear the other relationships.
    m_BaseLeft.RightNeighbor = m_BaseLeft.LeftNeighbor = m_BaseRight.RightNeighbor = m_BaseRight.LeftNeighbor = NULL;
}

void Patch::Render()
{
    data.clear();
        RecursRender (	&m_BaseLeft,
            0,				PATCH_SIZE,
            PATCH_SIZE,		0,
            0,				0);

        RecursRender(	&m_BaseRight,
            PATCH_SIZE,		0,
            0,				PATCH_SIZE,
            PATCH_SIZE,		PATCH_SIZE);

        glEnableVertexAttribArray(positionAttrib);
        glEnableVertexAttribArray(colorAttrib);
        glBindBuffer(GL_ARRAY_BUFFER, buff_data);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * data.size(), data.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, position));
        glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, color));
        if ( draw_triangles )
        {
            glDrawArrays(GL_TRIANGLES, 0, data.size());
        }
        else
            glDrawArrays(GL_LINES, 0, data.size());

}
