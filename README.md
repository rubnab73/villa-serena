# OpenGL 3D Architectural Walkthrough

An interactive, real-time 3D architectural house walkthrough and rendering engine built in modern C++ (C++17) using OpenGL 3.3 Core Profile, GLFW, GLAD, and GLM.

---

## Features

- **Interactive First-Person Fly/Walk Camera**: Freeform navigation with sprint, vertical flight, and mouse-look pitch/yaw controls.
- **Dynamic Lighting Pipeline**:
  - Full Phong lighting model with directional light, point lights, and spotlights.
  - **Day / Night Mode**: Seamlessly toggle between natural daylight and atmospheric evening lighting with custom ambient and directional colors.
  - **Flashlight System**: Toggleable player-attached spotlight with smooth cutoffs.
- **Architectural Multi-Room Environment**:
  - Ground Floor: Entrance, Living Room, Kitchen, Dining Room, Bathroom, and Staircase.
  - Second Floor: Master Bedroom, Bedroom 2, Study / Office Room, Balcony, and Bathroom.
  - Exterior: Front Yard, Driveway, Garage, Mailbox, Street, and distant surrounding cityscape.
- **Automated Quick Tour Presets**: Instant smooth camera repositioning across 11 predefined room and aerial vantage points (`1`–`9`, `0`, `P`).
- **Dynamic Props & Animation**: Built-in procedural fan models with rotating blades and animated components.
- **glTF 2.0 & Texture Asset Integration**: Supports complex 3D meshes and textured materials loaded via `cgltf` and `stb_image`.

---

## Controls

| Key / Input | Action |
| :--- | :--- |
| **W / A / S / D** | Move Forward / Backward / Strafe Left / Strafe Right |
| **Space** | Ascend (Fly Up) |
| **Left Ctrl** | Descend (Fly Down) |
| **Left Shift** | Sprint / Boost Movement Speed |
| **Mouse** | Look around (Yaw & Pitch) |
| **Tab** | Lock / Unlock Mouse Cursor |
| **L** | Toggle Day / Night Lighting |
| **F** | Toggle Flashlight (Spotlight) |
| **1 – 9, 0, P** | Quick Tour Camera Presets (see below) |
| **Esc** | Exit Application |

### Quick Tour Camera Presets

- **`1`**: Outside Front View (Yard, Driveway, Garage, Mailbox)
- **`2`**: Ground Floor Entrance & Hallway
- **`3`**: Ground Floor Living Room
- **`4`**: Ground Floor Kitchen & Dining Room
- **`5`**: Ground Floor Bathroom & Staircase
- **`6`**: Second Floor Master Bedroom
- **`7`**: Second Floor Bedroom 2
- **`8`**: Second Floor Study / Office Room
- **`9`**: Second Floor Balcony & Bathroom
- **`0`**: Street & Distant Buildings
- **`P`**: Bird's Eye Overhead Aerial View

---

## Project Structure

```
.
├── assets/                  # 3D models and textures (Interior, Exterior, Textures)
├── external/                # Third-party dependencies
│   ├── cgltf.h              # Single-file glTF 2.0 loader
│   ├── glad/                # OpenGL 3.3 Core function loader
│   ├── glfw/                # Windowing, context & input handling (MinGW binaries)
│   ├── glm/                 # OpenGL Mathematics library
│   └── stb_image.h          # Texture image loader
├── shaders/                 # GLSL shaders
│   ├── basic.vert / .frag   # Simple fallback vertex & fragment shaders
│   └── scene.vert / .frag   # Full Phong shader with multi-light & spotlight support
├── src/                     # C++ source code
│   ├── Camera.hpp           # First-person view & projection matrix controller
│   ├── FanModels.hpp        # Animated fan geometry and mesh generators
│   ├── Mesh.hpp / .cpp      # VAO / VBO / EBO mesh abstractions
│   ├── Model.hpp / .cpp     # Model loader and multi-mesh manager
│   ├── Scene.hpp / .cpp     # Room layouts, lighting parameters, and object placement
│   ├── Shader.hpp / .cpp    # GLSL program compilation and uniform management
│   ├── Texture.hpp / .cpp   # 2D OpenGL texture wrapper
│   └── main.cpp             # Game loop, window setup, and input dispatching
├── CMakeLists.txt           # Build configuration
└── README.md                # Project documentation
```

---

## Prerequisites & Dependencies

- **Compiler**: GCC / MinGW-w64 with C++17 support
- **Build System**: [CMake 3.20+](https://cmake.org/download/)
- **Graphics API**: OpenGL 3.3 Core Profile compatible GPU
- **Bundled Libraries**:
  - [GLFW 3.x](https://www.glfw.org/)
  - [GLAD](https://glad.dav1d.de/)
  - [GLM](https://github.com/g-truc/glm)
  - [stb_image](https://github.com/nothings/stb)
  - [cgltf](https://github.com/jkuhlmann/cgltf)

---

## Build & Run

### Using CMake and MinGW (Windows)

1. **Configure the build:**
   ```powershell
   cmake -S . -B build -G "MinGW Makefiles"
   ```

2. **Compile:**
   ```powershell
   cmake --build build
   ```

3. **Run the executable:**
   ```powershell
   .\build\OpenGL3D.exe
   ```

---

## License

This project is licensed under the MIT License - see the LICENSE file for details.
