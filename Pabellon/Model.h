#pragma once


namespace PabellonApp
{
	typedef unsigned int MeshIndex;

	typedef unsigned int MaterialIndex;

	typedef unsigned int TextureIndex;


	// 3D model loaded using the assimp library.
	// Can contain one or more meshes.
	class Model
	{
		typedef std::vector< std::unique_ptr<Mesh> > MeshCollection;

		typedef std::map<MaterialIndex, std::unique_ptr<MaterialInfo> > MaterialCollection;

		typedef std::map<MaterialIndex, std::unique_ptr<MaterialInfo> >::iterator MaterialItr;

		typedef std::map<MaterialIndex, std::unique_ptr<MaterialInfo> >::const_iterator MaterialItrConst;

		typedef std::pair<MaterialIndex, std::unique_ptr<MaterialInfo> > MaterialPair;

		typedef std::vector<aiTextureType> TextureTypes;

	public:
		Model(void);

		virtual ~Model();

		// Parameters: program  - GLSL program;
		//             filePath - path to the model file;
		//             spCamera - scene camera.
		bool initialize(GLuint program, const std::string& filePath, const std::unique_ptr<CommonLibOgl::Camera>& spCamera);

		void render() const;

		void updateMatrices();

		void translateX(GLfloat diff);
		void translateY(GLfloat diff);
		void translateZ(GLfloat diff);

		void rotateX(GLfloat angleDegrees);
		void rotateY(GLfloat angleDegrees);
		void rotateZ(GLfloat angleDegrees);

		void scale(GLfloat amount);

	private:
		bool initializeMatrices();

		// Recursively load scene meshes.
		// Parameters: pScene - assimp scene object;
		//             pNode  - assimp node object to extract the data from;
		bool loadMeshes(const aiScene* pScene, const aiNode* pNode);

		// Load materials for all meshes in the scene.
		// Parameters: pScene - assimp scene object.
		bool loadMaterials(const aiScene* pScene);

	private:
		// TODO: move to the Mesh class?
		// GLSL program (cached).
		GLuint m_program;

		// Scene camera.
		CommonLibOgl::Camera const * m_pCamera;
		//std::unique_ptr<CommonLibOgl::CameraSimple>& m_spCamera;

		// File path to the model file.
		std::string m_modelFilePath;

		// Directory of the model file.
		std::string m_modelDirectoryPath;

		MeshCollection m_meshes;

		// Types of texture files to load (according to the assimp library).
		static const TextureTypes TexTypes;

		// Material data. Stored here because several meshes in a scene can share a material.
		// Key: material index in the assimp scene; value: material data.
		MaterialCollection m_materials;

#if 0
		// View matrix uniform.
		GLint m_unView;
#endif

		// Model-View matrix uniform.
		GLint m_unModelView;

#if 0
		// Projection matrix uniform.
		GLint m_unProjection;
#endif

		// Model-View-Projection matrix uniform.
		GLint m_unMvp;

		// Normal matrix uniform.
		GLint m_unNormal;

		// Model matrix.
		glm::mat4 m_model;

		// View matrix.
		glm::mat4 m_view;

		// Projection matrix.
		glm::mat4 m_projection;

		// Model-View-Projection (MVP) matrix.
		glm::mat4 m_mvp;

		// Translation vector.
		glm::vec3 m_translation;

		// Rotation vector (in degrees).
		glm::vec3 m_rotationDegrees;

		// Scale factor.
		GLfloat m_scaleFactor;
	};
}
