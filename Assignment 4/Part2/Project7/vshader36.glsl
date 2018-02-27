#version 150

in  vec3 vPosition;
uniform mat4 model_view;
uniform mat4 projection;
void main() 
{
    gl_Position = projection*model_view*vec4(vPosition,1.0);
    //gl_Position = vec4(vPosition,1.0);
} 
