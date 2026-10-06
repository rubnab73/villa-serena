#pragma once

#include "Shader.hpp"
#include "Model.hpp"
#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>

struct SceneObject
{
    std::shared_ptr<Model> model;
    glm::mat4 transform;
    glm::mat3 normalMatrix;
    glm::vec4 tint;
    std::shared_ptr<Texture> customTexture = nullptr;
};

struct LightPoint
{
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    float radius;
    bool enabled = true;
    std::string name;
};

struct InteractiveDoor
{
    size_t objectIndex = 0;
    glm::vec3 hingePos{0.0f};
    float closedRotY = 0.0f;
    float openRotY = 0.0f;
    float currentAngle = 0.0f;
    float targetAngle = 0.0f;
    bool isOpen = false;
    std::string name;
};

struct AnimatedObject
{
    size_t objectIndex = 0;
    int type = 0; // 1: Ceiling Fan Blades, 2: Stand Fan Head, 3: Stand Fan Blades, 4: Traffic Car East, 5: Traffic Car West
    glm::vec3 basePos{0.0f};
    glm::vec3 baseRot{0.0f};
    glm::vec3 scale{1.0f};
    float speed = 1.0f;
    float param = 0.0f;
};

struct CollisionBox
{
    glm::vec3 minB;
    glm::vec3 maxB;
    int doorIndex = -1; // -1: always solid. >= 0: index into interactiveDoors (blocks only when door is closed)
};

class Scene
{
public:
    std::vector<SceneObject> objects;
    std::vector<LightPoint> pointLights;
    std::vector<InteractiveDoor> interactiveDoors;
    std::vector<AnimatedObject> animatedObjects;
    std::vector<CollisionBox> collisionBoxes;

    // FPS World Collisions & Floor Gravity
    void buildCollisionBoxes();
    void resolvePlayerMovement(glm::vec3 &position, const glm::vec3 &movementDelta, float playerRadius = 18.0f) const;
    float getFloorHeight(float x, float z, float currentY) const;

    // Realistic Fan Physics Simulation (smooth acceleration & slow deceleration with inertia)
    bool ceilingFanActive = true;
    float ceilingFanSpeed = 1.0f;       // 0.0f (stopped) to 1.0f (full speed)
    float ceilingFanAngle = 0.0f;

    bool diningFanActive = true;
    float diningFanSpeed = 1.0f;        // 0.0f (stopped) to 1.0f (full speed)
    float diningFanAngle = 0.0f;

    bool standFanActive = true;
    float standFanSpeed = 1.0f;         // 0.0f (stopped) to 1.0f (full speed)
    float standFanBladeAngle = 0.0f;
    float standFanOscAngle = 0.0f;
    float standFanOscPhase = 0.0f;

    void toggleCeilingFan();
    void toggleDiningFan();
    void toggleStandFan();
    void toggleAllFans();

    // Individual & Zone Light Controls
    void toggleLight(size_t index);
    void toggleGroundFloorLights();
    void toggleSecondFloorLights();
    void toggleExteriorLights();
    void toggleAllLights();

    void buildScene();
    void render(const Shader &shader);
    void update(float deltaTime, float totalTime);
    bool interactNearestDoor(const glm::vec3 &playerPos, float maxDist = 200.0f);

    void addObject(const std::string &path,
                   const glm::vec3 &position,
                   const glm::vec3 &rotationDegrees = glm::vec3(0.0f),
                   const glm::vec3 &scale = glm::vec3(1.0f),
                   const glm::vec4 &tint = glm::vec4(1.0f),
                   std::shared_ptr<Texture> customTexture = nullptr);

    void addCustomObject(std::shared_ptr<Model> model,
                         const glm::vec3 &position,
                         const glm::vec3 &rotationDegrees = glm::vec3(0.0f),
                         const glm::vec3 &scale = glm::vec3(1.0f),
                         const glm::vec4 &tint = glm::vec4(1.0f),
                         std::shared_ptr<Texture> customTexture = nullptr);

    void addDoor(const std::string &name,
                 const std::string &path,
                 const glm::vec3 &hingePos,
                 float closedRotY,
                 float openRotY,
                 const glm::vec3 &scale = glm::vec3(1.145f, 1.086f, 1.0f),
                 const glm::vec4 &tint = glm::vec4(0.58f, 0.38f, 0.24f, 1.0f));

    size_t getObjectCount() const { return objects.size(); }
};


