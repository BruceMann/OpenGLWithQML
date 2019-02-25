#ifndef VERTEXDATA_H
#define VERTEXDATA_H

//    float values[] = {
//        -0.5f, -0.5f, 0.0f, // Left
//          0.5f, -0.5f, 0.0f, // Right
//          0.0f,  0.5f, 0.0f  // Top
//    };


//    GLfloat vertices[] = {
//         0.5f,  0.5f, 0.0f,  // Top Right
//         0.5f, -0.5f, 0.0f,  // Bottom Right
//        -0.5f, -0.5f, 0.0f,  // Bottom Left
//        -0.5f,  0.5f, 0.0f   // Top Left
//    };
//    GLuint indices[] = {  // Note that we start from 0!
//        0, 1, 3,  // First Triangle
//        1, 2, 3   // Second Triangle
//    };

//    GLfloat vertices[] = {
//        // First triangle
//        -0.9f, -0.5f, 0.0f,  // Left
//        -0.0f, -0.5f, 0.0f,  // Right
//        -0.45f, 0.5f, 0.0f,  // Top
//        // Second triangle
//         0.0f, -0.5f, 0.0f,  // Left
//         0.9f, -0.5f, 0.0f,  // Right
//         0.45f, 0.5f, 0.0f   // Top
//    };

//GLfloat vertices[] = {
//    // Positions         // Colors
//     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Bottom Right
//    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // Bottom Left
//     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // Top
//};

GLfloat vertices[] = {
//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
};

GLuint indices[] = {  // Note that we start from 0!
    0, 1, 3, // First Triangle
    1, 2, 3  // Second Triangle
};

#endif // VERTEXDATA_H
