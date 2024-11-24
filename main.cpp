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

/* creo un array di due oggetti di tipo trackball e curr_tb mi tiene traccia dell'indice attivo tralle due tb*/
trackball tb[2];
int curr_tb;

/* projection matrix*/
glm::mat4 proj;

/* view matrix */
glm::mat4 view;

matrix_stack stack;
float scaling_factor = 1.0;

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

bool change_cam = false;
glm::mat4 backupview(1.0f);

void change_view(const race& r) 
{
    if (change_cam) {
        backupview = view;
		
		//view = r.cameramen()[3].frame;
		//view[0] = glm::vec4(0, 0, 0, 0);
		view = glm::lookAt(glm::vec3(r.cameramen()[1].frame[3]), 
						   glm::vec3(r.cameramen()[1].frame[3] - r.cameramen()[1].frame[2]), 
						   glm::vec3(r.cameramen()[1].frame[1]));
		//view += glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
		//view = glm::inverse(glm::translate(r.cameramen()[3].frame, glm::vec(0.0	f, 1.0f, 0.0f)));

	
		//COORDINATE COMERAMEN 1
		//Actual View Matrix:
		//mat4x4((7.927183, 0.000000, 0.000000, 0.000000), (0.000000, 6.595815, 4.397210, 0.000000), (0.000000, -4.397210, 6.595815, 0.000000), (2.327531, 0.731998, -1.314777, 1.000000))
	
    } else {
        // Se la matrice non e' vuota vuoldire che voglio ripristinare quella iniziale
        if (backupview != glm::mat4(1.0f)) {
            view = backupview;
        }
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	/* every time any key is presse it switch from controlling trackball tb[0] to tb[1] and viceversa */
	if (action == GLFW_PRESS) 
		curr_tb = 1 - curr_tb;
		if (key == GLFW_KEY_C) {
			if (change_cam) {
				change_cam = false; 
				return;
			}
			if (!change_cam) {
				change_cam = true; 
				return;
			}
		}
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
	/* declare the callback functions on mouse events */
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1); // Sincronizza con il refresh rate del monitor

	glewInit();
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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

	gltf_loader gltfLoader;

	box3 bbox_lamps, bbox_cars, bbox_trees, bbox_drones;
	std::vector<renderable> lamp_objects;
	std::vector<renderable> car_objects;
	std::vector<renderable> tree_objects;
	std::vector<renderable> drone_objects;

	/* carico le macchine quindi car_objects */
    gltfLoader.load_to_renderable("assets/models/car0.glb", car_objects, bbox_cars);
	/* carico le lamp quindi lamp_objects */
    gltfLoader.load_to_renderable("assets/models/lamp.glb", lamp_objects, bbox_lamps);
	/* carico gli alberi quindi tree_objects */
	gltfLoader.load_to_renderable("assets/models/trees.glb", tree_objects, bbox_trees);
	/* carico i droni quindi droni_objects */
	gltfLoader.load_to_renderable("assets/models/drone.glb", drone_objects, bbox_drones);


	shader basic_shader;
	basic_shader.create_program("shaders/basic.vert", "shaders/basic.frag");

	/* use the program shader "program_shader" */
	glUseProgram(basic_shader.program);

	tb[0].reset();
	tb[0].set_center_radius(glm::vec3(0, 0, 0), 1.f);
	curr_tb = 0;

	proj = glm::perspective(glm::radians(45.f), 1.f, 0.1f, 100.f);
	view = glm::lookAt(glm::vec3(0, 1.f, 1.5), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0, 1.f, 0.f));
	//view = glm::lookAt(glm::vec3(r.cameramen()[1].frame[3]), 
	//				   glm::vec3(r.cameramen()[1].frame[3] - r.cameramen()[1].frame[2]),
	//				   glm::vec3(r.cameramen()[1].frame[1]));
	
	std::cout << "View Matrix:\n" << glm::to_string(view) << std::endl;
	std::cout << "r.cameramen()[3].frame[3]: \n" << glm::to_string(*(glm::vec3*)&r.cameramen()[3].frame[3]) << std::endl;
	std::cout << "cameramen.pos: " << glm::to_string(r.cameramen()[3].pos) << std::endl;




	basic_shader.SetMatrix4x4("uProj", proj);
	basic_shader.SetMatrix4x4("uView", view);

	r.start(11, 0, 0, 20);
	r.update();

	matrix_stack stack;

	texture lamp_texture = LoadTexture("assets/textures/lampColor.png");
	texture terrain_texture = LoadTexture("common/carousel/grass_tile.png");
	texture track_texture = LoadTexture("common/carousel/street_tile.png");
	
		//Impostazioni luce della scena
	basic_shader.bind("uLampLigthColor");
	basic_shader.SetVector3("uLampLigthColor", glm::vec3(1, 0.97, 0.76));
	basic_shader.bind("uLampC1");
	basic_shader.SetFloat("uLampC1", 0.1f);
	basic_shader.bind("uLampC2");
	basic_shader.SetFloat("uLampC2", 0.02f);
	basic_shader.bind("uLampC3");
	basic_shader.SetFloat("uLampC3", 750.0f);
	basic_shader.SetVector3("LampTest", r.lamps()[0].pos);
	//Carica tutti i lampioni in shader
	basic_shader.bind("uLampsAmount");
	basic_shader.SetInt("uLampsAmount", r.lamps().size());



	// Imposta il colore dei fari
	basic_shader.bind("uHeadlightColor");
	basic_shader.SetVector3("uHeadlightColor", glm::vec3(1.0f, 0.9f, 0.8f)); // Colore bianco caldo

	// Imposta la attenuazione
	basic_shader.bind("uHeadlightC1");
	basic_shader.SetFloat("uHeadlightC1", 0.1f); // Costante
	basic_shader.bind("uHeadlightC2");
	basic_shader.SetFloat("uHeadlightC2", 0.02f); // Lineare
	basic_shader.bind("uHeadlightC3");
	basic_shader.SetFloat("uHeadlightC3", 100.f); // Quadratica

	// Angoli del cono (cutoff interno ed esterno)
	basic_shader.bind("uHeadlightCutOff");
	basic_shader.SetFloat("uHeadlightCutOff", glm::cos(glm::radians(5.5f))); // Interno
	basic_shader.bind("uHeadlightOuterCutOff");
	basic_shader.SetFloat("uHeadlightOuterCutOff", glm::cos(glm::radians(50.f))); // Esterno

	// Carica tutti i fari in shader (cars x 2)
	basic_shader.bind("uHeadlightAmount");
	basic_shader.SetInt("uHeadlightAmount", r.cars().size() * 2);


	glActiveTexture(GL_TEXTURE0);
	bool prev_change_cam = change_cam;
	//std::vector glm::vec3> cameramanPosition

	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClearColor(0.3f, 0.3f, 0.3f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		check_gl_errors(__LINE__, __FILE__);


		//Aggiorna viewMatrix della camera
		basic_shader.SetMatrix4x4("uProj", proj);
		// Aggiorna la viewMatrix solo se necessario
		if (change_cam != prev_change_cam) {
			change_view(r);  // Aggiorna la vista se la telecamera è cambiata
			prev_change_cam = change_cam;  // Memorizza lo stato precedente
		}
		basic_shader.SetMatrix4x4("uView", view * tb[0].matrix());
		//std::cout << "Actual View Matrix:\n" << glm::to_string(view * tb[0].matrix()) << std::endl;

		r.update();
		basic_shader.SetVector3("uSunDirection", r.sunlight_direction());
		stack.load_identity();
		stack.push();
		//stack.mult(tb[0].matrix());
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

		glDrawElements(GL_TRIANGLES, 390150, GL_UNSIGNED_INT, 0);
		glDepthRange(0.0, 1);
		

		for (unsigned int k = 0; k < r.cars().size(); ++k) {
			stack.push();
			stack.mult(r.cars()[k].frame);
			//stack.mult(glm::rotate(r.cars()[k].frame, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

			// Posizione primo faro
			glm::vec3 headlightPos1 = glm::vec3(glm::translate(stack.m(), glm::vec3(0.7f, 0.3f, 0.5f)) * glm::vec4(0, 0, 0, 1));
			basic_shader.bind("uHeadlights[" + std::to_string(k * 2) + "]");
			basic_shader.SetVector3("uHeadlights[" + std::to_string(k * 2) + "]", headlightPos1);

			// Posizione secondo faro
			glm::vec3 headlightPos2 = glm::vec3(glm::translate(stack.m(), glm::vec3(-0.7f, 0.3f, 0.5f)) * glm::vec4(0, 0, 0, 1));
			basic_shader.bind("uHeadlights[" + std::to_string(k * 2 + 1) + "]");
			basic_shader.SetVector3("uHeadlights[" + std::to_string(k * 2 + 1) + "]", headlightPos2);

			// Direzione dei fari
			glm::vec3 headlightDir = glm::normalize(glm::vec3(stack.m()[3]));  // Direzione del primo faro
			//glm::vec3 headlightDir = glm::normalize(glm::vec3(stack.m() * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));  // Direzione del primo faro
    		basic_shader.bind("uHeadlightDir");
			basic_shader.SetVector3("uHeadlightDir", headlightDir);

			for (unsigned int i = 0; i < car_objects.size(); ++i) {
				car_objects[i].bind();
				stack.push();
				stack.mult(car_objects[i].transform);
				glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
				glBindTexture(GL_TEXTURE_2D, car_objects[i].mater.base_color_texture);
				glUniform3f(basic_shader["uColor"], 1.f, 1.f, 1.f);
				glDrawElements(car_objects[i]().mode, car_objects[i]().count, car_objects[i]().itype, 0);    
				stack.pop(); 
			}

			stack.pop();
		}

		
		//fram.bind();
		for (int k = 0; k < r.cameramen().size(); ++k) {
			stack.push();
			glm::mat4 model = r.cameramen()[k].frame; // Matrice di trasformazione base (traslazione sul punto)
			model = model * glm::scale(glm::mat4(1.f), glm::vec3(0.5f, 0.5f, 0.5f));  // Scala
			model = model * glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 1.0f, 0.0f)); // Traslazione verso l'alto
			stack.mult(model);
			//std::cout << "model cameraman Matrix:\n" << glm::to_string(model) << std::endl;

			//glm::vec3 cameramanPosition = glm::vec3(model[3]); // Estrae la traslazione (posizione)

			for (unsigned int i = 0; i < drone_objects.size(); ++i) {
				drone_objects[i].bind();
				stack.push();
				stack.mult(drone_objects[i].transform);
				glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
				glBindTexture(GL_TEXTURE_2D, drone_objects[i].mater.base_color_texture);
				glUniform3f(basic_shader["uColor"], 1.f, 1.f, 1.f);
				glDrawElements(drone_objects[i].mode, drone_objects[i]().count, drone_objects[i]().itype, 0);
				stack.pop();
			}
			stack.pop();
		}
		glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);

		BindTexture(basic_shader, "uTexture", track_texture, 0);
		r_track.bind();
		glPointSize(3.0);
		glUniform3f(basic_shader["uColor"], 1.0f, 1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, r_track.vn);
		glPointSize(1.0);
		

	for (int k = 0; k < r.trees().size(); k++) {
		stick_object l = r.trees()[k];
		stack.push();
		stack.mult(glm::scale(glm::translate(glm::mat4(1.0f), l.pos), glm::vec3(0.1f))); // Traslazione

		for (unsigned int i = 0; i < tree_objects.size(); ++i) {
			tree_objects[i].bind();
			stack.push();
			stack.mult(tree_objects[i].transform);
			glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
			glBindTexture(GL_TEXTURE_2D, tree_objects[i].mater.base_color_texture);
			
			// Imposta il colore se necessario
			glm::vec3 objectColor = (i == 1) ? glm::vec3(0.45f, 0.17f, 0.02f) : glm::vec3(0.08f, 0.53f, 0.0f); 
			basic_shader.SetVector3("uColor", objectColor);

			// Aggiungi le normali (assicurati di avere le normali nel modello)
			//tree_objects[i].add_vertex_attribute(
			//	&tree_objects[i].mater.normal_texture,
			//	tree_objects[i].mater.normal_texture.size() / 3,
			//	2,
			//	3);

			glDrawElements(tree_objects[i]().mode, tree_objects[i]().count, tree_objects[i]().itype, 0);
			stack.pop();
		}
		stack.pop();
	}

	
		BindTexture(basic_shader, "uTexture", lamp_texture, 0);
		for (int k = 0;  k < r.lamps().size(); k++)
		{
			stick_object l = r.lamps()[k];
			stack.push();
			//stack.mult(glm::translate(glm::mat4(1), l.pos));
			stack.mult(glm::scale(glm::translate(glm::mat4(1), l.pos), glm::vec3(0.1)));
			basic_shader.bind("uLampLights[" + std::to_string(k) + "]");
			basic_shader.SetVector3("uLampLights[" + std::to_string(k) + "]", glm::vec3(glm::translate(stack.m(), glm::vec3(0, l.height, 0)) * glm::vec4(0, 0, 0, 1)));

			for (unsigned int i = 0; i < lamp_objects.size(); ++i) {
				lamp_objects[i].bind();
				//std::cout << "Posizione lampione: " << glm::to_string(l.pos) << std::endl;
				stack.push();
				//stack.mult(lamp_objects[i].transform);
				//glBindTexture(GL_TEXTURE_2D, lamp_objects[i].mater.base_color_texture);
				glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
				// Imposta il colore del lampione
				glUniform3f(basic_shader["uColor"], 1.0f, 1.0f, 1.0f);
				// Renderizza l'oggetto
				glDrawElements(lamp_objects[i]().mode, lamp_objects[i]().count, lamp_objects[i]().itype, 0);
				stack.pop();
			}
			stack.pop();
		}

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

