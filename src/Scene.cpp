#include "Scene.hpp"
#include "FanModels.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>

void Scene::addObject(const std::string &path,
                      const glm::vec3 &position,
                      const glm::vec3 &rotationDegrees,
                      const glm::vec3 &scale,
                      const glm::vec4 &tint,
                      std::shared_ptr<Texture> customTexture)
{
    auto model = ModelManager::instance().getModel(path);
    if (!model || !model->valid)
    {
        std::cerr << "Warning: Failed to add object from " << path << "\n";
        return;
    }

    addCustomObject(model, position, rotationDegrees, scale, tint, customTexture);
}

void Scene::addCustomObject(std::shared_ptr<Model> model,
                            const glm::vec3 &position,
                            const glm::vec3 &rotationDegrees,
                            const glm::vec3 &scale,
                            const glm::vec4 &tint,
                            std::shared_ptr<Texture> customTexture)
{
    if (!model || !model->valid)
        return;

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);

    if (rotationDegrees.y != 0.0f)
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationDegrees.y), glm::vec3(0.0f, 1.0f, 0.0f));
    if (rotationDegrees.x != 0.0f)
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationDegrees.x), glm::vec3(1.0f, 0.0f, 0.0f));
    if (rotationDegrees.z != 0.0f)
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationDegrees.z), glm::vec3(0.0f, 0.0f, 1.0f));

    modelMatrix = glm::scale(modelMatrix, scale);

    glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

    objects.push_back({model, modelMatrix, normalMat, tint, customTexture});
}

void Scene::addDoor(const std::string &name,
                    const std::string &path,
                    const glm::vec3 &hingePos,
                    float closedRotY,
                    float openRotY,
                    const glm::vec3 &scale,
                    const glm::vec4 &tint)
{
    size_t idx = objects.size();
    addObject(path, hingePos, glm::vec3(0.0f, closedRotY, 0.0f), scale, tint);
    interactiveDoors.push_back({idx, hingePos, closedRotY, openRotY, closedRotY, closedRotY, false, name});
}

bool Scene::interactNearestDoor(const glm::vec3 &playerPos, float maxDist)
{
    int bestIdx = -1;
    float bestDist = maxDist;

    for (size_t i = 0; i < interactiveDoors.size(); ++i)
    {
        float d = glm::distance(playerPos, interactiveDoors[i].hingePos);
        if (d < bestDist)
        {
            bestDist = d;
            bestIdx = static_cast<int>(i);
        }
    }

    if (bestIdx >= 0)
    {
        auto &door = interactiveDoors[bestIdx];
        door.isOpen = !door.isOpen;
        door.targetAngle = door.isOpen ? door.openRotY : door.closedRotY;
        std::cout << "[Interaction] Door '" << door.name << "' is now "
                  << (door.isOpen ? "OPENING" : "CLOSING")
                  << " (distance: " << static_cast<int>(bestDist) << " units)\n" << std::flush;
        return true;
    }

    return false;
}

void Scene::toggleCeilingFan()
{
    ceilingFanActive = !ceilingFanActive;
    std::cout << "[Fan (J)] Living Room Ceiling Fan: "
              << (ceilingFanActive ? "POWER ON (Accelerating to full speed)" : "POWER OFF (Spinning down slowly with inertia...)")
              << "\n" << std::flush;
}

void Scene::toggleDiningFan()
{
    diningFanActive = !diningFanActive;
    std::cout << "[Fan (H)] Dining Room Ceiling Fan: "
              << (diningFanActive ? "POWER ON (Accelerating to full speed)" : "POWER OFF (Spinning down slowly with inertia...)")
              << "\n" << std::flush;
}

void Scene::toggleStandFan()
{
    standFanActive = !standFanActive;
    std::cout << "[Fan (U)] Study Standing Floor Fan: "
              << (standFanActive ? "POWER ON (Accelerating to full speed)" : "POWER OFF (Spinning down slowly with inertia...)")
              << "\n" << std::flush;
}

void Scene::toggleAllFans()
{
    bool target = !(ceilingFanActive || diningFanActive || standFanActive);
    ceilingFanActive = target;
    diningFanActive = target;
    standFanActive = target;
    std::cout << "[Fan Master (Y)] All Fans set to: "
              << (target ? "ON (Ramping up to speed)" : "OFF (Spinning down slowly...)")
              << "\n" << std::flush;
}

void Scene::toggleLight(size_t index)
{
    if (index < pointLights.size())
    {
        pointLights[index].enabled = !pointLights[index].enabled;
        std::cout << "[Light Switch F" << (index + 1) << "] " << pointLights[index].name << ": "
                  << (pointLights[index].enabled ? "ON (Illuminated)" : "OFF (Extinguished)") << "\n" << std::flush;
    }
}

void Scene::toggleGroundFloorLights()
{
    bool anyOn = pointLights[1].enabled || pointLights[2].enabled || pointLights[3].enabled || pointLights[8].enabled;
    bool newState = !anyOn;
    pointLights[1].enabled = newState;
    pointLights[2].enabled = newState;
    pointLights[3].enabled = newState;
    pointLights[8].enabled = newState;
    std::cout << "[Zone Lights (Z)] Ground Floor Lights switched " << (newState ? "ON" : "OFF") << "\n" << std::flush;
}

void Scene::toggleSecondFloorLights()
{
    bool anyOn = pointLights[5].enabled || pointLights[6].enabled || pointLights[7].enabled || pointLights[9].enabled;
    bool newState = !anyOn;
    pointLights[5].enabled = newState;
    pointLights[6].enabled = newState;
    pointLights[7].enabled = newState;
    pointLights[9].enabled = newState;
    std::cout << "[Zone Lights (X)] Second Floor Lights switched " << (newState ? "ON" : "OFF") << "\n" << std::flush;
}

void Scene::toggleExteriorLights()
{
    bool anyOn = pointLights[0].enabled || pointLights[4].enabled;
    bool newState = !anyOn;
    pointLights[0].enabled = newState;
    pointLights[4].enabled = newState;
    std::cout << "[Zone Lights (V)] Porch & Garage Lights switched " << (newState ? "ON" : "OFF") << "\n" << std::flush;
}

void Scene::toggleAllLights()
{
    bool anyOn = false;
    for (const auto &pl : pointLights)
    {
        if (pl.enabled)
        {
            anyOn = true;
            break;
        }
    }
    bool newState = !anyOn;
    for (auto &pl : pointLights)
        pl.enabled = newState;
    std::cout << "[Master Switch (K)] All House Lights switched " << (newState ? "ALL ON" : "ALL OFF") << "\n" << std::flush;
}

