#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include <iostream>
using namespace std;

float canvas_size[] = {10.0f, 10.0f};
int raster_size[] = {800, 600};

// structure for storing 3 2D vertices of a triangle
int num_of_vertices = 0;
float v[2 * 3];
float color[3];

float mouse_pos[2];

void init()
{
	for (int i = 0; i < 6; i++)
		v[i] = 0.0f;
	mouse_pos[0] = mouse_pos[1] = 0.0f;
	color[0] = 1.0f;
	color[1] = color[2] = 0.0f;
}

void drawCursor()
{
	glColor3f(1.0f, 0.0f, 1.0f);
	glPointSize(10.0f);
	glBegin(GL_POINTS);
	glVertex2fv(mouse_pos);
	glEnd();
	glPointSize(1.0f);
}

void display()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3fv(color);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (num_of_vertices > 0 && num_of_vertices < 3)
	{
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < num_of_vertices; i++)
			glVertex2fv(v + i * 2);
		glVertex2fv(mouse_pos);
		glEnd();
	}
	else if (num_of_vertices == 3)
	{
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < num_of_vertices; i++)
			glVertex2fv(v + i * 2);
		glEnd();
	}

	drawCursor();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	raster_size[0] = w;
	raster_size[1] = h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, canvas_size[0], 0.0, canvas_size[1]);
	glViewport(0, 0, raster_size[0], raster_size[1]);

	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (num_of_vertices >= 3)
			num_of_vertices = 0;

		mouse_pos[0] = (float)x / raster_size[0] * canvas_size[0];
		mouse_pos[1] = (float)(raster_size[1] - y) / raster_size[1] * canvas_size[1];
		v[num_of_vertices * 2 + 0] = mouse_pos[0];
		v[num_of_vertices * 2 + 1] = mouse_pos[1];

		num_of_vertices++;
		glutPostRedisplay();
	}
}

void motion(int x, int y)
{
	// mouse events are handled by OS, eventually. When using mouse in the raster window, it assumes top-left is the origin.
	// Note: the raster window created by GLUT assumes bottom-left is the origin.
	mouse_pos[0] = (float)x / raster_size[0] * canvas_size[0];
	mouse_pos[1] = (float)(raster_size[1] - y) / raster_size[1] * canvas_size[1];

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	}
}

void menu(int value)
{
	switch (value)
	{
	case 0: // clear
		num_of_vertices = 0;
		glutPostRedisplay();
		break;
	case 1: //exit
		exit(0);
	case 2: // red
		color[0] = 1.0f;
		color[1] = 0.0f;
		color[2] = 0.0f;
		glutPostRedisplay();
		break;
	case 3: // green
		color[0] = 0.0f;
		color[1] = 1.0f;
		color[2] = 0.0f;
		glutPostRedisplay();
		break;
	case 4: // blue
		color[0] = 0.0f;
		color[1] = 0.0f;
		color[2] = 1.0f;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void createMenu()
{
	int colorMenu = glutCreateMenu(menu);
	glutAddMenuEntry("Red", 2);
	glutAddMenuEntry("Green", 3);
	glutAddMenuEntry("Blue", 4);

	glutCreateMenu(menu);
	glutAddMenuEntry("Clear", 0);
	glutAddSubMenu("Colors", colorMenu);
	glutAddMenuEntry("Exit", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char* argv[])
{
	init();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(raster_size[0], raster_size[1]);
	glutCreateWindow("Mouse Event - draw a triangle");

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(motion);
	createMenu();
	glutMainLoop();
	return 0;
}
