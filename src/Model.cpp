#include "Model.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <unordered_map>

Model::Model(const std::string &path)
    : filePath(path)
{
    valid = loadOBJ(path);
}

void Model::draw() const
{
    for (const auto &mesh : meshes)
    {
        mesh.draw();
    }
}



bool Model::loadOBJ(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open OBJ file: " << path << "\n";
        return false;
    }

    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec2> temp_texcoords;
    std::vector<glm::vec3> temp_normals;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::unordered_map<std::string, unsigned int> vertexLookup;
    vertexLookup.reserve(4096);

    minBounds = glm::vec3(1e9f);
    maxBounds = glm::vec3(-1e9f);

    std::string mtlFile;
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v")
        {
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            temp_positions.push_back(pos);
            minBounds = glm::min(minBounds, pos);
            maxBounds = glm::max(maxBounds, pos);
        }
        else if (prefix == "vt")
        {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            temp_texcoords.push_back(uv);
        }
        else if (prefix == "vn")
        {
            glm::vec3 norm;
            iss >> norm.x >> norm.y >> norm.z;
            temp_normals.push_back(norm);
        }
        else if (prefix == "mtllib")
        {
            iss >> mtlFile;
        }
        else if (prefix == "f")
        {
            std::string token;
            std::vector<unsigned int> faceIndices;

            while (iss >> token)
            {
                auto it = vertexLookup.find(token);
                if (it != vertexLookup.end())
                {
                    faceIndices.push_back(it->second);
                }
                else
                {
                    int vIdx = 0, vtIdx = 0, vnIdx = 0;
                    size_t slash1 = token.find('/');
                    if (slash1 == std::string::npos)
                    {
                        vIdx = std::stoi(token);
                    }
                    else
                    {
                        vIdx = std::stoi(token.substr(0, slash1));
                        size_t slash2 = token.find('/', slash1 + 1);
                        if (slash2 == std::string::npos)
                        {
                            std::string vtStr = token.substr(slash1 + 1);
                            if (!vtStr.empty()) vtIdx = std::stoi(vtStr);
                        }
                        else
                        {
                            std::string vtStr = token.substr(slash1 + 1, slash2 - slash1 - 1);
                            if (!vtStr.empty()) vtIdx = std::stoi(vtStr);
                            std::string vnStr = token.substr(slash2 + 1);
                            if (!vnStr.empty()) vnIdx = std::stoi(vnStr);
                        }
                    }

                    Vertex vert{};
                    if (vIdx > 0 && vIdx <= static_cast<int>(temp_positions.size()))
                        vert.Position = temp_positions[vIdx - 1];
                    else if (vIdx < 0)
                        vert.Position = temp_positions[temp_positions.size() + vIdx];

                    if (vtIdx > 0 && vtIdx <= static_cast<int>(temp_texcoords.size()))
                        vert.TexCoords = temp_texcoords[vtIdx - 1];
                    else if (vtIdx < 0)
                        vert.TexCoords = temp_texcoords[temp_texcoords.size() + vtIdx];

                    if (vnIdx > 0 && vnIdx <= static_cast<int>(temp_normals.size()))
                        vert.Normal = temp_normals[vnIdx - 1];
                    else if (vnIdx < 0)
                        vert.Normal = temp_normals[temp_normals.size() + vnIdx];

                    unsigned int newIdx = static_cast<unsigned int>(vertices.size());
                    vertices.push_back(vert);
                    vertexLookup[token] = newIdx;
                    faceIndices.push_back(newIdx);
                }
            }

            for (size_t i = 1; i + 1 < faceIndices.size(); ++i)
            {
                indices.push_back(faceIndices[0]);
                indices.push_back(faceIndices[i]);
                indices.push_back(faceIndices[i + 1]);
            }
        }
    }

    if (vertices.empty() || indices.empty())
    {
        std::cerr << "Warning: OBJ file loaded with empty geometry: " << path << "\n";
        return false;
    }

    // Compute normals if not present
    if (temp_normals.empty())
    {
        for (size_t i = 0; i + 2 < indices.size(); i += 3)
        {
            glm::vec3 p0 = vertices[indices[i]].Position;
            glm::vec3 p1 = vertices[indices[i + 1]].Position;
            glm::vec3 p2 = vertices[indices[i + 2]].Position;
            glm::vec3 edge1 = p1 - p0;
            glm::vec3 edge2 = p2 - p0;
            glm::vec3 fn = glm::cross(edge1, edge2);
            if (glm::length(fn) > 1e-6f)
            {
                fn = glm::normalize(fn);
                vertices[indices[i]].Normal += fn;
                vertices[indices[i + 1]].Normal += fn;
                vertices[indices[i + 2]].Normal += fn;
            }
        }
        for (auto &v : vertices)
        {
            if (glm::length(v.Normal) > 1e-6f)
                v.Normal = glm::normalize(v.Normal);
            else
                v.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }

    meshes.emplace_back(vertices, indices);

    // Resolve texture
    std::string texturePath = "assets/Textures/InteriorTexture_01.png";
    if (path.find("InteriorTexture_02") != std::string::npos ||
        path.find("Painting") != std::string::npos)
    {
        texturePath = "assets/Textures/InteriorTexture_02.png";
    }

    if (!mtlFile.empty())
    {
        std::filesystem::path objP(path);
        std::filesystem::path mtlP = objP.parent_path() / mtlFile;
        std::ifstream mtlStream(mtlP);
        if (mtlStream.is_open())
        {
            std::string mline;
            while (std::getline(mtlStream, mline))
            {
                if (mline.find("map_Kd") != std::string::npos)
                {
                    std::istringstream miss(mline);
                    std::string mtag, texName;
                    miss >> mtag >> texName;
                    if (!texName.empty())
                    {
                        if (texName.find("InteriorTexture_02") != std::string::npos)
                            texturePath = "assets/Textures/InteriorTexture_02.png";
                        else
                            texturePath = "assets/Textures/InteriorTexture_01.png";
                    }
                }
            }
        }
    }

    texture = ModelManager::instance().getTexture(texturePath);
    return true;
}

