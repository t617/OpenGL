
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>

#include <iostream>
#include <cmath>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lights and Shading", NULL, NULL);
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

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// Setup ImGui binding
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplGlfwGL3_Init(window, true);

	// Setup style
	ImGui::StyleColorsDark();

	bool my_tool_active = false;
	bool projection_select = false;

	// build and compile our shader zprogram
	// ------------------------------------
	Shader phongShader("phong_lighting.vs", "phong_lighting.fs");
	Shader gouraudShader("gouraud_lighting.vs", "gouraud_lighting.fs");
	Shader lampShader("lamp.vs", "lamp.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		-0.3f, -0.3f, -0.3f,  0.0f,  0.0f, -1.0f,
		0.3f, -0.3f, -0.3f,  0.0f,  0.0f, -1.0f,
		0.3f,  0.3f, -0.3f,  0.0f,  0.0f, -1.0f,
		0.3f,  0.3f, -0.3f,  0.0f,  0.0f, -1.0f,
		-0.3f,  0.3f, -0.3f,  0.0f,  0.0f, -1.0f,
		-0.3f, -0.3f, -0.3f,  0.0f,  0.0f, -1.0f,

		-0.3f, -0.3f,  0.3f,  0.0f,  0.0f,  1.0f,
		0.3f, -0.3f,  0.3f,  0.0f,  0.0f,  1.0f,
		0.3f,  0.3f,  0.3f,  0.0f,  0.0f,  1.0f,
		0.3f,  0.3f,  0.3f,  0.0f,  0.0f,  1.0f,
		-0.3f,  0.3f,  0.3f,  0.0f,  0.0f,  1.0f,
		-0.3f, -0.3f,  0.3f,  0.0f,  0.0f,  1.0f,

		-0.3f,  0.3f,  0.3f, -1.0f,  0.0f,  0.0f,
		-0.3f,  0.3f, -0.3f, -1.0f,  0.0f,  0.0f,
		-0.3f, -0.3f, -0.3f, -1.0f,  0.0f,  0.0f,
		-0.3f, -0.3f, -0.3f, -1.0f,  0.0f,  0.0f,
		-0.3f, -0.3f,  0.3f, -1.0f,  0.0f,  0.0f,
		-0.3f,  0.3f,  0.3f, -1.0f,  0.0f,  0.0f,

		0.3f,  0.3f,  0.3f,  1.0f,  0.0f,  0.0f,
		0.3f,  0.3f, -0.3f,  1.0f,  0.0f,  0.0f,
		0.3f, -0.3f, -0.3f,  1.0f,  0.0f,  0.0f,
		0.3f, -0.3f, -0.3f,  1.0f,  0.0f,  0.0f,
		0.3f, -0.3f,  0.3f,  1.0f,  0.0f,  0.0f,
		0.3f,  0.3f,  0.3f,  1.0f,  0.0f,  0.0f,

		-0.3f, -0.3f, -0.3f,  0.0f, -1.0f,  0.0f,
		0.3f, -0.3f, -0.3f,  0.0f, -1.0f,  0.0f,
		0.3f, -0.3f,  0.3f,  0.0f, -1.0f,  0.0f,
		0.3f, -0.3f,  0.3f,  0.0f, -1.0f,  0.0f,
		-0.3f, -0.3f,  0.3f,  0.0f, -1.0f,  0.0f,
		-0.3f, -0.3f, -0.3f,  0.0f, -1.0f,  0.0f,

		-0.3f,  0.3f, -0.3f,  0.0f,  1.0f,  0.0f,
		0.3f,  0.3f, -0.3f,  0.0f,  1.0f,  0.0f,
		0.3f,  0.3f,  0.3f,  0.0f,  1.0f,  0.0f,
		0.3f,  0.3f,  0.3f,  0.0f,  1.0f,  0.0f,
		-0.3f,  0.3f,  0.3f,  0.0f,  1.0f,  0.0f,
		-0.3f,  0.3f, -0.3f,  0.0f,  1.0f,  0.0f
	};
	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	float ambientStrength = 0.1, diffuseFactor = 1.0, specularStrength = 0.5, shininess = 32.0;
	int select = 0;
	float radius = sqrt(pow(1.2f, 2) + 4.0);
	bool isMove = false;
	ImVec4 object_color = ImVec4(1.0f, 0.5f, 0.31f, 1.00f);
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{

		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Begin("LightAndShadering", &my_tool_active, ImGuiWindowFlags_MenuBar);
		ImGui::SetWindowSize(ImVec2(450, 400), true);
		{
			ImGui::RadioButton("Phong Shading", &select, 0);
			ImGui::RadioButton("Gouraud Shading", &select, 1);

			ImGui::Checkbox("Move", &isMove);

			ImGui::ColorEdit3("object_color", (float*)&object_color);

			ImGui::SliderFloat("ambientStrength", &ambientStrength, 0.0f, 2.0f);
			ImGui::SliderFloat("diffuseFactor", &diffuseFactor, 0.0f, 2.0f);
			ImGui::SliderFloat("specularStrength", &specularStrength, 0.0f, 10.0f);
			ImGui::SliderFloat("shininess", &shininess, 0.0f, 64.0f);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		ImGui::End();
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model;
		model = glm::rotate(model, glm::radians(-80.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		if (select == 0) {
			// be sure to activate shader when setting uniforms/drawing objects
			phongShader.use();
			phongShader.setVec3("objectColor", object_color.x, object_color.y, object_color.z);
			phongShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			phongShader.setVec3("lightPos", lightPos);
			phongShader.setVec3("viewPos", glm::vec3(0.0f, 0.0f, 3.0f));

			phongShader.setFloat("ambientStrength", ambientStrength);
			phongShader.setFloat("diffuseFactor", diffuseFactor);
			phongShader.setFloat("specularStrength", specularStrength);
			phongShader.setFloat("shininess", shininess);
			// view/projection transformations
			phongShader.setMat4("projection", projection);
			phongShader.setMat4("view", view);

			// world transformation
			phongShader.setMat4("model", model);
		}
		else if (select == 1) {
			// be sure to activate shader when setting uniforms/drawing objects
			gouraudShader.use();
			gouraudShader.setVec3("objectColor", object_color.x, object_color.y, object_color.z);
			gouraudShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			gouraudShader.setVec3("lightPos", lightPos);
			gouraudShader.setVec3("viewPos", glm::vec3(0.0f, 0.0f, 3.0f));

			gouraudShader.setFloat("ambientStrength", ambientStrength);
			gouraudShader.setFloat("diffuseFactor", diffuseFactor);
			gouraudShader.setFloat("specularStrength", specularStrength);
			gouraudShader.setFloat("shininess", shininess);
			// view/projection transformations
			gouraudShader.setMat4("projection", projection);
			gouraudShader.setMat4("view", view);

			// world transformation
			gouraudShader.setMat4("model", model);
		}

		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		if (isMove) {
			lightPos = glm::vec3(camX, 1.0, camZ);
		}
		else {
			lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
		}
		// render the cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		// also draw the lamp object
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		
		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lampShader.setMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
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