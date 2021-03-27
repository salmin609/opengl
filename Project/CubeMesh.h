/*************************************************************
 *	Author				: Kim sang min.
 *	Assignment number	: Project assignment
 *	Course				: CS250.
 *  Term				: Spring 2020.
 *************************************************************/

#ifndef CS250_CUBEMESH_H
#define CS250_CUBEMESH_H

#include "Mesh_Object.h"

class CubeMesh : public Mesh_Object {
  public:
    CubeMesh();
    int VertexCount(void) override;
    Point GetVertex(int i) override;
    Vector Dimensions(void) override;
    Point Center(void) override;
    int FaceCount(void) override;
    Face GetFace(int i) override;
    int EdgeCount(void) override;
    Edge GetEdge(int i) override;
    virtual void SetVertices();

  private:
    static const Face faces[12];
    static const Edge edges[12];
};

#endif

