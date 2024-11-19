#include "ModelLoader.h"

Renderable ModelLoader::Load(std::string model_filename, Material* material)
{
	tinygltf::Model model;
	if (!LoadModel(model, model_filename)) {
		return Renderable(Mesh(), material);
	}

	std::vector<Mesh> meshes;


	const tinygltf::Scene& scene = model.scenes[model.defaultScene];
	for (size_t i = 0; i < scene.nodes.size(); ++i) {
		assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
		ModelLoader::VisitModelNodes(meshes, model, model.nodes[scene.nodes[i]]);
	}

	return Renderable(meshes, material);
}

std::vector<Mesh> ModelLoader::LoadMesh(std::string model_filename)
{
	tinygltf::Model model;
	if (!LoadModel(model, model_filename)) {
		std::cout << "Error loading mesh: " << model_filename << std::endl;
		return std::vector<Mesh>();
	}

	std::vector<Mesh> meshes;


	const tinygltf::Scene& scene = model.scenes[model.defaultScene];
	for (size_t i = 0; i < scene.nodes.size(); ++i) {
		assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
		ModelLoader::VisitModelNodes(meshes, model, model.nodes[scene.nodes[i]]);
	}

	return meshes;
}

bool ModelLoader::LoadModel(tinygltf::Model& model, const std::string filename) {
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	std::string ext = GetFilePathExtension(filename);
	bool res;
	if (ext.compare("glb") == 0) {
		// assume binary glTF.
		res = loader.LoadBinaryFromFile(&model, &err, &warn, filename.c_str());
	}
	else {
		// assume ascii glTF.
		res = loader.LoadASCIIFromFile(&model, &err, &warn, filename.c_str());
	}

	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cout << "ERR: " << err << std::endl;
	}

	if (!res)
		std::cout << "Failed to load glTF: " << filename << std::endl;
	else
		std::cout << "Loaded glTF: " << filename << std::endl;

	return res;
}

void ModelLoader::VisitModelNodes(std::vector<Mesh>& meshes, tinygltf::Model& model, tinygltf::Node& node) {

	if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
		std::vector<double>& p = node.translation;
		std::vector<double>& r = node.rotation;
		std::vector<double>& s = node.scale;
		glm::mat4 transform = glm::mat4(1.0f);
		glm::mat4 rotation = glm::mat4(1.0f);
		glm::mat4 scale = glm::mat4(1.0f);
		if (!p.empty())
		{
			transform = glm::translate(transform, glm::vec3(p[0], p[1], p[2]));
		}

		if (!r.empty())
		{
			rotation = glm::rotate(rotation, (float)glm::radians(r[0]), glm::vec3(1.0f, 0, 0));
			rotation = glm::rotate(rotation, (float)glm::radians(r[1]), glm::vec3(0, 1.0f, 0));
			rotation = glm::rotate(rotation, (float)glm::radians(r[2]), glm::vec3(0, 0, 1.0f));
			transform *= rotation;

			//sta cosa non so neanche se funziona :)
		}

		if (!s.empty())
		{
			scale = glm::scale(scale, glm::vec3(s[0], s[1], s[2]));
			transform *= scale;
		}
		//std::cout << glm::to_string(transform) << std::endl;

		ModelLoader::BindMeshes(meshes, model, model.meshes[node.mesh], transform);
	}

	for (size_t i = 0; i < node.children.size(); i++) {
		assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
		ModelLoader::VisitModelNodes(meshes, model, model.nodes[node.children[i]]);
	}
}

