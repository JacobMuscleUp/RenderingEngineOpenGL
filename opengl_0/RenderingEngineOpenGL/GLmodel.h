#ifndef CCKIT_GLMODEL_H
#define CCKIT_GLMODEL_H

#include <unordered_map>
#include <functional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "GLmesh.h"
#include "GLutils.h"

namespace cckit
{
	class GLmodel
	{
	public:
		GLmodel(std::function<void(std::vector<GLvertex>&, std::vector<GLuint>&, std::vector<GLtexture>&)> _config)
			: mMeshes(), mbImported(false) {
			std::vector<GLvertex> vertices;
			std::vector<GLuint> indices;
			std::vector<GLtexture> textures;
			_config(vertices, indices, textures);
			mMeshes.push_back(new GLmesh(vertices, indices, textures));
		}
		GLmodel(const std::string& _path) 
		: mPath(_path), mMeshes(), mbImported(true) {
			Load(_path);
		}
		~GLmodel() {
			if (!mbImported)
				for (auto pMesh : mMeshes)
					delete pMesh;
		}

		void render(const GLshader& _shader, std::function<void(const GLshader&)> _uniformConfig
			, const GLshader* _pShaderOutline = nullptr
			, std::function<void(const GLshader&)> _uniformConfigOutline = [](const GLshader&) {}
			, bool _bOutlined = false);

		static void load(const std::string& _path);
		static void unload();
	private:
		void Load(const std::string& _path);

		static void ProcessNode(aiNode* _pNode, const aiScene* _pScene, std::vector<GLmesh*>& _meshes);
		static GLmesh* ProcessMesh(aiMesh* _pMesh, const aiScene* _pScene);
		static std::vector<GLtexture> LoadMaterialTextures(aiMaterial* _mat, aiTextureType _textureType, const std::string& _typeName);
	public:
		std::vector<GLmesh*> mMeshes;
		std::string mPath;
	private:
		bool mbImported;

		static std::string mDirectory;
		static std::vector<GLtexture> mLoadedTextures;
		static std::unordered_map<size_t, std::vector<GLmesh*> > mMapHashedPath2LoadedMeshes;
		static std::hash<std::string> mStringHash;
	};
	std::string GLmodel::mDirectory;
	std::vector<GLtexture> GLmodel::mLoadedTextures;
	std::unordered_map<size_t, std::vector<GLmesh*> > GLmodel::mMapHashedPath2LoadedMeshes;
	std::hash<std::string> GLmodel::mStringHash;

	void GLmodel::render(const GLshader& _shader, std::function<void(const GLshader&)> _uniformConfig
		, const GLshader* _pShaderOutline, std::function<void(const GLshader&)> _uniformConfigOutline
		, bool _bOutlined) {
		glEnable(GL_CULL_FACE);

		if (_bOutlined) {
			glClear(GL_STENCIL_BUFFER_BIT);
			glEnable(GL_STENCIL_TEST);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
		}

		_shader.use();
		_uniformConfig(_shader);
		for (GLmesh* pMesh : mMeshes)
			pMesh->render(_shader);

		if (_bOutlined) {
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);

			_pShaderOutline->use();
			_uniformConfigOutline(*_pShaderOutline);
			for (GLmesh* pMesh : mMeshes)
				pMesh->render(*_pShaderOutline);

			glEnable(GL_DEPTH_TEST);
			glStencilMask(0xFF);
			glDisable(GL_STENCIL_TEST);
		}

