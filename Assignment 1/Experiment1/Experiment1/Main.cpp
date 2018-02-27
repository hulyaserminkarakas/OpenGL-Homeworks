#include "glew.h"
#include "freeglut.h"
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

GLuint LoadShaders(const char *vertex_file, const char *frag_file) {
	
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertShaderStr;
	std::ifstream vertexStream(vertex_file, std::ios::in);
	std::string line = "";

	while (!vertexStream.eof()) {
		std::getline(vertexStream, line);
		vertShaderStr.append(line + "\n");
	}
	vertexStream.close();
	const char *vertShaderSrc = vertShaderStr.c_str();

	std::string fragShaderStr;
	std::ifstream fragStream(frag_file, std::ios::in);

	line = "";
	while (!fragStream.eof()) {
		std::getline(fragStream, line);
		fragShaderStr.append(line + "\n");
	}
	fragStream.close();
	const char *fragShaderSrc = fragShaderStr.c_str();

	glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
	glCompileShader(vertShader);

	glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
	glCompileShader(fragShader);

	GLuint myshader = glCreateProgram();
	glAttachShader(myshader, vertShader);
	glAttachShader(myshader, fragShader);
	glLinkProgram(myshader);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return myshader;
}

void changeSize(int w, int h) {

	if (w>h)
		glViewport((w - h) / 2, 0, std::min(w, h), std::min(w, h));
	else
		glViewport(0, (h - w) / 2, std::min(w, h), std::min(w, h));

}

void display(void)
{
	glClearColor(1.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glewExperimental = GL_TRUE;

	GLuint BigID[2];
	GLuint SmlID[3];

	GLfloat x, y;
	GLfloat bigCir[720];
	GLfloat smlCir[720];

	for (int i = 0; i<360; i++){
		x = 0.5f*cos(i*2.0f*3.14f / 360);
		y = 0.5f*sin(i*2.0f*3.14f / 360);

		bigCir[2 * i] = x;
		bigCir[(2 * i) + 1] = y;

		smlCir[2 * i] = x / 2;
		smlCir[(2 * i) + 1] = y / 2;
	}

	GLuint big = LoadShaders("BigCirPos.glsl", "BigCirColor.glsl");
	glUseProgram(big);

	glBindBuffer(GL_ARRAY_BUFFER, BigID[0]);
	glBufferData(GL_ARRAY_BUFFER, 720 * sizeof(GLfloat), bigCir, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_POLYGON, 0, 360);
	glDisableClientState(GL_VERTEX_ARRAY);

	GLuint small = LoadShaders("SmlCirPos.glsl", "SmlCirColor.glsl");
	glUseProgram(small);

	glBindBuffer(GL_ARRAY_BUFFER, SmlID[2]);
	glBufferData(GL_ARRAY_BUFFER, 720 * sizeof(GLfloat), smlCir, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	glEnableClientState(GL_VERTEX_ARRAY);	
	glDrawArrays(GL_POLYGON, 0, 360);
	glDisableClientState(GL_VERTEX_ARRAY);

	glFlush();

}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(EXIT_SUCCESS);
		break;
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(800, 0);

	// If you are using freeglut, the next two lines will check if 
	// the code is truly 3.1. Otherwise, comment them out
	//glutInitContextVersion(3, 1);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutCreateWindow("My Window");

	glewExperimental = true;
	glewInit();

	glutDisplayFunc(display);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	return 0;
}

