#define _CRT_SECURE_NO_DEPRECATE

#include "Angel.h"
#include "glew.h"
#include "freeglut.h"
#include "glui.h"


typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

int   main_window;
GLUI  *glui;

GLuint program;
GLint position_id, color_id,type_id, model_view,projection;

GLuint vao;
GLuint vbo[2];

FILE * fp;

int verticesCount = 0;
int facesCount = 0;
float *vertice;
GLuint *face;

GLfloat  left = -1.0, right = 1.0;
GLfloat  bottom = -1.0, top = 1.0;
GLfloat  zNear = 0.5, zFar = 3.0;

GLfloat radius = 1.0;
GLfloat theta = 0.0;
GLfloat phi = 0.0;
const GLfloat  dr = 5.0 * DegreesToRadians;

GLUI_Rotation    *op_rotation; //operational rotation

#define rotation_ID      1

float surface[] = {
	-0.5f, 0.0f, -0.4f,  // Top-left
	0.5f, 0.0f, -0.4f,  // Top-right
	0.75f, -0.7f, 0.0f, // Bottom-right
	-0.75f, -0.7f, 0.0f  // Bottom-left
};


GLuint elements[] = {
	0, 1, 2,
	2, 3, 0
};

float deltaAngle = 0.0f;
float deltaMove = 0;


// the camera info  
float eye[3];
float lookat[3];

int Index = 0;

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-0.5, -0.5, 0.5, 1.0),
	point4(-0.5, 0.5, 0.5, 1.0),
	point4(0.5, 0.5, 0.5, 1.0),
	point4(0.5, -0.5, 0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5, 0.5, -0.5, 1.0),
	point4(0.5, 0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

// RGBA olors
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 1.0, 1.0),  // white
	color4(0.0, 1.0, 1.0, 1.0)   // cyan
};

void
quad(int a, int b, int c, int d)
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}

void pointer_cb(GLUI_Control* control)
{

	switch (control->get_id()){
	case 1: left *= 1.1; right *= 1.1; break;
	}
}

void
init()
{
	program = InitShader("vshader36.glsl", "fshader36.glsl");

	position_id = glGetAttribLocation(program, "vPosition");
	color_id = glGetAttribLocation(program, "vColor");
	type_id = glGetUniformLocation(program, "type");

	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");

	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbo);

	glBindVertexArray(vao);

}


void
display(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), surface, GL_STATIC_DRAW);
	glEnableVertexAttribArray(position_id);
	glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), elements, GL_STATIC_DRAW);

	glUniform1i(type_id, 1);
	glUseProgram(program);

	glScalef(1.0, 2.0, 1.0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, verticesCount*3*sizeof(float), vertice, GL_STATIC_DRAW);
	glEnableVertexAttribArray(position_id);
	glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, facesCount*3*sizeof(GLuint), face, GL_STATIC_DRAW);

	glUniform1i(type_id, 0);
	glUseProgram(program);
	glDrawElements(GL_TRIANGLES, 3*facesCount, GL_UNSIGNED_INT, 0);

	glLoadIdentity();
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	point4  eye(radius*sin(theta)*cos(phi),
		radius*sin(theta)*sin(phi),
		radius*cos(theta),
		1.0);
	point4  at(0.0, 0.0, 0.0, 1.0);
	vec4    up(0.0, 1.0, 0.0, 0.0);

	mat4  mv = LookAt(eye, at, up);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	mat4  p = Ortho(left, right, bottom, top, zNear, zFar);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	GLuint  model_view;  // model-view matrix uniform shader variable location


	// camera transform
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2], lookat[0], lookat[1], lookat[2], 0, 1, 0);

	glutSwapBuffers();
}


void
idle(void)
{
	if (glutGetWindow() != main_window)
		glutSetWindow(main_window);

	glutPostRedisplay();

}


void
reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

void
counter(){
	char current;
	int  count = 0, count1 = 0;


	fp = fopen("boundbunny_1k.smf", "r");

	if (!fp)
		printf("dosya açýlamadý!");

	while (!feof(fp)){
		current = getc(fp);
		if (current == 'v'){
			verticesCount++;
		}
		else if (current == 'f'){
			facesCount++;
		}
	}
	fclose(fp);


}

void
read(){
	char current;
	int  count = 0, count1 = 0;

	vertice = (float *)malloc(sizeof(float)* 3 * verticesCount);
	face = (GLuint *)malloc(sizeof(GLuint)* 3 * facesCount);
	fp = fopen("boundbunny_1k.smf", "r");

	if (!fp)
		printf("dosya okunamadý");

	while (!feof(fp)){
		current = getc(fp);
		if (current == 'v'){
			fscanf(fp, "%f %f %f", &vertice[count], &vertice[count+1], &vertice[count+2]);
			count+=3;
		}
		else if (current == 'f'){
		
			fscanf(fp, "%d %d %d", &face[count1], &face[count1+1], &face[count1+2]);
			face[count1] -= 1;
			face[count1+1] -= 1;
			face[count1+2] -= 1;
			count1+=3;
		}
	}
	
	fclose(fp);
	
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(EXIT_SUCCESS);
		break;

	case 'w': left *= 1.1; right *= 1.1; break;
	case 's': left *= 0.9; right *= 0.9; break;
	case 'a': bottom *= 1.1; top *= 1.1; break;
	case 'd': bottom *= 0.9; top *= 0.9; break;
	}
}

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	
	glutInitContextProfile(GLUT_CORE_PROFILE);
	main_window = glutCreateWindow("Stanford Bunny");
	glewExperimental = TRUE;
	glewInit();
	counter();
	read();
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	glui = GLUI_Master.create_glui("GLUI");

	GLUI_Rotation *op_rotation = glui->add_rotation("camera");
	glui->set_main_gfx_window(main_window);

	// initialize the camera
	eye[0] = 0;
	eye[1] = 4;
	eye[2] = 10;
	lookat[0] = 0;
	lookat[1] = 0;
	lookat[2] = 0;

	GLUI_Master.set_glutIdleFunc(idle);
	
	glutMainLoop();
	
	return 0;
}
