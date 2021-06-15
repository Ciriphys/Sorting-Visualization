@vertex

#version 330 core

layout(location = 0) in vec2 vPos;

uniform mat4 u_mvp;;

void main()
{
	gl_Position = u_mvp * vec4(vPos, 0.0f, 1.0f);
}

@fragment

#version 330 core

out vec4 fragment_color;
uniform vec4 u_color;

void main()
{
	fragment_color = u_color;
}