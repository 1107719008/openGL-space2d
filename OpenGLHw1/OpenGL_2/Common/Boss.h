#pragma once
#include "../Header/Angel.h"
//this is boss 

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define QUAD_BNUM 27

class Boss
{
private:
	vec4 _Points[QUAD_BNUM];
	
	// VAO
	GLuint _uiVao;
	// VBO
	GLuint _uiBuffer;
	//  for Shader
	GLuint _uiProgram;
	// Vertex Position Attribute
	GLuint _uiModelView, _uiProjection;

	// Matrix 
	mat4 _mxView, _mxProjection;
	mat4 _mxMVFinal, _mxTRS;

	// 紀錄是否有矩陣的更新
	bool  _bUpdateMV;
	bool  _bUpdateProj;

	void CreateBufferObject();


public:
	Boss();
	/*~Boss();*/

	vec4 _Colors[QUAD_BNUM];

	float fEnemyX;
	float fEnemyY;
	bool isMoving;
	float fScale = 0;
	int blood = 3;

	void setShader(mat4& mxModelView, mat4& mxProjection, GLuint uiShaderHandle = MAX_UNSIGNED_INT);
	GLuint getShaderHandle() { return _uiProgram; }
	void setViewMatrix(mat4& mat);
	void setProjectionMatrix(mat4& mat);
	void setTRSMatrix(mat4& mat);
	void setColor(GLfloat vColor[4]); // Single color

	void setEyeColor(GLfloat vColor[4]); //eye color

	void WingColor(GLfloat vColor[4]); //eye color

	void setVtxColors(GLfloat vLFColor[], GLfloat vLRColor[], GLfloat vTRColor[], GLfloat vTLColor[]); // four Vertices' Color

	void UltMove(float dt);
	void Move(float Xdt, float Ydt);
	void Disable(float dt);
	void StateChange(float dt);

	void draw();
	void drawW();


};



//Boss::~Boss()
//{
//
//}