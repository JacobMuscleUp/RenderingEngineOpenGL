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
		GLmodel(std::function<void(std::vector<GLvertex>&, std::vector<GLuint>&, std::vector<GLtexture>&)> _config
			= [](std::vector<GLvertex>&, std::vector<GLuint>&, std::vector<GLtexture>&) {})
			: mMeshes(), mLoadedTextures(), mbImported(false) {
			std::vector<GLvertex> vertices;
			std::vector<GLuint> indices;
			std::vector<GLtexture> textures;
			_config(vertices, indices, textures);
			mMeshes.push_back(new GLmesh(vertices, indices, textures));
		}
		GLmodel(const std::string& _path) 
		: mPath(_path), mMeshes(), mLoadedTextures(), mbImported(true) {
			Load(_path);
		}
		~GLmodel();

		void render(const GLshader& _shader, std::function<void(const GLshader&)> _uniformConfig
			, const GLshader* _pShaderOutline = nullptr
			, std::function<void(const GLshader&)> _uniformConfigOutline = [](const GLshader&) {}
			, bool _bOutlined = false);
	private:
		void Load(const std::string& _path);
		void ProcessNode(aiNode* _pNode, const aiScene* _pScene);
		GLmesh* ProcessMesh(aiMesh* _pMesh, const aiScene* _pScene);
		std::vector<GLtexture> LoadMaterialTextures(aiMaterial* _mat, aiTextureType _textureType, const std::string& _typeName);
	public:
		std::vector<GLmesh*> mMeshes;
		std::string mPath;
		std::string mDirectory;
	private:
		std::vector<GLtexture> mLoadedTextures;
		bool mbImported;

		static std::unordered_map<size_t, std::vector<GLmesh*> > mPath2LoadedMeshesMap;
		static std::hash<std::string> mStringHash;
	};
	std::unordered_map<size_t, std::vector<GLmesh*> > GLmodel::mPath2LoadedMeshesMap;
	std::hash<std::string> GLmodel::mStringHash;

	GLmodel::~GLmodel() {
		if (mbImported) {
			size_t pathHash = mStringHash(mPath);
			if (mPath2LoadedMeshesMap.count(pathHash) != 0) {
				for (auto pMesh : mPath2LoadedMeshesMap[pathHash])
					delete pMesh;
				mPath2LoadedMeshesMap.erase(pathHash);
			}
		}
		else {
			for (auto pMesh : mMeshes)
				delete pMesh;
		}
	}

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

	void GLmodel::Load(const std::string& _path) {
		mDirectory = _path.substr(0, _path.find_last_of('/'));

		size_t pathHash = mStringHash(_path);
		if (mPath2LoadedMeshesMap.count(pathHash) == 0) {
			Assimp::Importer importer;
			const aiScene* pScene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
			if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
				std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << "\n";
				return;
			}
			ProcessNode(pScene->mRootNode, pScene);

			mPath2LoadedMeshesMap[pathHash] = mMeshes;
		}
		else
			mMeshes = mPath2LoadedMeshesMap[pathHash];

		/*Assimp::Importer importer;
		const aiScene* pScene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
		if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << "\n";
		return;
		}
		mDirectory = _path.substr(0, _path.find_last_of('/'));

		ProcessNode(pScene->mRootNode, pScene);*/
	}

	inline void GLmodel::ProcessNode(aiNode* _pNode, const aiScene* _pScene) {
		for (size_t i = 0; i < _pNode->mNumMeshes; ++i) {
			aiMesh* pMesh = _pScene->mMeshes[_pNode->mMeshes[i]];
			mMeshes.push_back(ProcessMesh(pMesh, _pScene));
		}
		for (size_t i = 0; i < _pNode->mNumChildren; ++i) {
			ProcessNode(_pNode->mChildren[i], _pScene);
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

			GLvertex vertex(
				glm::vec3(aiVertex.x, aiVertex.y, aiVertex.z)
				, glm::vec3(aiNormal.x, aiNormal.y, aiNormal.z)
				, (aipTexCoords) ? glm::vec2(aipTexCoords[i].x, aipTexCoords[i].y) : glm::vec2(0, 0)
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
					TextureFromFile(aiTexturePath.C_Str(), mDirectory)
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
