
#version 150

//in vec3 color;
in vec2 UV;

out vec4 out_color;

uniform sampler2D myTextureSampler;

void main(void)
{
  	//out_Color = vec4(color, 1.0);
	out_color = texture(myTextureSampler,UV);
	//out_color = vec4(color,1.0);

}