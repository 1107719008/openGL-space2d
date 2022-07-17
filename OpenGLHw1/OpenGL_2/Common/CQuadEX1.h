#ifndef CQuadEX1_H
#define CQuadEX1_H
#include "../header/Angel.h"
#include "CShape.h"
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define QUAD_NUM_ONE 8  // 2 faces, 2 triangles/face 
#define MOVESPEED 2

class CQuadEX1 : public CShape
{
private:
	//vec4 _Points[QUAD_NUM_ONE];
	//vec4 _Colors[QUAD_NUM_ONE];
	//// VAO
	//GLuint _uiVao;
	//// VBO
	//GLuint _uiBuffer;
	////  for Shader
	//GLuint _uiProgram ;
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
	CQuadEX1();
	~CQuadEX1() {};

	/*float fEnemyX;
	float fEnemyY;
	int blood = 1;
	float fScale = 1;*/

	//void setShader(mat4 &mxModelView, mat4 &mxProjection, GLuint uiShaderHandle=MAX_UNSIGNED_INT);
	GLuint getShaderHandle() { return _uiProgram;}
	//void setViewMatrix(mat4 &mat);
	//void setProjectionMatrix(mat4 &mat);
	//void setTRSMatrix(mat4 &mat);
	//void setColor(GLfloat vColor[4]); // Single color
	//void setVtxColors(GLfloat vLFColor[], GLfloat vLRColor[], GLfloat vTRColor[], GLfloat vTLColor[]); // four Vertices' Color


	void Move(float dt);
	void Disable(float dt);

	void draw();
	void drawW();



};




#endif