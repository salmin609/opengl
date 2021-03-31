/*************************************************************
 *	Author				: Kim sang min.
 *	Assignment number	: Project assignment
 *	Course				: CS250.
 *  Term				: Spring 2020.
 *************************************************************/

#ifndef CS250_MESH_H
#define CS250_MESH_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include <string>
#include "Affine.h"
#include "Shader_Table.hpp"
#include "Shader.h"

struct Mesh {

	struct Face {
		unsigned int index1, index2, index3;
		Face(int i = -1, int j = -1, int k = -1)
			: index1(i), index2(j), index3(k) {}
	};

	struct Edge {
		int index1, index2;
		Edge(int i = -1, int j = -1)
			: index1(i), index2(j) {}
	};

	virtual ~Mesh(void)
	{
		glDeleteVertexArrays(1, &vao_id);
		glDeleteBuffers(1, &vbo_id);
		glDeleteProgram(shader->GetShaderId());
		delete shader;
	}
	virtual int VertexCount(void) = 0;
	virtual Point GetVertex(int i) = 0;
	virtual Vector Dimensions(void) = 0;
	virtual Point Center(void) = 0;
	virtual int FaceCount(void) = 0;
	virtual Face GetFace(int i) = 0;
	virtual int EdgeCount(void) = 0;
	virtual Edge GetEdge(int i) = 0;
	virtual unsigned Get_VAO_Id();
	virtual unsigned Get_Shader_Id();
	unsigned Get_Texture_Id() const;
	unsigned Get_Seconde_Texture_Id() const;

	void Initialize(const char* vertexPath, const char* fragmentPath);
	void Initialize_Object_Mesh(std::string vertex_path = shader_object_vertex, std::string frag_path = shader_object_fragment);
	void Initialize_Texture(std::string sprite_path);
	void InitializeTexturedObj(std::string sprite_path, std::string vertex_path = shader_texture_vertex, std::string frag_path = shader_texture_fragment);
	void InitializeInstanceObj(std::string spritePath, std::string vertexPath = shaderInstanceVertex, std::string fragPath = shaderInstanceFragment);
	void InitializeColoredParticle(std::string vertexPath, std::string fragmentPath);
	void Clear_Datas();
	bool Get_Is_Textured() const;
	void SetTexture();
	bool IsElemented();
	unsigned GetElementId();
	bool IsQuadObj();
	
	void Init_VAO();
	void Init_VBO(void* data, unsigned* slot, size_t arr_size, int stride, void* offset, int index, int vec_size, GLenum drawingType = GL_STATIC_DRAW, bool isNormalize = false);
	static void Unbind();
	bool IsInstancing();
	int InstancingNum() const;
	void MoveParticle();
protected:
	Shader* shader = nullptr;
	unsigned vao_id = 0;
	unsigned vbo_id = 0;
	unsigned color_id = 0;
	unsigned vbo_normal = 0;
	unsigned elementId = 0;
	unsigned texture_id = 0;
	unsigned specular_texture_id = 0;
	unsigned instancingId = 0;
	unsigned matrixId = 0;
	std::vector<unsigned> textureId;
	std::vector<unsigned int> elements;
	
	std::vector<Vertex> vertexDatas;
	std::vector<Vector3> offsetVec;
	std::vector<Vector> particleTranslation;
	
	int instancingNum;
	int particleNum;
	bool is_textured = false;
	bool isQuad = false;
	bool isInstancing = false;
};


#endif

