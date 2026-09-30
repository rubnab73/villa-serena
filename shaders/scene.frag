#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse;
uniform vec4 tintColor;
uniform vec3 viewPos;

// Lighting parameters
uniform vec3 sunDirection;
uniform vec3 sunColor;
uniform vec3 skyColor;
uniform vec3 groundColor;
uniform float dayNightFactor; // 1.0 = Day, 0.0 = Night

// Point lights for interior rooms (up to 8 lights)
struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float radius;
};
#define NUM_POINT_LIGHTS 12
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform int numPointLights;

// Flashlight
uniform bool flashlightOn;
uniform vec3 flashlightDir;

void main()
{
    vec4 texColor = texture(texture_diffuse, TexCoords);
    vec3 albedo = texColor.rgb * tintColor.rgb;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Detect interior regions (Main house & Attached garage)
    bool isMainHouse = (FragPos.x > -620.0 && FragPos.x < 620.0 &&
                        FragPos.z > -745.0 && FragPos.z < 745.0 &&
                        FragPos.y > -2.0 && FragPos.y < 605.0);
    bool isGarage = (FragPos.x > 625.0 && FragPos.x < 1120.0 &&
                     FragPos.z > -245.0 && FragPos.z < 495.0 &&
                     FragPos.y > -2.0 && FragPos.y < 305.0);
    bool isInterior = isMainHouse || isGarage;

    // Hemisphere Ambient Lighting
    float hemiFactor = clamp(norm.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 outdoorAmbient = mix(groundColor, skyColor, hemiFactor) * mix(0.18, 0.40, dayNightFactor);
    vec3 indoorAmbient = mix(vec3(0.20, 0.19, 0.17), vec3(0.28, 0.27, 0.24), hemiFactor) * mix(0.35, 0.50, dayNightFactor);
    vec3 ambient = isInterior ? indoorAmbient : outdoorAmbient;

    // Directional Sunlight / Moonlight
    // Interior surfaces receive subtle soft indirect window bounce, outdoor surfaces receive direct sun
    vec3 lightDir = normalize(-sunDirection);
    float diff = max(dot(norm, lightDir), 0.0);
    float sunMultiplier = isInterior ? 0.06 : 1.0;
    vec3 diffuse = diff * sunColor * sunMultiplier;

    // Blinn-Phong Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = 0.2 * spec * sunColor * sunMultiplier;

    vec3 directLighting = diffuse + specular;

    // Accumulate point lights (interior room lamps)
    vec3 interiorLighting = vec3(0.0);
    for (int i = 0; i < numPointLights; ++i)
    {
        vec3 plDir = pointLights[i].position - FragPos;
        float dist = length(plDir);
        if (dist < pointLights[i].radius)
        {
            plDir = normalize(plDir);
            float plDiff = max(dot(norm, plDir), 0.0);
            float atten = clamp(1.0 - (dist / pointLights[i].radius), 0.0, 1.0);
            atten = atten * atten;

            vec3 plHalf = normalize(plDir + viewDir);
            float plSpec = pow(max(dot(norm, plHalf), 0.0), 16.0);

            interiorLighting += (plDiff * pointLights[i].color * pointLights[i].intensity + 0.15 * plSpec * pointLights[i].color) * atten;
        }
    }

    // Flashlight
    vec3 flashLighting = vec3(0.0);
    if (flashlightOn)
    {
        vec3 flashVec = FragPos - viewPos;
        float flashDist = length(flashVec);
        vec3 flashDirNorm = normalize(flashVec);
        float theta = dot(flashDirNorm, normalize(flashlightDir));
        float cutOff = 0.94; // ~20 degrees
        float outerCutOff = 0.90;
        float epsilon = cutOff - outerCutOff;
        float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

        if (intensity > 0.0 && flashDist < 3500.0)
        {
            float flashDiff = max(dot(norm, -flashDirNorm), 0.0);
            float flashAtten = 1.0 / (1.0 + 0.0005 * flashDist + 0.000001 * flashDist * flashDist);
            flashLighting = vec3(1.0, 0.98, 0.9) * 1.5 * flashDiff * flashAtten * intensity;
        }
    }

    // Combine light components
    vec3 totalLight = ambient + directLighting + interiorLighting + flashLighting;
    // Filmic soft tone mapping to prevent harsh blown-out whites and preserve rich shading
    vec3 mappedLight = totalLight / (totalLight + vec3(1.0)) * 1.6;
    vec3 finalColor = albedo * mappedLight;

    // Atmospheric distance fog
    float distToCamera = length(viewPos - FragPos);
    float fogStart = 3000.0;
    float fogEnd = 12000.0;
    float fogFactor = clamp((distToCamera - fogStart) / (fogEnd - fogStart), 0.0, 0.85);
    vec3 fogColor = mix(vec3(0.05, 0.07, 0.12), skyColor, dayNightFactor);
    finalColor = mix(finalColor, fogColor, fogFactor);

    FragColor = vec4(finalColor, texColor.a * tintColor.a);
}
