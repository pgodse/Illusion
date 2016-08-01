//
//  Camera.cpp
//  Illusion
//
//  Created by Prabhat Godse on 3/29/15.
//  Copyright (c) 2015 biodigital. All rights reserved.
//

#include "Camera.h"
#include "shader.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

Camera::Camera(CameraType type, float width, float height) {
    _type = type;
    eyeZ = -10.0;
    _eyePosition = glm::vec3(0, 0, eyeZ);
    _orbitSpeed = 0.2;
    cameraRotation = glm::mat4();
    _prevX = 0;
    _prevY = 0.0;
    initMatrix(width, height);
}
void Camera::destroy() {
    //TODO:: destroy objects.
    sceneObjects.clear();
}

void Camera::initMatrix(float width, float height) {
    _width = width; _height = height;
    projectionMatrix = glm::perspective(45.0f, width / height, 0.1f, 100.0f);
    viewMatrix = glm::lookAt(_eyePosition,
                             glm::vec3(0, 0, 0),
                             glm::vec3(0, 1, 0));
    glm::quat a;
}

void Camera::mouseEvent(int button, int state, int x, int y) {
    if(button == 3) {
        eyeZ += 0.5;
    } else if(button == 4) {
        eyeZ -= 0.5;
    }else {
        _prevX = x; _prevY = y;
    }
}

void Camera::keyboardEvent(unsigned char c, int a, int b) {
    if (c == 'a' || c=='A') {
        eyeZ += 0.5;
    } else if (c == 'z' || c == 'Z') {
        eyeZ -= 0.5;
    }
    mouseMove(_prevX, _prevY);
}

double degToRad(double deg) {
    return  deg * M_PI / 180.0;
}

void Camera::mouseMove(int x, int y) {
    float _horizontalAngle = _orbitSpeed * (degToRad(x - _prevX));
    float _verticalAngle = _orbitSpeed * (degToRad(y - _prevY));
    _prevX = x;
    _prevY = y;
    
    viewMatrix = glm::mat4();
    viewMatrix = glm::translate(viewMatrix, glm::vec3(0, 0, eyeZ));
    
    glm::mat4 newRotation = glm::mat4();
    newRotation = glm::rotate(newRotation, _horizontalAngle, glm::vec3(0, 1, 0));
    newRotation = glm::rotate(newRotation, _verticalAngle, glm::vec3(1, 0, 0));
    cameraRotation = newRotation * cameraRotation;
    
    viewMatrix = viewMatrix * cameraRotation;
}

void Camera::addObject(Object *obj) {
    sceneObjects.push_back(obj);
}

glm::vec3 lightPos = glm::vec3(0.0, -3.5, -1.2);

void Camera::renderCamera() {
    //Draw Scene depth to texture.
    if (renderDepth) {
        //Check if depth buffer and texture created.
        if(depthFrameBuffer == 0) {
            glGenFramebuffers(1, &depthFrameBuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);
            
            // Create a depth texture
            glGenTextures(1, &depthTexture);
            glBindTexture(GL_TEXTURE_2D, depthTexture);
            
            glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
            //        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 900, 900, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
            
            glDrawBuffer(GL_NONE); // No color buffer is drawn to.
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            
            //Compile shader
            depthProgram = LoadShaders( "DepthVertexShader.frag", "DepthFragmentShader.frag" );
            lightMVPUniform = glGetUniformLocation(depthProgram, "MVP");
            modelMatrixUniform = glGetUniformLocation(depthProgram, "modelMatrix");
            
            
            //Build a full screen quad
            GLuint quad_VertexArrayID;
            glGenVertexArrays(1, &quad_VertexArrayID);
            glBindVertexArray(quad_VertexArrayID);
            
            static const GLfloat g_quad_vertex_buffer_data[] = {
                -1.0f, -1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                -1.0f,  1.0f, 0.0f,
                -1.0f,  1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                1.0f,  1.0f, 0.0f,
            };
            
            glGenBuffers(1, &depthQuadBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, depthQuadBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
            
            // Create and compile our GLSL program from the shaders
            quadProgrm = LoadShaders( "QuadVertex.frag", "QuadFragment.frag" );
            depthTextureUniform = glGetUniformLocation(quadProgrm, "renderedTexture");
        }
        
        //Render scene to our framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);
        glViewport(0, 0, _width, _height);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(depthProgram);
        
        //Draw Geometry from light's perspective
        glm::mat4 lightMatrix = glm::lookAt(lightPos, glm::vec3(0,0,0), glm::vec3(0,1,0));
        lightMatrix = projectionMatrix * lightMatrix;
        
        std::vector<Object*>::iterator itr = sceneObjects.begin();
        while (itr < sceneObjects.end()) {
            glUniformMatrix4fv(lightMVPUniform, 1, GL_FALSE, &lightMatrix[0][0]);
            glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, &(*itr)->modelMatrix[0][0]);
            (*itr)->drawObjectType("depth");
            itr++;
        }
        
        //Render to screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, _width, _height);
        
        //Draw the Object with
        
        //Render depth texture to quad.
        if(false) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUseProgram(quadProgrm);
            
            //Bind the rendered texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depthTexture);
            
            glUniform1i(depthTextureUniform, 0);
            
            //Enable the quad buffer
            glEnableVertexAttribArray(0);
            
            glBindBuffer(GL_ARRAY_BUFFER, depthQuadBuffer);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            
            glDrawArrays(GL_TRIANGLES, 0, 6);
            
            glDisableVertexAttribArray(0);
        }
    }
    
    
    //Create post processing color buffer
    if(bufferPostProcess == 0) {
        glGenFramebuffers(1, &bufferPostProcess);
        glBindFramebuffer(GL_FRAMEBUFFER, bufferPostProcess);
        
        // Create a depth texture
        glGenTextures(1, &texturePostProcess);
        glBindTexture(GL_TEXTURE_2D, texturePostProcess);
        
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, _width, _height, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texturePostProcess, 0);
        
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        programPostProcess = LoadShaders("PostProcVertex.frag", "PostProcFragment.frag");
        uniformPostProcessTexture = glGetUniformLocation(programPostProcess, "renderedTexture");
    }
    
    //Get the depth texture.
    //Apply the depth texture to ShadowMaterial.
    if(bufferPostProcess != 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, bufferPostProcess);
    }
    glViewport(0, 0, _width, _height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    std::vector<Object*>::iterator itr = sceneObjects.begin();
    while (itr < sceneObjects.end()) {
        (*itr)->setProjectionViewMatrix(projectionMatrix, viewMatrix);
        (*itr)->depthTexture = depthTexture;
        (*itr)->drawObject();
        itr++;
    }
    
}


void Camera::postProcessing() {
    this->renderCamera();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, _width, _height);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //render the scene to texture
    glUseProgram(programPostProcess);
    
    //Bind the rendered texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturePostProcess);
    
    glUniform1i(uniformPostProcessTexture, 0);
    
    //Enable the quad buffer
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, depthQuadBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(0);
    
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glViewport(0, 0, _width, _height);
    
}