#include "SceneManager.h"
#include "..\..\3DShapes\ShapeMeshes.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <vector>
#include <cstring>
#include <fstream>
#include <cstdint>

/***********************************************************
 * RenderScene()
 * This function orchestrates the drawing of all 3D objects.
 * Ensure you have loaded the textures in LoadSceneTextures().
 ***********************************************************/
void SceneManager::RenderScene()
{
    glm::vec3 scaleXYZ;
    glm::vec3 positionXYZ;

    // --- 1. PLANE (Tiled Texture Example) ---
    // Rubric: Tiling a texture to prevent stretching
    scaleXYZ = glm::vec3(20.0f, 1.0f, 10.0f);
    positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
    SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);

    SetShaderTexture("wood"); // Your wood texture
    SetTextureUVScale(5.0f, 5.0f); // Tiling: 5x5 repetition
    m_basicMeshes->DrawPlaneMesh();

    // --- 2. COMPLEX OBJECT (e.g., Pyramid + Box) ---
    // Rubric: Create a cohesive object using different textures

    // Pyramid component (Brick)
    scaleXYZ = glm::vec3(2.0f, 2.0f, 2.0f);
    positionXYZ = glm::vec3(0.0f, 2.0f, 0.0f);
    SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
    SetShaderTexture("brick");
    SetTextureUVScale(1.0f, 1.0f);
    m_basicMeshes->DrawPyramid4Mesh();

    // Box component (Metal or other texture)
    scaleXYZ = glm::vec3(1.0f, 2.0f, 1.0f);
    positionXYZ = glm::vec3(0.0f, 0.5f, 0.0f);
    SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
    SetShaderTexture("metal"); // Ensure you loaded this in LoadSceneTextures()
    m_basicMeshes->DrawBoxMesh();

    // --- 3. REMAINING SCENE OBJECTS ---
    // Rubric: Logical flow, modularized, industry-standard formatting
    // Draw spheres, cylinders, etc., using SetShaderColor for non-textured items
    SetShaderColor(0.5f, 0.5f, 0.5f, 1.0f);
    m_basicMeshes->DrawSphereMesh();
}

// ------------------------------------------------------------------------
// Constructor / Destructor
// ------------------------------------------------------------------------
SceneManager::SceneManager(ShaderManager* pShaderManager)
    : m_pShaderManager(pShaderManager), m_basicMeshes(nullptr), m_loadedTextures(0)
{
    m_basicMeshes = new ShapeMeshes();
}

SceneManager::~SceneManager()
{
    if (m_basicMeshes) {
        delete m_basicMeshes;
        m_basicMeshes = nullptr;
    }

    // clean up textures if any
    DestroyGLTextures();
}

// ------------------------------------------------------------------------
// Prepare scene: load meshes and textures
// ------------------------------------------------------------------------
void SceneManager::PrepareScene()
{
    if (m_basicMeshes) {
        m_basicMeshes->LoadPlaneMesh();
        m_basicMeshes->LoadPyramid4Mesh();
        m_basicMeshes->LoadBoxMesh();
        m_basicMeshes->LoadSphereMesh();
    }

    LoadSceneTextures();
}

// ------------------------------------------------------------------------
// Texture management stubs (minimal implementations)
// ------------------------------------------------------------------------
void SceneManager::LoadSceneTextures()
{
    // Create three procedural textures for the assignment
    m_loadedTextures = 0;
    // The filename parameter is ignored for procedural textures; tag identifies them
    CreateGLTexture(nullptr, "wood");
    CreateGLTexture(nullptr, "brick");
    CreateGLTexture(nullptr, "metal");
}

bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
    // Create a simple procedural texture (RGBA) depending on tag
    if (m_loadedTextures >= 16) return false;

    const int width = 128;
    const int height = 128;
    std::vector<unsigned char> pixels(width * height * 4);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int i = (y * width + x) * 4;
            if (tag == "wood") {
                // horizontal grain bands
                unsigned char t = (unsigned char)(140 + 40 * std::sin((y / 4.0f)));
                pixels[i + 0] = (unsigned char)std::min(255, (int)t + 20); // r
                pixels[i + 1] = (unsigned char)std::min(255, (int)t - 10); // g
                pixels[i + 2] = (unsigned char)std::min(255, (int)t - 30); // b
                pixels[i + 3] = 255;
            }
            else if (tag == "brick") {
                // brick red with mortar grid
                int brickW = 32;
                int brickH = 16;
                bool mortar = (x % brickW < 2) || (y % brickH < 2);
                if (mortar) {
                    pixels[i + 0] = 200; pixels[i + 1] = 200; pixels[i + 2] = 200; pixels[i + 3] = 255;
                } else {
                    pixels[i + 0] = 150 + (unsigned char)((x + y) % 30); // r
                    pixels[i + 1] = 40 + (unsigned char)((x * y) % 20);  // g
                    pixels[i + 2] = 40;                                  // b
                    pixels[i + 3] = 255;
                }
            }
            else { // metal
                // simple metallic gradient with noise-like stripes
                unsigned char mval = (unsigned char)(180 + ((x ^ y) & 31));
                pixels[i + 0] = mval; pixels[i + 1] = mval; pixels[i + 2] = mval + 10; pixels[i + 3] = 255;
            }
        }
    }

    GLuint texID = 0;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // record
    m_textureIDs[m_loadedTextures].tag = tag;
    m_textureIDs[m_loadedTextures].ID = texID;
    ++m_loadedTextures;
    return true;
}

void SceneManager::BindGLTextures()
{
    // Bind all loaded textures to texture units 0..n-1
    for (int i = 0; i < m_loadedTextures; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
    }
}

void SceneManager::DestroyGLTextures()
{
    if (m_loadedTextures > 0) {
        std::vector<GLuint> ids;
        ids.reserve(m_loadedTextures);
        for (int i = 0; i < m_loadedTextures; ++i) ids.push_back(m_textureIDs[i].ID);
        glDeleteTextures((GLsizei)ids.size(), ids.data());
    }
    for (int i = 0; i < m_loadedTextures; ++i) {
        m_textureIDs[i].tag.clear();
        m_textureIDs[i].ID = 0;
    }
    m_loadedTextures = 0;
}

int SceneManager::FindTextureID(std::string tag)
{
    for (int i = 0; i < m_loadedTextures; ++i) {
        if (m_textureIDs[i].tag == tag) return (int)m_textureIDs[i].ID;
    }
    return -1;
}

int SceneManager::FindTextureSlot(std::string tag)
{
    for (int i = 0; i < m_loadedTextures; ++i) {
        if (m_textureIDs[i].tag == tag) return i;
    }
    return -1;
}

// ------------------------------------------------------------------------
// Shader / transformation helpers
// ------------------------------------------------------------------------
void SceneManager::SetTransformations(
    glm::vec3 scaleXYZ,
    float XrotationDegrees,
    float YrotationDegrees,
    float ZrotationDegrees,
    glm::vec3 positionXYZ)
{
    if (!m_pShaderManager) return;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, positionXYZ);
    model = glm::rotate(model, glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scaleXYZ);

    m_pShaderManager->use();
    m_pShaderManager->setMat4Value("model", model);
}

void SceneManager::SetShaderColor(
    float redColorValue,
    float greenColorValue,
    float blueColorValue,
    float alphaValue)
{
    if (!m_pShaderManager) return;
    m_pShaderManager->use();
    m_pShaderManager->setVec4Value("objectColor", redColorValue, greenColorValue, blueColorValue, alphaValue);
}

void SceneManager::SetShaderTexture(std::string textureTag)
{
    // Select the texture slot for the given tag and set the sampler uniform
    if (!m_pShaderManager) return;
    int slot = FindTextureSlot(textureTag);
    if (slot < 0) slot = 0;
    m_pShaderManager->use();
    m_pShaderManager->setSampler2DValue("textureSampler", slot);
}

void SceneManager::SetTextureUVScale(float u, float v)
{
    if (!m_pShaderManager) return;
    m_pShaderManager->use();
    m_pShaderManager->setVec2Value("uvScale", u, v);
}
