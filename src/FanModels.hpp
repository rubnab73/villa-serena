#pragma once

#include "Model.hpp"
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace FanModels
{

inline void addBox(std::vector<Vertex> &verts, std::vector<unsigned int> &indices,
                   const glm::vec3 &bmin, const glm::vec3 &bmax,
                   const glm::vec2 &uvMin = glm::vec2(0.33f, 0.27f),
                   const glm::vec2 &uvMax = glm::vec2(0.35f, 0.29f))
{
    unsigned int base = static_cast<unsigned int>(verts.size());

    // 6 faces: +Y, -Y, +Z, -Z, +X, -X
    struct FaceDef
    {
        glm::vec3 n;
        glm::vec3 c[4];
    };

    FaceDef faces[6] = {
        // Top (+Y)
        {glm::vec3(0, 1, 0), {glm::vec3(bmin.x, bmax.y, bmin.z), glm::vec3(bmax.x, bmax.y, bmin.z), glm::vec3(bmax.x, bmax.y, bmax.z), glm::vec3(bmin.x, bmax.y, bmax.z)}},
        // Bottom (-Y)
        {glm::vec3(0, -1, 0), {glm::vec3(bmin.x, bmin.y, bmax.z), glm::vec3(bmax.x, bmin.y, bmax.z), glm::vec3(bmax.x, bmin.y, bmin.z), glm::vec3(bmin.x, bmin.y, bmin.z)}},
        // Front (+Z)
        {glm::vec3(0, 0, 1), {glm::vec3(bmin.x, bmin.y, bmax.z), glm::vec3(bmax.x, bmin.y, bmax.z), glm::vec3(bmax.x, bmax.y, bmax.z), glm::vec3(bmin.x, bmax.y, bmax.z)}},
        // Back (-Z)
        {glm::vec3(0, 0, -1), {glm::vec3(bmax.x, bmin.y, bmin.z), glm::vec3(bmin.x, bmin.y, bmin.z), glm::vec3(bmin.x, bmax.y, bmin.z), glm::vec3(bmax.x, bmax.y, bmin.z)}},
        // Right (+X)
        {glm::vec3(1, 0, 0), {glm::vec3(bmax.x, bmin.y, bmax.z), glm::vec3(bmax.x, bmin.y, bmin.z), glm::vec3(bmax.x, bmax.y, bmin.z), glm::vec3(bmax.x, bmax.y, bmax.z)}},
        // Left (-X)
        {glm::vec3(-1, 0, 0), {glm::vec3(bmin.x, bmin.y, bmin.z), glm::vec3(bmin.x, bmin.y, bmax.z), glm::vec3(bmin.x, bmax.y, bmax.z), glm::vec3(bmin.x, bmax.y, bmin.z)}}
    };

    for (int f = 0; f < 6; ++f)
    {
        unsigned int fbase = base + f * 4;
        verts.push_back({faces[f].c[0], faces[f].n, glm::vec2(uvMin.x, uvMin.y)});
        verts.push_back({faces[f].c[1], faces[f].n, glm::vec2(uvMax.x, uvMin.y)});
        verts.push_back({faces[f].c[2], faces[f].n, glm::vec2(uvMax.x, uvMax.y)});
        verts.push_back({faces[f].c[3], faces[f].n, glm::vec2(uvMin.x, uvMax.y)});

        indices.push_back(fbase);
        indices.push_back(fbase + 1);
        indices.push_back(fbase + 2);
        indices.push_back(fbase);
        indices.push_back(fbase + 2);
        indices.push_back(fbase + 3);
    }
}

inline void addCylinderY(std::vector<Vertex> &verts, std::vector<unsigned int> &indices,
                        float yBottom, float yTop, float rBottom, float rTop, int segments = 16,
                        const glm::vec3 &offset = glm::vec3(0.0f))
{
    float pi2 = glm::two_pi<float>();
    unsigned int base = static_cast<unsigned int>(verts.size());

    for (int i = 0; i <= segments; ++i)
    {
        float theta = (static_cast<float>(i) / segments) * pi2;
        float cosT = std::cos(theta);
        float sinT = std::sin(theta);
        float u = static_cast<float>(i) / segments;

        glm::vec3 n(cosT, 0.0f, sinT);
        glm::vec3 pBot(offset.x + rBottom * cosT, yBottom + offset.y, offset.z + rBottom * sinT);
        glm::vec3 pTop(offset.x + rTop * cosT, yTop + offset.y, offset.z + rTop * sinT);

        verts.push_back({pBot, n, glm::vec2(u * 0.05f + 0.33f, 0.27f)});
        verts.push_back({pTop, n, glm::vec2(u * 0.05f + 0.33f, 0.29f)});
    }

    for (int i = 0; i < segments; ++i)
    {
        unsigned int i0 = base + i * 2;
        unsigned int i1 = base + i * 2 + 1;
        unsigned int i2 = base + (i + 1) * 2;
        unsigned int i3 = base + (i + 1) * 2 + 1;

        indices.push_back(i0);
        indices.push_back(i1);
        indices.push_back(i3);
        indices.push_back(i0);
        indices.push_back(i3);
        indices.push_back(i2);
    }

    // Top cap
    unsigned int topCenter = static_cast<unsigned int>(verts.size());
    verts.push_back({glm::vec3(offset.x, yTop + offset.y, offset.z), glm::vec3(0, 1, 0), glm::vec2(0.34f, 0.28f)});
    for (int i = 0; i < segments; ++i)
    {
        unsigned int c0 = base + i * 2 + 1;
        unsigned int c1 = base + (i + 1) * 2 + 1;
        indices.push_back(topCenter);
        indices.push_back(c0);
        indices.push_back(c1);
    }

    // Bottom cap
    unsigned int botCenter = static_cast<unsigned int>(verts.size());
    verts.push_back({glm::vec3(offset.x, yBottom + offset.y, offset.z), glm::vec3(0, -1, 0), glm::vec2(0.34f, 0.28f)});
    for (int i = 0; i < segments; ++i)
    {
        unsigned int c0 = base + i * 2;
        unsigned int c1 = base + (i + 1) * 2;
        indices.push_back(botCenter);
        indices.push_back(c1);
        indices.push_back(c0);
    }
}

inline void addCylinderZ(std::vector<Vertex> &verts, std::vector<unsigned int> &indices,
                        float zBack, float zFront, float rBack, float rFront, int segments = 16,
                        const glm::vec3 &offset = glm::vec3(0.0f))
{
    float pi2 = glm::two_pi<float>();
    unsigned int base = static_cast<unsigned int>(verts.size());

    for (int i = 0; i <= segments; ++i)
    {
        float theta = (static_cast<float>(i) / segments) * pi2;
        float cosT = std::cos(theta);
        float sinT = std::sin(theta);
        float u = static_cast<float>(i) / segments;

        glm::vec3 n(cosT, sinT, 0.0f);
        glm::vec3 pBack(offset.x + rBack * cosT, offset.y + rBack * sinT, offset.z + zBack);
        glm::vec3 pFront(offset.x + rFront * cosT, offset.y + rFront * sinT, offset.z + zFront);

        verts.push_back({pBack, n, glm::vec2(u * 0.05f + 0.33f, 0.27f)});
        verts.push_back({pFront, n, glm::vec2(u * 0.05f + 0.33f, 0.29f)});
    }

    for (int i = 0; i < segments; ++i)
    {
        unsigned int i0 = base + i * 2;
        unsigned int i1 = base + i * 2 + 1;
        unsigned int i2 = base + (i + 1) * 2;
        unsigned int i3 = base + (i + 1) * 2 + 1;

        indices.push_back(i0);
        indices.push_back(i3);
        indices.push_back(i1);
        indices.push_back(i0);
        indices.push_back(i2);
        indices.push_back(i3);
    }

    // Front cap (+Z)
    unsigned int frontCenter = static_cast<unsigned int>(verts.size());
    verts.push_back({glm::vec3(offset.x, offset.y, offset.z + zFront), glm::vec3(0, 0, 1), glm::vec2(0.34f, 0.28f)});
    for (int i = 0; i < segments; ++i)
    {
        unsigned int c0 = base + i * 2 + 1;
        unsigned int c1 = base + (i + 1) * 2 + 1;
        indices.push_back(frontCenter);
        indices.push_back(c0);
        indices.push_back(c1);
    }

    // Back cap (-Z)
    unsigned int backCenter = static_cast<unsigned int>(verts.size());
    verts.push_back({glm::vec3(offset.x, offset.y, offset.z + zBack), glm::vec3(0, 0, -1), glm::vec2(0.34f, 0.28f)});
    for (int i = 0; i < segments; ++i)
    {
        unsigned int c0 = base + i * 2;
        unsigned int c1 = base + (i + 1) * 2;
        indices.push_back(backCenter);
        indices.push_back(c1);
        indices.push_back(c0);
    }
}

inline void addTorusZ(std::vector<Vertex> &verts, std::vector<unsigned int> &indices,
                     float ringRadius, float tubeRadius, int ringSegs = 24, int tubeSegs = 8,
                     const glm::vec3 &offset = glm::vec3(0.0f))
{
    float pi2 = glm::two_pi<float>();
    unsigned int base = static_cast<unsigned int>(verts.size());

    for (int i = 0; i <= ringSegs; ++i)
    {
        float phi = (static_cast<float>(i) / ringSegs) * pi2;
        float cosPhi = std::cos(phi);
        float sinPhi = std::sin(phi);

        for (int j = 0; j <= tubeSegs; ++j)
        {
            float theta = (static_cast<float>(j) / tubeSegs) * pi2;
            float cosTheta = std::cos(theta);
            float sinTheta = std::sin(theta);

            float r = ringRadius + tubeRadius * cosTheta;
            glm::vec3 pos(r * cosPhi + offset.x, r * sinPhi + offset.y, tubeRadius * sinTheta + offset.z);
            glm::vec3 norm(cosTheta * cosPhi, cosTheta * sinPhi, sinTheta);

            verts.push_back({pos, norm, glm::vec2(0.34f, 0.28f)});
        }
    }

    for (int i = 0; i < ringSegs; ++i)
    {
        for (int j = 0; j < tubeSegs; ++j)
        {
            unsigned int i0 = base + i * (tubeSegs + 1) + j;
            unsigned int i1 = base + (i + 1) * (tubeSegs + 1) + j;
            unsigned int i2 = base + (i + 1) * (tubeSegs + 1) + (j + 1);
            unsigned int i3 = base + i * (tubeSegs + 1) + (j + 1);

            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i3);
        }
    }
}

