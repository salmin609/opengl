/*************************************************************
 *	Author				: Kim sang min.
 *	Assignment number	: Project assignment
 *	Course				: CS250.
 *  Term				: Spring 2020.
 *************************************************************/

#ifndef CS250_SimpleCubeMesh_H
#define CS250_SimpleCubeMesh_H

#include "Mesh.h"

class SimpleCubeMesh : public Mesh {
public:
    SimpleCubeMesh(bool is_light = false);
    int VertexCount(void) override;
    Point GetVertex(int i) override;
    Vector Dimensions(void) override;
    Point Center(void) override;
    int FaceCount(void) override;
    Face GetFace(int i) override;
    int EdgeCount(void) override;
    Edge GetEdge(int i) override;
    void Set_Normal();
    //void Add_Vetrtex(Point P, Point Q, Point R);

private:
    static const Point vertices[8];
    static const Face faces[12];
    static const Edge edges[12];
};

#endif

