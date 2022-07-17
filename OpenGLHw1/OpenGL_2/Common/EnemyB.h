#pragma once
#include "../Header/Angel.h"
#include "CShape.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define QUAD_BNUM 12

class EnemyB : public CShape
{
private:
	//vec4 _Points[QUAD_BNUM];
	//vec4 _Colors[QUAD_BNUM];
	//// VAO
	//GLuint _uiVao;
	//// VBO
	//GLuint _uiBuffer;
	////  for Shader
	//GLuint _uiProgram;
	//// Vertex Position Attribute
	//GLuint _uiModelView, _uiProjection;

	//// Matrix 
	//mat4 _mxView, _mxProjection;
	//mat4 _mxMVFinal, _mxTRS;

	//// 紀錄是否有矩陣的更新
	//bool  _bUpdateMV;
	//bool  _bUpdateProj;

	//void CreateBufferObject();


public:
	EnemyB();
	~EnemyB() {};
	float Yvalue = 0;//use for move
	/*float fEnemyX;
	float fEnemyY;*/
	/*bool isMoving;
	int blood = 1;
	float fScale = 1;*/
	/*void setShader(mat4& mxModelView, mat4& mxProjection, GLuint uiShaderHandle = MAX_UNSIGNED_INT);*/
	GLuint getShaderHandle() { return _uiProgram; }
	//void setViewMatrix(mat4& mat);
	//void setProjectionMatrix(mat4& mat);
	//void setTRSMatrix(mat4& mat);
	//void setColor(GLfloat vColor[4]); // Single color
	//void setVtxColors(GLfloat vLFColor[], GLfloat vLRColor[], GLfloat vTRColor[], GLfloat vTLColor[]); // four Vertices' Color

	void Move(float Xdt, float Ydt);
	void Disable(float dt);
	void draw();
	void drawW();


};



//EnemyB::~EnemyB()
//{
//
//}