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
    vec3 position;  //平行光暂时不需要光源位置 只需要光源方向
    //vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    //衰减系数
    float constant;
    float linear;
    float quadratic;
    //聚光
    vec3 direction;
    float cutOff;
    float outCutOff;
};
uniform Light light;

//平行光结构
struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

//点光源结构
struct PointLight
{
    vec3 position;
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform PointLight pointLight;


//材质结构
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

vec3 CalcDirLight(DirLight light,vec3 normal,vec3 viewDir){
    vec3 lightDir = normalize(-light.direction);
    //计算漫反射光强
    float diff = max(dot(lightDir,normal),0.0);
    //计算镜面反射光强
    vec3 reflectDir = reflect(-lightDir,normal);
    float spec = pow(max(dot(reflectDir,viewDir),0.0),material.shininess);
    //合并各个光照分量
    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoord));
    vec3 diffuse = light.diffuse * vec3(texture(material.diffuse,TexCoord));
    vec3 specular = light.specular*vec3(texture(material.specular,TexCoord));
    return (ambient+diffuse+specular);
}

vec3 CalcPointLight(PointLight light,vec3 normal,vec3 fragPos,vec3 viewDir){
    vec3 lightDir = normalize(light.position - fragPos);
    //计算漫反射光强
    float diff = max(dot(lightDir,normal),0.0);
    //计算镜面反射
    vec3 reflectDir = reflect(-lightDir,normal);
    float spec = pow(max(dot(reflectDir,viewDir),0.0),material.shininess);
    //计算衰减
    float distance = length(light.position-fragPos);
    float attenuation = 1.0/(light.constant+distance*light.linear+distance*distance*light.quadratic);
    //将各个分量合并
    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoord));
    vec3 diffuse = light.diffuse *diff* vec3(texture(material.diffuse,TexCoord));
    vec3 specular = light.specular*vec3(texture(material.specular,TexCoord));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *=attenuation;
    return (ambient+diffuse+specular);
}


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
    vec3 lightDir = normalize(light.position - FragPos);
    //vec3 lightDir = normalize(-light.direction);
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

    //计算点光源衰减
    //float distance = length(light.position - FragPos);
    //float attenuation = 1.0f/(light.constant+light.linear*distance+light.quadratic*(distance*distance));

    //ambient *= attenuation;
    //diffuse *= attenuation;
    //specular *= attenuation;

    //计算聚光裁剪范围
    // vec3 result = vec3(0.0);
    // float theta = dot(lightDir,normalize(-light.direction));
    // if(theta>light.cutOff){
    //     result = diffuse+specular;
    // }else if(theta<light.cutOff&&theta>light.outCutOff){
    //     float epsilon = light.cutOff - light.outCutOff;
    //     float intensity = clamp((theta-light.outCutOff)/epsilon,0.0,1.0);
    //     diffuse*=intensity;
    //     //specular*=intensity;
    //     result = diffuse;
    // }else{
    //     result = vec3(0.0);
    // }
    // result+=ambient;
    //简化聚光边缘的计算
    float theta = dot(lightDir,normalize(-light.direction));
    float intensity = smoothstep(light.outCutOff,light.cutOff,theta);
    vec3 result = ambient+intensity*(diffuse+specular);





    //vec3 result = (ambient + diffuse + specular) * objectColor;

     //result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0f);

    //FragColor = vec4(objectColor,1.0);

}
