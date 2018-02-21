#version 450 core
layout(location = 1) 
in vec4 vertexPosition;
layout(location = 2) 
in vec4 particleVelocity;
uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vertexPosition;
}
