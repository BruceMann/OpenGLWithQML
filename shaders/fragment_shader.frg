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

struct Light
{
    //vec3 position; 平行光暂时不需要光源位置 只需要光源方向
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

struct Material
{
    //vec3 ambient;
    //vec3 diffuse;
    sampler2D diffuse;
    //vec3 specular;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};
uniform Material material;



void main() {
    //FragColor =texture(ourTexture1, TexCoord);
    //FragColor = mix(texture(ourTexture1, TexCoord), texture(ourTexture2,TexCoord), 0.2)*vec4(ourColor,1.0);
    //FragColor = mix(texture(ourTexture1, TexCoord), texture(ourTexture2,TexCoord), 0.2);
    //FragColor = mix(texture(ourTexture1, TexCoord), texture(ourTexture2,TexCoord), mixValue);

    //Color and light test
    //计算环境光
    //float ambientStrength = 0.1f;
    //vec3 ambient = ambientStrength * lightColor;
    //vec3 ambient = lightColor*material.ambient;
    //vec3 ambient = light.ambient*material.ambient;
    vec3 ambient = light.ambient*vec3(texture(material.diffuse,TexCoord));

    //计算漫反射光照
    vec3 norm = normalize(Normal);
    //vec3 lightDir = normalize(light.position - FragPos);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm,lightDir),0.0);
    //vec3 diffuse = diff*lightColor;
    //vec3 diffuse = light.diffuse*diff*material.diffuse;
    vec3 diffuse = light.diffuse*diff*vec3(texture(material.diffuse,TexCoord));

    // 计算镜面反射光照
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //vec3 specular = specularStrength * spec * lightColor;
    vec3 specular = light.specular*spec*vec3(texture(material.specular,TexCoord));

    //添加发射光贴图
    vec3 emit = vec3(texture(material.emission,TexCoord));

    //vec3 result = (ambient + diffuse + specular) * objectColor;

    vec3 result = (ambient+diffuse+specular);
    FragColor = vec4(result, 1.0f);

    //FragColor = vec4(objectColor,1.0);

}
