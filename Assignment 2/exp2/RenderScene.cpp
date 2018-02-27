#include "common_header.h"

#include "win_OpenGLApp.h"

#include "shaders.h"
#define DEGREES_TO_RADIANS(x) (3.14159265358979323846 * x / 180.0)

GLfloat fElipse[1080];
GLfloat fTriangle[9]; 
GLfloat fCircle[1080];

GLfloat fElipseColor[1080];
GLfloat fTriangleColor[9];
GLfloat fCircleColor[1080];


UINT uiVBO[6];
UINT uiVAO[3];

CShader shVertex, shFragment;
CShaderProgram spMain;

void InitScene(LPVOID lpParam)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	int count = 0;
	int b = 1;
	int c = 2;

	//Setup circle vertices
	
	count = 0;
	for (GLfloat i = 0; i < 360.0f; i += (360.0f / 360))
	{
		fCircle[count++] = -0.6f + (cosf(DEGREES_TO_RADIANS(i)) / 4);
		fCircle[count++] = (sinf(DEGREES_TO_RADIANS(i)) / 4);
	}
	//Setup circle color

	count = 0;
	for (GLfloat i = 360; i >0.0f; i -= (360.0f / 360))
	{
		if (count >= 1){
			fCircleColor[count++] = 0.95f;
			fCircleColor[count++] = 0.0f;
			fCircleColor[count++] = 0.0f;
		}
		else{
			fCircleColor[count++] = 0.0f;
			fCircleColor[count++] = 0.0f;
			fCircleColor[count++] = 0.0f;
		}
	}

	// Setup triangle vertices
	fTriangle[0] = -0.25f; fTriangle[1] = -0.25f; fTriangle[2] = 0.0f;
	fTriangle[3] = 0.25f; fTriangle[4] = -0.25f; fTriangle[5] = 0.0f;
	fTriangle[6] = 0.0f; fTriangle[7] = 0.25f; fTriangle[8] = 0.0f;

	// Setup triangle color

	fTriangleColor[0] = 0.0f; fTriangleColor[1] = 0.0f; fTriangleColor[2] = 1.0f;
	fTriangleColor[3] = 0.0f; fTriangleColor[4] = 1.0f; fTriangleColor[5] = 0.0f;
	fTriangleColor[6] = 1.0f; fTriangleColor[7] = 0.0f; fTriangleColor[8] = 0.0f;
 
	//Setup elipse vertices
	count = 0;
	for (GLfloat i = 0; i < 360.0f; i += (360.0f / 360))
	{
		fElipse[count++] = 0.6f + (cosf(DEGREES_TO_RADIANS(i)) / 4);
		fElipse[count++] = ((sinf(DEGREES_TO_RADIANS(i)) / 4)*0.6);
	}
	//Setup elipse color

	count = 0;
	for (GLfloat i = 0; i < 360.0f; i += (360.0f / 360))
	{
		fElipseColor[count++] = 1.0; fElipseColor[count++] = 0.6;   fElipseColor[count++] = 0.6;
	}


	glGenVertexArrays(3, uiVAO); // Generate three VAOs, one for elipse, one for triangle and one for circle
	glGenBuffers(6, uiVBO); // And six VBOs

	//Setup whole circle
	glBindVertexArray(uiVAO[2]);

	glBindBuffer(GL_ARRAY_BUFFER, uiVBO[4]);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(fCircle) / sizeof(GLfloat))*sizeof(GLfloat), fCircle, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uiVBO[5]);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(fCircleColor) / sizeof(GLfloat))*sizeof(GLfloat), fCircleColor, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);


	// Setup whole triangle
	glBindVertexArray(uiVAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, uiVBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fTriangle)*sizeof(GLfloat), fTriangle, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uiVBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fTriangleColor)*sizeof(GLfloat), fTriangleColor, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);


	//Setup whole elipse
	glBindVertexArray(uiVAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(fElipse) / sizeof(GLfloat))*sizeof(GLfloat), fElipse, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uiVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(fElipseColor) / sizeof(GLfloat))*sizeof(GLfloat), fElipseColor, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Load shaders and create shader program

	shVertex.LoadShader("data\\shaders\\shader.vert", GL_VERTEX_SHADER);
	shFragment.LoadShader("data\\shaders\\shader.frag", GL_FRAGMENT_SHADER);

	spMain.CreateProgram();
	spMain.AddShaderToProgram(&shVertex);
	spMain.AddShaderToProgram(&shFragment);

	spMain.LinkProgram();
	spMain.UseProgram();
}

void RenderScene(LPVOID lpParam)
{
	// Typecast lpParam to COpenGLControl pointer
	COpenGLControl* oglControl = (COpenGLControl*)lpParam;

	// We just clear color
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(uiVAO[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 360);

	glBindVertexArray(uiVAO[1]);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(uiVAO[2]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 360);

	oglControl->SwapBuffersM();
}


void ReleaseScene(LPVOID lpParam)
{
	spMain.DeleteProgram();

	shVertex.DeleteShader();
	shFragment.DeleteShader();
}