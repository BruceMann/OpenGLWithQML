#version 330 core

in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;

void main() {
    //FragColor = vec3(1.0f, 0.5f, 0.2f);
    FragColor = texture(ourTexture,TexCoord);
}
