#version 150

out vec4 fColor;
uniform int type;
void main() 
{ 
if(type==0)
    fColor = vec4(0.3f,0.7f,0.7f,1.0f);
else
	fColor = vec4(0.1f,0.2,0.6f,1.0f);

} 

