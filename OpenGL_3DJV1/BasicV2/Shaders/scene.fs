 #version 330

uniform sampler2D u_sampler;

in Vertex
{
    vec2 texcoords;
} IN;

out vec4 Fragment;

void main(void)
{    
    Fragment = texture(u_sampler, IN.texcoords);
}