/*************************************************************
 *	Author				: Kim sang min.
 *	Assignment number	: Project assignment
 *	Course				: CS250.
 *  Term				: Spring 2020.
 *************************************************************/
#include "Sphere_mesh.h"

const Point SphereMesh::vertices[9] = {
	Point(0,0,0),//0
	Point(0,0,1),//1
	Point(0.75f,0,0.75f),//2
	Point(1,0,0),//3
	Point(0.75f,0,-0.75f),//4
	Point(0,0,-1.f),//5
	Point(-0.75f,0,-0.75f),//6
	Point(-1.f,0,0),//7
	Point(-0.75f,0,0.75f),//8
};
const SphereMesh::Face SphereMesh::faces[8] = {
	Face(1,0,2), 
	Face(2,0,3),
	Face(3,0,4), 
	Face(4,0,5),
	Face(5,0,6), 
	Face(6,0,7),
	Face(7,0,8), 
	Face(8,0,1),
};
const SphereMesh::Edge SphereMesh::edges[12] = {
	Edge(0,4), Edge(4, 6),
	Edge(6,2), Edge(2,0),
	Edge(4,5), Edge(5,7),
	Edge(7,6), Edge(2,3),
	Edge(3,1), Edge(1,0),
	Edge(1,5), Edge(7,3)
};

SphereMesh::SphereMesh()
{
	Set_Normal();
	//Initialize_Shadow_Mesh();
	Initialize(shader_shadow_vertex.c_str(), shader_shadow_fragment.c_str());
}

int SphereMesh::VertexCount()
{
	/*
	*	Since professor mentioned "as simple as possible", and
	*	also the vertices is const static array, return just size of vertices using magic number.
	*/
	return 9;
}

Point SphereMesh::GetVertex(int i)
{
	return vertices[i];
}

Vector SphereMesh::Dimensions()
{
	/*
	*	Since professor mentioned "as simple as possible", and
	*	also the vertices is const static array, value is fixed with {-1, 1},
	*	return the calculated delta x,y,z.
	*/
	return Vector(2.f, 2.f, 2.f);
}

Point SphereMesh::Center()
{
	/*
	*	Since professor mentioned "as simple as possible", and
	*	also the vertices is const static array, value is fixed with {-1, 1},
	*	return the calculated center value.
	*/
	return Point(0.f, 0.f, 0.f);
}

int SphereMesh::FaceCount()
{
	/*
	*	Since professor mentioned "as simple as possible", and
	*	also the faces is const static array, return just size of faces using magic number.
	*/
	return 8;
}

Mesh::Face SphereMesh::GetFace(int i)
{
	return faces[i];
}

int SphereMesh::EdgeCount()
{
	/*
	*	Since professor mentioned "as simple as possible", and
	*	also the edges is const static array, return just size of edges using magic number.
	*/
	return 12;
}

Mesh::Edge SphereMesh::GetEdge(int i)
{
	return edges[i];
}

void SphereMesh::Set_Normal()
{
	const size_t face_count = FaceCount();

	const Point E(0.f, 0.f, 0.f);

	for (size_t i = 0; i < face_count; ++i)
	{
		const Mesh::Face face = GetFace(static_cast<int>(i));

		const Point P = vertices[face.index1];
		const Point Q = vertices[face.index2];
		const Point R = vertices[face.index3];

		const Vector Q_minus_P = Q - P;
		const Vector R_minus_P = R - P;

		Vector orientation_vec(cross(Q_minus_P, R_minus_P));

		orientation_vec = normalize(orientation_vec);

		Add_Vetrtex(P, Q, R);
	}
}

void SphereMesh::Add_Vetrtex(Point P, Point Q, Point R)
{
	vertexDatas.push_back(Vertex{ P.x, P.y, P.z });
	vertexDatas.push_back(Vertex{ Q.x, Q.y, Q.z });
	vertexDatas.push_back(Vertex{ R.x, R.y, R.z });
}
