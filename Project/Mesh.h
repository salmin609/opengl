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
#include "ParticleInstance.h"
#include "Shader_Table.hpp"
#include "Shader.h"

class Texture;
class VAO;

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

	virtual ~Mesh(void);
	virtual int FaceCount(void) = 0;
	virtual unsigned Get_VAO_Id();
	virtual unsigned Get_Shader_Id();

	void Initialize(const char* vertexPath, const char* fragmentPath);
	void InitializeTexturedObj(std::string spritePath, std::string vertex = shader_texture_vertex, std::string fragment = shader_texture_fragment);
	void Initialize_Texture(std::string sprite_path);
	void InitializeInstanceObj(std::string spritePath, std::string vertexPath = shaderInstanceVertex, std::string fragPath = shaderInstanceFragment);
	void InitializeColoredParticle(std::string vertexPath, std::string fragmentPath);
	void SetTexture();
	bool IsElemented();
	unsigned GetElementId();
	Shader* GetShader();
	void Init_VAO();
	void Init_VBO(void* data, unsigned* slot, size_t arr_size, int stride, void* offset, int index, int vec_size, GLenum drawingType = GL_STATIC_DRAW, bool isNormalize = false);
	void Unbind();
	bool IsInstancing();
	int InstancingNum() const;
protected:
	Shader* shader = nullptr;
	unsigned vao_id = 0;
	unsigned vbo_id = 0;
	unsigned color_id = 0;
	unsigned rotateId = 0;
	unsigned elementId = 0;
	unsigned instancingId = 0;
	unsigned matrixId = 0;
	
	std::vector<Texture*> textures;
	std::vector<unsigned int> elements;
	
	std::vector<Vertex> vertexDatas;
	std::vector<Vector3> offsetVec;
	
	std::vector<ParticleInstance> particles;
	
	int instancingNum;
	int particleNum;
	bool isQuad = false;
	bool isInstancing = false;
	float timer;
	VAO* vao;
};


#endif

