#ifndef MATERIAL_pointL_H
#define MATERIAL_pointL_H
#include <glm/glm.hpp>  
#include <glm/ext.hpp>

#include <vector>


struct point_light
{
    std::vector<glm::mat4> light_matrices;

    void set(glm::vec3 position)
    {
        float aspect = (float)sm_size_x/(float)sm_size_y;
        float near = 1.0f;
        float far = 25.0f;
        glm::mat4 proj = glm::perspective(glm::radians(90.0f), aspect, near, far);

        light_matrices.push_back(proj * 
                 glm::lookAt(position, position + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
        light_matrices.push_back(proj * 
                        glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
        light_matrices.push_back(proj * 
                        glm::lookAt(position, position + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        light_matrices.push_back(proj * 
                        glm::lookAt(position, position + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
        light_matrices.push_back(proj * 
                        glm::lookAt(position, position + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
        light_matrices.push_back(proj * 
                        glm::lookAt(position, position + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));
    }

    glm::mat4 light_matrix(int face)
    {
        return light_matrices[face];
    }

	// size of one face in texels
	int sm_size_x, sm_size_y;
};

#endif