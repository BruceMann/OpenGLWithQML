#version 330 core
//varying highp vec3 coords;
in vec3 tttColor;
out vec3 FragColor;
void main() {
    //FragColor = vec3(1.0f, 0.5f, 0.2f);
    FragColor = tttColor;
}
