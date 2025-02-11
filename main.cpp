#define NANOSVG_IMPLEMENTATION	// Expands implementation
#include "3dparty/nanosvg/src/nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "3dparty/nanosvg/src/nanosvgrast.h"

// Include OpenGL-related libraries
#include <GL/glew.h>      // Always first for OpenGL
#include <GLFW/glfw3.h>

// Standard library includes
#include <string>
#include <iostream>

#include <algorithm>
#include <sys/stat.h>
#include "common/gltf_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "3dparty/tinygltf/stb_image.h"
#include "3dparty/tinygltf/stb_image_write.h"
#include "common/texture.h"

// Project-specific headers
#include "common/utilities.h"
#include "common/debugging.h"
#include "common/renderable.h"
#include "common/shaders.h"
#include "common/simple_shapes.h"
#include "common/carousel/carousel.h"
#include "common/carousel/carousel_to_renderable.h"
#include "common/carousel/carousel_loader.h"
#include "common/matrix_stack.h"
#include "common/intersection.h"
#include "common/trackball.h"
#include "common/trackball.h"
#include "common/projector.h"
#include "common/frame_buffer_object.h"
#include "common/point_light.h"

/* creo un array di due oggetti di tipo trackball e curr_tb mi tiene traccia dell'indice attivo tralle due tb*/
trackball tb[2];
int curr_tb;

/* projection matrix*/
glm::mat4 proj;

/* view matrix */
glm::mat4 view;

matrix_stack stack;
float scaling_factor = 1.0;

//Shadow Mapping del sole
float depth_bias = 0;
float k_plane_approx = 0.5;

frame_buffer_object ligthDepthFbo;
projector Lproj;
glm::vec4 Ldir;


frame_buffer_object spotDepthFbo;
projector spotProj;
glm::vec3 spotPos;
glm::vec4 spotDir;

box3 bbox_scene;
std::vector<renderable> scene;

//Callback per fare gestire il resize della finestra
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
	proj = glm::perspective(glm::radians(45.f), (float)width/height, 0.1f, 100.f);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	tb[curr_tb].mouse_move(proj, view, xpos, ypos);
}

/* callback function called when a mouse button is pressed */
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

	/* Qui ho implementato il premere la rotella */
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		tb[curr_tb].mouse_middle_press(proj, view, xpos, ypos);
	}
	else
		if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
			tb[curr_tb].mouse_middle_release();
		}

	/* Qui ho implementato il tasto sx che fa il semplice drag */
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		//Quindi se sono qui dentro e' perche' sto premendo i tasti giusti
		double xpos, ypos;
		//ricavo le coordinate del mouse x e y rispetto alla finestra
		glfwGetCursorPos(window, &xpos, &ypos);
		tb[curr_tb].mouse_sx_press(xpos, ypos);
	}
	else
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			tb[curr_tb].mouse_sx_release();
		}

}

/* callback function called when a mouse wheel is rotated */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (curr_tb == 0)
		tb[0].mouse_scroll(xoffset, yoffset);
}

