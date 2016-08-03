//
//  main.cpp
//  Illusion
//
//  Created by Prabhat Godse on 3/28/15.
//  Copyright (c) 2015 biodigital. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <GLUT/glut.h>
#include "Object.h"
#include "Camera.h"
#include "shader.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

Camera *camera;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    camera->renderCamera();
    camera->postProcessing();
    glFlush();
}

void motionFunction(int x, int y) {
    camera->mouseMove(x, y);
    glutPostRedisplay();
}

void mouseEvent(int button, int state, int x, int y) {
    camera->mouseEvent(button, state, x, y);
    glutPostRedisplay();
}

void keyboard(unsigned char c, int a, int b) {
    camera->keyboardEvent(c, a, b);
    glutPostRedisplay();
}

void buildScene() {
    GLuint shaderProgram = LoadShaders("SimpleVertexShader.frag", "SimpleFragmentShader.frag");
    
    Object *object = new Object("", "", shaderProgram);
    object->modelMatrix = glm::translate(object->modelMatrix, glm::vec3(0.0, -3.0, -1.2));
//    object->modelMatrix = glm::scale(object->modelMatrix, glm::vec3(1.0, 2.0, 1.0));
    object->setProjectionViewMatrix(camera->projectionMatrix, camera->viewMatrix);
    object->baseColor = glm::vec3(0.4, 0.26, 0.21);
    camera->addObject(object);
    
    Object *object2 = new Object("", "", shaderProgram);
    object2->modelMatrix = glm::translate(object2->modelMatrix, glm::vec3(0.0, -3.0, 0.0));
    object2->modelMatrix = glm::scale(object2->modelMatrix, glm::vec3(10, 10, 0.2));
    object2->setProjectionViewMatrix(camera->projectionMatrix, camera->viewMatrix);
    object2->baseColor = glm::vec3(0.3, 0.34, 0.25);
    camera->addObject(object2);
    
    Object *object3 = new Object("", "", shaderProgram);
    object3->modelMatrix = glm::translate(object3->modelMatrix, glm::vec3(2.0, -3.0, -1.2));
    object3->modelMatrix = glm::scale(object3->modelMatrix, glm::vec3(0.5, 1.3, 0.78));
    object3->setProjectionViewMatrix(camera->projectionMatrix, camera->viewMatrix);
    object3->baseColor = glm::vec3(0.2, 0.7, 0.41);
    camera->addObject(object3);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.4, 1.0, 1.0, 1.0);
}

void window2() {
    
}

int main(int argc, char** argv)
{
    int W = 900, H = 900;
    
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
    
    glutInitWindowSize(W, H);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("GLUT Program");
    
    camera = new Camera(CAMERA_ORBIT, W, H);
    camera->renderDepth = true;
    buildScene();
    
    glutDisplayFunc(display);
    
    //Mouse events
    glutMotionFunc(motionFunction);
    glutMouseFunc(mouseEvent);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    
    return 0;
}