		glDisable(GL_CULL_FACE);
	}

	void GLmodel::load(const std::string& _path) {
		mDirectory = _path.substr(0, _path.find_last_of('/'));
		size_t hashedPath = mStringHash(_path);

		if (mMapHashedPath2LoadedMeshes.count(hashedPath) == 0) {
			Assimp::Importer importer;
			const aiScene* pScene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
			if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
				std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << "\n";
				return;
			}
			std::vector<GLmesh*> meshes;
			ProcessNode(pScene->mRootNode, pScene, meshes);

			mMapHashedPath2LoadedMeshes[hashedPath] = meshes;
		}
	}

	void GLmodel::unload() {
		for (auto pairHash2Meshes : mMapHashedPath2LoadedMeshes) {
			for (auto pMesh : pairHash2Meshes.second)
				delete pMesh;
		}
		mMapHashedPath2LoadedMeshes.clear();
	}

	void GLmodel::Load(const std::string& _path) {
		mDirectory = _path.substr(0, _path.find_last_of('/'));
		size_t hashedPath = mStringHash(_path);

		if (mMapHashedPath2LoadedMeshes.count(hashedPath) == 0) {
			Assimp::Importer importer;
			const aiScene* pScene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
			if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
				std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << "\n";
				return;
			}
			ProcessNode(pScene->mRootNode, pScene, mMeshes);
			
			mMapHashedPath2LoadedMeshes[hashedPath] = mMeshes;
		}
		else
			mMeshes = mMapHashedPath2LoadedMeshes[hashedPath];
	}

	inline void GLmodel::ProcessNode(aiNode* _pNode, const aiScene* _pScene, std::vector<GLmesh*>& _meshes) {
		for (size_t i = 0; i < _pNode->mNumMeshes; ++i) {
			aiMesh* pMesh = _pScene->mMeshes[_pNode->mMeshes[i]];
			_meshes.push_back(ProcessMesh(pMesh, _pScene));
		}
		for (size_t i = 0; i < _pNode->mNumChildren; ++i) {
			ProcessNode(_pNode->mChildren[i], _pScene, _meshes);
		}
	}

	GLmesh* GLmodel::ProcessMesh(aiMesh* _pMesh, const aiScene* _pScene) {
		std::vector<GLvertex> vertices;
		std::vector<GLuint> indices;
		std::vector<GLtexture> textures;
		// vertices
		aiVector3D* aipTexCoords = _pMesh->mTextureCoords[0];
		for (size_t i = 0; i < _pMesh->mNumVertices; ++i) {
			aiVector3D aiVertex = _pMesh->mVertices[i]
				, aiNormal = _pMesh->mNormals[i];
			aiVector3D aiTangent, aiBitangent;
			if (aiVector3D* pTangents = _pMesh->mTangents)
				aiTangent = pTangents[i];
			if (aiVector3D* pBitangents = _pMesh->mBitangents)
				aiBitangent = pBitangents[i];

			GLvertex vertex(
				glm::vec3(aiVertex.x, aiVertex.y, aiVertex.z)
				, glm::vec3(aiNormal.x, aiNormal.y, aiNormal.z)
				, (aipTexCoords) ? glm::vec2(aipTexCoords[i].x, aipTexCoords[i].y) : glm::vec2(0, 0)
				, glm::vec3(aiTangent.x, aiTangent.y, aiTangent.z)
				, glm::vec3(aiBitangent.x, aiBitangent.y, aiBitangent.z)
			);

			vertices.push_back(vertex);
		}
		// indices
		for (size_t i = 0; i < _pMesh->mNumFaces; ++i) {
			const aiFace& face = _pMesh->mFaces[i];
			for (size_t j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
			}
		}
		// textures
		aiMaterial* pMaterial = _pScene->mMaterials[_pMesh->mMaterialIndex];
		std::vector<GLtexture> diffuseMap = LoadMaterialTextures(pMaterial, aiTextureType_DIFFUSE, DIFFUSE_TEXTURE_IN_SHADER);
		textures.insert(textures.end(), diffuseMap.begin(), diffuseMap.end());
		std::vector<GLtexture> specularMap = LoadMaterialTextures(pMaterial, aiTextureType_SPECULAR, SPECULAR_TEXTURE_IN_SHADER);
		textures.insert(textures.end(), specularMap.begin(), specularMap.end());
		std::vector<GLtexture> normalMap = LoadMaterialTextures(pMaterial, aiTextureType_HEIGHT, NORMAL_TEXTURE_IN_SHADER);
		textures.insert(textures.end(), normalMap.begin(), normalMap.end());
		std::vector<GLtexture> heightMap = LoadMaterialTextures(pMaterial, aiTextureType_AMBIENT, HEIGHT_TEXTURE_IN_SHADER);
		textures.insert(textures.end(), heightMap.begin(), heightMap.end());

		return new GLmesh(vertices, indices, textures);
	}

	std::vector<GLtexture> GLmodel::LoadMaterialTextures(aiMaterial* _mat, aiTextureType _textureType, const std::string& _typeName) {
		std::vector<GLtexture> textures;

		for (size_t i = 0; i < _mat->GetTextureCount(_textureType); ++i) {
			aiString aiTexturePath;
			_mat->GetTexture(_textureType, i, &aiTexturePath);

			bool bTextureAlreadyLoaded = false;
			for (const GLtexture& texture : mLoadedTextures) {
				if (strcmp(texture.mPath.data(), aiTexturePath.C_Str()) == 0) {
					textures.push_back(texture);
					bTextureAlreadyLoaded = true;
					break;
				}
			}
			if (!bTextureAlreadyLoaded) {
				GLtexture texture(
					load_texture(aiTexturePath.C_Str(), mDirectory)
					, _typeName
					, aiTexturePath.C_Str()
				);
				mLoadedTextures.push_back(texture);
				textures.push_back(texture);
			}
		}

		return textures;
	}
}

#endif // !CCKIT_GLMODEL_H