// -------------------------------------------------------------
// 1. Ceiling Fan Base Model
// -------------------------------------------------------------
inline std::shared_ptr<Model> createCeilingFanBase()
{
    auto model = std::make_shared<Model>();
    model->filePath = "procedural_ceiling_fan_base";

    std::vector<Vertex> verts;
    std::vector<unsigned int> inds;

    // Canopy against ceiling (Y = 0)
    addCylinderY(verts, inds, -3.0f, 0.0f, 6.0f, 4.5f, 16);
    // Downrod down to motor housing
    addCylinderY(verts, inds, -22.0f, -3.0f, 1.4f, 1.4f, 12);
    // Upper motor housing cone
    addCylinderY(verts, inds, -25.0f, -22.0f, 11.5f, 4.0f, 20);
    // Central motor cylinder
    addCylinderY(verts, inds, -30.0f, -25.0f, 11.5f, 11.5f, 20);
    // Lower bowl / frosted light housing
    addCylinderY(verts, inds, -34.0f, -30.0f, 4.5f, 10.0f, 16);

    model->meshes.emplace_back(verts, inds);
    model->minBounds = glm::vec3(-12.0f, -35.0f, -12.0f);
    model->maxBounds = glm::vec3(12.0f, 0.0f, 12.0f);
    model->valid = true;
    model->texture = ModelManager::instance().getTexture("assets/Textures/InteriorTexture_01.png");
    return model;
}

