#include "CBG.h"
//background Data
CBG::CBG()
{
	//bg obj square rec
	_Points[0] = vec4(-0.015f, 0.6f, 0.0f, 1.0f);
	_Points[1] = vec4(-0.015f, -0.6f, 0.0f, 1.0f);
	_Points[2] = vec4(0.015f, -0.6f, 0.0f, 1.0f);
	_Points[3] = vec4(-0.015f, 0.6f, 0.0f, 1.0f);
	_Points[4] = vec4(0.015f, -0.6f, 0.0f, 1.0f);
	_Points[5] = vec4(0.015f, 0.6f, 0.0f, 1.0f);

	_Colors[0] = vec4( 1.0f, 1.0f,  1.0f, 1.0f);  // (r, g, b, a)
	_Colors[1] = vec4( 1.0f, 1.0f,  1.0f, 1.0f);
	_Colors[2] = vec4( 1.0f, 1.0f,  1.0f, 1.0f);
	_Colors[3] = vec4( 1.0f, 1.0f,  1.0f, 1.0f);
	_Colors[4] = vec4( 1.0f, 1.0f,  1.0f, 1.0f);
	_Colors[5] = vec4( 1.0f, 1.0f,  1.0f, 1.0f);

	// Create and initialize a buffer object 
	CreateBufferObject();
	_bUpdateProj = false;
}


void CBG::CreateBufferObject()
{
    glGenVertexArrays( 1, &_uiVao );
    glBindVertexArray( _uiVao );

    // Create and initialize a buffer object

    glGenBuffers( 1, &_uiBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, _uiBuffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(_Points) + sizeof(_Colors), NULL, GL_STATIC_DRAW );

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(_Points), _Points ); 
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(_Points), sizeof(_Colors), _Colors );
}

void CBG::setShader(mat4 &mxView, mat4 &mxProjection, GLuint uiShaderHandle)
{
    // Load shaders and use the resulting shader program
	if( uiShaderHandle == MAX_UNSIGNED_INT) _uiProgram = InitShader("vsVtxColor.glsl", "fsVtxColor.glsl");
	else _uiProgram = uiShaderHandle;

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( _uiProgram, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( _uiProgram, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(_Points)) );

	_uiModelView = glGetUniformLocation( _uiProgram, "ModelView" );
	_mxView = mxView;
	glUniformMatrix4fv( _uiModelView, 1, GL_TRUE, _mxView );

    _uiProjection = glGetUniformLocation( _uiProgram, "Projection" );
	_mxProjection = mxProjection;
	glUniformMatrix4fv( _uiProjection, 1, GL_TRUE, _mxProjection );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void CBG::setViewMatrix(mat4 &mat)
{
	_mxView = mat;
	_bUpdateMV = true;
}

void CBG::setProjectionMatrix(mat4 &mat)
{
	_mxProjection = mat;
	_bUpdateProj = true;
}

void CBG::setTRSMatrix(mat4 &mat)
{
	_mxTRS = mat;
	_bUpdateMV = true;
}


void CBG::setColor(GLfloat vColor[4])
{
	for( int i = 0 ; i < 6 ; i++ ) {
		_Colors[i].x = vColor[0];
		_Colors[i].y = vColor[1];
		_Colors[i].z = vColor[2];
		_Colors[i].w = vColor[3];
	}
	glBindBuffer( GL_ARRAY_BUFFER, _uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(_Points), sizeof(_Colors), _Colors );
}

void CBG::setVtxColors(GLfloat vLFColor[], GLfloat vLRColor[], GLfloat vTRColor[], GLfloat vTLColor[])
{
	_Colors[0].x = vLFColor[0];
	_Colors[0].y = vLFColor[1];
	_Colors[0].z = vLFColor[2];
	_Colors[0].w = vLFColor[3];
	_Colors[3] = _Colors[0];

	_Colors[1].x = vLRColor[0];
	_Colors[1].y = vLRColor[1];
	_Colors[1].z = vLRColor[2];
	_Colors[1].w = vLRColor[3];

	_Colors[2].x = vTRColor[0];
	_Colors[2].y = vTRColor[1];
	_Colors[2].z = vTRColor[2];
	_Colors[2].w = vTRColor[3];
	_Colors[4] = _Colors[2];

	_Colors[5].x = vTLColor[0];
	_Colors[5].y = vTLColor[1];
	_Colors[5].z = vTLColor[2];
	_Colors[5].w = vTLColor[3];

	glBindBuffer( GL_ARRAY_BUFFER, _uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(_Points), sizeof(_Colors), _Colors );
}

void CBG::draw()
{
	glUseProgram( _uiProgram );
	glBindVertexArray( _uiVao );
	if( _bUpdateMV ) {
		_mxMVFinal = _mxView*_mxTRS;
		_bUpdateMV = false;
	}
	glUniformMatrix4fv( _uiModelView, 1, GL_TRUE, _mxMVFinal );

	if( _bUpdateProj ) {
		glUniformMatrix4fv( _uiProjection, 1, GL_TRUE, _mxProjection );
		_bUpdateProj = false;
	}
	glDrawArrays( GL_TRIANGLES, 0, BG_NUM );
}

void CBG::drawW()
{
	glBindVertexArray( _uiVao );

	if( _bUpdateMV ) {
		_mxMVFinal = _mxView*_mxTRS;
		_bUpdateMV = false;
	}

	glUniformMatrix4fv( _uiModelView, 1, GL_TRUE, _mxMVFinal );
	if( _bUpdateProj ) {
		glUniformMatrix4fv( _uiProjection, 1, GL_TRUE, _mxProjection );
		_bUpdateProj = false;
	}

	glDrawArrays( GL_TRIANGLES, 0, BG_NUM );
}
void CBG::moving(float dt)
{
	//dropping
	


}
