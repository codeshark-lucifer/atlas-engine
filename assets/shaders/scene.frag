#version 460 core


in VS_OUT {
    vec2 TexCoord;
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace[16];
} fs_in;

out vec4 FragColor;

uniform sampler2D diffuse_texture1;
uniform sampler2D shadowMaps[16];
uniform samplerCube shadowCubeMap; // New sampler for point light shadows
uniform vec3 viewPos;
uniform float far_plane; // New uniform for point light far plane

struct Light {
    int type; // 0 = Dir, 1 = Point, 2 = Spot
    vec3 position;
    vec3 color;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    bool castsShadow;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
uniform Light lights[16];
uniform int lightCount;

float ShadowCalculationDirectional(vec4 fragPosLightSpace, sampler2D shadowMap, vec3 lightDir, vec3 norm)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
        return 0.0;

    float shadow = 0.0;
    float bias = max(0.0025 * (1.0 - dot(norm, -lightDir)), 0.00025);
    int samples = 8;
    float texelSize = 1.0 / 4096.0;

    for(int x = -samples; x <= samples; ++x)
    {
        for(int y = -samples; y <= samples; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            if(projCoords.z - bias > pcfDepth)
                shadow += 1.0;
        }
    }
    shadow /= pow((samples * 2 + 1), 2);

    return shadow;
}

float ShadowCalculationSpot(vec4 fragPosLightSpace, sampler2D shadowMap, vec3 lightDir, vec3 norm)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
        return 0.0;

    float shadow = 0.0;
    float bias = max(0.0005 * (1.0 - dot(norm, -lightDir)), 0.00005);
    int samples = 8;
    float texelSize = 1.0 / 4096.0;

    for(int x = -samples; x <= samples; ++x)
    {
        for(int y = -samples; y <= samples; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            if(projCoords.z - bias > pcfDepth)
                shadow += 1.0;
        }
    }
    shadow /= pow((samples * 2 + 1), 2);

    return shadow;
}

// New function for point light shadow calculation
float ShadowCalculationPoint(vec3 fragPos, vec3 lightPos, float far_plane)
{
    vec3 fragToLight = fragPos - lightPos;
    float closestDepth = texture(shadowCubeMap, fragToLight).r;
    closestDepth *= far_plane;
    float currentDepth = length(fragToLight);
    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

vec3 CalculateDirectionalLight(vec3 norm, vec3 viewDir, Light light, vec4 fragPosLightSpace, sampler2D shadowMap)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);


    vec3 diffuse  = material.diffuse * diff * light.color;
    vec3 specular = material.specular * spec * light.color;

    float shadow = ShadowCalculationDirectional(fragPosLightSpace, shadowMap, lightDir, norm);
    return (1.0 - shadow) * (diffuse + specular);
}

vec3 CalculatePointLight(vec3 norm, vec3 viewDir, Light light, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                               light.quadratic * distance * distance);

    vec3 ambient  = material.ambient * light.color * attenuation;
    vec3 diffuse  = material.diffuse * diff * light.color * attenuation;
    vec3 specular = material.specular * spec * light.color * attenuation;

    float shadow = light.castsShadow ? ShadowCalculationPoint(fragPos, light.position, far_plane) : 0.0;
    return (1.0 - shadow) * (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(vec3 norm, vec3 viewDir, Light light, vec3 fragPos, vec4 fragPosLightSpace, sampler2D shadowMap)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                               light.quadratic * distance * distance);

    // Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = max(light.cutOff - light.outerCutOff, 0.001); // prevent divide by zero
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient  = material.ambient * light.color * attenuation * intensity;
    vec3 diffuse  = material.diffuse * diff * light.color * attenuation * intensity;
    vec3 specular = material.specular * spec * light.color * attenuation * intensity;

    float shadow = ShadowCalculationSpot(fragPosLightSpace, shadowMap, lightDir, norm);
    return (1.0 - shadow) * (diffuse + specular);
}


void main()
{
    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 albedo = texture(diffuse_texture1, fs_in.TexCoord).rgb;

    // Start with ambient color
    vec3 result = material.ambient * albedo;

    // Loop through all lights
    for(int i = 0; i < lightCount; i++)
    {
        Light light = lights[i];
        vec3 contribution = vec3(0.0);

        if(light.type == 0) // Directional
        {
            contribution = CalculateDirectionalLight(norm, viewDir, light, fs_in.FragPosLightSpace[i], shadowMaps[i]);
        }
        else if(light.type == 1) // Point
        {
            // The fragPosLightSpace is not needed for point light shadow calculation
            contribution = CalculatePointLight(norm, viewDir, light, fs_in.FragPos);
        }
        else if(light.type == 2) // Spot
        {
            contribution = CalculateSpotLight(norm, viewDir, light, fs_in.FragPos, fs_in.FragPosLightSpace[i], shadowMaps[i]);
        }

        result += contribution;
    }

    FragColor = vec4(result, 1.0);
}