// -------------------------------------------------------------
// 2. Ceiling Fan Blades Model (Rotor & 4 pitched blades)
// -------------------------------------------------------------
inline std::shared_ptr<Model> createCeilingFanBlades()
{
    auto model = std::make_shared<Model>();
    model->filePath = "procedural_ceiling_fan_blades";

    std::vector<Vertex> verts;
    std::vector<unsigned int> inds;

    // Center rotor ring at Y = -27.5
    addCylinderY(verts, inds, -28.5f, -26.5f, 11.8f, 11.8f, 20);

    // 4 Pitched Blades radiating outwards
    for (int b = 0; b < 4; ++b)
    {
        float angle = glm::radians(b * 90.0f);
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);

        // Blade extends from radius 11 to 52, width 7.5
        glm::vec3 rDir(cosA, 0.0f, sinA);
        glm::vec3 wDir(-sinA, 0.0f, cosA);

        float rStart = 11.0f;
        float rEnd = 50.0f;
        float halfW = 3.6f;
        float pitchAngle = glm::radians(10.0f);
        float pitchH = std::sin(pitchAngle) * halfW;

        float yBase = -27.5f;

        // 4 corners of blade
        glm::vec3 c0 = rDir * rStart - wDir * halfW + glm::vec3(0, yBase - pitchH, 0);
        glm::vec3 c1 = rDir * rStart + wDir * halfW + glm::vec3(0, yBase + pitchH, 0);
        glm::vec3 c2 = rDir * rEnd   + wDir * (halfW * 1.15f) + glm::vec3(0, yBase + pitchH, 0);
        glm::vec3 c3 = rDir * rEnd   - wDir * (halfW * 1.15f) + glm::vec3(0, yBase - pitchH, 0);

        glm::vec3 n = glm::normalize(glm::cross(c1 - c0, c3 - c0));

        unsigned int base = static_cast<unsigned int>(verts.size());
        // Top surface
        verts.push_back({c0, n, glm::vec2(0.33f, 0.27f)});
        verts.push_back({c1, n, glm::vec2(0.35f, 0.27f)});
        verts.push_back({c2, n, glm::vec2(0.35f, 0.29f)});
        verts.push_back({c3, n, glm::vec2(0.33f, 0.29f)});

        inds.push_back(base);
        inds.push_back(base + 1);
        inds.push_back(base + 2);
        inds.push_back(base);
        inds.push_back(base + 2);
        inds.push_back(base + 3);

        // Bottom surface
        verts.push_back({c0, -n, glm::vec2(0.33f, 0.27f)});
        verts.push_back({c3, -n, glm::vec2(0.33f, 0.29f)});
        verts.push_back({c2, -n, glm::vec2(0.35f, 0.29f)});
        verts.push_back({c1, -n, glm::vec2(0.35f, 0.27f)});

        inds.push_back(base + 4);
        inds.push_back(base + 5);
        inds.push_back(base + 6);
        inds.push_back(base + 4);
        inds.push_back(base + 6);
        inds.push_back(base + 7);
    }

    model->meshes.emplace_back(verts, inds);
    model->minBounds = glm::vec3(-52.0f, -30.0f, -52.0f);
    model->maxBounds = glm::vec3(52.0f, -25.0f, 52.0f);
    model->valid = true;
    model->texture = ModelManager::instance().getTexture("assets/Textures/InteriorTexture_01.png");
    return model;
}