int main(int argc, char** argv)
{
	//Setuppiamo GLFW---------------------------------------------
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(800, 800, "CarOusel", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glewInit();
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	/* declare the callback functions on mouse events */
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Sincronizza con il refresh rate del monitor

	//Setuppiamo OPENGL---------------------------------------------
	/* define the viewport  */
	glViewport(0, 0, 800, 800);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_MULTISAMPLE);
	glDepthRange(0.01, 1);
	printout_opengl_glsl_info();

	//Carichiamo le risorse (shaders, textures, modelli, matrici...) ---------------------------------------------
	shader basic_shader;
	basic_shader.create_program("shaders/basic.vert", "shaders/basic.frag");
	
	shader depth_shader;
	depth_shader.create_program("shaders/depth.vert", "shaders/depth.frag");


	gltf_loader gltfLoader;
	gltfLoader.load_to_renderable("assets/models/scene.glb", scene, bbox_scene);

	
	//Setuppa porjection e view matrix
	proj = glm::perspective(glm::radians(45.f), (float)1.0f, 0.1f, 100.f);
	view = glm::lookAt(glm::vec3(0, 1.0f, 1.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0, 1.f, 0.f));
	
	//Setuppa trackball
	tb[0].reset();
	tb[0].set_center_radius(glm::vec3(0, 0, 0), 0.5f);
	curr_tb = 0;

	glm::mat4 mainCamera = glm::lookAt(glm::vec3(0, 1.0f, 1.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0, 1.f, 0.f));

	/* initial light direction */
	Ldir = glm::vec4(0.0, 1.0, 0.0, 0.0);
	k_plane_approx = 0.5;

	/* light projection */
	Lproj.sm_size_x = 512;
	Lproj.sm_size_y = 512;
	Lproj.distance_light = 0.5;
	depth_bias = 0;
	
	ligthDepthFbo.create(Lproj.sm_size_x, Lproj.sm_size_y,true);

	/* SpotLight seutp */
	spotPos = glm::vec3(0, 0.5, 1.0f);
	spotDir = glm::vec4(glm::normalize(-spotPos), 0); //la spotLight guarda verso <0,0,0>
	spotProj.sm_size_x = 512;
	spotProj.sm_size_y = 512;
	spotProj.distance_light = 0.5;

	spotDepthFbo.create(spotProj.sm_size_x, spotProj.sm_size_y, true);

	//Impostazioni luce della scena
	glUseProgram(basic_shader.program);

	//Impostazioni spotLight
	basic_shader.bind("uBias");
	basic_shader.SetFloat("uBias", 0.01f);
	basic_shader.bind("uSpotShadowMapSize");
	basic_shader.SetVector2("uSpotShadowMapSize", glm::vec2(spotProj.sm_size_x, spotProj.sm_size_y));

	check_gl_errors(__LINE__, __FILE__);

	glClearColor(0.3f, 0.3f, 0.3f, 1.f);
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		//glClearColor(0.3f, 0.3f, 0.3f, 1.f);
		check_gl_errors(__LINE__, __FILE__);

		glm::vec3 c = bbox_scene.center();
		c.y = 0;
		stack.load_identity();
		stack.push();

		stack.mult(glm::scale(glm::mat4(1), glm::vec3(1/bbox_scene.diagonal())));
		stack.mult(glm::translate(glm::mat4(1), -c));

		//Depth Pass --------------------------------------------------------------------------------------
		{
			//Disegno sul framebuffer la depth dal punto di vista della luce 
			glBindFramebuffer(GL_FRAMEBUFFER, spotDepthFbo.id_fbo);
			glViewport(0, 0, spotProj.sm_size_x, spotProj.sm_size_y);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			glUseProgram(depth_shader.program);

			glm::mat4 view = glm::lookAt(glm::vec3(spotPos), glm::vec3(0), glm::vec3(0.0, 1.0, 0.0));
			glm::mat4 proj = glm::perspective(glm::radians(45.f), (float)1.0f, 0.1f, 100.f);

			depth_shader.SetMatrix4x4("uSpotLightMatrix", proj * view);
			depth_shader.SetFloat("uPlaneApprox", k_plane_approx);

			stack.push();
			DrawModel(scene, depth_shader, stack.m());
			stack.pop();

		}
		check_gl_errors(__LINE__, __FILE__);


		//Rendering Pass --------------------------------------------------------------------------------------
		{

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			glUseProgram(basic_shader.program);
			basic_shader.SetVector3("uColor", glm::vec3(1, 1, 1));

			glActiveTexture(GL_TEXTURE0);
    		glBindTexture(GL_TEXTURE_2D, spotDepthFbo.id_tex);

			basic_shader.SetMatrix4x4("uProj", proj);
			basic_shader.SetMatrix4x4("uView", mainCamera * tb[0].matrix());

			basic_shader.bind("uSpotShadowMap");
			basic_shader.SetInt("uSpotShadowMap", 0);
			basic_shader.bind("uSpotLightMatrix");
			basic_shader.SetMatrix4x4("uSpotLightMatrix", Lproj.light_matrix());

			//Disegna cose qui
			stack.push();

			DrawModel(scene, basic_shader, stack.m());

			stack.pop();
		}
		check_gl_errors(__LINE__, __FILE__);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glUseProgram(0);
	glfwTerminate();
	return 0;
}
