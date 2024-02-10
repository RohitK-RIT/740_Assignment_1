﻿#include <GL/freeglut.h>
#include <iostream>
#include <vector>

using namespace std;

struct vertex
{
	float x, y;
};

struct color
{
	float r, g, b;
};

struct object
{
	vector<vertex> vertices;
	GLenum draw_type;
	const color* color;
	const float* point_size;
};

constexpr float small_point = 1.0f, medium_point = 5.0f, large_point = 10.0f;
constexpr color red = {1.0f, 0.0f, 0.0f}, green = {0.0f, 1.0f, 0.0f}, blue = {0.0f, 0.0f, 1.0f};

float canvas_size[] = {10.0f, 10.0f};
int raster_size[] = {800, 600};

float mouse_pos[2];
int total_vertices;
object* current_object;
vector<object*> objects;

void create_new_object(GLenum draw_type, const color* color, const float* point_size)
{
	objects.push_back(new object());
	current_object = objects.back();
	current_object->vertices.clear();
	current_object->draw_type = draw_type;
	current_object->color = color;
	current_object->point_size = point_size;
}

void init()
{
	mouse_pos[0] = mouse_pos[1] = 0.0f;
	create_new_object(GL_LINE_LOOP, &red, &small_point);
	total_vertices = 3;
}

void drawCursor()
{
	glColor3f(1.0f, 0.0f, 1.0f); // Purple
	glPointSize(large_point);
	glBegin(GL_POINTS);
	glVertex2fv(mouse_pos);
	glEnd();
	glPointSize(medium_point);
}

void display()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Draw completed objects
	for (const object* object : objects)
	{
		glColor3f(object->color->r, object->color->g, object->color->b);
		glPointSize(*object->point_size);
		glBegin(object->draw_type);
		if (current_object == object)
		{
			if (total_vertices == 2)
			{
				if (!current_object->vertices.empty())
				{
					const auto first_vertex = current_object->vertices[0];
					glVertex2f(first_vertex.x, first_vertex.y);
					glVertex2f(first_vertex.x, mouse_pos[1]);
					glVertex2fv(mouse_pos);
					glVertex2f(mouse_pos[0], first_vertex.y);
				}
			}
			else
			{
				for (const vertex& vertex : current_object->vertices)
					glVertex2f(vertex.x, vertex.y);

				glVertex2fv(mouse_pos);
			}
		}
		else
		{
			for (const vertex& vertex : object->vertices)
				glVertex2f(vertex.x, vertex.y);
		}
		glEnd();
		glPointSize(*current_object->point_size);
	}

	drawCursor();
	glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mouse_pos[0] = static_cast<float>(x) / raster_size[0] * canvas_size[0];
		mouse_pos[1] = static_cast<float>(raster_size[1] - y) / raster_size[1] * canvas_size[1];

		current_object->vertices.push_back({mouse_pos[0], mouse_pos[1]});

		if (static_cast<int>(current_object->vertices.size()) == total_vertices)
		{
			const auto temp = current_object->draw_type;
			switch (total_vertices)
			{
			case 2:
				current_object->draw_type = GL_QUADS;
				const auto first_vertex = current_object->vertices[0];
				const auto second_vertex = current_object->vertices[1];
				current_object->vertices.insert(current_object->vertices.begin() + 1, {{first_vertex.x, second_vertex.y}});
				current_object->vertices.push_back({second_vertex.x, first_vertex.y});
				break;
			case 3:
				current_object->draw_type = GL_TRIANGLES;
				break;
			case -2:
				current_object->draw_type = GL_POLYGON;
				break;
			default:
				break;
			}

			create_new_object(temp, current_object->color, current_object->point_size);
		}

		glutPostRedisplay();
	}
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

void try_store_object()
{
	if (static_cast<int>(current_object->vertices.size()) != total_vertices && total_vertices != -2 && total_vertices != -1)
	{
		current_object->vertices.clear();
		return;
	}

	switch (total_vertices)
	{
	case 2:
		current_object->draw_type = GL_QUADS;
		break;
	case 3:
		current_object->draw_type = GL_TRIANGLES;
		break;
	case -2:
		current_object->draw_type = GL_POLYGON;
		break;
	default:
		break;
	}

	create_new_object(current_object->draw_type, current_object->color, current_object->point_size);
}

void motion(int x, int y)
{
	// mouse events are handled by OS, eventually. When using mouse in the raster window, it assumes top-left is the origin.
	// Note: the raster window created by GLUT assumes bottom-left is the origin.
	mouse_pos[0] = static_cast<float>(x) / raster_size[0] * canvas_size[0];
	mouse_pos[1] = static_cast<float>(raster_size[1] - y) / raster_size[1] * canvas_size[1];

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
	default:
		break;
	}
}

void menu(int value)
{
	switch (value)
	{
	// main menu
	case 0: // clear
		current_object->vertices.clear();
		objects.clear();
		glutPostRedisplay();
		break;
	case 1: //exit
		exit(0);

	// object selection
	case 5: // point
		try_store_object();
		total_vertices = 1;
		current_object->draw_type = GL_POINTS;
		glutPostRedisplay();
		break;
	case 6: // line
		try_store_object();
		total_vertices = -1;
		current_object->draw_type = GL_LINE_STRIP;
		glutPostRedisplay();
		break;
	case 7: // triangle
		try_store_object();
		total_vertices = 3;
		current_object->draw_type = GL_LINE_LOOP;
		glutPostRedisplay();
		break;
	case 8: // quad
		try_store_object();
		total_vertices = 2;
		current_object->draw_type = GL_LINE_LOOP;
		glutPostRedisplay();
		break;
	case 9: // polygon
		try_store_object();
		total_vertices = -2;
		current_object->draw_type = GL_LINE_LOOP;
		glutPostRedisplay();
		break;

	// color selection
	case 10: // red
		current_object->color = &red;
		glutPostRedisplay();
		break;
	case 11: // green
		current_object->color = &green;
		glutPostRedisplay();
		break;
	case 12: // blue
		current_object->color = &blue;
		glutPostRedisplay();
		break;

	// point size selection
	case 13: // small
		current_object->point_size = &small_point;
		glutPostRedisplay();
		break;
	case 14: // medium
		current_object->point_size = &medium_point;
		glutPostRedisplay();
		break;
	case 15: // large
		current_object->point_size = &large_point;
		glutPostRedisplay();
		break;

	default:
		break;
	}
}

void createMenu()
{
	const int object_menu = glutCreateMenu(menu);
	glutAddMenuEntry("Point", 5);
	glutAddMenuEntry("Line", 6);
	glutAddMenuEntry("Triangles", 7);
	glutAddMenuEntry("Quad", 8);
	glutAddMenuEntry("Polygon", 9);

	const int color_menu = glutCreateMenu(menu);
	glutAddMenuEntry("Red", 10);
	glutAddMenuEntry("Green", 11);
	glutAddMenuEntry("Blue", 12);

	const int point_size_menu = glutCreateMenu(menu);
	glutAddMenuEntry("Small", 13);
	glutAddMenuEntry("Medium", 14);
	glutAddMenuEntry("Large", 15);

	glutCreateMenu(menu);
	glutAddMenuEntry("Clear", 0);
	glutAddSubMenu("Objects", object_menu);
	glutAddSubMenu("Colors", color_menu);
	glutAddSubMenu("Sizes", point_size_menu);
	glutAddMenuEntry("Quit", 4);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char* argv[])
{
	init();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(raster_size[0], raster_size[1]);
	glutCreateWindow("2D Drawing");

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(motion);
	createMenu();
	glutMainLoop();
	return 0;
}