// -------------------------------------------------------------
// 3. Standing Fan Pedestal Base & Pole
// -------------------------------------------------------------
inline std::shared_ptr<Model> createStandFanPedestal()
{
    auto model = std::make_shared<Model>();
    model->filePath = "procedural_stand_fan_pedestal";

    std::vector<Vertex> verts;
    std::vector<unsigned int> inds;

    // Substantial weighted circular base plate (multi-tier weighted disc stand)
    // 1. Outer rubber edge rim / floor footing
    addCylinderY(verts, inds, 0.0f, 2.5f, 26.0f, 25.5f, 32);
    // 2. Bevelled circular weighted disc plate
    addCylinderY(verts, inds, 2.5f, 5.5f, 25.5f, 22.0f, 32);
    // 3. Concentric stepped circular accent ring
    addCylinderY(verts, inds, 5.5f, 8.0f, 22.0f, 15.0f, 28);
    // 4. Center raised circular dome
    addCylinderY(verts, inds, 8.0f, 11.5f, 15.0f, 8.5f, 24);
    // 5. Conical pole socket mounting collar boss
    addCylinderY(verts, inds, 11.5f, 15.0f, 8.5f, 4.5f, 20);
    // 6. Base locking collar ring
    addCylinderY(verts, inds, 15.0f, 18.0f, 4.5f, 3.0f, 16);
    // 7. Lower telescopic pole tube
    addCylinderY(verts, inds, 18.0f, 58.0f, 2.4f, 2.4f, 16);
    // 8. Height adjustment collar knob
    addCylinderY(verts, inds, 58.0f, 63.0f, 3.6f, 3.6f, 16);
    // 9. Upper chrome extension tube
    addCylinderY(verts, inds, 63.0f, 96.0f, 1.8f, 1.8f, 16);
    // 10. Neck bracket pivot mount
    addBox(verts, inds, glm::vec3(-2.8f, 96.0f, -4.0f), glm::vec3(2.8f, 102.0f, 4.0f));

    model->meshes.emplace_back(verts, inds);
    model->minBounds = glm::vec3(-26.0f, 0.0f, -26.0f);
    model->maxBounds = glm::vec3(26.0f, 102.0f, 26.0f);
    model->valid = true;
    model->texture = ModelManager::instance().getTexture("assets/Textures/InteriorTexture_01.png");
    return model;
}

