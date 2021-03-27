/*************************************************************
 *	Author				: Kim sang min.
 *	Assignment number	: Project assignment
 *	Course				: CS250.
 *  Term				: Spring 2020.
 *************************************************************/

#include "SimpleCube_Mesh.h"


const Point SimpleCubeMesh::vertices[8] = {
	Point(1,-1,1), //0
	Point(1,1,1), //1
	Point(1,-1, -1), //2
	Point(1,1,-1), //3
	Point(-1,-1,1), //4
	Point(-1,1,1), //5
	Point(-1,-1,-1), //6
	Point(-1,1,-1) //7
};
const SimpleCubeMesh::Face SimpleCubeMesh::faces[12] = {
	Face(0,4,6), Face(0,6,2),
	Face(1,3,7), Face(1,7,5),
	Face(4,5,7), Face(4,7,6),
	Face(0,2,3), Face(0,3,1),
	Face(0,1,4), Face(1,5,4),
	Face(2,6,3), Face(3,6,7)
};
const SimpleCubeMesh::Edge SimpleCubeMesh::edges[12] = {
	Edge(0,4), Edge(4, 6),
	Edge(6,2), Edge(2,0),
	Edge(4,5), Edge(5,7),
	Edge(7,6), Edge(2,3),
	Edge(3,1), Edge(1,0),
	Edge(1,5), Edge(7,3)
};

SimpleCubeMesh::SimpleCubeMesh(bool is_light)
{
	if(!is_light)
	{
		Set_Normal();
		//Initialize_Ground_Mesh();
		Initialize(shader_simple_vertex.c_str(), shader_simple_fragment.c_str());
	}
	else
	{
		Set_Normal();
		Initialize(shader_light_vertex.c_str(), shader_light_fragment.c_str());
		//Initialize_Ground_Mesh(shader_light_vertex, shader_light_fragment);
	}
}

int SimpleCubeMesh::VertexCount()
{
	/*
	*	Since professor mentioned "as simple as possible", and
	*	also the vertices is const static array, return just size of vertices using magic number.
	*/
	return 8;
}

Point SimpleCubeMesh::GetVertex(int i)
{
	return vertices[i];
}

Vector SimpleCubeMesh::Dimensions()
{
	/*
	*	Since professor mentioned "as simple as possible", and
	*	also the vertices is const static array, value is fixed with {-1, 1},
	*	return the calculated delta x,y,z.
	*/
	return Vector(2.f, 2.f, 2.f);
}

Point SimpleCubeMesh::Center()
{
	/*
	*	Since professor mentioned "as simple as possible", and
	*	also the vertices is const static array, value is fixed with {-1, 1},
	*	return the calculated center value.
	*/
	return Point(0.f, 0.f, 0.f);
}

int SimpleCubeMesh::FaceCount()
{
	/*
	*	Since professor mentioned "as simple as possible", and
	*	also the faces is const static array, return just size of faces using magic number.
	*/
	return 12;
}

Mesh::Face SimpleCubeMesh::GetFace(int i)
{
	return faces[i];
}

int SimpleCubeMesh::EdgeCount()
{
	/*
	*	Since professor mentioned "as simple as possible", and
	*	also the edges is const static array, return just size of edges using magic number.
	*/
	return 12;
}

Mesh::Edge SimpleCubeMesh::GetEdge(int i)
{
	return edges[i];
}

void SimpleCubeMesh::Set_Normal()
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

		//Add_Vetrtex(P, Q, R);
	}
}

//void SimpleCubeMesh::Add_Vetrtex(Point P, Point Q, Point R)
//{
//	vertex_datas.push_back(P.x);
//	vertex_datas.push_back(P.y);
//	vertex_datas.push_back(P.z);
//
//	vertex_datas.push_back(Q.x);
//	vertex_datas.push_back(Q.y);
//	vertex_datas.push_back(Q.z);
//
//	vertex_datas.push_back(R.x);
//	vertex_datas.push_back(R.y);
//	vertex_datas.push_back(R.z);
//}
