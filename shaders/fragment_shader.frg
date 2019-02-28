#version 330 core

in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform float mixValue;

//uniform sampler2D ourTexture;
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;


void main() {
    //FragColor = vec3(1.0f, 0.5f, 0.2f);
    //FragColor =texture(ourTexture1, TexCoord);
    //FragColor = mix(texture(ourTexture1, TexCoord), texture(ourTexture2,TexCoord), 0.2)*vec4(ourColor,1.0);
    //FragColor = mix(texture(ourTexture1, TexCoord), texture(ourTexture2,TexCoord), 0.2);
    FragColor = mix(texture(ourTexture1, TexCoord), texture(ourTexture2,TexCoord), mixValue);
}
