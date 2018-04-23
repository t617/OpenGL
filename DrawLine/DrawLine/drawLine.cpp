#include<glad/glad.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <iostream>
using namespace std;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void drawTriangle(int x0, int x1, int y0, int y1, float* farray);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
"}\n\0";

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
int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ImGui OpenGL3", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Setup ImGui binding
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplGlfwGL3_Init(window, true);

	// Setup style
	ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();

	bool triangle = false;
	bool circle = false;
	bool my_tool_active = false;
	bool fill_triangle = false;
	// vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// link shaders
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// as we only have a single shader, we could also just activate our shader once beforehand if we want to 
	glUseProgram(shaderProgram);

	// render loop
	// -----------
	
	float vertexs_c[] = {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	};
	float vertexs_c1[] = {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	};

	float farray1[3003];
	float farray2[3003];
	float farray3[3003];
	drawTriangle(-400, 400, 400, 400, farray1);
	drawTriangle(-400, 0, 400, -400, farray2);
	drawTriangle(400, 0, 400, -400, farray3);


	float f = 0.0f, g = 0.0f;
	float fill_t[] = {
		-0.8f, 0.8f, 0.0f,
		0.8f, 0.8f, 0.0f,
		0.0f, -0.8f, 0.0f,
	};
	while (!glfwWindowShouldClose(window))
	{
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Begin("Opengl Imgui", &my_tool_active, ImGuiWindowFlags_MenuBar);
		ImGui::SetWindowSize(ImVec2(450, 400), true);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("menu"))
			{
				if (ImGui::MenuItem("triangle", "...")) { 
					triangle = true;
					circle = false;
					fill_triangle = false;
				}
				if (ImGui::MenuItem("circle", "...")) { 
					triangle = false;
					circle = true;
					fill_triangle = false;
					
				}
				if (ImGui::MenuItem("fill", "...")) {
					triangle = false;
					circle = false;
					fill_triangle = true;

				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
			{
				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
				ImGui::SameLine();
				
			}
		}
		ImGui::End();
		if (triangle) {
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(0);
			// color attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(1);

			glBufferData(GL_ARRAY_BUFFER, sizeof(farray1), farray1, GL_STATIC_DRAW);
			glDrawArrays(GL_POINTS, 0, sizeof(farray1));
	
			glBufferData(GL_ARRAY_BUFFER, sizeof(farray2), farray2, GL_STATIC_DRAW);
			glDrawArrays(GL_POINTS, 0, sizeof(farray2));

			glBufferData(GL_ARRAY_BUFFER, sizeof(farray3), farray3, GL_STATIC_DRAW);
			glDrawArrays(GL_POINTS, 0, sizeof(farray3));
		}

		if (fill_triangle) {
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(0);
			// color attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(1);

			glBufferData(GL_ARRAY_BUFFER, sizeof(fill_t), fill_t, GL_STATIC_DRAW);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			
		}

		if (circle) {
			int r = 250 + 200 * (f);
			int cx = 0, cy = 0;
			float fcx = normalize(cx);
			float fcy = normalize(cy);
			int x = 0, y = r;
			int d = 1 - r;
			while (y > x) {
				float fx = normalize(x);
				float fy = normalize(y);

				vertexs_c[0] = fcx + fx;
				vertexs_c[1] = fcy + fy;
				vertexs_c[3] = fcx - fx;
				vertexs_c[4] = fcy - fy;
				vertexs_c[6] = fcx - fx;
				vertexs_c[7] = fcy + fy;
				vertexs_c[9] = fcx + fx;
				vertexs_c[10] = fcy - fy;

				vertexs_c[12] = fcx + fy;
				vertexs_c[13] = fcy + fx;
				vertexs_c[15] = fcx - fy;
				vertexs_c[16] = fcy - fx;
				vertexs_c[18] = fcx - fy;
				vertexs_c[19] = fcy + fx;
				vertexs_c[21] = fcx + fy;
				vertexs_c[22] = fcy - fx;

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
				glEnableVertexAttribArray(1);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertexs_c), vertexs_c, GL_STATIC_DRAW);
				glDrawArrays(GL_POINTS, 0, 8);
				if (d < 0) {
					d = d + 2 * x + 3;
				} else {
					d = d + 2 * (x - y) + 5;
					--y;
				}
				++x;
			}
			float diagonal = float(r) / sqrt(2);
			vertexs_c[0] = fcx + diagonal;
			vertexs_c[1] = fcy + diagonal;
			vertexs_c[3] = fcx - diagonal;
			vertexs_c[4] = fcy + diagonal;
			vertexs_c[6] = fcx + diagonal;
			vertexs_c[7] = fcy - diagonal;
			vertexs_c[9] = fcx - diagonal;
			vertexs_c[10] = fcy + diagonal;
			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(0);
			// color attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(1);

			glBufferData(GL_ARRAY_BUFFER, sizeof(vertexs_c), vertexs_c, GL_STATIC_DRAW);
			glDrawArrays(GL_POINTS, 0, 4);
			x = 0;
			y = r;
		}
		// Rendering
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		// input
		// -----
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// -------------------------------------------------------------------------------
		glfwPollEvents();
	}
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


void drawTriangle(int x_0, int x_1, int y_0, int y_1, float *farray) {
	int x0 = x_0, x1 = x_1, y0 = y_0, y1 = y_1;
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
	} else {
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
			} else {
				//-1<=m<0
				yarray[0] = -y0;
				yarray[length - 1] = -y1;
				int p0 = 2 * (-dy) - dx;
				bresenham(yarray, p0, 0, length, dx, -dy);
				for (int i = 0; i<length; i++)
					yarray[i] = -yarray[i];
			}
		} else {
			length = y1 - y0 + 1;
			for (int i = 0; i<length; i++)
				yarray[i] = y0 + i;
			if (dx >= 0) {
				//m>1
				xarray[0] = x0;
				xarray[length - 1] = x1;
				int p0 = 2 * dx - dy;
				bresenham(xarray, p0, 0, length, dy, dx);
			} else {
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
		farray[i * 3] = fxarray[i];
		farray[i * 3 + 1] = fyarray[i];
		farray[i * 3 + 2] = 0.0f;
	}
}