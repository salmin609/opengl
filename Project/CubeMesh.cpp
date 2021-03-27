/*************************************************************
 *	Author				: Kim sang min.
 *	Assignment number	: Project assignment
 *	Course				: CS250.
 *  Term				: Spring 2020.
 *************************************************************/

#include "CubeMesh.h"

const CubeMesh::Face CubeMesh::faces[12] = {
	Face(0,4,6), Face(0,6,2),
	Face(1,3,7), Face(1,7,5),
	Face(4,5,7), Face(4,7,6),
	Face(0,2,3), Face(0,3,1),
	Face(0,1,4), Face(1,5,4),
	Face(2,6,3), Face(3,6,7)
};
const CubeMesh::Edge CubeMesh::edges[12] = {
	Edge(0,4), Edge(4, 6),
	Edge(6,2), Edge(2,0),
	Edge(4,5), Edge(5,7),
	Edge(7,6), Edge(2,3),
	Edge(3,1), Edge(1,0),
	Edge(1,5), Edge(7,3)
};

CubeMesh::CubeMesh()
{
	CubeMesh::SetVertices();
	InitializeTextureObj("container.png");
}

int CubeMesh::VertexCount()
{
	/*
	*	Since professor mentioned "as simple as possible", and
	*	also the vertices is const static array, return just size of vertices using magic number.
	*/
	return 8;
}

Point CubeMesh::GetVertex(int i)
{
	return vertices[i];
}

Vector CubeMesh::Dimensions()
{
	/*
	*	Since professor mentioned "as simple as possible", and
	*	also the vertices is const static array, value is fixed with {-1, 1},
	*	return the calculated delta x,y,z.
	*/
	return Vector(2.f, 2.f, 2.f);
}

Point CubeMesh::Center()
{
	/*
	*	Since professor mentioned "as simple as possible", and
	*	also the vertices is const static array, value is fixed with {-1, 1},
	*	return the calculated center value.
	*/
	return Point(0.f, 0.f, 0.f);
}

int CubeMesh::FaceCount()
{
	/*
	*	Since professor mentioned "as simple as possible", and
	*	also the faces is const static array, return just size of faces using magic number.
	*/
	return 12;
}

Mesh::Face CubeMesh::GetFace(int i)
{
	return faces[i];
}

int CubeMesh::EdgeCount()
{
	/*
	*	Since professor mentioned "as simple as possible", and
	*	also the edges is const static array, return just size of edges using magic number.
	*/
	return 12;
}

Mesh::Edge CubeMesh::GetEdge(int i)
{
	return edges[i];
}
void CubeMesh::SetVertices()
{
	const int vert_count = VertexCount();

	const Point vertarr[] = {
		Point(1, -1, 1),
		Point(1, 1, 1),
		Point(1, -1, -1),
		Point(1, 1, -1),
		Point(-1, -1, 1),
		Point(-1, 1, 1),
		Point(-1, -1, -1),
		Point(-1, 1, -1)
	};

	vertices.insert(vertices.end(), vertarr, vertarr + vert_count);

	for (int i = 0; i < vert_count; ++i)
	{
		normal.push_back(Hcoord());
	}
}