#version 450 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

// Material properties
struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
};

// Light types
struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float range;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float range;
    float cutOff;
    float outerCutOff;
};

// Light uniforms
#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS 8

uniform int numDirectionalLights;
uniform int numPointLights;
uniform int numSpotLights;

uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform Material material;
uniform vec3 viewPos;

// Function declarations
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Calculate lighting
    vec3 result = vec3(0.0);

    // Directional lights
    for(int i = 0; i < numDirectionalLights && i < MAX_DIRECTIONAL_LIGHTS; i++) {
        result += calculateDirectionalLight(directionalLights[i], norm, viewDir);
    }

    // Point lights
    for(int i = 0; i < numPointLights && i < MAX_POINT_LIGHTS; i++) {
        result += calculatePointLight(pointLights[i], norm, FragPos, viewDir);
    }

    // Spot lights
    for(int i = 0; i < numSpotLights && i < MAX_SPOT_LIGHTS; i++) {
        result += calculateSpotLight(spotLights[i], norm, FragPos, viewDir);
    }

    // Ambient light
    vec3 ambient = vec3(0.03) * material.albedo * material.ao;
    result += ambient;

    FragColor = vec4(result, 1.0);
}

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * light.intensity * diff * material.albedo;
    
    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = light.color * light.intensity * spec * (1.0 - material.roughness);
    
    return diffuse + specular;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * light.intensity * diff * material.albedo;
    
    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = light.color * light.intensity * spec * (1.0 - material.roughness);
    
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    
    diffuse *= attenuation;
    specular *= attenuation;
    
    return diffuse + specular;
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Spot light cone calculation
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * light.intensity * diff * material.albedo;
    
    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = light.color * light.intensity * spec * (1.0 - material.roughness);
    
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (distance * distance);
    
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return diffuse + specular;
}