ModelManager &ModelManager::instance()
{
    static ModelManager mgr;
    return mgr;
}

std::shared_ptr<Model> ModelManager::getModel(const std::string &path)
{
    auto it = cache.find(path);
    if (it != cache.end())
    {
        return it->second;
    }

    auto model = std::make_shared<Model>(path);
    cache[path] = model;
    return model;
}

std::shared_ptr<Texture> ModelManager::getTexture(const std::string &path)
{
    auto it = textureCache.find(path);
    if (it != textureCache.end())
    {
        return it->second;
    }

    auto tex = std::make_shared<Texture>(path);
    textureCache[path] = tex;
    return tex;
}

std::shared_ptr<Model> ModelManager::createPlane(const std::string &id, float width, float depth,
                                                 const glm::vec2 &uvMin, const glm::vec2 &uvMax,
                                                 const std::string &texturePath)
{
    auto it = cache.find(id);
    if (it != cache.end())
    {
        return it->second;
    }

    auto model = std::make_shared<Model>();
    model->filePath = id;
    float hw = width * 0.5f;
    float hd = depth * 0.5f;

    std::vector<Vertex> vertices = {
        {glm::vec3(-hw, 0.0f, -hd), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(uvMin.x, uvMin.y)},
        {glm::vec3( hw, 0.0f, -hd), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(uvMax.x, uvMin.y)},
        {glm::vec3( hw, 0.0f,  hd), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(uvMax.x, uvMax.y)},
        {glm::vec3(-hw, 0.0f,  hd), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(uvMin.x, uvMax.y)}
    };
    std::vector<unsigned int> indices = {0, 1, 2, 0, 2, 3};

    model->meshes.emplace_back(vertices, indices);
    model->minBounds = glm::vec3(-hw, -1.0f, -hd);
    model->maxBounds = glm::vec3( hw,  1.0f,  hd);
    model->valid = true;
    model->texture = getTexture(texturePath);

    cache[id] = model;
    return model;
}

void ModelManager::preload(const std::vector<std::string> &paths)
{
    for (const auto &p : paths)
    {
        getModel(p);
    }
}