void Scene::update(float deltaTime, float totalTime)
{
    // Living Room Ceiling Fan: spins down gradually with bearing drag (~3.5s) or accelerates smoothly (~2s)
    if (ceilingFanActive)
        ceilingFanSpeed = std::min(1.0f, ceilingFanSpeed + deltaTime * 0.45f);
    else
        ceilingFanSpeed = std::max(0.0f, ceilingFanSpeed - deltaTime * 0.28f);

    ceilingFanAngle = std::fmod(ceilingFanAngle + ceilingFanSpeed * 620.0f * deltaTime, 360.0f);

    // Dining Room Ceiling Fan: spins down gradually (~3.5s) or accelerates smoothly (~2s)
    if (diningFanActive)
        diningFanSpeed = std::min(1.0f, diningFanSpeed + deltaTime * 0.45f);
    else
        diningFanSpeed = std::max(0.0f, diningFanSpeed - deltaTime * 0.28f);

    diningFanAngle = std::fmod(diningFanAngle + diningFanSpeed * 620.0f * deltaTime, 360.0f);

    // Standing Floor Fan: spins down gradually (~2.8s) or accelerates (~1.3s)
    if (standFanActive)
        standFanSpeed = std::min(1.0f, standFanSpeed + deltaTime * 0.75f);
    else
        standFanSpeed = std::max(0.0f, standFanSpeed - deltaTime * 0.35f);

    standFanBladeAngle = std::fmod(standFanBladeAngle + standFanSpeed * 1400.0f * deltaTime, 360.0f);
    standFanOscPhase += standFanSpeed * 1.6f * deltaTime;
    standFanOscAngle = std::sin(standFanOscPhase) * 35.0f;

    // 1. Smoothly animate interactive doors swinging open/closed
    for (auto &door : interactiveDoors)
    {
        float diff = door.targetAngle - door.currentAngle;
        if (std::abs(diff) > 0.05f)
        {
            door.currentAngle += diff * std::min(deltaTime * 8.0f, 1.0f);

            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, door.hingePos);
            modelMatrix = glm::rotate(modelMatrix, glm::radians(door.currentAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1.145f, 1.086f, 1.0f));

            if (door.objectIndex < objects.size())
            {
                objects[door.objectIndex].transform = modelMatrix;
                objects[door.objectIndex].normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
            }
        }
    }

    // 2. Animate dynamic scene objects (Ceiling fan, Stand fan, Moving suburban cars)
    for (const auto &anim : animatedObjects)
    {
        if (anim.objectIndex >= objects.size())
            continue;

        glm::mat4 m(1.0f);

        switch (anim.type)
        {
        case 1: // Ceiling Fan Blades (Spin around central vertical Y axis with inertia)
        {
            m = glm::translate(m, anim.basePos);
            m = glm::rotate(m, glm::radians(ceilingFanAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            m = glm::scale(m, anim.scale);
            break;
        }
        case 2: // Standing Fan Head (Left-to-right smooth oscillation with inertia)
        {
            m = glm::translate(m, anim.basePos);
            m = glm::rotate(m, glm::radians(anim.baseRot.y + standFanOscAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            m = glm::scale(m, anim.scale);
            break;
        }
        case 3: // Standing Fan Blades (Oscillates with head + high-speed spin around cage Z axis with inertia)
        {
            m = glm::translate(m, anim.basePos);
            m = glm::rotate(m, glm::radians(anim.baseRot.y + standFanOscAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            // Rotate around the local blade spinner hub at Y=100.0, Z=1.0
            m = glm::translate(m, glm::vec3(0.0f, 100.0f, 1.0f));
            m = glm::rotate(m, glm::radians(standFanBladeAngle), glm::vec3(0.0f, 0.0f, 1.0f));
            m = glm::translate(m, glm::vec3(0.0f, -100.0f, -1.0f));
            m = glm::scale(m, anim.scale);
            break;
        }
        case 4: // Traffic Car Eastbound (Near lane along Z = 2180, travels from -4500 to +4500 and repeats)
        {
            float span = 9000.0f;
            float xPos = -4500.0f + std::fmod(totalTime * anim.speed, span);
            m = glm::translate(m, glm::vec3(xPos, anim.basePos.y, anim.basePos.z));
            m = glm::rotate(m, glm::radians(anim.baseRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
            m = glm::scale(m, anim.scale);
            break;
        }
        case 5: // Traffic Car Westbound (Far lane along Z = 2420, travels from +4500 to -4500 and repeats)
        {
            float span = 9000.0f;
            float xPos = 4500.0f - std::fmod((totalTime + 4.5f) * anim.speed, span);
            m = glm::translate(m, glm::vec3(xPos, anim.basePos.y, anim.basePos.z));
            m = glm::rotate(m, glm::radians(anim.baseRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
            m = glm::scale(m, anim.scale);
            break;
        }
        case 6: // Dining Room Ceiling Fan Blades (Spin around central vertical Y axis with inertia)
        {
            m = glm::translate(m, anim.basePos);
            m = glm::rotate(m, glm::radians(diningFanAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            m = glm::scale(m, anim.scale);
            break;
        }
        default:
            break;
        }

        objects[anim.objectIndex].transform = m;
        objects[anim.objectIndex].normalMatrix = glm::transpose(glm::inverse(glm::mat3(m)));
    }
}

void Scene::buildScene()
{
    objects.clear();
    pointLights.clear();
    interactiveDoors.clear();
    animatedObjects.clear();

    // ========================================================
    // Model Asset Paths from assets
    // ========================================================
    // Architecture & Flooring
    const std::string floorTileMdl = "assets/Exterior/Floor/Floor1_01.obj";
    const std::string floorWoodLiving = "assets/Exterior/Floor/Floor13_01.obj";
    const std::string floorTileKitchen = "assets/Exterior/Floor/Floor10_01.obj";
    const std::string floorSlateHall = "assets/Exterior/Floor/Floor11_01.obj";
    const std::string floorBathMdl = "assets/Exterior/Floor/Floor17_01.obj";
    const std::string floorMasterBed = "assets/Exterior/Floor/Floor16_01.obj";
    const std::string floorBed2 = "assets/Exterior/Floor/Floor24_01.obj";
    const std::string floorStudy = "assets/Exterior/Floor/Floor6_01.obj";
    const std::string floorGarageMdl = "assets/Exterior/Floor/Floor15_01.obj";
    const std::string floorDrivewayMdl = "assets/Exterior/Floor/Floor12_01.obj";
    const std::string floorSidewalkMdl = "assets/Exterior/Floor/Floor14_01.obj";
    const std::string floorStreetMdl = "assets/Exterior/Floor/Floor20_01.obj";

    const std::string wallSolid = "assets/Exterior/Walls/Wall1/Wall1_C1_01.obj";
    const std::string wallDoor = "assets/Exterior/Walls/Wall1/Wall1_C1_02.obj";
    const std::string wallWindow = "assets/Exterior/Walls/Wall1/Wall1_C1_03.obj";
    const std::string doorMdl = "assets/Exterior/Doors/Door1_C1.obj";
    const std::string windowMdl = "assets/Exterior/Windows/Window1_01.obj";
    const std::string stairsMdl = "assets/Exterior/Stairs/Stairs1_C1.obj";

    // Roofs & Chimney
    const std::string roofHouseMid = "assets/Exterior/Roof/Roof4_C1_01.obj";
    const std::string roofHouseCap = "assets/Exterior/Roof/Roof4_C1_02.obj";
    const std::string roofHouseDormer = "assets/Exterior/Roof/Roof4_C1_04.obj";
    const std::string roofGarageMid = "assets/Exterior/Roof/Roof1_C1_01.obj";
    const std::string roofGarageCap = "assets/Exterior/Roof/Roof1_C1_02.obj";
    const std::string chimneyMdl = "assets/Exterior/Chimney/Chimney1_C1.obj";

    // Terrace & Outdoor Props
    const std::string terraceMdl = "assets/Exterior/Terrace/Terrace1_C1.obj";
    const std::string fenceMdl = "assets/Exterior/ExteriorProps/Fence_01.obj";
    const std::string flowerbed1 = "assets/Exterior/ExteriorProps/FlowerBed_01.obj";
    const std::string flowerbed2 = "assets/Exterior/ExteriorProps/FlowerBed_02.obj";
    const std::string benchMdl = "assets/Exterior/ExteriorProps/Bench_01.obj";
    const std::string grillMdl = "assets/Exterior/ExteriorProps/Grill_01_01.obj";
    const std::string doghouseMdl = "assets/Exterior/ExteriorProps/Doghouse_01.obj";
    const std::string hydrantMdl = "assets/Exterior/ExteriorProps/Hydrant.obj";
    const std::string trashMdl = "assets/Exterior/ExteriorProps/Trash_01.obj";
    const std::string garbageMdl = "assets/Exterior/ExteriorProps/Garbage_01.obj";
    const std::string potMdl = "assets/Exterior/ExteriorProps/Pot_01.obj";
    const std::string pot2Mdl = "assets/Exterior/ExteriorProps/Pot_02.obj";
    const std::string loungerMdl = "assets/Exterior/ExteriorProps/Lounger_01.obj";
    const std::string carMdl = "assets/Interior/Props/Car_01.obj";

    // Interior Props & Furniture
    const std::string sofaMdl = "assets/Interior/Room/Sofa1_C1.obj";
    const std::string sofa2Mdl = "assets/Interior/Room/Sofa2_C1.obj";
    const std::string tvTableMdl = "assets/Interior/Room/TV_Table1_C1.obj";
    const std::string tvMdl = "assets/Interior/Props/TV_01.obj";
    const std::string fireplaceMdl = "assets/Interior/Props/Fireplace_01.obj";
    const std::string coffeeTableMdl = "assets/Interior/Room/SmallTable_01_C1.obj";
    const std::string carpet1Mdl = "assets/Interior/Props/Carpet1_C1.obj";
    const std::string carpet2Mdl = "assets/Interior/Props/Carpet2_C1.obj";
    const std::string clockMdl = "assets/Interior/Props/Clock1_C1.obj";
    const std::string painting1Mdl = "assets/Interior/Props/Painting_01.obj";
    const std::string painting2Mdl = "assets/Interior/Props/Painting_02.obj";

    const std::string diningTableMdl = "assets/Interior/KitchenProps/Table_01_C1.obj";
    const std::string diningChairMdl = "assets/Interior/KitchenProps/Chair_01_C1.obj";
    const std::string counterMdl = "assets/Interior/Kitchen/C1/Kitchen1_C1_01.obj";
    const std::string fridgeMdl = "assets/Interior/KitchenProps/Refrigerator1_C1_01.obj";
    const std::string ovenMdl = "assets/Interior/KitchenProps/Oven1_01.obj";
    const std::string hobMdl = "assets/Interior/KitchenProps/Hob1_C1.obj";
    const std::string hoodMdl = "assets/Interior/KitchenProps/Hood1_C1_01.obj";
    const std::string microMdl = "assets/Interior/KitchenProps/Microwave_C1_01.obj";
    const std::string sinkKitchenMdl = "assets/Interior/Kitchen/Plumbing/Sink2_C1.obj";
    const std::string dishwasherMdl = "assets/Interior/KitchenProps/Dishwasher1_01.obj";
    const std::string coffeeMdl = "assets/Interior/KitchenProps/CoffeeMachine_01.obj";
    const std::string toasterMdl = "assets/Interior/KitchenProps/Toaster_C1_01.obj";

    const std::string toiletMdl = "assets/Interior/Bathroom/Toilet1_C1_01.obj";
    const std::string bathSinkMdl = "assets/Interior/Bathroom/Bathroom_01.obj";
    const std::string showerMdl = "assets/Interior/Bathroom/Shower1_C1_01.obj";
    const std::string mirrorMdl = "assets/Interior/Bathroom/Mirror_01_C1.obj";
    const std::string washerMdl = "assets/Interior/Bathroom/Washer_01.obj";

    const std::string bedMasterMdl = "assets/Interior/Room/Bed1_C1.obj";
    const std::string bedSingleMdl = "assets/Interior/Room/Bed3_C1.obj";
    const std::string bedTable1Mdl = "assets/Interior/Room/BedTable1_C1.obj";
    const std::string bedTable2Mdl = "assets/Interior/Room/BedTable2_C1.obj";
    const std::string wardrobeMdl = "assets/Interior/Room/Wardrobe1_C1.obj";
    const std::string dresserMdl = "assets/Interior/Room/Dresser1_C1.obj";

    const std::string pcTableMdl = "assets/Interior/Room/PC_Table1_C1.obj";
    const std::string pcChairMdl = "assets/Interior/Room/PC_Chair1_C1.obj";
    const std::string pcMdl = "assets/Interior/Props/PC_01.obj";
    const std::string pcKeyMdl = "assets/Interior/Props/PC_Keyboard_01.obj";
    const std::string pcMouseMdl = "assets/Interior/Props/PC_Mouse_01.obj";
    const std::string laptopMdl = "assets/Interior/Props/Laptop_01.obj";
    const std::string shelfMdl = "assets/Interior/Room/Shelf_01.obj";
    const std::string bookMdl = "assets/Interior/Props/Book_01.obj";
    const std::string globeMdl = "assets/Interior/Props/Globe_01.obj";
    const std::string printerMdl = "assets/Interior/Props/Printer_01.obj";

    const std::string treeTallMdl = "assets/Interior/Plants/Plant_15.obj";
    const std::string treeMedMdl = "assets/Interior/Plants/Plant_14.obj";
    const std::string bushMdl = "assets/Interior/Plants/Plant_08.obj";
    const std::string indoorPlant1 = "assets/Interior/Plants/Plant_01.obj";
    const std::string indoorPlant2 = "assets/Interior/Plants/Plant_02.obj";
    const std::string indoorPlant3 = "assets/Interior/Plants/Plant_05.obj";
    const std::string boxMdl = "assets/Interior/Props/Box_01.obj";

    // ========================================================
    // 1. OUTSIDE GROUNDS: Grass Lawn, Asphalt Street, Sidewalk, Driveway
    // ========================================================

    // Continuous, lush green grass lawn covering yard and neighborhood perimeter
    // Samples vibrant lawn foliage coordinates from assets/Textures/InteriorTexture_01.png
    auto yardGrass = ModelManager::instance().createPlane("yard_grass", 6500.0f, 4000.0f,
                                                          glm::vec2(0.33f, 0.27f), glm::vec2(0.35f, 0.29f),
                                                          "assets/Textures/InteriorTexture_01.png");
    addCustomObject(yardGrass, glm::vec3(0.0f, -4.5f, 750.0f));

    auto worldGround = ModelManager::instance().createPlane("world_grass", 25000.0f, 25000.0f,
                                                            glm::vec2(0.33f, 0.27f), glm::vec2(0.35f, 0.29f),
                                                            "assets/Textures/InteriorTexture_01.png");
    addCustomObject(worldGround, glm::vec3(0.0f, -6.5f, 0.0f));

    // Asphalt Street (Z = 2050 to 2550, spanning X = -4000 to +4000)
    for (float x = -3750.0f; x <= 3750.0f; x += 500.0f)
    {
        for (float z = 2050.0f; z <= 2550.0f; z += 500.0f)
        {
            addObject(floorStreetMdl, glm::vec3(x, -3.5f, z), glm::vec3(0.0f), glm::vec3(2.0f, 0.8f, 2.0f));
        }
    }

    // Street markings (dashed center lines) - pure crisp white
    for (float x = -3500.0f; x <= 3500.0f; x += 300.0f)
    {
        addObject(floorTileMdl, glm::vec3(x, -2.0f, 2300.0f), glm::vec3(0.0f), glm::vec3(0.6f, 0.85f, 0.1f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // Sidewalk along the front of properties (Z = 1800)
    for (float x = -3000.0f; x <= 3000.0f; x += 250.0f)
    {
        addObject(floorSidewalkMdl, glm::vec3(x, -1.8f, 1800.0f), glm::vec3(0.0f), glm::vec3(1.0f));
    }

    // Driveway: Paved stone from Garage (Z = 500) to Sidewalk (Z = 1800) at X = 750 and 1000
    for (float z = 500.0f; z <= 1750.0f; z += 250.0f)
    {
        addObject(floorDrivewayMdl, glm::vec3(750.0f, -2.2f, z), glm::vec3(0.0f), glm::vec3(1.0f));
        addObject(floorDrivewayMdl, glm::vec3(1000.0f, -2.2f, z), glm::vec3(0.0f), glm::vec3(1.0f));
    }

    // Front Walkway to entrance (from Sidewalk Z = 1800 to front door at X = 0, Z = 750)
    for (float z = 750.0f; z <= 1750.0f; z += 250.0f)
    {
        addObject(floorDrivewayMdl, glm::vec3(0.0f, -1.9f, z), glm::vec3(0.0f), glm::vec3(1.0f));
    }

    // ========================================================
    // 2. FENCE & PERIMETER
    // ========================================================
    // Front fence along Z = 1680
    for (float x = -1750.0f; x <= -150.0f; x += 100.0f)
        addObject(fenceMdl, glm::vec3(x, 0.0f, 1680.0f), glm::vec3(0.0f));
    for (float x = 150.0f; x <= 650.0f; x += 100.0f)
        addObject(fenceMdl, glm::vec3(x, 0.0f, 1680.0f), glm::vec3(0.0f));
    for (float x = 1150.0f; x <= 1850.0f; x += 100.0f)
        addObject(fenceMdl, glm::vec3(x, 0.0f, 1680.0f), glm::vec3(0.0f));

    // Side and back perimeter fence
    for (float z = -850.0f; z <= 1680.0f; z += 100.0f)
    {
        addObject(fenceMdl, glm::vec3(-1950.0f, 0.0f, z), glm::vec3(0.0f, 90.0f, 0.0f));
        addObject(fenceMdl, glm::vec3(1950.0f, 0.0f, z), glm::vec3(0.0f, 90.0f, 0.0f));
    }
    for (float x = -1950.0f; x <= 1950.0f; x += 100.0f)
        addObject(fenceMdl, glm::vec3(x, 0.0f, -850.0f), glm::vec3(0.0f));

    // ========================================================
    // 3. OUTSIDE PROPS: Mailbox, Trees, Garden, Grill, Bench, Hydrant
    // ========================================================
    // Trees throughout front yard and garden
    addObject(treeTallMdl, glm::vec3(-1400.0f, 0.0f, 1300.0f), glm::vec3(0.0f, 35.0f, 0.0f), glm::vec3(3.2f));
    addObject(treeTallMdl, glm::vec3(-1550.0f, 0.0f, 400.0f), glm::vec3(0.0f, 80.0f, 0.0f), glm::vec3(3.5f));
    addObject(treeTallMdl, glm::vec3(1500.0f, 0.0f, 1300.0f), glm::vec3(0.0f, 120.0f, 0.0f), glm::vec3(3.0f));
    addObject(treeTallMdl, glm::vec3(1550.0f, 0.0f, 300.0f), glm::vec3(0.0f, 210.0f, 0.0f), glm::vec3(3.4f));
    addObject(treeTallMdl, glm::vec3(-850.0f, 0.0f, 1400.0f), glm::vec3(0.0f, 45.0f, 0.0f), glm::vec3(2.8f));

    addObject(treeMedMdl, glm::vec3(-500.0f, 0.0f, 1450.0f), glm::vec3(0.0f, 15.0f, 0.0f), glm::vec3(2.2f));
    addObject(treeMedMdl, glm::vec3(450.0f, 0.0f, 1450.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(2.0f));
    addObject(bushMdl, glm::vec3(-350.0f, 0.0f, 1050.0f), glm::vec3(0.0f, 40.0f, 0.0f), glm::vec3(1.8f));
    addObject(bushMdl, glm::vec3(-350.0f, 0.0f, 1350.0f), glm::vec3(0.0f, 130.0f, 0.0f), glm::vec3(1.6f));
    addObject(bushMdl, glm::vec3(1300.0f, 0.0f, 850.0f), glm::vec3(0.0f, 75.0f, 0.0f), glm::vec3(1.9f));

    // Flowerbeds along the walkway and porch (spaced wide apart for completely clear entrance path)
    addObject(flowerbed1, glm::vec3(-380.0f, 0.0f, 850.0f), glm::vec3(0.0f), glm::vec3(1.3f));
    addObject(flowerbed2, glm::vec3(380.0f, 0.0f, 850.0f), glm::vec3(0.0f), glm::vec3(1.3f));
    addObject(flowerbed1, glm::vec3(-800.0f, 0.0f, 950.0f), glm::vec3(0.0f, 45.0f, 0.0f), glm::vec3(1.3f));

    addObject(benchMdl, glm::vec3(-1100.0f, 0.0f, 1000.0f), glm::vec3(0.0f, 75.0f, 0.0f), glm::vec3(1.2f));
    addObject(grillMdl, glm::vec3(-950.0f, 0.0f, 300.0f), glm::vec3(0.0f, 120.0f, 0.0f), glm::vec3(1.1f));
    addObject(doghouseMdl, glm::vec3(-1250.0f, 0.0f, 50.0f), glm::vec3(0.0f, 45.0f, 0.0f), glm::vec3(1.2f));

    // Potted plants flanking the entrance porch, firmly grounded at Y = 0.0f
    addObject(indoorPlant1, glm::vec3(-170.0f, 0.0f, 780.0f), glm::vec3(0.0f), glm::vec3(1.4f));
    addObject(indoorPlant1, glm::vec3(170.0f, 0.0f, 780.0f), glm::vec3(0.0f), glm::vec3(1.4f));

    addObject(hydrantMdl, glm::vec3(550.0f, 0.0f, 1920.0f), glm::vec3(0.0f), glm::vec3(1.2f));
    addObject(trashMdl, glm::vec3(1100.0f, 0.0f, 1750.0f), glm::vec3(0.0f), glm::vec3(1.2f));
    addObject(garbageMdl, glm::vec3(1180.0f, 0.0f, 1750.0f), glm::vec3(0.0f), glm::vec3(1.2f));

    // ========================================================
    // 4. ATTACHED GARAGE & FAMILY CAR
    // Footprint: X = +625 to +1125 (width 500), Z = -250 to +500 (depth 750)
    // ========================================================
    for (float x = 750.0f; x <= 1000.0f; x += 250.0f)
    {
        for (float z = -125.0f; z <= 375.0f; z += 250.0f)
        {
            addObject(floorGarageMdl, glm::vec3(x, 0.0f, z), glm::vec3(0.0f), glm::vec3(1.0f));
        }
    }

    // Garage Right Wall (X = 1125, Z = -250 to 500)
    for (float z = -125.0f; z <= 375.0f; z += 250.0f)
        addObject(wallSolid, glm::vec3(1125.0f, 0.0f, z), glm::vec3(0.0f, -90.0f, 0.0f));

    // Garage Back Wall (Z = -250, X = 625 to 1125)
    addObject(wallSolid, glm::vec3(750.0f, 0.0f, -250.0f), glm::vec3(0.0f, 180.0f, 0.0f));
    addObject(wallSolid, glm::vec3(1000.0f, 0.0f, -250.0f), glm::vec3(0.0f, 180.0f, 0.0f));

    // Garage Large Open Gate Portal (Z = 500) - wide jamb pillars and high header beam
    addObject(wallSolid, glm::vec3(660.0f, 0.0f, 500.0f), glm::vec3(0.0f), glm::vec3(0.28f, 1.0f, 1.0f));
    addObject(wallSolid, glm::vec3(1090.0f, 0.0f, 500.0f), glm::vec3(0.0f), glm::vec3(0.28f, 1.0f, 1.0f));
    addObject(wallSolid, glm::vec3(875.0f, 230.0f, 500.0f), glm::vec3(0.0f), glm::vec3(1.95f, 0.25f, 1.0f));

    // Garage Pitched Roof: Width 500 centered at X = 875, Y = 0 (base sits atop Y = 300 wall)
    addObject(roofGarageCap, glm::vec3(875.0f, 0.0f, -125.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    addObject(roofGarageMid, glm::vec3(875.0f, 0.0f, 125.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    addObject(roofGarageCap, glm::vec3(875.0f, 0.0f, 375.0f), glm::vec3(0.0f, 180.0f, 0.0f));

    // Family Car parked facing forward toward street (+Z)
    addObject(carMdl, glm::vec3(875.0f, 0.0f, 150.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(22.0f));

    addObject(shelfMdl, glm::vec3(1090.0f, 0.0f, -50.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.2f));
    addObject(boxMdl, glm::vec3(1090.0f, 0.0f, 120.0f), glm::vec3(0.0f), glm::vec3(1.4f));

    // ========================================================
    // 5. GROUND FLOOR ARCHITECTURE (Y = 0 to 300)
    // House footprint: X = -625 to +625 (1250 wide), Z = -750 to +750 (1500 deep)
    // 5 x 6 grid = 30 tiles per floor (Huge, open, and spacious!)
    // ========================================================

    // Dedicated room floors
    for (float x = -500.0f; x <= 500.0f; x += 250.0f)
    {
        for (float z = -625.0f; z <= 625.0f; z += 250.0f)
        {
            if (x <= -250.0f && z >= 0.0f)
            {
                // Grand Living Room - Warm Parquet
                addObject(floorWoodLiving, glm::vec3(x, 0.0f, z), glm::vec3(0.0f));
            }
            else if (x == 0.0f && z >= 0.0f)
            {
                // Grand Entrance / Foyer Gallery - Slate
                addObject(floorSlateHall, glm::vec3(x, 0.0f, z), glm::vec3(0.0f));
            }
            else if (x >= 250.0f && z >= 250.0f)
            {
                // Dining Room - Hardwood Parquet
                addObject(floorWoodLiving, glm::vec3(x, 0.0f, z), glm::vec3(0.0f));
            }
            else if (x >= 250.0f && z < 250.0f)
            {
                // Chef's Kitchen - Terracotta Ceramic Tiles
                addObject(floorTileKitchen, glm::vec3(x, 0.0f, z), glm::vec3(0.0f));
            }
            else if (x <= -250.0f && z < -250.0f)
            {
                // Powder Room / Laundry - Mint Ceramic
                addObject(floorBathMdl, glm::vec3(x, 0.0f, z), glm::vec3(0.0f));
            }
            else
            {
                // Hallway & Stairwell - Slate
                addObject(floorSlateHall, glm::vec3(x, 0.0f, z), glm::vec3(0.0f));
            }
        }
    }

    // Exterior Walls - Front (Z = +750)
    addObject(wallWindow, glm::vec3(-500.0f, 0.0f, 750.0f), glm::vec3(0.0f));
    addObject(windowMdl, glm::vec3(-500.0f, 0.0f, 750.0f), glm::vec3(0.0f));
    addObject(wallWindow, glm::vec3(-250.0f, 0.0f, 750.0f), glm::vec3(0.0f));
    addObject(windowMdl, glm::vec3(-250.0f, 0.0f, 750.0f), glm::vec3(0.0f));
    addObject(wallDoor, glm::vec3(0.0f, 0.0f, 750.0f), glm::vec3(0.0f));
    addDoor("Front Entrance Door", doorMdl, glm::vec3(50.93f, 0.0f, 750.0f), 0.0f, -90.0f);
    addObject(wallWindow, glm::vec3(250.0f, 0.0f, 750.0f), glm::vec3(0.0f));
    addObject(windowMdl, glm::vec3(250.0f, 0.0f, 750.0f), glm::vec3(0.0f));
    addObject(wallWindow, glm::vec3(500.0f, 0.0f, 750.0f), glm::vec3(0.0f));
    addObject(windowMdl, glm::vec3(500.0f, 0.0f, 750.0f), glm::vec3(0.0f));

    // Exterior Walls - Left (X = -625)
    addObject(wallWindow, glm::vec3(-625.0f, 0.0f, 625.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    addObject(windowMdl, glm::vec3(-625.0f, 0.0f, 625.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    addObject(wallSolid, glm::vec3(-625.0f, 0.0f, 375.0f), glm::vec3(0.0f, 90.0f, 0.0f)); // Fireplace wall
    addObject(wallWindow, glm::vec3(-625.0f, 0.0f, 125.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    addObject(windowMdl, glm::vec3(-625.0f, 0.0f, 125.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    addObject(wallSolid, glm::vec3(-625.0f, 0.0f, -125.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    addObject(wallSolid, glm::vec3(-625.0f, 0.0f, -375.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    addObject(wallWindow, glm::vec3(-625.0f, 0.0f, -625.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    addObject(windowMdl, glm::vec3(-625.0f, 0.0f, -625.0f), glm::vec3(0.0f, 90.0f, 0.0f));

    // Exterior Walls - Back (Z = -750)
    addObject(wallSolid, glm::vec3(-500.0f, 0.0f, -750.0f), glm::vec3(0.0f, 180.0f, 0.0f));
    addObject(wallWindow, glm::vec3(-250.0f, 0.0f, -750.0f), glm::vec3(0.0f, 180.0f, 0.0f));
    addObject(windowMdl, glm::vec3(-250.0f, 0.0f, -750.0f), glm::vec3(0.0f, 180.0f, 0.0f));
    addObject(wallDoor, glm::vec3(0.0f, 0.0f, -750.0f), glm::vec3(0.0f, 180.0f, 0.0f));
    addDoor("Backyard Patio Door", doorMdl, glm::vec3(-50.93f, 0.0f, -750.0f), 180.0f, 90.0f);
    addObject(wallWindow, glm::vec3(250.0f, 0.0f, -750.0f), glm::vec3(0.0f, 180.0f, 0.0f));
    addObject(windowMdl, glm::vec3(250.0f, 0.0f, -750.0f), glm::vec3(0.0f, 180.0f, 0.0f));
    addObject(wallSolid, glm::vec3(500.0f, 0.0f, -750.0f), glm::vec3(0.0f, 180.0f, 0.0f));

    // Exterior Walls - Right (X = +625)
    addObject(wallWindow, glm::vec3(625.0f, 0.0f, 625.0f), glm::vec3(0.0f, -90.0f, 0.0f));
    addObject(windowMdl, glm::vec3(625.0f, 0.0f, 625.0f), glm::vec3(0.0f, -90.0f, 0.0f));
    addObject(wallSolid, glm::vec3(625.0f, 0.0f, 375.0f), glm::vec3(0.0f, -90.0f, 0.0f));
    addObject(wallSolid, glm::vec3(625.0f, 0.0f, 125.0f), glm::vec3(0.0f, -90.0f, 0.0f));
    addObject(wallDoor, glm::vec3(625.0f, 0.0f, -125.0f), glm::vec3(0.0f, -90.0f, 0.0f));
    addDoor("Garage Access Door", doorMdl, glm::vec3(625.0f, 0.0f, -74.07f), -90.0f, 0.0f);
    addObject(wallSolid, glm::vec3(625.0f, 0.0f, -375.0f), glm::vec3(0.0f, -90.0f, 0.0f));
    addObject(wallWindow, glm::vec3(625.0f, 0.0f, -625.0f), glm::vec3(0.0f, -90.0f, 0.0f));
    addObject(windowMdl, glm::vec3(625.0f, 0.0f, -625.0f), glm::vec3(0.0f, -90.0f, 0.0f));

    // Interior Partition Walls - Ground Floor (Warm architectural linen drywall finish)
    const glm::vec4 wallTintInterior(0.85f, 0.83f, 0.79f, 1.0f);
    // Elegant archway separating living room and foyer
    addObject(wallDoor, glm::vec3(-125.0f, 0.0f, 375.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f), wallTintInterior);
    // Living room solid wall along Z = 0 separating living room from side wing
    addObject(wallSolid, glm::vec3(-500.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f), wallTintInterior);

    // Foyer / Dining partition
    addObject(wallSolid, glm::vec3(125.0f, 0.0f, 625.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f), wallTintInterior);
    addObject(wallDoor, glm::vec3(125.0f, 0.0f, 375.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f), wallTintInterior);

    // Stair alcove west wall
    addObject(wallSolid, glm::vec3(-375.0f, 0.0f, -125.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f), wallTintInterior);

    // Powder Room / Laundry - Fully Enclosed Private Bath (Solid walls on all sides with dedicated hallway door)
    // Wall separating bath from stair alcove along Z = -250 (seamlessly spans X = -625 to -125)
    addObject(wallSolid, glm::vec3(-500.0f, 0.0f, -250.0f), glm::vec3(0.0f), glm::vec3(1.0f), wallTintInterior);
    addObject(wallSolid, glm::vec3(-250.0f, 0.0f, -250.0f), glm::vec3(0.0f), glm::vec3(1.0f), wallTintInterior);
    // Hallway entry wall along X = -125 (covers Z from -250 to -750 seamlessly)
    addObject(wallDoor, glm::vec3(-125.0f, 0.0f, -375.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f), wallTintInterior);
    addDoor("Powder Room Door", doorMdl, glm::vec3(-125.0f, 0.0f, -324.07f), -90.0f, -180.0f);
    addObject(wallSolid, glm::vec3(-125.0f, 0.0f, -625.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f), wallTintInterior);

    // Kitchen partition wall
    addObject(wallDoor, glm::vec3(125.0f, 0.0f, -250.0f), glm::vec3(0.0f), glm::vec3(1.0f), wallTintInterior);
    addObject(wallSolid, glm::vec3(375.0f, 0.0f, -250.0f), glm::vec3(0.0f), glm::vec3(1.0f), wallTintInterior);
    addObject(wallSolid, glm::vec3(500.0f, 0.0f, -250.0f), glm::vec3(0.0f), glm::vec3(1.0f), wallTintInterior);

    // ========================================================
    // 6. GROUND FLOOR FURNISHING (DECLUTTERED & SPACIOUS)
    // ========================================================

    // --- ENTRANCE / FOYER ---
    // Runner rug leading inside
    addObject(carpet1Mdl, glm::vec3(0.0f, 2.0f, 600.0f), glm::vec3(0.0f), glm::vec3(1.0f, 1.0f, 1.6f));
    addObject(clockMdl, glm::vec3(120.0f, 180.0f, 570.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f));

    // --- GRAND LIVING ROOM ---
    // Fireplace centered on solid left wall (moved slightly inside room to X = -594.0f to completely prevent Z-fighting)
    addObject(fireplaceMdl, glm::vec3(-594.0f, 0.0f, 375.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.1f));
    addObject(clockMdl, glm::vec3(-601.0f, 210.0f, 375.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(0.9f));
    // Art photo frames positioned cleanly on solid walls flanking the fireplace (completely clear of all windows)
    addObject(painting1Mdl, glm::vec3(-612.0f, 160.0f, 280.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(0.95f));
    addObject(painting2Mdl, glm::vec3(-612.0f, 160.0f, 470.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(0.95f));

    // Seating arrangement with huge walking clearance
    addObject(carpet1Mdl, glm::vec3(-375.0f, 2.0f, 375.0f), glm::vec3(0.0f), glm::vec3(2.2f, 1.0f, 2.2f));
    addObject(sofaMdl, glm::vec3(-250.0f, 0.0f, 375.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.1f)); // 3-seater facing fireplace, 70 units clear of wall
    addObject(coffeeTableMdl, glm::vec3(-375.0f, 0.0f, 375.0f), glm::vec3(0.0f), glm::vec3(1.2f)); // Centered coffee table
    addObject(sofa2Mdl, glm::vec3(-375.0f, 0.0f, 200.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f)); // Loveseat
    addObject(tvTableMdl, glm::vec3(-375.0f, 0.0f, 580.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(1.1f)); // TV credenza
    addObject(tvMdl, glm::vec3(-375.0f, 82.0f, 580.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(1.2f));

    addObject(indoorPlant1, glm::vec3(-580.0f, 0.0f, 680.0f), glm::vec3(0.0f), glm::vec3(1.3f));
    addObject(indoorPlant2, glm::vec3(-580.0f, 0.0f, 70.0f), glm::vec3(0.0f), glm::vec3(1.3f));

    // --- DINING ROOM ---
    addObject(diningTableMdl, glm::vec3(375.0f, 0.0f, 550.0f), glm::vec3(0.0f), glm::vec3(1.3f));
    addObject(diningChairMdl, glm::vec3(280.0f, 0.0f, 550.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f));
    addObject(diningChairMdl, glm::vec3(470.0f, 0.0f, 550.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f));
    addObject(diningChairMdl, glm::vec3(375.0f, 0.0f, 640.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(1.0f));
    addObject(diningChairMdl, glm::vec3(375.0f, 0.0f, 460.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    addObject(painting2Mdl, glm::vec3(375.0f, 170.0f, 740.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(1.2f));

    // --- CHEF'S KITCHEN ---
    // Back kitchen counter run along solid back wall (Z = -240)
    addObject(counterMdl, glm::vec3(250.0f, 0.0f, -240.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    addObject(ovenMdl, glm::vec3(375.0f, 0.0f, -220.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    addObject(hobMdl, glm::vec3(375.0f, 75.0f, -220.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    addObject(hoodMdl, glm::vec3(375.0f, 170.0f, -235.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    addObject(counterMdl, glm::vec3(500.0f, 0.0f, -240.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    addObject(sinkKitchenMdl, glm::vec3(500.0f, 0.0f, -240.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));

    // Side kitchen counters along right exterior wall (X = 616)
    addObject(counterMdl, glm::vec3(616.0f, 0.0f, -60.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f));
    addObject(microMdl, glm::vec3(585.0f, 88.5f, -60.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f));
    addObject(counterMdl, glm::vec3(616.0f, 0.0f, 20.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f));
    addObject(coffeeMdl, glm::vec3(585.0f, 88.5f, 20.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f));
    addObject(dishwasherMdl, glm::vec3(585.0f, 0.0f, 100.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f));

    // Dedicated Full Refrigerator Station
    addObject(fridgeMdl, glm::vec3(616.0f, 0.0f, 200.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f));

    // Modern Kitchen Island with Counter Cabinets & Toaster
    addObject(counterMdl, glm::vec3(345.0f, 0.0f, 30.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(1.0f));
    addObject(counterMdl, glm::vec3(405.0f, 0.0f, 30.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(1.0f));
    addObject(toasterMdl, glm::vec3(375.0f, 88.5f, 10.0f), glm::vec3(0.0f), glm::vec3(1.0f));

    // --- REAR BREAKFAST NOOK & PANTRY (Z = -250 to -750, X = 250 to 500) ---
    addObject(diningTableMdl, glm::vec3(375.0f, 0.0f, -500.0f), glm::vec3(0.0f), glm::vec3(1.1f));
    addObject(diningChairMdl, glm::vec3(300.0f, 0.0f, -500.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f));
    // Pantry / Nook Storage Credenza (firmly on floor at Y = 0, top surface at Y = 79.2) supporting books
    addObject(dresserMdl, glm::vec3(600.0f, 0.0f, -400.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f, 0.88f, 1.0f));
    addObject(bookMdl, glm::vec3(600.0f, 81.0f, -400.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f));
    // Mounted upper wall shelf with book neatly placed inside
    addObject(shelfMdl, glm::vec3(618.0f, 160.0f, -400.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.5f, 1.0f, 1.2f));
    addObject(bookMdl, glm::vec3(610.0f, 152.0f, -400.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(0.8f));
    addObject(painting1Mdl, glm::vec3(375.0f, 160.0f, -740.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(1.3f));
    addObject(indoorPlant1, glm::vec3(600.0f, 0.0f, -650.0f), glm::vec3(0.0f), glm::vec3(1.3f));

    // --- GROUND FLOOR POWDER ROOM & LAUNDRY ---
    // Grounded toilet securely inside bathroom facing inward (+90 deg, flush on tile floor at Y = 0)
    addObject(toiletMdl, glm::vec3(-612.0f, -20.0f, -450.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f));
    addObject(bathSinkMdl, glm::vec3(-500.0f, 0.0f, -700.0f), glm::vec3(0.0f), glm::vec3(1.0f));
    addObject(mirrorMdl, glm::vec3(-500.0f, 150.0f, -740.0f), glm::vec3(0.0f), glm::vec3(1.0f));
    addObject(washerMdl, glm::vec3(-585.0f, 0.0f, -620.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f));

    // --- STAIRCASE (In dedicated architectural stair alcove, ascending smoothly to 2nd floor) ---
    addObject(stairsMdl, glm::vec3(-250.0f, 0.0f, -125.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));

    // ========================================================
    // 7. SECOND FLOOR ARCHITECTURE (Y = 300 to 600)
    // ========================================================

    // Dedicated room hardwood and tile floors
    for (float x = -500.0f; x <= 500.0f; x += 250.0f)
    {
        for (float z = -625.0f; z <= 625.0f; z += 250.0f)
        {
            if (x == -250.0f && z == -125.0f)
                continue; // Stairwell opening directly above staircase!

            if (x <= -250.0f && z >= 250.0f)
                addObject(floorMasterBed, glm::vec3(x, 300.0f, z), glm::vec3(0.0f)); // Master Bedroom
            else if (x >= 250.0f && z >= 250.0f)
                addObject(floorBed2, glm::vec3(x, 300.0f, z), glm::vec3(0.0f)); // Bedroom 2
            else if (x >= 250.0f && z < 250.0f)
                addObject(floorStudy, glm::vec3(x, 300.0f, z), glm::vec3(0.0f)); // Executive Study
            else if (x <= -250.0f && z < -250.0f)
                addObject(floorBathMdl, glm::vec3(x, 300.0f, z), glm::vec3(0.0f)); // 2nd Floor Bath
            else
                addObject(floorWoodLiving, glm::vec3(x, 300.0f, z), glm::vec3(0.0f));
        }
    }

    // Exterior Walls - Second Floor Front (Z = +750)
    addObject(wallWindow, glm::vec3(-500.0f, 300.0f, 750.0f), glm::vec3(0.0f));
    addObject(windowMdl, glm::vec3(-500.0f, 300.0f, 750.0f), glm::vec3(0.0f));
    addObject(wallWindow, glm::vec3(-250.0f, 300.0f, 750.0f), glm::vec3(0.0f));
    addObject(windowMdl, glm::vec3(-250.0f, 300.0f, 750.0f), glm::vec3(0.0f));
    addObject(wallDoor, glm::vec3(0.0f, 300.0f, 750.0f), glm::vec3(0.0f));
    addDoor("Second Floor Balcony Door", doorMdl, glm::vec3(50.93f, 300.0f, 750.0f), 0.0f, -90.0f);
    addObject(wallWindow, glm::vec3(250.0f, 300.0f, 750.0f), glm::vec3(0.0f));
    addObject(windowMdl, glm::vec3(250.0f, 300.0f, 750.0f), glm::vec3(0.0f));
    addObject(wallWindow, glm::vec3(500.0f, 300.0f, 750.0f), glm::vec3(0.0f));
    addObject(windowMdl, glm::vec3(500.0f, 300.0f, 750.0f), glm::vec3(0.0f));

    // Exterior Walls - Second Floor Left (X = -625)
    addObject(wallWindow, glm::vec3(-625.0f, 300.0f, 625.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    addObject(windowMdl, glm::vec3(-625.0f, 300.0f, 625.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    addObject(wallWindow, glm::vec3(-625.0f, 300.0f, 375.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    addObject(windowMdl, glm::vec3(-625.0f, 300.0f, 375.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    addObject(wallWindow, glm::vec3(-625.0f, 300.0f, 125.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    addObject(windowMdl, glm::vec3(-625.0f, 300.0f, 125.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    addObject(wallSolid, glm::vec3(-625.0f, 300.0f, -125.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    addObject(wallSolid, glm::vec3(-625.0f, 300.0f, -375.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    addObject(wallWindow, glm::vec3(-625.0f, 300.0f, -625.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    addObject(windowMdl, glm::vec3(-625.0f, 300.0f, -625.0f), glm::vec3(0.0f, 90.0f, 0.0f));

    // Exterior Walls - Second Floor Back (Z = -750)
    addObject(wallSolid, glm::vec3(-500.0f, 300.0f, -750.0f), glm::vec3(0.0f, 180.0f, 0.0f));
    addObject(wallWindow, glm::vec3(-250.0f, 300.0f, -750.0f), glm::vec3(0.0f, 180.0f, 0.0f));
    addObject(windowMdl, glm::vec3(-250.0f, 300.0f, -750.0f), glm::vec3(0.0f, 180.0f, 0.0f));
    addObject(wallSolid, glm::vec3(0.0f, 300.0f, -750.0f), glm::vec3(0.0f, 180.0f, 0.0f));
    addObject(wallWindow, glm::vec3(250.0f, 300.0f, -750.0f), glm::vec3(0.0f, 180.0f, 0.0f));
    addObject(windowMdl, glm::vec3(250.0f, 300.0f, -750.0f), glm::vec3(0.0f, 180.0f, 0.0f));
    addObject(wallSolid, glm::vec3(500.0f, 300.0f, -750.0f), glm::vec3(0.0f, 180.0f, 0.0f));

    // Exterior Walls - Second Floor Right (X = +625)
    addObject(wallWindow, glm::vec3(625.0f, 300.0f, 625.0f), glm::vec3(0.0f, -90.0f, 0.0f));
    addObject(windowMdl, glm::vec3(625.0f, 300.0f, 625.0f), glm::vec3(0.0f, -90.0f, 0.0f));
    addObject(wallWindow, glm::vec3(625.0f, 300.0f, 375.0f), glm::vec3(0.0f, -90.0f, 0.0f));
    addObject(windowMdl, glm::vec3(625.0f, 300.0f, 375.0f), glm::vec3(0.0f, -90.0f, 0.0f));
    addObject(wallWindow, glm::vec3(625.0f, 300.0f, 125.0f), glm::vec3(0.0f, -90.0f, 0.0f));
    addObject(windowMdl, glm::vec3(625.0f, 300.0f, 125.0f), glm::vec3(0.0f, -90.0f, 0.0f));
    addObject(wallSolid, glm::vec3(625.0f, 300.0f, -125.0f), glm::vec3(0.0f, -90.0f, 0.0f));
    addObject(wallWindow, glm::vec3(625.0f, 300.0f, -375.0f), glm::vec3(0.0f, -90.0f, 0.0f));
    addObject(windowMdl, glm::vec3(625.0f, 300.0f, -375.0f), glm::vec3(0.0f, -90.0f, 0.0f));
    addObject(wallSolid, glm::vec3(625.0f, 300.0f, -625.0f), glm::vec3(0.0f, -90.0f, 0.0f));

    // Interior Partition Walls - Second Floor (Warm architectural linen drywall finish)
    // Master bedroom partition wall along Z = 250 (Stair landing is fully open from Z = 0 to 250)
    addObject(wallSolid, glm::vec3(-500.0f, 300.0f, 250.0f), glm::vec3(0.0f), glm::vec3(1.0f), wallTintInterior);
    addObject(wallDoor, glm::vec3(-250.0f, 300.0f, 250.0f), glm::vec3(0.0f), glm::vec3(1.0f), wallTintInterior);
    addDoor("Master Bedroom Door", doorMdl, glm::vec3(-199.07f, 300.0f, 250.0f), 0.0f, 90.0f);

    // Master Bedroom Hallway Privacy Wall along X = -125 (from Z = 250 to Z = 750)
    addObject(wallSolid, glm::vec3(-125.0f, 300.0f, 375.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f), wallTintInterior);
    addObject(wallSolid, glm::vec3(-125.0f, 300.0f, 625.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f), wallTintInterior);

    // Bedroom 2 wall along Z = 250
    addObject(wallSolid, glm::vec3(250.0f, 300.0f, 250.0f), glm::vec3(0.0f), glm::vec3(1.0f), wallTintInterior);
    addObject(wallSolid, glm::vec3(500.0f, 300.0f, 250.0f), glm::vec3(0.0f), glm::vec3(1.0f), wallTintInterior);

    // Bedroom 2 & Study Hallway Privacy Wall along X = +125
    addObject(wallDoor, glm::vec3(125.0f, 300.0f, 375.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f), wallTintInterior);
    addDoor("Bedroom 2 Door", doorMdl, glm::vec3(125.0f, 300.0f, 425.93f), -90.0f, -180.0f);
    addObject(wallSolid, glm::vec3(125.0f, 300.0f, 625.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f), wallTintInterior);

    // Study Hallway Wall & Door along X = +125
    addObject(wallDoor, glm::vec3(125.0f, 300.0f, 125.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f), wallTintInterior);
    addDoor("Study Door", doorMdl, glm::vec3(125.0f, 300.0f, 175.93f), -90.0f, -180.0f);
    addObject(wallSolid, glm::vec3(125.0f, 300.0f, -125.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f), wallTintInterior);

    // Study back partition along Z = -250
    addObject(wallSolid, glm::vec3(250.0f, 300.0f, -250.0f), glm::vec3(0.0f), glm::vec3(1.0f), wallTintInterior);
    addObject(wallSolid, glm::vec3(500.0f, 300.0f, -250.0f), glm::vec3(0.0f), glm::vec3(1.0f), wallTintInterior);

    // Second floor bath partition along Z = -250 and X = -125 (closing 125-unit gap to stairs completely!)
    addObject(wallSolid, glm::vec3(-500.0f, 300.0f, -250.0f), glm::vec3(0.0f), glm::vec3(1.0f), wallTintInterior);
    addObject(wallSolid, glm::vec3(-250.0f, 300.0f, -250.0f), glm::vec3(0.0f), glm::vec3(1.0f), wallTintInterior);
    // Hallway entry wall along X = -125 (covers Z from -250 to -750 seamlessly)
    addObject(wallDoor, glm::vec3(-125.0f, 300.0f, -375.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f), wallTintInterior);
    addDoor("Second Floor Bath Door", doorMdl, glm::vec3(-125.0f, 300.0f, -324.07f), -90.0f, -180.0f);
    addObject(wallSolid, glm::vec3(-125.0f, 300.0f, -625.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f), wallTintInterior);

    // Second floor stair alcove west wall & landing safety banister
    addObject(wallSolid, glm::vec3(-375.0f, 300.0f, -125.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f), wallTintInterior);
    addObject(fenceMdl, glm::vec3(-125.0f, 300.0f, -185.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(0.6f, 1.0f, 1.0f));
    addObject(fenceMdl, glm::vec3(-167.5f, 300.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.85f, 1.0f, 1.0f));

    // ========================================================
    // 8. BALCONY (Front Facing, Grand 500-Unit Width, Z = 750 to 1000)
    // ========================================================
    // Grand widened stone balcony floor platform spanning X = -250 to +250
    addObject(floorTileMdl, glm::vec3(-125.0f, 298.0f, 875.0f), glm::vec3(0.0f), glm::vec3(1.0f, 0.2f, 1.0f));
    addObject(floorTileMdl, glm::vec3(125.0f, 298.0f, 875.0f), glm::vec3(0.0f), glm::vec3(1.0f, 0.2f, 1.0f));

    // Structural architectural support columns from ground level at outer balcony corners
    addObject(fenceMdl, glm::vec3(-245.0f, 0.0f, 995.0f), glm::vec3(0.0f), glm::vec3(0.35f, 3.0f, 0.35f));
    addObject(fenceMdl, glm::vec3(245.0f, 0.0f, 995.0f), glm::vec3(0.0f), glm::vec3(0.35f, 3.0f, 0.35f));

    // Seamless front safety railing along Z = 1000.0f (5 continuous sections)
    for (float fx = -200.0f; fx <= 200.0f; fx += 100.0f)
    {
        addObject(fenceMdl, glm::vec3(fx, 300.0f, 1000.0f), glm::vec3(0.0f), glm::vec3(0.98f, 0.85f, 1.0f));
    }

    // Left side safety railing along X = -250.0f (Z = 750 to 1000, 2 seamless sections)
    addObject(fenceMdl, glm::vec3(-250.0f, 300.0f, 812.5f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.23f, 0.85f, 1.0f));
    addObject(fenceMdl, glm::vec3(-250.0f, 300.0f, 937.5f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.23f, 0.85f, 1.0f));

    // Right side safety railing along X = +250.0f (Z = 750 to 1000, 2 seamless sections)
    addObject(fenceMdl, glm::vec3(250.0f, 300.0f, 812.5f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.23f, 0.85f, 1.0f));
    addObject(fenceMdl, glm::vec3(250.0f, 300.0f, 937.5f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.23f, 0.85f, 1.0f));

    // Balcony furnishing: Twin sun loungers, central table, and corner planters
    addObject(loungerMdl, glm::vec3(-120.0f, 300.0f, 875.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.9f));
    addObject(loungerMdl, glm::vec3(120.0f, 300.0f, 875.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.9f));
    addObject(coffeeTableMdl, glm::vec3(0.0f, 300.0f, 875.0f), glm::vec3(0.0f), glm::vec3(0.8f));
    addObject(potMdl, glm::vec3(-220.0f, 300.0f, 970.0f), glm::vec3(0.0f), glm::vec3(1.2f));
    addObject(potMdl, glm::vec3(220.0f, 300.0f, 970.0f), glm::vec3(0.0f), glm::vec3(1.2f));

    // ========================================================
    // 9. SECOND FLOOR FURNISHING (DECLUTTERED & SPACIOUS)
    // ========================================================

    // --- MASTER BEDROOM SUITE ---
    // King Bed with headboard against solid interior wall at Z = 250 facing room
    addObject(bedMasterMdl, glm::vec3(-430.0f, 300.0f, 375.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.1f));
    addObject(bedTable1Mdl, glm::vec3(-550.0f, 300.0f, 375.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    addObject(bedTable1Mdl, glm::vec3(-310.0f, 300.0f, 375.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    addObject(carpet2Mdl, glm::vec3(-430.0f, 302.0f, 400.0f), glm::vec3(0.0f), glm::vec3(1.6f, 1.0f, 1.6f));
    addObject(wardrobeMdl, glm::vec3(-148.0f, 300.0f, 650.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.1f));
    addObject(dresserMdl, glm::vec3(-375.0f, 300.0f, 720.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(1.0f));
    addObject(painting1Mdl, glm::vec3(-430.0f, 460.0f, 260.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.2f));

    // --- SECOND FLOOR STAIR LANDING & GALLERY ---
    // Moved away from the stair opening ledge to sit safely against landing gallery wall
    addObject(indoorPlant1, glm::vec3(-160.0f, 300.0f, 180.0f), glm::vec3(0.0f), glm::vec3(1.2f));

    // --- BEDROOM 2 ---
    addObject(bedSingleMdl, glm::vec3(375.0f, 300.0f, 480.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(1.0f));
    addObject(bedTable2Mdl, glm::vec3(250.0f, 300.0f, 480.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(1.0f));
    addObject(wardrobeMdl, glm::vec3(500.0f, 300.0f, 275.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    addObject(carpet2Mdl, glm::vec3(375.0f, 302.0f, 600.0f), glm::vec3(0.0f), glm::vec3(1.2f));
    addObject(clockMdl, glm::vec3(375.0f, 460.0f, 260.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.9f));

    // --- EXECUTIVE STUDY / OFFICE ---
    // Desk centered in room with ample clearance
    addObject(pcTableMdl, glm::vec3(380.0f, 300.0f, 0.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.1f));
    addObject(pcChairMdl, glm::vec3(290.0f, 300.0f, 0.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f));
    // Cleanly spaced items on desk (NOT crammed or hanging off!)
    addObject(pcMdl, glm::vec3(400.0f, 381.5f, 35.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f));
    addObject(pcKeyMdl, glm::vec3(365.0f, 381.5f, 0.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f));
    addObject(pcMouseMdl, glm::vec3(365.0f, 381.5f, 25.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f));
    addObject(laptopMdl, glm::vec3(390.0f, 381.5f, -25.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(0.85f));

    // Executive Bookshelf Credenza (stands firmly on 2nd floor at Y = 300, top surface at Y = 379.2) supporting books and globe
    addObject(dresserMdl, glm::vec3(600.0f, 300.0f, -105.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.2f, 0.88f, 1.0f));
    addObject(bookMdl, glm::vec3(600.0f, 381.5f, -75.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f));
    addObject(globeMdl, glm::vec3(600.0f, 380.0f, -135.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f));
    // Mounted upper wall bookshelf at eye level with books inside
    addObject(shelfMdl, glm::vec3(618.0f, 460.0f, -105.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.8f, 1.1f, 1.3f));
    addObject(bookMdl, glm::vec3(610.0f, 452.0f, -105.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(0.85f));

    // Side Credenza for Printer mounted clear of back wall (Z = -190)
    addObject(tvTableMdl, glm::vec3(380.0f, 300.0f, -190.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.7f, 0.8f, 0.8f));
    addObject(printerMdl, glm::vec3(380.0f, 332.0f, -190.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.9f));
    addObject(indoorPlant3, glm::vec3(570.0f, 300.0f, 200.0f), glm::vec3(0.0f), glm::vec3(1.1f));

    // --- SECOND FLOOR FULL BATHROOM ---
    addObject(showerMdl, glm::vec3(-570.0f, 300.0f, -738.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.1f));
    // Grounded toilet securely inside bathroom facing inward (+90 deg, flush on 2nd floor tile at Y = 300)
    addObject(toiletMdl, glm::vec3(-612.0f, 280.0f, -450.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f));
    addObject(bathSinkMdl, glm::vec3(-450.0f, 300.0f, -700.0f), glm::vec3(0.0f), glm::vec3(1.0f));
    addObject(mirrorMdl, glm::vec3(-450.0f, 450.0f, -740.0f), glm::vec3(0.0f), glm::vec3(1.0f));

    // ========================================================
    // 10. SECOND FLOOR FLAT ROOF / CEILING SYSTEM
    // Seamlessly seals second-floor rooms at wall plate height (Y = 600)
    // ========================================================
    for (float x = -500.0f; x <= 500.0f; x += 250.0f)
    {
        for (float z = -625.0f; z <= 625.0f; z += 250.0f)
        {
            addObject(floorTileMdl, glm::vec3(x, 600.0f, z), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec4(0.86f, 0.85f, 0.82f, 1.0f));
        }
    }

    // ========================================================
    // 11. ROOF SYSTEM (MAIN HOUSE PITCHED ROOF & CHIMNEY)
    // Pitched Roof4 perfectly spans 1250 house width (scale X = 1.25, Y = 1.15)
    // 6 sections along Z from -750 to +750 (1500 house depth)
    // Base sits flush at Y = 284 atop second-floor walls (Y = 600) with zero eaves gap
    // ========================================================
    const glm::vec3 roofScale(1.25f, 1.15f, 1.0f);
    const float roofY = 284.0f;
    // Back gable end with closed gable face and overhang
    addObject(roofHouseCap, glm::vec3(0.0f, roofY, -625.0f), glm::vec3(0.0f, 0.0f, 0.0f), roofScale);
    // Middle roof shingle sections
    addObject(roofHouseMid, glm::vec3(0.0f, roofY, -375.0f), glm::vec3(0.0f, 0.0f, 0.0f), roofScale);
    addObject(roofHouseMid, glm::vec3(0.0f, roofY, -125.0f), glm::vec3(0.0f, 0.0f, 0.0f), roofScale);
    addObject(roofHouseMid, glm::vec3(0.0f, roofY, 125.0f), glm::vec3(0.0f, 0.0f, 0.0f), roofScale);
    addObject(roofHouseMid, glm::vec3(0.0f, roofY, 375.0f), glm::vec3(0.0f, 0.0f, 0.0f), roofScale);
    // Front gable end with closed gable face and overhang
    addObject(roofHouseCap, glm::vec3(0.0f, roofY, 625.0f), glm::vec3(0.0f, 180.0f, 0.0f), roofScale);

    // Brick Chimney centered firmly on the roof ridge
    addObject(chimneyMdl, glm::vec3(0.0f, 940.0f, -125.0f), glm::vec3(0.0f), glm::vec3(1.4f));

    // ========================================================
    // 11. DISTANT BUILDINGS (Suburban neighborhood backdrop - complete, fully-enclosed exteriors)
    // ========================================================
    auto buildNeighborExterior = [&](float cx, float cz, float rotFront, const glm::vec4 &tint)
    {
        // Foundation / porch slab (covers 500x500 footprint)
        for (float dx : {-125.0f, 125.0f})
            for (float dz : {-125.0f, 125.0f})
                addObject(floorSidewalkMdl, glm::vec3(cx + dx, 0.0f, cz + dz), glm::vec3(0.0f));

        if (rotFront == 180.0f) // Front faces South (-Z towards street, e.g. Houses across street)
        {
            // Front wall at cz - 250 (Door + Window)
            addObject(wallDoor, glm::vec3(cx - 125.0f, 0.0f, cz - 250.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(1.0f), tint);
            addObject(doorMdl, glm::vec3(cx - 125.0f + 50.93f, 0.0f, cz - 250.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(1.145f, 1.086f, 1.0f), glm::vec4(0.48f, 0.28f, 0.16f, 1.0f));
            addObject(wallWindow, glm::vec3(cx + 125.0f, 0.0f, cz - 250.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(1.0f), tint);
            addObject(windowMdl, glm::vec3(cx + 125.0f, 0.0f, cz - 250.0f), glm::vec3(0.0f, 180.0f, 0.0f));

            // Back wall at cz + 250 (Window + Solid)
            addObject(wallWindow, glm::vec3(cx - 125.0f, 0.0f, cz + 250.0f), glm::vec3(0.0f), glm::vec3(1.0f), tint);
            addObject(windowMdl, glm::vec3(cx - 125.0f, 0.0f, cz + 250.0f), glm::vec3(0.0f));
            addObject(wallSolid, glm::vec3(cx + 125.0f, 0.0f, cz + 250.0f), glm::vec3(0.0f), glm::vec3(1.0f), tint);

            // Left wall at cx - 250 (Solid + Window)
            addObject(wallSolid, glm::vec3(cx - 250.0f, 0.0f, cz - 125.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f), tint);
            addObject(wallWindow, glm::vec3(cx - 250.0f, 0.0f, cz + 125.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f), tint);
            addObject(windowMdl, glm::vec3(cx - 250.0f, 0.0f, cz + 125.0f), glm::vec3(0.0f, 90.0f, 0.0f));

            // Right wall at cx + 250 (Window + Solid)
            addObject(wallWindow, glm::vec3(cx + 250.0f, 0.0f, cz - 125.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f), tint);
            addObject(windowMdl, glm::vec3(cx + 250.0f, 0.0f, cz - 125.0f), glm::vec3(0.0f, -90.0f, 0.0f));
            addObject(wallSolid, glm::vec3(cx + 250.0f, 0.0f, cz + 125.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f), tint);

            // Seamless Pitched Gable Roof with zero gap atop Y=300 walls
            addObject(roofGarageCap, glm::vec3(cx, 0.0f, cz - 125.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
            addObject(roofGarageCap, glm::vec3(cx, 0.0f, cz + 125.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(1.0f));
            addObject(chimneyMdl, glm::vec3(cx, 505.0f, cz - 30.0f), glm::vec3(0.0f), glm::vec3(1.0f, 1.25f, 1.0f));
        }
        else // Front faces North (+Z towards street, e.g. Side Neighbors)
        {
            // Front wall at cz + 250 (Door + Window)
            addObject(wallDoor, glm::vec3(cx - 125.0f, 0.0f, cz + 250.0f), glm::vec3(0.0f), glm::vec3(1.0f), tint);
            addObject(doorMdl, glm::vec3(cx - 125.0f + 50.93f, 0.0f, cz + 250.0f), glm::vec3(0.0f), glm::vec3(1.145f, 1.086f, 1.0f), glm::vec4(0.48f, 0.28f, 0.16f, 1.0f));
            addObject(wallWindow, glm::vec3(cx + 125.0f, 0.0f, cz + 250.0f), glm::vec3(0.0f), glm::vec3(1.0f), tint);
            addObject(windowMdl, glm::vec3(cx + 125.0f, 0.0f, cz + 250.0f), glm::vec3(0.0f));

            // Back wall at cz - 250 (Solid + Window)
            addObject(wallSolid, glm::vec3(cx - 125.0f, 0.0f, cz - 250.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(1.0f), tint);
            addObject(wallWindow, glm::vec3(cx + 125.0f, 0.0f, cz - 250.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(1.0f), tint);
            addObject(windowMdl, glm::vec3(cx + 125.0f, 0.0f, cz - 250.0f), glm::vec3(0.0f, 180.0f, 0.0f));

            // Left wall at cx - 250 (Solid + Window)
            addObject(wallSolid, glm::vec3(cx - 250.0f, 0.0f, cz - 125.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f), tint);
            addObject(wallWindow, glm::vec3(cx - 250.0f, 0.0f, cz + 125.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f), tint);
            addObject(windowMdl, glm::vec3(cx - 250.0f, 0.0f, cz + 125.0f), glm::vec3(0.0f, 90.0f, 0.0f));

            // Right wall at cx + 250 (Window + Solid)
            addObject(wallWindow, glm::vec3(cx + 250.0f, 0.0f, cz - 125.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f), tint);
            addObject(windowMdl, glm::vec3(cx + 250.0f, 0.0f, cz - 125.0f), glm::vec3(0.0f, -90.0f, 0.0f));
            addObject(wallSolid, glm::vec3(cx + 250.0f, 0.0f, cz + 125.0f), glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3(1.0f), tint);

            // Seamless Pitched Gable Roof with zero gap atop Y=300 walls
            addObject(roofGarageCap, glm::vec3(cx, 0.0f, cz - 125.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
            addObject(roofGarageCap, glm::vec3(cx, 0.0f, cz + 125.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(1.0f));
            addObject(chimneyMdl, glm::vec3(cx, 505.0f, cz - 30.0f), glm::vec3(0.0f), glm::vec3(1.0f, 1.25f, 1.0f));
        }
    };

    // House 1 across the street (Center-Left: X = -1000, Z = 3400) - Warm Cream Siding
    buildNeighborExterior(-1000.0f, 3400.0f, 180.0f, glm::vec4(0.92f, 0.89f, 0.82f, 1.0f));
    addObject(treeTallMdl, glm::vec3(-1350.0f, 0.0f, 3200.0f), glm::vec3(0.0f), glm::vec3(3.2f));

    // House 2 across the street (Center-Right: X = +1250, Z = 3500) - Modern Slate Blue
    buildNeighborExterior(1250.0f, 3500.0f, 180.0f, glm::vec4(0.84f, 0.88f, 0.92f, 1.0f));
    addObject(treeTallMdl, glm::vec3(1650.0f, 0.0f, 3300.0f), glm::vec3(0.0f), glm::vec3(3.0f));

    // House 3 - Far left neighbor (X = -2750, Z = 800) - Cozy Cedar Tan
    buildNeighborExterior(-2750.0f, 800.0f, 0.0f, glm::vec4(0.88f, 0.83f, 0.74f, 1.0f));
    addObject(treeTallMdl, glm::vec3(-2350.0f, 0.0f, 1100.0f), glm::vec3(0.0f), glm::vec3(3.5f));

    // House 4 - Far right neighbor (X = +2750, Z = 800) - Classic Linen Gray
    buildNeighborExterior(2750.0f, 800.0f, 0.0f, glm::vec4(0.86f, 0.87f, 0.85f, 1.0f));
    addObject(treeTallMdl, glm::vec3(2350.0f, 0.0f, 1100.0f), glm::vec3(0.0f), glm::vec3(3.5f));

    // ========================================================
    // 12. DYNAMIC & ANIMATED PROPS (Ceiling Fans, Standing Fan, Moving Cars)
    // ========================================================
    // A. Living Room Ceiling Fan (Mounted on ceiling at Y = 300, X = -375, Z = 375)
    auto ceilingFanBase = FanModels::createCeilingFanBase();
    auto ceilingFanBlades = FanModels::createCeilingFanBlades();
    addCustomObject(ceilingFanBase, glm::vec3(-375.0f, 300.0f, 375.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec4(0.25f, 0.25f, 0.27f, 1.0f));
    size_t fan1BladeIdx = objects.size();
    addCustomObject(ceilingFanBlades, glm::vec3(-375.0f, 300.0f, 375.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec4(0.42f, 0.26f, 0.15f, 1.0f));
    animatedObjects.push_back({fan1BladeIdx, 1, glm::vec3(-375.0f, 300.0f, 375.0f), glm::vec3(0.0f), glm::vec3(1.0f), 620.0f, 0.0f});

    // B. Dining Room Ceiling Fan (Mounted on ceiling at Y = 300, X = 375, Z = 550, directly above dining table)
    addCustomObject(ceilingFanBase, glm::vec3(375.0f, 300.0f, 550.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec4(0.25f, 0.25f, 0.27f, 1.0f));
    size_t diningFanBladeIdx = objects.size();
    addCustomObject(ceilingFanBlades, glm::vec3(375.0f, 300.0f, 550.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec4(0.42f, 0.26f, 0.15f, 1.0f));
    animatedObjects.push_back({diningFanBladeIdx, 6, glm::vec3(375.0f, 300.0f, 550.0f), glm::vec3(0.0f), glm::vec3(1.0f), 620.0f, 0.0f});

    // C. Executive Study Standing Pedestal Fan (Floor at Y = 300, X = 230, Z = -40, angled toward desk)
    auto standPedestal = FanModels::createStandFanPedestal();
    auto standHead = FanModels::createStandFanHead();
    auto standBlades = FanModels::createStandFanBlades();
    addCustomObject(standPedestal, glm::vec3(230.0f, 300.0f, -40.0f), glm::vec3(0.0f, 45.0f, 0.0f), glm::vec3(1.0f), glm::vec4(0.22f, 0.22f, 0.25f, 1.0f));
    size_t standHeadIdx = objects.size();
    addCustomObject(standHead, glm::vec3(230.0f, 300.0f, -40.0f), glm::vec3(0.0f, 45.0f, 0.0f), glm::vec3(1.0f), glm::vec4(0.20f, 0.20f, 0.22f, 1.0f));
    animatedObjects.push_back({standHeadIdx, 2, glm::vec3(230.0f, 300.0f, -40.0f), glm::vec3(0.0f, 45.0f, 0.0f), glm::vec3(1.0f), 1.0f, 0.0f});
    size_t standBladesIdx = objects.size();
    addCustomObject(standBlades, glm::vec3(230.0f, 300.0f, -40.0f), glm::vec3(0.0f, 45.0f, 0.0f), glm::vec3(1.0f), glm::vec4(0.15f, 0.60f, 0.85f, 1.0f));
    animatedObjects.push_back({standBladesIdx, 3, glm::vec3(230.0f, 300.0f, -40.0f), glm::vec3(0.0f, 45.0f, 0.0f), glm::vec3(1.0f), 1400.0f, 0.0f});

    // C. Moving Traffic Cars on Suburban Road (Widely separated non-colliding lanes)
    // Car 1 (Eastbound in near lane at Z = 2140)
    size_t carEastIdx = objects.size();
    addObject(carMdl, glm::vec3(-4500.0f, 0.0f, 2140.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(22.0f));
    animatedObjects.push_back({carEastIdx, 4, glm::vec3(0.0f, 0.0f, 2140.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(22.0f), 780.0f, 0.0f});

    // Car 2 (Westbound in far lane at Z = 2460, sporty crimson finish - clear 50-unit lane gap)
    size_t carWestIdx = objects.size();
    addObject(carMdl, glm::vec3(4500.0f, 0.0f, 2460.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(22.0f), glm::vec4(0.95f, 0.35f, 0.35f, 1.0f));
    animatedObjects.push_back({carWestIdx, 5, glm::vec3(0.0f, 0.0f, 2460.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(22.0f), 700.0f, 0.0f});

    // ========================================================
    // 13. POINT LIGHTS (Interior & Porch warm illumination)
    // Soft, realistic indoor intensities that do not blow out to blinding white
    // ========================================================
    // 1. Entrance / Porch Light
    pointLights.push_back({glm::vec3(0.0f, 250.0f, 720.0f), glm::vec3(1.0f, 0.85f, 0.65f), 0.80f, 700.0f, true, "Entrance & Porch"});
    // 2. Living Room Chandelier
    pointLights.push_back({glm::vec3(-340.0f, 260.0f, 375.0f), glm::vec3(1.0f, 0.82f, 0.60f), 0.90f, 850.0f, true, "Living Room Chandelier"});
    // 3. Dining Room Pendant Light
    pointLights.push_back({glm::vec3(375.0f, 250.0f, 550.0f), glm::vec3(1.0f, 0.88f, 0.70f), 0.85f, 750.0f, true, "Dining Room Pendant"});
    // 4. Kitchen Ceiling Light
    pointLights.push_back({glm::vec3(420.0f, 260.0f, -50.0f), glm::vec3(1.0f, 0.95f, 0.85f), 0.85f, 750.0f, true, "Kitchen Ceiling Light"});
    // 5. Garage Light
    pointLights.push_back({glm::vec3(875.0f, 260.0f, 125.0f), glm::vec3(0.9f, 0.95f, 1.0f), 0.75f, 800.0f, true, "Garage Workshop Light"});
    // 6. Master Bedroom Light
    pointLights.push_back({glm::vec3(-375.0f, 550.0f, 380.0f), glm::vec3(1.0f, 0.85f, 0.65f), 0.85f, 800.0f, true, "Master Bedroom Chandelier"});
    // 7. Study / Office Lamp
    pointLights.push_back({glm::vec3(380.0f, 540.0f, 0.0f), glm::vec3(1.0f, 0.90f, 0.75f), 0.85f, 800.0f, true, "Study / Office Lamp"});
    // 8. Bedroom 2 Light
    pointLights.push_back({glm::vec3(375.0f, 550.0f, 480.0f), glm::vec3(1.0f, 0.88f, 0.70f), 0.80f, 750.0f, true, "Bedroom 2 Ceiling Light"});
    // 9. Ground Floor Powder Room Light
    pointLights.push_back({glm::vec3(-400.0f, 220.0f, -450.0f), glm::vec3(1.0f, 0.90f, 0.75f), 0.85f, 650.0f, true, "Ground Floor Powder Room"});
    // 10. Second Floor Bathroom Light
    pointLights.push_back({glm::vec3(-400.0f, 520.0f, -450.0f), glm::vec3(1.0f, 0.90f, 0.75f), 0.85f, 650.0f, true, "Second Floor Bathroom"});

    std::cout << "Scene constructed successfully! Total objects: " << objects.size() << "\n";
    buildCollisionBoxes();
}

void Scene::render(const Shader &shader)
{
    // Pass point lights to shader
    shader.setInt("numPointLights", static_cast<int>(pointLights.size()));
    for (size_t i = 0; i < pointLights.size() && i < 12; ++i)
    {
        std::string base = "pointLights[" + std::to_string(i) + "].";
        shader.setVec3(base + "position", pointLights[i].position);
        shader.setVec3(base + "color", pointLights[i].color);
        shader.setFloat(base + "intensity", pointLights[i].enabled ? pointLights[i].intensity : 0.0f);
        shader.setFloat(base + "radius", pointLights[i].radius);
    }

    // Render all scene objects with proper per-object or model texture binding
    GLuint currentBoundTexture = 0;
    for (const auto &obj : objects)
    {
        shader.setMat4("model", obj.transform);
        glUniformMatrix3fv(glGetUniformLocation(shader.ID, "normalMatrix"), 1, GL_FALSE, &obj.normalMatrix[0][0]);
        shader.setVec4("tintColor", obj.tint);

        std::shared_ptr<Texture> texToBind = obj.customTexture ? obj.customTexture : (obj.model ? obj.model->texture : nullptr);
        if (texToBind && texToBind->ID != currentBoundTexture)
        {
            texToBind->bind(0);
            currentBoundTexture = texToBind->ID;
        }

        if (obj.model)
            obj.model->draw();
    }
}

void Scene::buildCollisionBoxes()
{
    collisionBoxes.clear();

    for (const auto &obj : objects)
    {
        if (!obj.model)
            continue;

        const std::string &path = obj.model->filePath;

        bool isWallSolid = (path.find("Wall1_C1_01") != std::string::npos);
        bool isWallWindow = (path.find("Wall1_C1_03") != std::string::npos);
        bool isWallDoor = (path.find("Wall1_C1_02") != std::string::npos);
        bool isFence = (path.find("Fence_01") != std::string::npos);
        bool isFireplace = (path.find("Fireplace_01") != std::string::npos);

        if (!isWallSolid && !isWallWindow && !isWallDoor && !isFence && !isFireplace)
            continue;

        auto transformAABB = [&](const glm::vec3 &localMin, const glm::vec3 &localMax) -> CollisionBox
        {
            glm::vec3 corners[8] = {
                {localMin.x, localMin.y, localMin.z},
                {localMax.x, localMin.y, localMin.z},
                {localMin.x, localMax.y, localMin.z},
                {localMax.x, localMax.y, localMin.z},
                {localMin.x, localMin.y, localMax.z},
                {localMax.x, localMin.y, localMax.z},
                {localMin.x, localMax.y, localMax.z},
                {localMax.x, localMax.y, localMax.z}
            };

            glm::vec3 worldMin(1e9f);
            glm::vec3 worldMax(-1e9f);

            for (int i = 0; i < 8; ++i)
            {
                glm::vec4 w = obj.transform * glm::vec4(corners[i], 1.0f);
                worldMin = glm::min(worldMin, glm::vec3(w));
                worldMax = glm::max(worldMax, glm::vec3(w));
            }

            return {worldMin, worldMax, -1};
        };

        if (isWallSolid || isWallWindow)
        {
            collisionBoxes.push_back(transformAABB(glm::vec3(-125.0f, 0.0f, -7.5f), glm::vec3(125.0f, 300.0f, 7.5f)));
        }
        else if (isWallDoor)
        {
            // Left jamb post
            collisionBoxes.push_back(transformAABB(glm::vec3(-125.0f, 0.0f, -7.5f), glm::vec3(-50.93f, 300.0f, 7.5f)));
            // Right jamb post
            collisionBoxes.push_back(transformAABB(glm::vec3(50.93f, 0.0f, -7.5f), glm::vec3(125.0f, 300.0f, 7.5f)));

            // Middle doorway opening controlled by nearest interactive door
            glm::vec4 center4 = obj.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec3 wallCenter(center4);

            int matchedDoorIdx = -1;
            float bestDist = 160.0f;
            for (size_t d = 0; d < interactiveDoors.size(); ++d)
            {
                float dist = glm::distance(interactiveDoors[d].hingePos, wallCenter);
                if (dist < bestDist)
                {
                    bestDist = dist;
                    matchedDoorIdx = static_cast<int>(d);
                }
            }

            if (matchedDoorIdx >= 0)
            {
                CollisionBox doorBox = transformAABB(glm::vec3(-50.93f, 0.0f, -7.5f), glm::vec3(50.93f, 221.5f, 7.5f));
                doorBox.doorIndex = matchedDoorIdx;
                collisionBoxes.push_back(doorBox);
            }
        }
        else if (isFence)
        {
            collisionBoxes.push_back(transformAABB(glm::vec3(-50.0f, 0.0f, -5.0f), glm::vec3(50.0f, 122.0f, 5.0f)));
        }
        else if (isFireplace)
        {
            collisionBoxes.push_back(transformAABB(obj.model->minBounds, obj.model->maxBounds));
        }
    }

    // Central handrail divider between Flight 1 and Flight 2 in switchback stairs (prevents walking across the stair void)
    collisionBoxes.push_back({glm::vec3(-212.0f, 0.0f, -165.0f), glm::vec3(-202.0f, 320.0f, 5.0f), -1});
    // Ground floor barrier under the upper flight so players cannot walk into the underside of the stairs
    collisionBoxes.push_back({glm::vec3(-375.0f, 0.0f, -40.0f), glm::vec3(-202.0f, 220.0f, 5.0f), -1});

    std::cout << "[Physics] Built " << collisionBoxes.size() << " world collision boundaries & wall colliders.\n" << std::flush;
}

float Scene::getFloorHeight(float x, float z, float currentY) const
{
    // If camera is in high aerial overview mode:
    if (currentY > 700.0f)
        return currentY - 165.0f;

    // 1. Switchback Staircase: X in [-375, -125], Z in [-250, 0]
    // The staircase is a U-shaped / switchback design:
    // - East flight (X in [-210, -125]): ascending from Ground Floor (Z=0, Y=0) to Landing (Z=-165, Y=150)
    // - Intermediate Landing (Z in [-250, -165]): flat turn-around at Y=150
    // - West flight (X in [-375, -210]): ascending from Landing (Z=-165, Y=150) to Second Floor (Z=0, Y=300)
    if (x >= -375.0f && x <= -125.0f && z >= -250.0f && z <= 0.0f)
    {
        // Intermediate landing at the back
        if (z <= -165.0f)
        {
            return 150.0f;
        }

        // East flight (ascending from ground level towards the landing)
        if (x > -210.0f)
        {
            float t = std::clamp(-z / 165.0f, 0.0f, 1.0f);
            return t * 150.0f;
        }
        else // West flight (ascending from the landing towards the second floor)
        {
            float t = std::clamp((z + 165.0f) / 165.0f, 0.0f, 1.0f);
            return 150.0f + t * 150.0f;
        }
    }

    // 2. Second Floor Footprint & Balcony
    bool onSecondFloorArea = (x >= -625.0f && x <= 625.0f && z >= -750.0f && z <= 750.0f) ||
                             (x >= -260.0f && x <= 260.0f && z >= 750.0f && z <= 1000.0f);

    if (onSecondFloorArea)
    {
        // Second floor interior floor height is 300.0f (eye level ~465.0f)
        if (currentY > 220.0f)
        {
            return 300.0f;
        }
    }

    // 3. Ground Level (Interior ground floor & outside yard/street)
    return 0.0f;
}

void Scene::resolvePlayerMovement(glm::vec3 &position, const glm::vec3 &movementDelta, float playerRadius) const
{
    float r = playerRadius;
    float playerFeetY = position.y - 165.0f;
    float playerHeadY = position.y + 10.0f;

    auto isBoxBlocking = [&](const CollisionBox &box, const glm::vec3 &pos) -> bool
    {
        // Vertical check (skip walls on other floors)
        if (playerHeadY <= box.minB.y + 5.0f || playerFeetY >= box.maxB.y - 5.0f)
            return false;

        // Interactive door check
        if (box.doorIndex >= 0 && box.doorIndex < static_cast<int>(interactiveDoors.size()))
        {
            const auto &door = interactiveDoors[box.doorIndex];
            if (door.isOpen)
            {
                float angleDiff = std::abs(door.currentAngle - door.openRotY);
                if (angleDiff < 45.0f)
                    return false; // Open doorway! Free passage
            }
        }

        // Horizontal AABB overlap check
        if (pos.x > (box.minB.x - r) && pos.x < (box.maxB.x + r) &&
            pos.z > (box.minB.z - r) && pos.z < (box.maxB.z + r))
        {
            return true;
        }

        return false;
    };

    // Step X with wall sliding
    float oldX = position.x;
    position.x += movementDelta.x;
    for (const auto &box : collisionBoxes)
    {
        if (isBoxBlocking(box, position))
        {
            float xMin = box.minB.x - r;
            float xMax = box.maxB.x + r;
            if (oldX <= box.minB.x)
                position.x = xMin;
            else if (oldX >= box.maxB.x)
                position.x = xMax;
            else
            {
                if (std::abs(position.x - xMin) < std::abs(position.x - xMax))
                    position.x = xMin;
                else
                    position.x = xMax;
            }
        }
    }

    // Step Z with wall sliding
    float oldZ = position.z;
    position.z += movementDelta.z;
    for (const auto &box : collisionBoxes)
    {
        if (isBoxBlocking(box, position))
        {
            float zMin = box.minB.z - r;
            float zMax = box.maxB.z + r;
            if (oldZ <= box.minB.z)
                position.z = zMin;
            else if (oldZ >= box.maxB.z)
                position.z = zMax;
            else
            {
                if (std::abs(position.z - zMin) < std::abs(position.z - zMax))
                    position.z = zMin;
                else
                    position.z = zMax;
            }
        }
    }

    // World Boundary Limits (Street & suburban neighborhood perimeter)
    position.x = std::clamp(position.x, -3700.0f, 3700.0f);
    position.z = std::clamp(position.z, -900.0f, 3500.0f);
}

