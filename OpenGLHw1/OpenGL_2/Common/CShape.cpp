#include "CShape.h"

CShape::CShape()
{
	_Points = nullptr;
	_Colors = nullptr;
	_nVtx = 0;
	_pVXshader = NULL;
	_pFSshader = NULL;
	
	_fClock = 0;

	_uiVao = 0;
	_uiBuffer = 0;

	// 紀錄是否有矩陣的更新
	_bUpdateMV = false;
	_bUpdateProj = false;

	/*_uiProgram = 0;
	_uiProjection = 0;
	_uiModelView = 0;*/
	_fColor[0] = _fColor[1] = _fColor[2] = _fColor[3] = 1.0f;

}

CShape::~CShape()
{
	if (_Points != nullptr)delete[]_Points;
	if (_Colors != nullptr)delete[]_Colors;

	if (_pVXshader != nullptr)delete[]_pVXshader;
	if (_pFSshader != nullptr)delete[]_pFSshader;
}

void CShape::CreateBufferObject()
{
	glGenVertexArrays(1, &_uiVao);
	glBindVertexArray(_uiVao);

	// Create and initialize a buffer object
	glGenBuffers(1, &_uiBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _uiBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * _nVtx * 2, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4)* _nVtx, _Points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4)* _nVtx, sizeof(vec4)* _nVtx,_Colors);

}

void CShape::setShader(mat4& mxView, mat4& mxProjection, GLuint uiShaderHandle)
{
	// Load shaders and use the resulting shader program
	if (uiShaderHandle == MAX_UNSIGNED_INT) _uiProgram = InitShader(_pVXshader, _pFSshader);
	else _uiProgram = uiShaderHandle;

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(_uiProgram, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(_uiProgram, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(_Points)));

	_uiModelView = glGetUniformLocation(_uiProgram, "ModelView");
	_mxView = mxView;
	glUniformMatrix4fv(_uiModelView, 1, GL_TRUE, _mxView);

	_uiProjection = glGetUniformLocation(_uiProgram, "Projection");
	_mxProjection = mxProjection;
	glUniformMatrix4fv(_uiProjection, 1, GL_TRUE, _mxProjection);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void CShape::setShaderName(const char vxShader[], const char fsShader[])
{
	int len;
	len = strlen(vxShader);
	_pVXshader = new char[len + 1];
	memcpy(_pVXshader, vxShader, len + 1);

	len = strlen(fsShader);
	_pFSshader = new char[len + 1];
	memcpy(_pFSshader, fsShader, len + 1);
}

void CShape::setViewMatrix(mat4 &mat)
{
	_mxView = mat;
	_bUpdateMV = true;
}

void CShape::setProjectionMatrix(mat4 &mat)
{
	_mxProjection = mat;
	_bUpdateProj = true;
}
void CShape::setTRSMatrix(mat4 &mat)
{
	_mxTRS = mat;
	_bUpdateMV = true;
}

void CShape::setColor(vec4 vColor)
{
	_fColor[0] = vColor.x; _fColor[1] = vColor.y;
	_fColor[2] = vColor.z; _fColor[3] = vColor.w;

	for (int i = 0; i < _nVtx; i++) {
		_Colors[i].x = _fColor[0];
		_Colors[i].y = _fColor[1];
		_Colors[i].z = _fColor[2];
		_Colors[i].w = _fColor[3];
	}
	glBindBuffer(GL_ARRAY_BUFFER, _uiBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, _nVtx * sizeof(vec4), _nVtx * sizeof(vec4), _Colors);
}

