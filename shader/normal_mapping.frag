#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 TBNLightPos;
in vec3 TBNViewPos;
in vec3 TBNFragPos;

uniform sampler2D DiffuseMap;
uniform sampler2D NormalMap;
uniform sampler2D DepthMap;

uniform int isNormalMapping;
uniform float height_scale;

vec2 getParallaxMappingTexCoord(vec2 texCoord, vec3 viewDir)
{
    // number of depth layers
    const float minLayers = 10;
    const float maxLayers = 20;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  

    // calculate the size of each layer
    float layerStep = 1.0 / numLayers;

    // depth of current layer
    float currentLayerDepth = 0.0;

    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 texCoordStep = -viewDir.xy / viewDir.z * height_scale * layerStep;
  
    // get initial values
    vec2 currentTexCoord = texCoord;
    float currentDepthMapValue = texture(DepthMap, currentTexCoord).r;
    
    // like ray marching, every step will get deeper until the ray cross the "hill" from depth map.
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoord += texCoordStep;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(DepthMap, currentTexCoord).r;  
        // get depth of next layer
        currentLayerDepth += layerStep;
    }
    
    // -- parallax occlusion mapping interpolation from here on
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoord = currentTexCoord - texCoordStep;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(DepthMap, prevTexCoord).r - currentLayerDepth + layerStep;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoord = prevTexCoord * weight + currentTexCoord * (1.0 - weight);

    return finalTexCoord;
}

void main()
{           
    vec3 viewDir = normalize(TBNViewPos - TBNFragPos);
    vec2 texCoord = TexCoords;
    if(isNormalMapping == 0)
    {
        texCoord = getParallaxMappingTexCoord(texCoord, viewDir);
        if(texCoord.x > 1.0 || texCoord .y > 1.0 || texCoord.x < 0.0 || texCoord.y < 0.0) discard;
    }

    // Obtain normal from normal map in range [0,1]
    vec3 normal = texture(NormalMap, texCoord).rgb;
    // Transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

    // Get diffuse color
    vec3 color = texture(DiffuseMap, texCoord).rgb;
    // Ambient
    vec3 ambient = 0.1 * color;
    // Diffuse
    vec3 lightDir = normalize(TBNLightPos - TBNFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.2) * spec;
    
    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}