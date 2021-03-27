#pragma once

#include "Mesh_Object.h"

class LoadedObj : public Mesh_Object
{
public :
    LoadedObj();
    LoadedObj(std::string filePath, bool isTextured = false, bool instacing = false);
    int VertexCount(void) { return 0; }
    Point GetVertex(int i) { (i); return Point{}; }
    Vector Dimensions(void) { return Vector(2, 2, 2); }
    Point Center(void) { return Point(0, 0, 0); }
    int FaceCount(void) { return faceCount; }
    Face GetFace(int i) { (i); return Face{}; }
    int EdgeCount(void) { return 150; }
    Edge GetEdge(int i) { (i); return Edge{}; }
    virtual void SetVertices();
    void SetIsRoughModel(bool trigger);

private :
    std::string file;
    bool consistNormal = false;
    bool consistTexture = false;
    int faceCount = 0;
};