void ModelLoader::BindMeshes(std::vector<Mesh>& meshes, tinygltf::Model& model, tinygltf::Mesh& mesh, glm::mat4 transform) {

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indicesVector;
	float currentIndAmount = 0;
	for (size_t i = 0; i < mesh.primitives.size(); ++i)
	{

		tinygltf::Primitive primitive = mesh.primitives[i];
		if (primitive.indices < 0) return;

		const tinygltf::Accessor& accessorIndices = model.accessors[primitive.indices];

		if (MESHLOAD_DEBUG_INFO) 
		{
			for (auto const& x : primitive.attributes)
			{
				std::cout << x.first  // string (key)
					<< ':'
					<< x.second // string's value 
					<< std::endl;
			}
		}

		const tinygltf::Accessor& accessor = model.accessors[primitive.attributes["POSITION"]];
		const tinygltf::Accessor& accessorNormal = model.accessors[primitive.attributes["NORMAL"]];
		const tinygltf::Accessor& accessorUv = model.accessors[primitive.attributes["TEXCOORD_0"]];
		const tinygltf::Accessor& accessorTangent = model.accessors[primitive.attributes["TANGENT"]];

		const tinygltf::BufferView& bufferViewIndices = model.bufferViews[accessorIndices.bufferView];
		const tinygltf::BufferView& bufferViewPosition = model.bufferViews[accessor.bufferView];
		const tinygltf::BufferView& bufferViewNormal = model.bufferViews[accessorNormal.bufferView];
		const tinygltf::BufferView& bufferViewUv = model.bufferViews[accessorUv.bufferView];
		const tinygltf::BufferView& bufferViewTangent = model.bufferViews[accessorTangent.bufferView];


		const tinygltf::Buffer& bufferIndices = model.buffers[bufferViewIndices.buffer];
		const tinygltf::Buffer& bufferPosition = model.buffers[bufferViewPosition.buffer];
		const tinygltf::Buffer& bufferNormal = model.buffers[bufferViewNormal.buffer];
		const tinygltf::Buffer& bufferUv = model.buffers[bufferViewUv.buffer];
		const tinygltf::Buffer& bufferTangent = model.buffers[bufferViewTangent.buffer];

		// bufferView byteoffset + accessor byteoffset tells you where the actual position data is within the buffer. From there
		// you should already know how the data needs to be interpreted.
		const unsigned int* indices = reinterpret_cast<const unsigned int*>(&bufferIndices.data[bufferViewIndices.byteOffset + accessorIndices.byteOffset]);
		const float* positions = reinterpret_cast<const float*>(&bufferPosition.data[bufferViewPosition.byteOffset + accessor.byteOffset]);
		const float* normal = reinterpret_cast<const float*>(&bufferNormal.data[bufferViewNormal.byteOffset + accessorNormal.byteOffset]);
		const float* uv = reinterpret_cast<const float*>(&bufferUv.data[bufferViewUv.byteOffset + accessorUv.byteOffset]);
		const float* tangent = reinterpret_cast<const float*>(&bufferUv.data[bufferViewTangent.byteOffset + accessorTangent.byteOffset]);

		// From here, you choose what you wish to do with this position data. In this case, we  will display it out.
		for (size_t i = 0; i < accessor.count; ++i) {
			// Positions are Vec3 components, so for each vec3 stride, offset for x, y, and z.
			/*
			std::cout << "(" << tangent[i * 3 + 0] << ", "// x
				<< tangent[i * 3 + 1] << ", " // y
				<< tangent[i * 3 + 2] << ")" // z
				<< "\n";
			*/

			vertices.push_back(
				{
					transform * glm::vec4(positions[i * 3 + 0] , positions[i * 3 + 1], positions[i * 3 + 2], 1.0f),
					glm::vec3(normal[i * 3 + 0], normal[i * 3 + 1], normal[i * 3 + 2]),
					glm::vec2(uv[i * 2 + 0], uv[i * 2 + 1]),
					glm::vec3(tangent[i * 3 + 0] , tangent[i * 3 + 1], tangent[i * 3 + 2])
				}
			);
		}

	}

	if (false)
	{
		std::cout << "fatto" << std::endl;
		for (int i = 0; i < vertices.size(); i += 3)
		{
			Vertex& v0 = vertices[i];
			Vertex& v1 = vertices[i + 1];
			Vertex& v2 = vertices[i + 2];

			glm::vec3 deltaPos1 = v1.Position - v0.Position;
			glm::vec3 deltaPos2 = v2.Position - v0.Position;

			glm::vec2 deltaUV1 = v1.UV - v0.UV;
			glm::vec2 deltaUV2 = v2.UV - v0.UV;

			float r = 1 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
			vertices[i].Tangent = tangent;
			vertices[i + 1].Tangent = tangent;
			vertices[i + 2].Tangent = tangent;
		}
	}

	for (size_t i = 0; i < mesh.primitives.size(); ++i)
	{

		tinygltf::Primitive primitive = mesh.primitives[i];

		const tinygltf::Accessor& accessorIndices = model.accessors[primitive.indices];
		const tinygltf::BufferView& bufferViewIndices = model.bufferViews[accessorIndices.bufferView];
		const tinygltf::Buffer& bufferIndices = model.buffers[bufferViewIndices.buffer];

		//Molto brutto ma non so come migliorarlo
		if (accessorIndices.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
		{
			const unsigned short* indices = reinterpret_cast<const unsigned short*>(&bufferIndices.data[bufferViewIndices.byteOffset + accessorIndices.byteOffset]);

			for (size_t i = 0; i < accessorIndices.count; i++)
			{
				indicesVector.push_back(indices[i]);
			}
		}
		else {
			const unsigned int* indices = reinterpret_cast<const unsigned int*>(&bufferIndices.data[bufferViewIndices.byteOffset + accessorIndices.byteOffset]);

			for (size_t i = 0; i < accessorIndices.count; i++)
			{
				indicesVector.push_back(indices[i]);
			}
		}

		currentIndAmount += accessorIndices.count;
	}
	meshes.push_back(Mesh(vertices, indicesVector));

}

std::string ModelLoader::GetFilePathExtension(const std::string& FileName) {
	if (FileName.find_last_of(".") != std::string::npos)
		return FileName.substr(FileName.find_last_of(".") + 1);
	return "";
}