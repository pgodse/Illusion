#version 330 core
layout(location=0) out vec4 color;
in vec3 vertWorldSpace;
in vec3 transformedNormal;
in vec2 uvs;
in vec4 fragPosLightSpace;
in vec3 eyePos;

uniform vec3 dirLightVec;
uniform vec3 dirLightColor;
uniform vec4 baseColor;

uniform samplerCube envMap;
uniform sampler2D myTexture;
uniform sampler2D depthTexture;


float getShadowFactor() {
    vec3 projCoord = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoord = projCoord * 0.5 + 0.5;
    float depthFromTex = texture(depthTexture, projCoord.xy).r;
    float currentDepth = projCoord.z;
    
    float bias = 0.005;
    float shadow = currentDepth - bias > depthFromTex ? 0.0 : 1.0;
    
    return shadow;
}

#define PI 3.1415926535897932384626433832795

void main()
{
    vec3 I = normalize(vertWorldSpace - eyePos);
    vec3 R = reflect(I, normalize(transformedNormal));
    vec4 reflectColor = texture(envMap, R);
    
    float shadow = getShadowFactor();
//
    float lightVal = max(dot(normalize(dirLightVec), normalize(transformedNormal)), 0);
    
    vec3 matColor = texture(myTexture, uvs).rgb + baseColor.rgb;
    vec3 ambientLight = vec3(0.10, 0.09, 0.11);
//
    vec3 fragColor = matColor * (dirLightColor * lightVal + ambientLight); // texture(myTexture, uvs).rgb * ; //+ dirLightColor * lightVal * vertWorldSpace;
    fragColor += reflectColor.rgb * 0.5;
    fragColor *= shadow;
    color = vec4(fragColor.rgb, baseColor.a);
}
