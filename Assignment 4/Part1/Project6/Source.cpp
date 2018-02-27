//
// Perspective view of a color cube using LookAt() and Ortho()
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.
//

#include "Angel.h"
#include "freeglut.h"
#include "glew.h"
#include "glui.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

GLUI_Button      *x_btn;
GLUI_Button      *X_btn;
GLUI_Button      *y_btn;
GLUI_Button      *Y_btn;
GLUI_Button      *z_btn;
GLUI_Button      *Z_btn;
GLUI_Button      *r_btn;
GLUI_Button      *R_btn;
GLUI_Button      *o_btn;
GLUI_Button      *O_btn;
GLUI_Button      *p_btn;
GLUI_Button      *P_btn;
GLUI_Button      *default_btn;


/********** User IDs for callbacks ********/
#define X_ID			1
#define X1_ID			2
#define Y_ID			3
#define Y1_ID			4
#define Z_ID			5
#define Z1_ID			6
#define R_ID			7
#define R1_ID			8
#define O_ID			9
#define O1_ID			10
#define P_ID			11
#define P1_ID			12
#define DEFAULT_ID      13

int   main_window;
GLUI  *glui;
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

// Viewing transformation parameters

GLfloat radius = 1.0;
GLfloat theta = 0.0;
GLfloat phi = 0.0;

const GLfloat  dr = 5.0 * DegreesToRadians;

GLuint  model_view;  // model-view matrix uniform shader variable location

// Projection transformation parameters

GLfloat  left = -1.0, right = 1.0;
GLfloat  bottom = -1.0, top = 1.0;
GLfloat  zNear = 0.5, zFar = 3.0;

GLuint  projection; // projection matrix uniform shader variable location

//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices

int Index = 0;

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

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

//----------------------------------------------------------------------------
void pointer_cb(GLUI_Control* control)
{

	switch (control->get_id()){
	case 1: left *= 1.1; right *= 1.1; break;
	case 2: left *= 0.9; right *= 0.9; break;
	case 3: bottom *= 1.1; top *= 1.1; break;
	case 4: bottom *= 0.9; top *= 0.9; break;
	case 5: zNear *= 1.1; zFar *= 1.1; break;
	case 6: zNear *= 0.9; zFar *= 0.9; break;
	case 7: radius *= 2.0; break;
	case 8: radius *= 0.5; break;
	case 9: theta += dr; break;
	case 10: theta -= dr; break;
	case 11: phi += dr; break;
	case 12: phi -= dr; break;

	case 13:  // reset values to their defaults
		left = -1.0;
		right = 1.0;
		bottom = -1.0;
		top = 1.0;
		zNear = 0.5;
		zFar = 3.0;

		radius = 1.0;
		theta = 0.0;
		phi = 0.0;
		break;

	}


}
// OpenGL initialization
void
init()
{
	colorcube();

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points)+sizeof(colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader41.glsl", "fshader41.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	glutSwapBuffers();
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
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
}

//----------------------------------------------------------------------------

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);

	glutInitContextProfile(GLUT_CORE_PROFILE);
	main_window = glutCreateWindow("Color Cube");
	glewExperimental = TRUE;
	glewInit();

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	/****************************************/
	/*         Here's the GLUI code         */
	/****************************************/

	glui = GLUI_Master.create_glui("GLUI");

	/****** Button to Open Command Line Window ******/
	x_btn = new GLUI_Button(glui, "x", X_ID, pointer_cb);
	X_btn = new GLUI_Button(glui, "X", X1_ID, pointer_cb);
	y_btn = new GLUI_Button(glui, "y", Y_ID, pointer_cb);
	Y_btn = new GLUI_Button(glui, "Y", Y1_ID, pointer_cb);
	z_btn = new GLUI_Button(glui, "z", Z_ID, pointer_cb);
	Z_btn = new GLUI_Button(glui, "Z", Z1_ID, pointer_cb);
	r_btn = new GLUI_Button(glui, "r", R_ID, pointer_cb);
	R_btn = new GLUI_Button(glui, "R", R1_ID, pointer_cb);
	o_btn = new GLUI_Button(glui, "o", O_ID, pointer_cb);
	O_btn = new GLUI_Button(glui, "O", O1_ID, pointer_cb);
	p_btn = new GLUI_Button(glui, "p", P_ID, pointer_cb);
	P_btn = new GLUI_Button(glui, "P", P1_ID, pointer_cb);
	default_btn = new GLUI_Button(glui, "default", DEFAULT_ID, pointer_cb);


	glui->set_main_gfx_window(main_window);

	/* We register the idle callback with GLUI, *not* with GLUT */
	GLUI_Master.set_glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
