//
//  Object.h
//  Illusion
//
//  Created by Prabhat Godse on 3/29/15.
//  Copyright (c) 2015 biodigital. All rights reserved.
//

#ifndef __Illusion__Object__
#define __Illusion__Object__

#include <stdio.h>
#include <OpenGL/gl3.h>
#include <iostream>
#include "Material.h"

#include "glm/fwd.hpp"
#include "glm/glm.hpp"

class Object {
public:
    Object();
    Object(GLuint shader);
    
    Material *material;
    
    std::string objFile;
    
    GLuint shaderProgram;
    GLuint vertexBuffer;
    GLuint normalBuffer;
    GLuint uvsBuffer;
    int _polyCount;
    
//    GLuint dirVecUniform, dirColorUniformuniformBaseColor , texture0Uniform, texture0
    GLuint depthTextureUniform;
    glm::mat4 _projMat, _projView, _viewMatrix, _normalMatrix, modelMatrix;
    glm::vec4 baseColor;
    GLuint depthTexture;
    
    void destroy();
    
    void setProjectionViewMatrix(glm::mat4 projMat, glm::mat4 viewMat);
    
    GLuint depthFrameBuffer = 0;
    GLuint depthProgram = 0;
    GLuint depthQuadBuffer = 0;
    
    //Blending functions
    bool blending = false;
    bool skybox = false;
    
    //Shadertype: standard :: uses regular color texture
    // depth:: renders to depth texture.
    
    virtual void drawObject();
    void drawObjectType(std::string type);
    void drawObjectDepth();
    
    void initGeometry(std::string fileName);
    
};
#endif /* defined(__Illusion__Object__) */
