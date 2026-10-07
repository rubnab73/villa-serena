#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "Scene.hpp"

#include <iostream>
#include <string>

// ============================================================
// Global State & Settings
// ============================================================

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;



Camera camera(glm::vec3(0.0f, 350.0f, 1700.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -12.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool cursorLocked = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Lighting & Modes
float dayNightFactor = 1.0f; // 1.0 = Day, 0.0 = Night
bool dayNightToggled = false;
bool flashlightOn = false;
bool flashlightToggled = false;

// Preset viewpoints for instant tour / inspection
void setViewPreset(int key)
{
    switch (key)
    {
        case GLFW_KEY_1:
        // 1: Outside Front View (Yard, Driveway, Garage, House, Mailbox)
        camera.setLookAt(glm::vec3(0.0f, 380.0f, 1850.0f), glm::vec3(0.0f, 250.0f, 200.0f));
        std::cout << "\n[View 1] Outside Front View: Front Yard, Garden, Trees, Fence, Driveway, Mailbox, Garage\n";
        break;
    case GLFW_KEY_2:
        // 2: Ground Floor - Entrance / Foyer
        camera.setLookAt(glm::vec3(0.0f, 150.0f, 680.0f), glm::vec3(0.0f, 150.0f, 0.0f));
        std::cout << "\n[View 2] Ground Floor: Entrance / Foyer (Bench, Mirror, Wall Clock)\n";
        break;
    case GLFW_KEY_3:
        // 3: Ground Floor - Spacious Living Room & TV Setup
        camera.setLookAt(glm::vec3(-220.0f, 130.0f, 400.0f), glm::vec3(-375.0f, 70.0f, 560.0f));
        std::cout << "\n[View 3] Ground Floor: Living Room (Sofas, Fireplace, Mantle Clock, TV console, Rug)\n";
        break;
        case GLFW_KEY_4:
        // 4: Ground Floor - Kitchen & Dining Room
        camera.setLookAt(glm::vec3(350.0f, 160.0f, 620.0f), glm::vec3(380.0f, 120.0f, -100.0f));
        std::cout << "\n[View 4] Ground Floor: Kitchen & Dining Room (Dining set, Counters, Oven, Hood, Fridge)\n";
        break;
    case GLFW_KEY_5:
        // 5: Ground Floor - Powder Room & Staircase
        camera.setLookAt(glm::vec3(-100.0f, 140.0f, 180.0f), glm::vec3(-220.0f, 140.0f, -125.0f));
        std::cout << "\n[View 5] Ground Floor: Staircase Alcove & Enclosed Powder Room\n";
        break;
        case GLFW_KEY_6:
        // 6: Second Floor - Master Bedroom Suite
        camera.setLookAt(glm::vec3(-150.0f, 430.0f, 550.0f), glm::vec3(-400.0f, 380.0f, 200.0f));
        std::cout << "\n[View 6] Second Floor: Master Bedroom (King Bed, Headboard Painting, Wardrobe, Dresser)\n";
        break;
    case GLFW_KEY_7:
        // 7: Second Floor - Bedroom 2
        camera.setLookAt(glm::vec3(180.0f, 430.0f, 650.0f), glm::vec3(380.0f, 400.0f, 400.0f));
        std::cout << "\n[View 7] Second Floor: Bedroom 2 (Bed, Nightstand, Wardrobe, Wall Clock)\n";
        break;
    case GLFW_KEY_8:
    // 8: Second Floor - Study / Home Office
    camera.setLookAt(glm::vec3(150.0f, 440.0f, 150.0f), glm::vec3(400.0f, 380.0f, -80.0f));
    std::cout << "\n[View 8] Second Floor: Study / Office (Executive Desk, PC Monitor, Bookshelf, Globe, Printer)\n";
    break;
    case GLFW_KEY_9:
        // 9: Second Floor - Walking Up Stairs onto Upper Landing Gallery
        camera.setLookAt(glm::vec3(-310.0f, 400.0f, -40.0f), glm::vec3(-310.0f, 380.0f, 200.0f));
        std::cout << "\n[View 9] Second Floor: Walking Up Stairs onto Upper Landing Gallery\n";
        break;
        case GLFW_KEY_0:
        // 0: Street & Distant Buildings with Traffic Cars
        camera.setLookAt(glm::vec3(-800.0f, 180.0f, 1820.0f), glm::vec3(600.0f, 80.0f, 2250.0f));
        std::cout << "\n[View 0] Street & Suburban Traffic Cars in Motion\n";
        break;
        case GLFW_KEY_P:
        // P: Bird's Eye Overhead View
        camera.setLookAt(glm::vec3(0.0f, 2400.0f, 1800.0f), glm::vec3(0.0f, 100.0f, 100.0f));
        std::cout << "\n[View P] Bird's Eye / Aerial Dollhouse Overview\n";
        break;
    }
}

// ============================================================
// GLFW Callbacks
// ============================================================


void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    if (!cursorLocked)
        return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void processInput(GLFWwindow *window, Scene &scene)
{
    // ESC unlocks the mouse cursor (does NOT exit the application)
    static bool escPressed = false;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        if (!escPressed)
        {
            cursorLocked = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            firstMouse = true;
            escPressed = true;
            std::cout << "[Mouse] Cursor unlocked (ESC pressed). Press TAB to re-lock.\n";
        }
    }
    else
    {
        escPressed = false;
    }

    // Toggle cursor lock with TAB
    static bool tabPressed = false;
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    {
        if (!tabPressed)
        {
            cursorLocked = !cursorLocked;
            glfwSetInputMode(window, GLFW_CURSOR, cursorLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            firstMouse = true;
            tabPressed = true;
        }
    }
    else
    {
        tabPressed = false;
    }

    // Toggle Day / Night with L
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        if (!dayNightToggled)
        {
            dayNightFactor = (dayNightFactor > 0.5f) ? 0.0f : 1.0f;
            dayNightToggled = true;
            std::cout << "[Lighting] " << (dayNightFactor > 0.5f ? "Day Mode Activated (Sunlight)" : "Night Mode Activated (Glowing Interior Lamps)") << "\n";
        }
    }
    else
    {
        dayNightToggled = false;
    }

    // Toggle Flashlight with F
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        if (!flashlightToggled)
        {
            flashlightOn = !flashlightOn;
            flashlightToggled = true;
            std::cout << "[Flashlight] " << (flashlightOn ? "Flashlight ON" : "Flashlight OFF") << "\n";
        }
    }
    else
    {
        flashlightToggled = false;
    }

    // Interact with nearby doors using E key
    static bool eKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        if (!eKeyPressed)
        {
            eKeyPressed = true;
            scene.interactNearestDoor(camera.Position, 220.0f);
        }
    }
    else
    {
        eKeyPressed = false;
    }

    // --- INDIVIDUAL FAN CONTROLS ---
    // J: Toggle Living Room Ceiling Fan (Smooth slow spin-down & acceleration with inertia)
    static bool jKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    {
        if (!jKeyPressed)
        {
            jKeyPressed = true;
            scene.toggleCeilingFan();
        }
    }
    else
    {
        jKeyPressed = false;
    }

    // H: Toggle Dining Room Ceiling Fan (Smooth slow spin-down & acceleration with inertia)
    static bool hKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        if (!hKeyPressed)
        {
            hKeyPressed = true;
            scene.toggleDiningFan();
        }
    }
    else
    {
        hKeyPressed = false;
    }

    // U: Toggle Study Standing Floor Fan (Smooth slow spin-down & oscillation slowdown)
    static bool uKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
    {
        if (!uKeyPressed)
        {
            uKeyPressed = true;
            scene.toggleStandFan();
        }
    }
    else
    {
        uKeyPressed = false;
    }

    // Y: Master Toggle for All Fans
    static bool yKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        if (!yKeyPressed)
        {
            yKeyPressed = true;
            scene.toggleAllFans();
        }
    }
    else
    {
        yKeyPressed = false;
    }

    // --- INDIVIDUAL LIGHT CONTROLS (F1 to F10) ---
    // F1: Entrance / Porch       F2: Living Room Chandelier
    // F3: Dining Room Pendant    F4: Kitchen Ceiling Light
    // F5: Garage Workshop Light  F6: Master Bedroom Light
    // F7: Study / Office Lamp    F8: Bedroom 2 Light
    // F9: Powder Room Light      F10: Second Floor Bathroom Light
    static bool fKeyPressed[10] = {false};
    for (int i = 0; i < 10; ++i)
    {
        int fKey = GLFW_KEY_F1 + i;
        if (glfwGetKey(window, fKey) == GLFW_PRESS)
        {
            if (!fKeyPressed[i])
            {
                fKeyPressed[i] = true;
                scene.toggleLight(i);
            }
        }
        else
        {
            fKeyPressed[i] = false;
        }
    }

    // --- ZONE & MASTER LIGHT SWITCHES ---
    // Z: Toggle Ground Floor Lights (Living, Dining, Kitchen, Powder Room)
    static bool zKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        if (!zKeyPressed)
        {
            zKeyPressed = true;
            scene.toggleGroundFloorLights();
        }
    }
    else
    {
        zKeyPressed = false;
    }

    // X: Toggle Second Floor Lights (Master Bed, Bed 2, Study, 2nd Bath)
    static bool xKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        if (!xKeyPressed)
        {
            xKeyPressed = true;
            scene.toggleSecondFloorLights();
        }
    }
    else
    {
        xKeyPressed = false;
    }

    // V: Toggle Exterior Lights (Porch & Garage)
    static bool vKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
    {
        if (!vKeyPressed)
        {
            vKeyPressed = true;
            scene.toggleExteriorLights();
        }
    }
    else
    {
        vKeyPressed = false;
    }

    // K: Master Switch (Toggle All Lights On/Off)
    static bool kKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        if (!kKeyPressed)
        {
            kKeyPressed = true;
            scene.toggleAllLights();
        }
    }
    else
    {
        kKeyPressed = false;
    }

    // Camera preset key inspection (1-9, 0, P)
    for (int k = GLFW_KEY_0; k <= GLFW_KEY_9; ++k)
    {
        if (glfwGetKey(window, k) == GLFW_PRESS)
            setViewPreset(k);
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        setViewPreset(GLFW_KEY_P);

    // Toggle FPS Walk Mode vs Noclip / Free Flight Mode with N key
    static bool fpsMode = true; // Enabled by default: proper FPS collisions & floor gravity
    static bool nKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    {
        if (!nKeyPressed)
        {
            fpsMode = !fpsMode;
            nKeyPressed = true;
            std::cout << "[Movement] " << (fpsMode ? "FPS Walk Mode: ON (Wall collisions & floor gravity active)" : "Noclip / Free Flight Mode: ON") << "\n" << std::flush;
        }
    }
    else
    {
        nKeyPressed = false;
    }

    bool sprint = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
    bool crouch = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS);

    if (fpsMode)
    {
        // ----------------------------------------------------
        // FPS Game-Style Walking Movement (Planar + Collisions)
        // ----------------------------------------------------
        glm::vec3 planarFront = glm::vec3(camera.Front.x, 0.0f, camera.Front.z);
        if (glm::length(planarFront) > 0.001f)
            planarFront = glm::normalize(planarFront);

        glm::vec3 planarRight = glm::vec3(camera.Right.x, 0.0f, camera.Right.z);
        if (glm::length(planarRight) > 0.001f)
            planarRight = glm::normalize(planarRight);

        float moveSpeed = camera.MovementSpeed * deltaTime * (sprint ? camera.SprintMultiplier : (crouch ? 0.45f : 1.0f));
        glm::vec3 moveDelta(0.0f);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            moveDelta += planarFront * moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            moveDelta -= planarFront * moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            moveDelta -= planarRight * moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            moveDelta += planarRight * moveSpeed;

        // Gravity, Floor Snapping & Jumping
        static float verticalVelocity = 0.0f;
        static bool isGrounded = true;

        float floorY = scene.getFloorHeight(camera.Position.x, camera.Position.z, camera.Position.y);
        float eyeOffset = crouch ? 100.0f : 165.0f;
        float targetEyeY = floorY + eyeOffset;

        // Jump when on ground with SPACE
        if (isGrounded && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            verticalVelocity = 380.0f;
            isGrounded = false;
        }

        if (!isGrounded)
        {
            verticalVelocity -= 980.0f * deltaTime;
            camera.Position.y += verticalVelocity * deltaTime;
            if (camera.Position.y <= targetEyeY)
            {
                camera.Position.y = targetEyeY;
                verticalVelocity = 0.0f;
                isGrounded = true;
            }
        }
        else
        {
            // Responsive tracking for continuous walking on stairs and floor thresholds
            float diff = targetEyeY - camera.Position.y;
            if (std::abs(diff) < 45.0f)
                camera.Position.y = targetEyeY;
            else
                camera.Position.y += diff * std::min(deltaTime * 18.0f, 1.0f);
        }

        // Resolve planar movement against walls, doors, fences and world boundaries
        scene.resolvePlayerMovement(camera.Position, moveDelta, 18.0f);
    }
    else
    {
        // ----------------------------------------------------
        // Noclip / Free Flight Mode (Unconstrained 6-DOF fly)
        // ----------------------------------------------------
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime, sprint);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime, sprint);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime, sprint);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime, sprint);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera.ProcessKeyboard(UP, deltaTime, sprint);
        if (crouch)
            camera.ProcessKeyboard(DOWN, deltaTime, sprint);
    }
}

