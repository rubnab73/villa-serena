#pragma once

#include "Mesh.hpp"
#include "Texture.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>

class Model
{
public:
    std::string filePath;
    std::vector<Mesh> meshes;
    glm::vec3 minBounds{1e9f};
    glm::vec3 maxBounds{-1e9f};
    bool valid = false;
    std::shared_ptr<Texture> texture;

    Model() = default;
    explicit Model(const std::string &path);

    void draw() const;
    glm::vec3 getSize() const { return maxBounds - minBounds; }
    glm::vec3 getCenter() const { return (minBounds + maxBounds) * 0.5f; }

    bool loadOBJ(const std::string &path);
};

class ModelManager
{
public:
    static ModelManager &instance();

    std::shared_ptr<Model> getModel(const std::string &path);
    std::shared_ptr<Texture> getTexture(const std::string &path);
    std::shared_ptr<Model> createPlane(const std::string &id, float width, float depth,
                                       const glm::vec2 &uvMin = glm::vec2(0.33f, 0.27f),
                                       const glm::vec2 &uvMax = glm::vec2(0.35f, 0.29f),
                                       const std::string &texturePath = "assets/Textures/InteriorTexture_01.png");
    void preload(const std::vector<std::string> &paths);
    size_t getLoadedCount() const { return cache.size(); }

private:
    ModelManager() = default;
    std::unordered_map<std::string, std::shared_ptr<Model>> cache;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textureCache;
};

