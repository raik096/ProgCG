#define NANOSVG_IMPLEMENTATION	// Expands implementation
#include "3dparty/nanosvg/src/nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "3dparty/nanosvg/src/nanosvgrast.h"

#include "common/texture.h"


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

/* creo un array di due oggetti di tipo trackball e curr_tb mi tiene traccia dell'indice attivo tralle due tb*/
trackball tb[2];
int curr_tb;

/* projection matrix*/
glm::mat4 proj;

/* view matrix */
glm::mat4 view;

matrix_stack stack;
float scaling_factor = 1.0;


/* callback function called when the mouse is moving
	vediamo di capirci qualcosa:
	la funzione sotto mi serve per gestire il meccanismo si trackball e prende come argomento:
	la posizione sull'asse x e quella sull'asse y, essendo il mouse sulla finestra non ha bisogno della
	coordinata z, a questo punto h come arogmento il puntatore alla finestra di rendering
*/
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	/* every time any key is presse it switch from controlling trackball tb[0] to tb[1] and viceversa */
	if (action == GLFW_PRESS)
		curr_tb = 1 - curr_tb;
}

/* fase texture */




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
	/* declare the callback functions on mouse events */
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glewInit();

	printout_opengl_glsl_info();

	//Setuppiamo OPENGL---------------------------------------------
	/* define the viewport  */
	glViewport(0, 0, 800, 800);

	glEnable(GL_DEPTH_TEST);


	//Setuppiamo il carosello---------------------------------------------
	race r;
	carousel_loader::load("small_test.svg", "terrain_256.png", r);

	//add 10 cars
	for (int i = 0; i < 10; ++i)
		r.add_car();

	renderable fram = shape_maker::frame();
	renderable r_cube = shape_maker::cube();

	renderable r_track;
	r_track.create();
	game_to_renderable::to_track(r, r_track);

	renderable r_terrain;
	r_terrain.create();
	game_to_renderable::to_heightfield(r, r_terrain);

	renderable r_trees;
	r_trees.create();
	game_to_renderable::to_tree(r, r_trees);

	//renderable r_lamps;
	//r_lamps.create();
	//game_to_renderable::to_lamps(r, r_lamps);

	gltf_loader gltfLoader;

	box3 bbox;
	std::vector<renderable> obj;
	try {
        gltfLoader.load_to_renderable("/home/re/ProgCG/assets/models/lamp.glb", obj, bbox);
        std::cout << "Model loaded successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading model: " << e.what() << std::endl;
        glfwTerminate();
        return -1;
    }

	shader basic_shader;
	basic_shader.create_program("shaders/basic.vert", "shaders/basic.frag");

	/* use the program shader "program_shader" */
	glUseProgram(basic_shader.program);

	tb[0].reset();
	tb[0].set_center_radius(glm::vec3(0, 0, 0), 1.f);
	curr_tb = 0;

	proj = glm::perspective(glm::radians(45.f), 1.f, 0.1f, 100.f);
	view = glm::lookAt(glm::vec3(0, 1.f, 1.5), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0, 1.f, 0.f));
	basic_shader.SetMatrix4x4("uProj", proj);
	basic_shader.SetMatrix4x4("uView", view);

	r.start(11, 0, 0, 600);
	r.update();

	matrix_stack stack;

	texture terrain_texture = LoadTexture("common/carousel/grass_tile.png");
	texture track_texture = LoadTexture("common/carousel/street_tile.png");
	
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClearColor(0.3f, 0.3f, 0.3f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		check_gl_errors(__LINE__, __FILE__);

		//Aggiorna viewMatrix della camera
		basic_shader.SetMatrix4x4("uView", view);

		r.update();
		stack.load_identity();
		stack.push();
		stack.mult(tb[0].matrix());
		glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
		glUniform3f(basic_shader["uColor"], -1.f, 0.6f, 0.f);
		fram.bind();
		glDrawArrays(GL_LINES, 0, 6);

		glColor3f(0, 0, 1);
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(r.sunlight_direction().x, r.sunlight_direction().y, r.sunlight_direction().z);
		glEnd();


		float s = 1.f / r.bbox().diagonal();
		glm::vec3 c = r.bbox().center();

		stack.mult(glm::scale(glm::mat4(1.f), glm::vec3(s)));
		stack.mult(glm::translate(glm::mat4(1.f), -c));


		glDepthRange(0.01, 1);
		glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
		glUniform3f(basic_shader["uColor"], 1, 1, 1.0);

		BindTexture(basic_shader, "uTexture", terrain_texture, 0);
		r_terrain.bind();
		//glDrawArrays(GL_POINTS, 0, r_terrain.vn);		
		// Debug dei vertici e degli indici
		glDrawElements(GL_TRIANGLES, 390150, GL_UNSIGNED_INT, 0);
		glDepthRange(0.0, 1);

		for (unsigned int ic = 0; ic < r.cars().size(); ++ic) {
			stack.push();
			stack.mult(r.cars()[ic].frame);
			stack.mult(glm::translate(glm::mat4(1.f), glm::vec3(0, 0.1, 0.0)));
			glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
			glUniform3f(basic_shader["uColor"], -1.f, 0.6f, 0.f);
			fram.bind();
			glDrawArrays(GL_LINES, 0, 6);
			stack.pop();
		}

		fram.bind();
		for (unsigned int ic = 0; ic < r.cameramen().size(); ++ic) {
			stack.push();
			stack.mult(r.cameramen()[ic].frame);
			stack.mult(glm::scale(glm::mat4(1.f), glm::vec3(4, 4, 4)));
			glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
			glUniform3f(basic_shader["uColor"], -1.f, 0.6f, 0.f);
			glDrawArrays(GL_LINES, 0, 6);
			stack.pop();
		}
		glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);

		BindTexture(basic_shader, "uTexture", track_texture, 0);
		r_track.bind();
		glPointSize(3.0);
		glUniform3f(basic_shader["uColor"], 1.0f, 1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, r_track.vn);
		glPointSize(1.0);
		
		r_trees.bind();
		glUniform3f(basic_shader["uColor"], 0.f, 1.0f, 0.f);
		glDrawArrays(GL_LINES, 0, r_trees.vn);
		
		//Disegno i lampioni		
		obj[0].bind();
		for (stick_object l : r.lamps())
		{
			std::cout << "Posizione lampione: " << glm::to_string(l.pos) << std::endl;
			stack.push();
			// Trasla la matrice 
			// Crea una matrice di trasformazione combinata: traslazione + trasformazione base
			stack.mult(obj[0].transform * glm::translate(glm::mat4(1), l.pos)); // Applica la trasformazione base del modello GLB

			//glm::mat4 model = stack.m() * glm::translate(glm::mat4(1), l.pos);
			std::cout << "Matrice trasformazione: " << glm::to_string(stack.m()) << std::endl;

			// Passa la matrice combinata allo shader
			glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);

			// Imposta il colore del lampione
			glUniform3f(basic_shader["uColor"], 0.0f, 0.0f, 0.0f);

			// Renderizza l'oggetto
			glDrawElements(obj[0]().mode, obj[0]().count, obj[0]().itype, 0);

			stack.pop(); // Ripristina lo stato precedente
		}
						
		/* metodo funzionale nel rendering di almeno un lampione
		obj[0].bind();
		for (stick_object l : r.lamps())
		{
			stack.push();
			stack.mult(obj[0].transform);
			//glm::mat4 model = glm::translate(glm::mat4(1), l.pos);
			glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
			glUniform3f(basic_shader["uColor"], 0.0f, 0.0f, 0.0f);
			glDrawElements(obj[0]().mode, obj[0]().count, obj[0]().itype, 0);
			stack.pop();
		}
		*/
		//r_lamps.bind();
		//glDrawArrays(GL_LINES, 0, r_lamps.vn);

		stack.pop();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glUseProgram(0);
	glfwTerminate();
	return 0;
}