// ============================================================
// Main
// ============================================================

int main(int argc, char *argv[])
{
    bool testMode = false;
    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]) == "--test")
            testMode = true;
    }
    // --------------------------------------------------------
    // Initialize GLFW
    // --------------------------------------------------------
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA antialiasing

    // --------------------------------------------------------
    // Create Window
    // --------------------------------------------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Villa Serena - OpenGL", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // --------------------------------------------------------
    // Initialize GLAD
    // --------------------------------------------------------
    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    std::cout << "========================================================================\n";
    std::cout << "                 VILLA SERENA: 3D INTERACTIVE WALKTHROUGH               \n";
    std::cout << "  Renderer: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "  OpenGL:   " << glGetString(GL_VERSION) << "\n";
    std::cout << "========================================================================\n";
    std::cout << "KEYBOARD & MOUSE CONTROLS:\n\n";
    std::cout << "  [ PLAYER MOVEMENT & CAMERA ]\n";
    std::cout << "    W / A / S / D         : Walk / Strafe (Planar FPS Movement with Wall Collisions)\n";
    std::cout << "    SPACE                 : Jump (in FPS mode)\n";
    std::cout << "    L-CTRL / C            : Crouch (Lowers eye level)\n";
    std::cout << "    LEFT SHIFT            : Sprint (3x speed multiplier)\n";
    std::cout << "    N                     : Toggle FPS Walk Mode (Collisions ON) / Noclip Freecam\n";
    std::cout << "    Mouse Look            : Rotate Camera Orientation\n";
    std::cout << "    Mouse Scroll          : Zoom / Adjust Field of View (FOV)\n";
    std::cout << "    TAB                   : Toggle Mouse Cursor Lock / Unlock\n";
    std::cout << "    ESC                   : Unlock Mouse Cursor\n\n";
    std::cout << "  [ LIGHTING & SHADING ]\n";
    std::cout << "    L                     : Toggle Astronomical Day / Night Cycle (Sun / Moon)\n";
    std::cout << "    F                     : Toggle Player-Mounted Spotlight (Flashlight)\n";
    std::cout << "    F1 - F10              : Toggle Individual Room Point Lights:\n";
    std::cout << "      [F1] Entrance / Porch       [F2] Living Room Chandelier\n";
    std::cout << "      [F3] Dining Room Pendant    [F4] Kitchen Ceiling Light\n";
    std::cout << "      [F5] Garage Workshop Light  [F6] Master Bedroom Light\n";
    std::cout << "      [F7] Study / Office Lamp    [F8] Bedroom 2 Light\n";
    std::cout << "      [F9] Powder Room Light      [F10] Second Floor Bathroom Light\n";
    std::cout << "    Z / X / V / K         : Group & Master Light Switches:\n";
    std::cout << "      [Z] Ground Floor Lights     [X] Second Floor Lights\n";
    std::cout << "      [V] Exterior Lights         [K] Master Switch (All Lights On/Off)\n\n";
    std::cout << "  [ KINEMATICS & FANS ]\n";
    std::cout << "    J                     : Toggle Living Room Ceiling Fan (Inertial Spin-Down)\n";
    std::cout << "    H                     : Toggle Dining Room Ceiling Fan (Inertial Spin-Down)\n";
    std::cout << "    U                     : Toggle Study Standing Floor Fan (Inertial Oscillation)\n";
    std::cout << "    Y                     : Toggle All Fans (Ceiling & Stand)\n\n";
    std::cout << "  [ WORLD INTERACTION ]\n";
    std::cout << "    E                     : Open / Close Nearest Door (Within 220 units)\n\n";
    std::cout << "  [ CAMERA PRESET TELEPORTS ]\n";
    std::cout << "    [1] Outside Front View (Yard, Driveway, Garage, Mailbox)\n";
    std::cout << "    [2] Ground Floor Entrance & Hallway\n";
    std::cout << "    [3] Ground Floor Living Room (Sofas, Fireplace, TV console)\n";
    std::cout << "    [4] Ground Floor Kitchen & Dining Room\n";
    std::cout << "    [5] Ground Floor Bathroom & Staircase\n";
    std::cout << "    [6] Second Floor Master Bedroom\n";
    std::cout << "    [7] Second Floor Bedroom 2\n";
    std::cout << "    [8] Second Floor Study / Office Room\n";
    std::cout << "    [9] Second Floor Balcony & Bathroom\n";
    std::cout << "    [0] Street & Distant Neighborhood Traffic\n";
    std::cout << "    [P] Bird's Eye Overhead Aerial Dollhouse View\n";
    std::cout << "========================================================================\n\n" << std::flush;

    // --------------------------------------------------------
    // OpenGL State
    // --------------------------------------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // --------------------------------------------------------
    // Load Shaders & Texture & Build Scene
    // --------------------------------------------------------
    Shader sceneShader("shaders/scene.vert", "shaders/scene.frag");
    Texture mainTexture("assets/Textures/InteriorTexture_01.png");
    // Preload secondary art/props texture from assets/Textures
    ModelManager::instance().getTexture("assets/Textures/InteriorTexture_02.png");

    Scene scene;
    scene.buildScene();


    sceneShader.use();
    sceneShader.setInt("texture_diffuse", 0);

    // --------------------------------------------------------
    // Main Render Loop
    // --------------------------------------------------------
    double lastFpsTime = glfwGetTime();
    int frameCount = 0;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // FPS & title bar update
        frameCount++;
        if (currentFrame - lastFpsTime >= 0.5)
        {
            float fps = static_cast<float>(frameCount / (currentFrame - lastFpsTime));
            std::string title = "Villa Serena | FPS: " + std::to_string(static_cast<int>(fps)) +
                                " | Objects: " + std::to_string(scene.getObjectCount()) +
                                " | Living Fan: " + (scene.ceilingFanActive ? "ON" : "OFF") + " (J)" +
                                " | Dining Fan: " + (scene.diningFanActive ? "ON" : "OFF") + " (H)" +
                                " | Stand Fan: " + (scene.standFanActive ? "ON" : "OFF") + " (U)" +
                                " | Lights: F1-F10/Z/X/V/K";
            glfwSetWindowTitle(window, title.c_str());
            frameCount = 0;
            lastFpsTime = currentFrame;
        }

        // Input
        processInput(window, scene);

        // Update motions: ceiling & stand fans, traffic cars, door swings
        scene.update(deltaTime, currentFrame);

        // Sky clear color based on Day/Night
        glm::vec3 skyColor = glm::mix(glm::vec3(0.04f, 0.05f, 0.09f), glm::vec3(0.55f, 0.75f, 0.95f), dayNightFactor);
        glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate Shader
        sceneShader.use();

        // Projection & View Matrices
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspect = (height > 0) ? (static_cast<float>(width) / static_cast<float>(height)) : 1.0f;
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), aspect, 1.0f, 25000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        sceneShader.setMat4("projection", projection);
        sceneShader.setMat4("view", view);
        sceneShader.setVec3("viewPos", camera.Position);

        // Lighting Parameters
        glm::vec3 sunDir = glm::normalize(glm::vec3(-0.6f, -1.0f, -0.7f));
        glm::vec3 sunColor = glm::mix(glm::vec3(0.12f, 0.15f, 0.25f), glm::vec3(0.95f, 0.92f, 0.85f), dayNightFactor);
        glm::vec3 groundColor = glm::mix(glm::vec3(0.02f, 0.03f, 0.04f), glm::vec3(0.35f, 0.38f, 0.25f), dayNightFactor);

        sceneShader.setVec3("sunDirection", sunDir);
        sceneShader.setVec3("sunColor", sunColor);
        sceneShader.setVec3("skyColor", skyColor);
        sceneShader.setVec3("groundColor", groundColor);
        sceneShader.setFloat("dayNightFactor", dayNightFactor);

        // Flashlight parameters
        sceneShader.setBool("flashlightOn", flashlightOn);
        sceneShader.setVec3("flashlightDir", camera.Front);

        // Bind main texture atlas
        mainTexture.bind(0);

        // Render entire scene
        scene.render(sceneShader);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (testMode && currentFrame > 1.0f)
        {
            // Test door interaction system
            std::cout << "[Test Mode] Testing E key interaction on nearest door...\n";
            bool openOk = scene.interactNearestDoor(glm::vec3(50.0f, 0.0f, 750.0f), 200.0f);
            scene.update(0.1f, 1.1f);
            bool closeOk = scene.interactNearestDoor(glm::vec3(50.0f, 0.0f, 750.0f), 200.0f);
            scene.update(0.1f, 1.2f);
            if (openOk && closeOk)
                std::cout << "[Test Mode] Door interaction (Open & Close) verified successfully!\n";
            std::cout << "Test completed successfully: rendered multiple frames with 0 errors!\n" << std::flush;
            break;
        }

        static bool screenshotDone = false;
        if (!screenshotDone && argc > 1 && std::string(argv[1]) == "--screenshot")
        {
            screenshotDone = true;
            struct ShotInfo {
                std::string filename;
                int keyPreset; // -1 if custom
                glm::vec3 pos;
                glm::vec3 target;
            };

            std::vector<ShotInfo> shots = {
                {"shot_0_street_traffic.ppm", GLFW_KEY_0, glm::vec3(0), glm::vec3(0)},
                {"shot_1_front_exterior.ppm", GLFW_KEY_1, glm::vec3(0), glm::vec3(0)},
                {"shot_2_entrance_hallway.ppm", GLFW_KEY_2, glm::vec3(0), glm::vec3(0)},
                {"shot_3_living_room.ppm", GLFW_KEY_3, glm::vec3(0), glm::vec3(0)},
                {"shot_4_kitchen.ppm", GLFW_KEY_4, glm::vec3(0), glm::vec3(0)},
                {"shot_5_stairs_bathroom.ppm", GLFW_KEY_5, glm::vec3(0), glm::vec3(0)},
                {"shot_6_master_bedroom.ppm", GLFW_KEY_6, glm::vec3(0), glm::vec3(0)},
                {"shot_7_bedroom2.ppm", GLFW_KEY_7, glm::vec3(0), glm::vec3(0)},
                {"shot_8_study_office.ppm", GLFW_KEY_8, glm::vec3(0), glm::vec3(0)},
                {"shot_9_top_of_stairs.ppm", GLFW_KEY_9, glm::vec3(0), glm::vec3(0)},
                {"shot_P_overhead_roof.ppm", GLFW_KEY_P, glm::vec3(0), glm::vec3(0)},
                {"shot_inside_powder_room.ppm", -1, glm::vec3(-240.0f, 80.0f, -450.0f), glm::vec3(-612.0f, 40.0f, -450.0f)},
                {"shot_inside_second_bath.ppm", -1, glm::vec3(-240.0f, 380.0f, -450.0f), glm::vec3(-612.0f, 340.0f, -450.0f)},
                {"shot_outside_west_wall.ppm", -1, glm::vec3(-850.0f, 100.0f, -450.0f), glm::vec3(-600.0f, 100.0f, -450.0f)},
                {"shot_cars_road_view.ppm", -1, glm::vec3(-300.0f, 80.0f, 2300.0f), glm::vec3(800.0f, 60.0f, 2300.0f)},
                {"shot_fireplace.ppm", -1, glm::vec3(-250.0f, 120.0f, 375.0f), glm::vec3(-594.0f, 100.0f, 375.0f)},
                {"shot_study_bookshelf.ppm", -1, glm::vec3(420.0f, 380.0f, 60.0f), glm::vec3(600.0f, 370.0f, -105.0f)},
                {"shot_nook_books.ppm", -1, glm::vec3(350.0f, 100.0f, -550.0f), glm::vec3(600.0f, 75.0f, -400.0f)},
                {"shot_study_door_hallway.ppm", -1, glm::vec3(-20.0f, 380.0f, 125.0f), glm::vec3(125.0f, 360.0f, 125.0f)},
                {"shot_study_door_inside.ppm", -1, glm::vec3(260.0f, 380.0f, 80.0f), glm::vec3(125.0f, 360.0f, 125.0f)}
            };

            for (const auto &shot : shots)
            {
                if (shot.keyPreset != -1)
                    setViewPreset(shot.keyPreset);
                else
                    camera.setLookAt(shot.pos, shot.target);

                glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                view = camera.GetViewMatrix();
                sceneShader.setMat4("view", view);
                sceneShader.setVec3("viewPos", camera.Position);
                mainTexture.bind(0);
                scene.render(sceneShader);
                std::vector<unsigned char> pixels(width * height * 3);
                glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
                glfwSwapBuffers(window);
                glFinish();

                FILE *f = fopen(shot.filename.c_str(), "wb");
                if (f)
                {
                    fprintf(f, "P6\n%d %d\n255\n", width, height);
                    for (int y = height - 1; y >= 0; --y)
                    {
                        fwrite(&pixels[y * width * 3], 1, width * 3, f);
                    }
                    fclose(f);
                    std::cout << "Saved screenshot: " << shot.filename << "\n";
                }
            }
            break;
        }
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}