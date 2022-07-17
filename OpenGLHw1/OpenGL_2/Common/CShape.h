#ifndef CSHAPE_H
#define CSHAPE_H
#include "../header/Angel.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

class CShape
{
private:

public:

	int _nVtx;
	char* _pVXshader, * _pFSshader;
	GLfloat _fColor[4];
	GLfloat _fClock;

	vec4* _Points;
	vec4* _Colors;
	

	//  for Shader
	GLuint _uiProgram;
	// Vertex Position Attribute
	GLuint _uiModelView, _uiProjection;
	// Matrix 
	mat4 _mxView, _mxProjection;
	mat4 _mxMVFinal, _mxTRS;

	// VAO
	GLuint _uiVao;
	// VBO
	GLuint _uiBuffer;

	// 紀錄是否有矩陣的更新
	bool  _bUpdateMV;
	bool  _bUpdateProj;

	void CreateBufferObject();

	CShape();
	virtual  ~CShape();
	
	virtual void draw() = 0;
	virtual void drawW() = 0; 
	
	
	void setShaderName(const char vxShader[], const char fsShader[]);
	void setShader(mat4 &mxModelView, mat4 &mxProjection, GLuint uiShaderHandle = MAX_UNSIGNED_INT);
	GLuint getShaderHandle() { return _uiProgram; }
	void setViewMatrix(mat4 &mat);
	void setProjectionMatrix(mat4 &mat);
	void setTRSMatrix(mat4 &mat);
	void setColor(vec4 vColor);

	float fEnemyX;
	float fEnemyY;
	bool isMoving;
	int blood = 1;
	float fScale = 1;
};

#endif
