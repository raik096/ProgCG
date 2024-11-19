#pragma once

#include "Renderable.h"

#include "../vendor/tiny_gltf.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#define MESHLOAD_DEBUG_INFO false

namespace ModelLoader
{
	std::string GetFilePathExtension(const std::string& FileName);


	void BindMeshes(std::vector<Mesh>& meshes, tinygltf::Model& model, tinygltf::Mesh& mesh, glm::mat4 transform);


	void VisitModelNodes(std::vector<Mesh>& meshes, tinygltf::Model& model, tinygltf::Node& node);

	bool LoadModel(tinygltf::Model& model, const std::string filename);

	std::vector<Mesh> LoadMesh(std::string model_filename);

	Renderable Load(std::string model_filename, Material* material);
}