// -------------------------------------------------------------
// 4. Standing Fan Head (Motor Housing & Wire Cage)
// Center of pivot is at (0, 100, 0)
// -------------------------------------------------------------
inline std::shared_ptr<Model> createStandFanHead()
{
    auto model = std::make_shared<Model>();
    model->filePath = "procedural_stand_fan_head";

    std::vector<Vertex> verts;
    std::vector<unsigned int> inds;

    // Motor casing (behind cage, Z = -1.0 to -14.0)
    addCylinderZ(verts, inds, -14.0f, -1.0f, 4.2f, 5.2f, 16, glm::vec3(0, 100.0f, 0));
    // Oscillation push/pull pin on top of motor
    addCylinderY(verts, inds, 104.5f, 107.5f, 0.8f, 0.8f, 8, glm::vec3(0, 0, -8.0f));

    // Outer protective wire cage rim
    addTorusZ(verts, inds, 16.0f, 0.9f, 24, 8, glm::vec3(0, 100.0f, 0));
    addTorusZ(verts, inds, 14.5f, 0.7f, 20, 6, glm::vec3(0, 100.0f, 2.0f));

    // Center emblem cap on front of cage
    addCylinderZ(verts, inds, 2.0f, 3.2f, 3.2f, 2.8f, 16, glm::vec3(0, 100.0f, 0));

    // Radial wire spokes for cage guard (8 spokes)
    for (int s = 0; s < 8; ++s)
    {
        float angle = glm::radians(s * 45.0f);
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);

        glm::vec3 p0(cosA * 3.0f, 100.0f + sinA * 3.0f, 2.6f);
        glm::vec3 p1(cosA * 16.0f, 100.0f + sinA * 16.0f, 0.0f);

        glm::vec3 dir = p1 - p0;
        glm::vec3 perp(-sinA * 0.4f, cosA * 0.4f, 0.0f);

        unsigned int base = static_cast<unsigned int>(verts.size());
        verts.push_back({p0 - perp, glm::vec3(0, 0, 1), glm::vec2(0.34f, 0.28f)});
        verts.push_back({p0 + perp, glm::vec3(0, 0, 1), glm::vec2(0.34f, 0.28f)});
        verts.push_back({p1 + perp, glm::vec3(0, 0, 1), glm::vec2(0.34f, 0.28f)});
        verts.push_back({p1 - perp, glm::vec3(0, 0, 1), glm::vec2(0.34f, 0.28f)});

        inds.push_back(base);
        inds.push_back(base + 1);
        inds.push_back(base + 2);
        inds.push_back(base);
        inds.push_back(base + 2);
        inds.push_back(base + 3);
    }

    model->meshes.emplace_back(verts, inds);
    model->minBounds = glm::vec3(-17.0f, 83.0f, -15.0f);
    model->maxBounds = glm::vec3(17.0f, 117.0f, 4.0f);
    model->valid = true;
    model->texture = ModelManager::instance().getTexture("assets/Textures/InteriorTexture_01.png");
    return model;
}

