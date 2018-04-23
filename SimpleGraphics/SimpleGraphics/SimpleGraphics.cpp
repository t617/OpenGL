#include <iostream>
#include <math.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
using namespace std;
//g++ -o line line.cpp -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
void show(float xarray[], float yarray[], int length) {
	bool init = true;
	if (!glfwInit()) {
		cout << "Initialization failed" << endl;
		init = false;
		//return?
	}

	//create a window with its OpenGL context
	GLFWwindow* window1 = glfwCreateWindow(640, 640, "line", NULL, NULL);

	if (!window1) {
		cout << "Window or OpenGL context creation failed" << endl;
		glfwTerminate();
		//return?
	}
	if (init&&window1) {
		// Make the window's context current */
		glfwMakeContextCurrent(window1);

		while (!glfwWindowShouldClose(window1)) {
			// Keep running
			/* Draw a triangle */
			glBegin(GL_POINTS);

			glColor3f(1, 0.52, 0.0);    // Orange
			for (int i = 0; i<length; i++) {
				glVertex2f(xarray[i], yarray[i]);
			}

			glEnd();

			//When the frame is rendered, swap the buffer with one another
			glfwSwapBuffers(window1);

			/* Poll for and process events */
			glfwPollEvents();
		}

		if (glfwWindowShouldClose(window1))
			glfwDestroyWindow(window1);

		//done before exit
		glfwTerminate();
	}
}


float normalize(int input) {
	return float(input) / 500;
}
void bresenham(int array[], int p, int i, int length, int dx, int dy) {
	if (i == length - 1)
		return;
	int pnext;
	if (p <= 0) {
		array[i + 1] = array[i];
		pnext = p + 2 * dy;
	}
	else {
		array[i + 1] = array[i] + 1;
		pnext = p + 2 * dy - 2 * dx;
	}
	bresenham(array, pnext, i + 1, length, dx, dy);
}
void computation() {
	int x0 = -400, x1 = 400, y0 = -400, y1 = 400;
	int length;
	int xarray[1001], yarray[1001];
	int dx;
	int dy;

	if (x0 == x1) {
		if (y0>y1) {
			int tempy = y0;
			y0 = y1;
			y1 = tempy;
		}
		length = y1 - y0 + 1;
		for (int i = 0; i<length; i++) {
			xarray[i] = x0;
			yarray[i] = y0 + i;
		}
	}
	else {
		//|m|<=1,let point 1 be on the right of point 0
		float m = float(y1 - y0) / float(x1 - x0);
		if (fabs(m) <= 1 && x0>x1) {
			int tempx = x0;
			int tempy = y0;
			x0 = x1;
			y0 = y1;
			x1 = tempx;
			y1 = tempy;
		}
		//|m|>1, let point 1 be on the top of point 0
		if (fabs(m)>1 && y0>y1) {
			int tempy = y0;
			int tempx = x0;
			y0 = y1;
			x0 = x1;
			y1 = tempy;
			x1 = tempx;
		}
		dx = x1 - x0;
		dy = y1 - y0;
		m = float(dy) / float(dx);

		if (fabs(m) <= 1) {
			length = x1 - x0 + 1;
			for (int i = 0; i<length; i++)
				xarray[i] = x0 + i;
			//0<m<=1
			if (dy >= 0) {
				yarray[0] = y0;
				yarray[length - 1] = y1;
				int p0 = 2 * dy - dx;
				bresenham(yarray, p0, 0, length, dx, dy);
			}
			else {
				//-1<=m<0
				yarray[0] = -y0;
				yarray[length - 1] = -y1;
				int p0 = 2 * (-dy) - dx;
				bresenham(yarray, p0, 0, length, dx, -dy);
				for (int i = 0; i<length; i++)
					yarray[i] = -yarray[i];
			}
		}
		else {
			length = y1 - y0 + 1;
			for (int i = 0; i<length; i++)
				yarray[i] = y0 + i;
			if (dx >= 0) {
				//m>1
				xarray[0] = x0;
				xarray[length - 1] = x1;
				int p0 = 2 * dx - dy;
				bresenham(xarray, p0, 0, length, dy, dx);
			}
			else {
				//m<-1
				xarray[0] = -x0;
				xarray[length - 1] = -x1;
				int p0 = 2 * (-dx) - dy;
				bresenham(xarray, p0, 0, length, dy, -dx);
				for (int i = 0; i<length; i++)
					xarray[i] = -xarray[i];
			}
		}
	}
	float fxarray[1001];
	float fyarray[1001];
	for (int i = 0; i<length; i++) {
		fxarray[i] = normalize(xarray[i]);
		fyarray[i] = normalize(yarray[i]);
	}
	show(fxarray, fyarray, length);
}

int main() {
	computation();
	return 0;
}