#version 130
in  vec4 vPosition;
in  vec4 vColor;
out vec4 color;

uniform mat4 ModelView;
uniform mat4 Projection;

void main()
{
    gl_Position = Projection * ModelView * vPosition;
	color = vec4(0.388, 0.611, 0.913, 1);
	
}
