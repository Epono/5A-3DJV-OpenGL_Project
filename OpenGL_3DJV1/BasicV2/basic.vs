
#version 150

in vec3 a_position;
//in vec3 in_color;
in vec2 vertexUV;


out vec3 color;
out vec2 UV;


uniform mat4 u_projectionMatrix;
//uniform mat4 u_viewMatrix;
uniform mat4 u_modelviewMatrix;
//uniform mat4 u_worldMatrix;







void main(void)
{
	gl_Position = u_projectionMatrix  *	 u_modelviewMatrix*  vec4(a_position,1.0);


	UV = vertexUV;	
	//color = in_color;
}