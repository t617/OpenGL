
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 400;

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

	bool rotation = false;
	bool translation = false;
	bool scale = false;
	bool my_tool_active = false;
	bool depth_test = false;

	// configure global opengl state
	// -----------------------------
	//glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("6.2.coordinate_systems.vs", "6.2.coordinate_systems.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		-0.4f, -0.4f, -0.4f,  0.0f, 0.0f,
		0.4f, -0.4f, -0.4f,  1.0f, 0.0f,
		0.4f,  0.4f, -0.4f,  1.0f, 1.0f,
		0.4f,  0.4f, -0.4f,  1.0f, 1.0f,
		-0.4f,  0.4f, -0.4f,  0.0f, 1.0f,
		-0.4f, -0.4f, -0.4f,  0.0f, 0.0f,

		-0.4f, -0.4f,  0.4f,  0.0f, 0.0f,
		0.4f, -0.4f,  0.4f,  1.0f, 0.0f,
		0.4f,  0.4f,  0.4f,  1.0f, 1.0f,
		0.4f,  0.4f,  0.4f,  1.0f, 1.0f,
		-0.4f,  0.4f,  0.4f,  0.0f, 1.0f,
		-0.4f, -0.4f,  0.4f,  0.0f, 0.0f,

		-0.4f,  0.4f,  0.4f,  1.0f, 0.0f,
		-0.4f,  0.4f, -0.4f,  1.0f, 1.0f,
		-0.4f, -0.4f, -0.4f,  0.0f, 1.0f,
		-0.4f, -0.4f, -0.4f,  0.0f, 1.0f,
		-0.4f, -0.4f,  0.4f,  0.0f, 0.0f,
		-0.4f,  0.4f,  0.4f,  1.0f, 0.0f,

		0.4f,  0.4f,  0.4f,  1.0f, 0.0f,
		0.4f,  0.4f, -0.4f,  1.0f, 1.0f,
		0.4f, -0.4f, -0.4f,  0.0f, 1.0f,
		0.4f, -0.4f, -0.4f,  0.0f, 1.0f,
		0.4f, -0.4f,  0.4f,  0.0f, 0.0f,
		0.4f,  0.4f,  0.4f,  1.0f, 0.0f,

		-0.4f, -0.4f, -0.4f,  0.0f, 1.0f,
		0.4f, -0.4f, -0.4f,  1.0f, 1.0f,
		0.4f, -0.4f,  0.4f,  1.0f, 0.0f,
		0.4f, -0.4f,  0.4f,  1.0f, 0.0f,
		-0.4f, -0.4f,  0.4f,  0.0f, 0.0f,
		-0.4f, -0.4f, -0.4f,  0.0f, 1.0f,

		-0.4f,  0.4f, -0.4f,  0.0f, 1.0f,
		0.4f,  0.4f, -0.4f,  1.0f, 1.0f,
		0.4f,  0.4f,  0.4f,  1.0f, 0.0f,
		0.4f,  0.4f,  0.4f,  1.0f, 0.0f,
		-0.4f,  0.4f,  0.4f,  0.0f, 0.0f,
		-0.4f,  0.4f, -0.4f,  0.0f, 1.0f
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// load and create a texture 
	// -------------------------
	unsigned int texture1, texture2;
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load("container2_specular.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	// texture 2
	// ---------
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	data = stbi_load("matrix.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);


	// render loop
	// -----------
	static float s = 0.0f;
	static bool b = false;
	static float e = 0.0f;
	static float init_t = 0.0f;

	static float init_s = 0.0f;
	static bool f = false;
	static float a = 0.0f;
	static float scale_f = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Begin("Transformation", &my_tool_active, ImGuiWindowFlags_MenuBar);
		ImGui::SetWindowSize(ImVec2(450, 400), true);
		if (ImGui::BeginMenuBar())
		{
			ImGui::EndMenuBar();
			{
				ImGui::Checkbox("DepthTest", &depth_test);
				ImGui::Checkbox("Translation", &translation);
				ImGui::Checkbox("Rotation", &rotation);
				ImGui::Checkbox("Scale", &scale);
				ImGui::SameLine();
			}
		}
		ImGui::End();
		if (depth_test) {
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
															// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		// activate shader
		ourShader.use();

		// create transformations
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;

		if (rotation) {
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(100.0f), glm::vec3(1.0f, 1.0f, 1.0f));	
		}
		else {
			model = glm::rotate(model, glm::radians(50.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		}

		if (scale) {
			if (!f) {
				f = true;
				init_s = (float)glfwGetTime();
			}
			a = (float)glfwGetTime() - init_s - (int)((float)glfwGetTime() - init_s);
			if (a >= 0.0f && a < 0.25f) {
				scale_f = a * 2 + 1.0f;
			}
			if (a >= 0.25f && a < 0.75f) {
				scale_f = 2.0f - 2 * a;
			}
			if (a >= 0.75f && a < 1.0f) {
				scale_f = 2 * a - 1.0f;
			}
			model = glm::scale(model, glm::vec3(scale_f, scale_f, scale_f));
		}
		else {
			f = false;
		}
		if (translation) {
			if (!b) {
				b = true;
				init_t = (float)glfwGetTime();
			}
			e = (float)glfwGetTime() - init_t - (int)((float)glfwGetTime() - init_t);
			if (e >= 0.0f && e < 0.25f) {
				s = e;
			}
			if (e >= 0.25f && e < 0.75f) {
				s = 0.5f - e;
			}
			if (e >= 0.75f && e < 1.0f) {
				s = e - 1.0f;
			}
			s *= 4;
			view = glm::translate(view, glm::vec3(s, 0.0f, -4.0f));
		}
		else {
			b = false;
			view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f));
		}
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		// retrieve the matrix uniform locations
		unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
		// pass them to the shaders (3 different ways)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		ourShader.setMat4("projection", projection);

		// render box
		glBindVertexArray(VAO);
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