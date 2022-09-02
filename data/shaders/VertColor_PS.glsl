#version 130

in		vec4	VertColor;

out 	vec4 	out2Color;
out 	vec4 	out1Color;

void main()
{
	out1Color = VertColor;
	out2Color = vec4(1,0,0,1);
}