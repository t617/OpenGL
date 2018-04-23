#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";

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

	// glfw window creation
	// --------------------
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

	bool show_demo_window = false;
	bool triangle = false;
	bool point = false;
	bool line = false;
	bool m_triangle = false;
	bool my_tool_active = false;

	ImVec4 clear_color1 = ImVec4(0.0f, 0.0f, 1.0f, 1.00f);
	ImVec4 clear_color2 = ImVec4(1.0f, 0.0f, 0.0f, 1.00f);
	ImVec4 clear_color3 = ImVec4(0.0f, 1.0f, 0.0f, 1.00f);

	// build and compile our shader program
	// ------------------------------------
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

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions         // colors
		0.5f, -0.5f, 0.0f, clear_color1.x, clear_color1.y, clear_color1.z,  // bottom right
		-0.5f, -0.5f, 0.0f, clear_color2.x, clear_color2.y, clear_color2.z,  // bottom left
		0.0f,  0.5f, 0.0f,  clear_color3.x, clear_color3.y, clear_color3.z // top 

	};
	float vertices_p[] = {
		// positions         // colors
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f// top  
	};
	float vertices_l[] = {
		// positions         // colors
		0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,// top 
		0.0f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f
	};

	float vertices_m[] = {
		// first triangle
		-0.4f, 0.0f, 0.0f,  // left 
		0.4f, 0.0f, 0.0f,  // right
		0.0f, 0.6f, 0.0f,  // top 
						   // second triangle
		-0.8f, -0.6f, 0.0f,  // left
		0.0f, -0.6f, 0.0f,  // right
		//-0.4f, 0.0f, 0.0f,   // top
							 // third triangle
		0.0f, -0.6f, 0.0f,  // left
		0.8f, -0.6f, 0.0f  // right
		//0.4f, 0.0f, 0.0f   // top 
	};
	unsigned int indices[] = { // 注意索引从0开始! 
		0, 1, 2, // 第一个三角形
		3, 4, 0,  // 第二个三角形
		5, 6, 1  // 第三个三角形
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	// glBindVertexArray(0);

	// as we only have a single shader, we could also just activate our shader once beforehand if we want to 
	glUseProgram(shaderProgram);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Begin("Opengl Imgui", &my_tool_active, ImGuiWindowFlags_MenuBar);
		ImGui::SetWindowSize(ImVec2(450, 400), true);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("menu"))
			{
				if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
				if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
				if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("select"))
			{
				if (ImGui::MenuItem("Point", ".")) { 
					/* Do stuff */ 
					
				}
				if (ImGui::MenuItem("LINE_STRIP", "-")) {
					/* Do stuff */
					
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();

			{
				static float f = 0.0f;
				//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
				ImGui::ColorEdit3("clear color1", (float*)&clear_color1); // Edit 3 floats representing a color
				ImGui::ColorEdit3("clear color2", (float*)&clear_color2);
				ImGui::ColorEdit3("clear color3", (float*)&clear_color3);
				ImGui::Checkbox("TRIANGLE", &triangle);
				ImGui::Checkbox("POINT", &point);
				ImGui::Checkbox("LINE", &line);
				ImGui::Checkbox("MULTIPLE TRIANGLE", &m_triangle);
				ImGui::SameLine();
			}
		}
		ImGui::End();
		
		// 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
		if (triangle)
		{
			// render
			// ------
			vertices[3] = clear_color1.x, vertices[4] = clear_color1.y, vertices[5] = clear_color1.z;
			vertices[9] = clear_color2.x, vertices[10] = clear_color2.y, vertices[11] = clear_color2.z;
			vertices[15] = clear_color3.x, vertices[16] = clear_color3.y, vertices[17] = clear_color3.z;
			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// color attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

		// 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
		if (point)
		{
			//ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
			//ImGui::ShowDemoWindow(&show_demo_window);
			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(0);
			// color attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_p), vertices_p, GL_STATIC_DRAW);
			glDrawArrays(GL_POINTS, 0, 1);
		}
		if (line)
		{
			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// color attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(1 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_l), vertices_l, GL_STATIC_DRAW);
			glDrawArrays(GL_LINE_STRIP, 0, 2);
		}
		
		if (m_triangle)
		{
			unsigned int EBO;
			glGenBuffers(1, &EBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_m), vertices_m, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// color attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)(1 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
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
		//glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		//glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

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
