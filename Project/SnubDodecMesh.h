// SnubDodecMesh.h
// -- mesh for a snub dodecahedron
// cs250 1/15

#ifndef CS250_SNUBDODEC_H
#define CS250_SNUBDODEC_H

#include "Mesh_Object.h"

class SnubDodecMesh : public Mesh_Object {
  public:
    SnubDodecMesh();
    SnubDodecMesh(std::string vertexPath, std::string fragPath);
    int VertexCount(void) { return 60; }
    Point GetVertex(int i) { return vertices[i]; }
    Vector Dimensions(void) { return Vector(2,2,2); }
    Point Center(void) { return Point(0,0,0); }
    int FaceCount(void) { return 116; }
    Face GetFace(int i) { return faces[i]; }
    int EdgeCount(void) { return 150; }
    Edge GetEdge(int i) { return edges[i]; }
    virtual void SetVertices();

  private:
    static Face faces[116];
    static Edge edges[150];
};


#endif

