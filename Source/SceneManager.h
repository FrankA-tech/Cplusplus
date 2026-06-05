#pragma once

#include <string>
#include <glm/glm.hpp>
#include "..\..\Utilities\ShaderManager.h"

class ShaderManager;
class ShapeMeshes;

class SceneManager
{
public:
	// constructor / destructor
	SceneManager(ShaderManager* pShaderManager);
	~SceneManager();

	// scene setup and rendering
	void PrepareScene();
	void RenderScene();

	// loads textures from image files
	void LoadSceneTextures();

	// methods for managing OpenGL textures
	bool CreateGLTexture(const char* filename, std::string tag);
	void BindGLTextures();
	void DestroyGLTextures();
	int FindTextureID(std::string tag);
	int FindTextureSlot(std::string tag);

	// set the transformation values
	void SetTransformations(
		glm::vec3 scaleXYZ,
		float XrotationDegrees,
		float YrotationDegrees,
		float ZrotationDegrees,
		glm::vec3 positionXYZ);

	// set the color values into the shader
	void SetShaderColor(
		float redColorValue,
		float greenColorValue,
		float blueColorValue,
		float alphaValue);

	// set the texture data into the shader
	void SetShaderTexture(std::string textureTag);

	// set the texture UV scale into the shader
	void SetTextureUVScale(float u, float v);

private:
	ShaderManager* m_pShaderManager;
	ShapeMeshes* m_basicMeshes;

	struct TextureInfo {
		std::string tag;
		unsigned int ID;
	};

	TextureInfo m_textureIDs[16];
	int m_loadedTextures;
};