// -------------------------------------------------------------
// 5. Standing Fan Rotor Blades (3 aerodynamic blades)
// Rotor center at (0, 100, 1.0)
// -------------------------------------------------------------
inline std::shared_ptr<Model> createStandFanBlades()
{
    auto model = std::make_shared<Model>();
    model->filePath = "procedural_stand_fan_blades";

    std::vector<Vertex> verts;
    std::vector<unsigned int> inds;

    // Spinner nosecone hub
    addCylinderZ(verts, inds, 0.0f, 2.4f, 2.8f, 1.2f, 16, glm::vec3(0, 100.0f, 1.0f));

    // 3 Aerodynamic curved blades
    for (int b = 0; b < 3; ++b)
    {
        float angle = glm::radians(b * 120.0f);
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);

        glm::vec3 rDir(cosA, sinA, 0.0f);
        glm::vec3 wDir(-sinA, cosA, 0.0f);

        float r0 = 2.6f;
        float r1 = 14.2f;
        float halfW = 2.4f;

        float zTilt = 0.5f;

        glm::vec3 center(0.0f, 100.0f, 1.0f);
        glm::vec3 c0 = center + rDir * r0 - wDir * (halfW * 0.7f) - glm::vec3(0, 0, zTilt);
        glm::vec3 c1 = center + rDir * r0 + wDir * (halfW * 0.7f) + glm::vec3(0, 0, zTilt);
        glm::vec3 c2 = center + rDir * r1 + wDir * (halfW * 1.3f) + glm::vec3(0, 0, zTilt);
        glm::vec3 c3 = center + rDir * r1 - wDir * (halfW * 1.3f) - glm::vec3(0, 0, zTilt);

        glm::vec3 n = glm::normalize(glm::cross(c1 - c0, c3 - c0));

        unsigned int base = static_cast<unsigned int>(verts.size());
        // Front face
        verts.push_back({c0, n, glm::vec2(0.33f, 0.27f)});
        verts.push_back({c1, n, glm::vec2(0.35f, 0.27f)});
        verts.push_back({c2, n, glm::vec2(0.35f, 0.29f)});
        verts.push_back({c3, n, glm::vec2(0.33f, 0.29f)});

        inds.push_back(base);
        inds.push_back(base + 1);
        inds.push_back(base + 2);
        inds.push_back(base);
        inds.push_back(base + 2);
        inds.push_back(base + 3);

        // Back face
        verts.push_back({c0, -n, glm::vec2(0.33f, 0.27f)});
        verts.push_back({c3, -n, glm::vec2(0.33f, 0.29f)});
        verts.push_back({c2, -n, glm::vec2(0.35f, 0.29f)});
        verts.push_back({c1, -n, glm::vec2(0.35f, 0.27f)});

        inds.push_back(base + 4);
        inds.push_back(base + 5);
        inds.push_back(base + 6);
        inds.push_back(base + 4);
        inds.push_back(base + 6);
        inds.push_back(base + 7);
    }

    model->meshes.emplace_back(verts, inds);
    model->minBounds = glm::vec3(-15.0f, 85.0f, 0.0f);
    model->maxBounds = glm::vec3(15.0f, 115.0f, 4.0f);
    model->valid = true;
    model->texture = ModelManager::instance().getTexture("assets/Textures/InteriorTexture_01.png");
    return model;
}

} // namespace FanModels
