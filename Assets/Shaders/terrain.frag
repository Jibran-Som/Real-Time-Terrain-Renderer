#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in float Height;

// --- Texture Units ---
uniform sampler2D sandAlbedo;
uniform sampler2D sandNormal;
uniform sampler2D sandRoughness;
uniform sampler2D sandAO;

uniform sampler2D grassAlbedo;
uniform sampler2D grassNormal;
uniform sampler2D grassRoughness;
uniform sampler2D grassAO;

uniform sampler2D rockAlbedo;
uniform sampler2D rockNormal;
uniform sampler2D rockRoughness;
uniform sampler2D rockAO;

uniform sampler2D snowAlbedo;
uniform sampler2D snowNormal;
uniform sampler2D snowRoughness;
uniform sampler2D snowAO;

// --- Thresholds ---
uniform float sandHeight;
uniform float grassHeight;
uniform float rockHeight;
uniform float snowHeight;

// --- Lighting ---
uniform vec3 lightPos;     
uniform vec3 lightColor;   
uniform vec3 viewPos;      

// 计算TBN矩阵
mat3 computeTBN(vec3 normal, vec3 worldPos, vec2 texCoords) {
    vec3 dPosX = dFdx(worldPos);
    vec3 dPosY = dFdy(worldPos);
    vec2 dTexX = dFdx(texCoords);
    vec2 dTexY = dFdy(texCoords);
    
    vec3 tangent = normalize(dPosX * dTexY.t - dPosY * dTexX.t);
    vec3 bitangent = normalize(dPosY * dTexX.s - dPosX * dTexY.s);
    
    return mat3(tangent, bitangent, normal);
}

void main()
{
    vec3 dx = dFdx(WorldPos);
    vec3 dy = dFdy(WorldPos);
    vec3 geometricNormal = normalize(cross(dx, dy));
    
    float blendRange = 8.0;
    float wSand = 1.0 - smoothstep(sandHeight - blendRange, sandHeight + blendRange, Height);
    float wGrass = smoothstep(sandHeight - blendRange, sandHeight + blendRange, Height) 
                   * (1.0 - smoothstep(rockHeight - blendRange, rockHeight + blendRange, Height));
    float wRock = smoothstep(rockHeight - blendRange, rockHeight + blendRange, Height) 
                  * (1.0 - smoothstep(snowHeight - blendRange, snowHeight + blendRange, Height));
    float wSnow = smoothstep(snowHeight - blendRange, snowHeight + blendRange, Height);
    
    float totalWeight = wSand + wGrass + wRock + wSnow;
    if (totalWeight > 0.0) {
        wSand /= totalWeight;
        wGrass /= totalWeight;
        wRock /= totalWeight;
        wSnow /= totalWeight;
    }

    vec2 tiledCoords = TexCoords * 6.0;
    
    vec3 albedo = vec3(0.0);
    vec3 blendedNormalMap = vec3(0.5, 0.5, 1.0);
    float roughness = 0.0;
    float ao = 0.0;
    
    if (wSand > 0.01) {
        albedo += texture(sandAlbedo, tiledCoords).rgb * wSand;
        blendedNormalMap += (texture(sandNormal, tiledCoords).rgb - vec3(0.5, 0.5, 0.5)) * wSand;
        roughness += texture(sandRoughness, tiledCoords).r * wSand;
        ao += texture(sandAO, tiledCoords).r * wSand;
    }
    if (wGrass > 0.01) {
        albedo += texture(grassAlbedo, tiledCoords).rgb * wGrass;
        blendedNormalMap += (texture(grassNormal, tiledCoords).rgb - vec3(0.5, 0.5, 0.5)) * wGrass;
        roughness += texture(grassRoughness, tiledCoords).r * wGrass;
        ao += texture(grassAO, tiledCoords).r * wGrass;
    }
    if (wRock > 0.01) {
        albedo += texture(rockAlbedo, tiledCoords).rgb * wRock;
        blendedNormalMap += (texture(rockNormal, tiledCoords).rgb - vec3(0.5, 0.5, 0.5)) * wRock;
        roughness += texture(rockRoughness, tiledCoords).r * wRock;
        ao += texture(rockAO, tiledCoords).r * wRock;
    }
    if (wSnow > 0.01) {
        albedo += texture(snowAlbedo, tiledCoords).rgb * wSnow;
        blendedNormalMap += (texture(snowNormal, tiledCoords).rgb - vec3(0.5, 0.5, 0.5)) * wSnow;
        roughness += texture(snowRoughness, tiledCoords).r * wSnow;
        ao += texture(snowAO, tiledCoords).r * wSnow;
    }
    
    
    roughness = clamp(roughness, 0.05, 0.95);
    ao = clamp(ao, 0.3, 1.0);
    
    mat3 TBN = computeTBN(geometricNormal, WorldPos, TexCoords);
    vec3 tangentNormal = normalize(blendedNormalMap * 2.0 - 1.0);
    vec3 N = normalize(TBN * tangentNormal);

    vec3 L = normalize(lightPos - WorldPos);
    vec3 V = normalize(viewPos - WorldPos);
    vec3 H = normalize(L + V);
    
 
    vec3 ambient = 0.15 * albedo * ao * lightColor;
    
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = NdotL * lightColor * albedo;
    
    float NdotH = max(dot(N, H), 0.0);
    float spec = pow(NdotH, 64.0 * (1.0 - roughness));
    vec3 specular = vec3(0.1) * spec * lightColor * (1.0 - roughness);
    
    float skyLight = max(dot(N, vec3(0.0, 1.0, 0.0)), 0.2);
    vec3 skyAmbient = skyLight * albedo * vec3(0.5, 0.6, 0.8) * 0.2;
    
    vec3 finalColor = ambient + diffuse + specular + skyAmbient;
    
    finalColor = finalColor * finalColor * (2.0 - finalColor);
    
    // Gamma
    finalColor = pow(finalColor, vec3(1.0/2.2));
    
    FragColor = vec4(finalColor, 1.0);
}