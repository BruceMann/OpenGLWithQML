#version 330 core

in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform float mixValue;


//uniform sampler2D ourTexture;
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;


void main() {
    //FragColor =texture(ourTexture1, TexCoord);
    //FragColor = mix(texture(ourTexture1, TexCoord), texture(ourTexture2,TexCoord), 0.2)*vec4(ourColor,1.0);
    //FragColor = mix(texture(ourTexture1, TexCoord), texture(ourTexture2,TexCoord), 0.2);
    //FragColor = mix(texture(ourTexture1, TexCoord), texture(ourTexture2,TexCoord), mixValue);

    //Color and light test
    //计算环境光
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;
    //计算漫反射光照
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm,lightDir),0.0);
    vec3 diffuse = diff*lightColor;
    // Specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;

    FragColor = vec4(result, 1.0f);

    //FragColor = vec4(objectColor,1.0);

}
