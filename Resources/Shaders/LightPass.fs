#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D posData;
uniform sampler2D normalData;
uniform sampler2D albedoData;

uniform sampler2D depthMap;

uniform float far_plane;

uniform float metallic;
uniform float roughness;
uniform float ao;

uniform bool b_shadows;

uniform mat4 lightSpaceMatrix;

struct Light {
    vec3 Position;
    vec3 Color;
};

const int lightNum = 4;
uniform Light light[lightNum];

uniform vec3 camPos;

const float PI = 3.14159265359;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(depthMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    vec3 normal = normalize(texture(normalData, TexCoords).xyz);
    vec3 lightDir = normalize(light[0].Position - texture(posData, TexCoords).xyz);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }

    shadow /= 9.0;

    if (projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

void main()
{
    vec3 FragPos = texture(posData, TexCoords).rgb;
    vec3 Normal = texture(normalData, TexCoords).rgb;
    vec3 Albedo = texture(albedoData, TexCoords).rgb;
    float Specular = texture(albedoData, TexCoords).a;

    vec4 FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    vec3 N = normalize(Normal);
    vec3 V = normalize(camPos - FragPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo, metallic);

    vec3 Lo = vec3(0.0);

    float shadow = b_shadows ? ShadowCalculation(FragPosLightSpace) : 0.0;

    for(int i = 0; i < 4; ++i)
    {
        vec3 L = normalize(light[i].Position - FragPos);
        vec3 H = normalize(V + L);

        float distance = length(light[i].Position - FragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = light[i].Color * attenuation;

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

        vec3 nominator = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular = (nominator / max(denominator, 0.001));

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;

        kD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.0);

        Lo += ((kD * Albedo / PI) + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * Albedo * ao;

    vec3 color = ambient + (1.0 - shadow) * Lo;

    color = color / (color + vec3(1.0));

    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}