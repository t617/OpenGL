#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include "camera.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

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

  glfwSetCursorPosCallback(window, mouse_callback);
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glEnable(GL_DEPTH_TEST);

  // Setup ImGui binding
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  ImGui_ImplGlfwGL3_Init(window, true);

  // Setup style
  ImGui::StyleColorsDark();
  ImGui::StyleColorsClassic();

  bool my_tool_active = false;
  bool projection_select = false;
  
  Shader ourShader("coordinate_systems.vs", "coordinate_systems.fs");
  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------

  float vertices[] = {
	//position				// color
    -0.2f, -0.2f, -0.2f, 1.0f, 0.0f, 0.0f,
    0.2f, -0.2f, -0.2f,  1.0f, 0.0f, 0.0f,
    0.2f,  0.2f, -0.2f,  1.0f, 0.0f, 0.0f,
    0.2f,  0.2f, -0.2f,  1.0f, 0.0f, 0.0f,
    -0.2f,  0.2f, -0.2f,  1.0f, 0.0f, 0.0f,
    -0.2f, -0.2f, -0.2f,  1.0f, 0.0f, 0.0f,

    -0.2f, -0.2f,  0.2f,  0.0f, 1.0f, 0.0f,
    0.2f, -0.2f,  0.2f,  0.0f, 1.0f, 0.0f,
    0.2f,  0.2f,  0.2f,  0.0f, 1.0f, 0.0f,
    0.2f,  0.2f,  0.2f,  0.0f, 1.0f, 0.0f,
    -0.2f,  0.2f,  0.2f,  0.0f, 1.0f, 0.0f,
    -0.2f, -0.2f,  0.2f,  0.0f, 1.0f, 0.0f,

    -0.2f,  0.2f,  0.2f,  0.0f, 0.0f, 1.0f,
    -0.2f,  0.2f, -0.2f,  0.0f, 0.0f, 1.0f,
    -0.2f, -0.2f, -0.2f,  0.0f, 0.0f, 1.0f,
    -0.2f, -0.2f, -0.2f,  0.0f, 0.0f, 1.0f,
    -0.2f, -0.2f,  0.2f,  0.0f, 0.0f, 1.0f,
    -0.2f,  0.2f,  0.2f,  0.0f, 0.0f, 1.0f,

    0.2f,  0.2f,  0.2f,  0.0f, 1.0f, 1.0f,
    0.2f,  0.2f, -0.2f,  0.0f, 1.0f, 1.0f,
    0.2f, -0.2f, -0.2f,  0.0f, 1.0f, 1.0f,
    0.2f, -0.2f, -0.2f,  0.0f, 1.0f, 1.0f,
    0.2f, -0.2f,  0.2f,  0.0f, 1.0f, 1.0f,
    0.2f,  0.2f,  0.2f,  0.0f, 1.0f, 1.0f,

    -0.2f, -0.2f, -0.2f,  1.0f, 0.0f, 1.0f,
    0.2f, -0.2f, -0.2f,  1.0f, 0.0f, 1.0f,
    0.2f, -0.2f,  0.2f,  1.0f, 0.0f, 1.0f,
    0.2f, -0.2f,  0.2f,  1.0f, 0.0f, 1.0f,
    -0.2f, -0.2f,  0.2f,  1.0f, 0.0f, 1.0f,
    -0.2f, -0.2f, -0.2f,  1.0f, 0.0f, 1.0f,

    -0.2f,  0.2f, -0.2f,  1.0f, 1.0f, 0.0f,
    0.2f,  0.2f, -0.2f,  1.0f, 1.0f, 0.0f,
    0.2f,  0.2f,  0.2f,  1.0f, 1.0f, 0.0f,
    0.2f,  0.2f,  0.2f,  1.0f, 1.0f, 0.0f,
    -0.2f,  0.2f,  0.2f,  1.0f, 1.0f, 0.0f,
    -0.2f,  0.2f, -0.2f,  1.0f, 1.0f, 0.0f
  };

  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  ourShader.use();
  // render loop
  // -----------
  float left = -10.0f, right = 10.0f, top = -10.0f, bottom = 10.0f, near_ = -3.0f, far_ = 7.8f;
  float radian = 125.0f, width = 800.0f, height = 600.0f;
  int select = 0;

  while (!glfwWindowShouldClose(window))
  {
	// per-frame time logic
	// --------------------
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

    ImGui_ImplGlfwGL3_NewFrame();
    ImGui::Begin("Camera", &my_tool_active, ImGuiWindowFlags_MenuBar);
    ImGui::SetWindowSize(ImVec2(450, 400), true);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    // input
    // -----
    processInput(window);
	{
		ImGui::RadioButton("orthographic projection", &select, 0);
		ImGui::RadioButton("perspective projection", &select, 1);
		ImGui::RadioButton("View Changing", &select, 2);
		ImGui::RadioButton("Camera", &select, 3);
	}
	ImGui::End();
    // render
    // ------
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
                              // bind textures on corresponding texture units
    ourShader.use();

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
    model = glm::rotate(model, glm::radians(50.0f), glm::vec3(1.0f, 0.3f, 0.5f));
	float radius = 20.0f;
	float camX = sin(glfwGetTime()) * radius;
	float camZ = cos(glfwGetTime()) * radius;

	if (select == 0) {
		model = glm::translate(model, glm::vec3(-1.5f, 0.5f, -1.5f));
		view = glm::lookAt(glm::vec3(-1.5f, 0.5f, 0.0f), glm::vec3(-1.5f, 0.5f, -1.5f), glm::vec3(0.0f, 1.0f, 0.0f));
		projection = glm::ortho(left, right, top, bottom, near_, far_);
		ImGui::Begin("Projection", &my_tool_active, ImGuiWindowFlags_MenuBar);
		ImGui::Text("orthographic projection");
		ImGui::SliderFloat("left", &left, -10.0f, 0.0f);
		ImGui::SliderFloat("right", &right, 0.0f, 10.0f);
		ImGui::SliderFloat("top", &top, -10.0f, 0.0f);
		ImGui::SliderFloat("bottom", &bottom, 0.0f, 10.0f);
		ImGui::SliderFloat("near", &near_, -0.5f, 1.0f);
		ImGui::SliderFloat("far", &far_, 0.0f, 100.0f);
		ImGui::End();
	}
	else if (select == 1) {
		model = glm::translate(model, glm::vec3(-1.5f, 0.5f, -1.5f));
		view = glm::lookAt(glm::vec3(-1.5f, 0.5f, 1.0f), glm::vec3(-1.5f, 0.5f, -1.5f), glm::vec3(0.0f, 1.0f, 0.0f));
		projection = glm::perspective(glm::radians(radian), width / height, 0.1f, 100.0f);
		ImGui::Begin("Projection", &my_tool_active, ImGuiWindowFlags_MenuBar);
		ImGui::Text("perspective projection");
		ImGui::SliderFloat("radian", &radian, 0.0f, 720.0f);
		ImGui::SliderFloat("width", &width, 0.0f, 1600.0f);
		ImGui::SliderFloat("height", &height, 0.0f, 1200.0f);
		ImGui::End();
	}
	else if (select == 2) {
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	}
	else if(select == 3) {
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		// pass projection matrix to shader (note that in this case it could change every frame)
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		// camera/view transformation
		view = camera.GetViewMatrix();
	}
	ourShader.setMat4("view", view);
	ourShader.setMat4("model", model);
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

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}