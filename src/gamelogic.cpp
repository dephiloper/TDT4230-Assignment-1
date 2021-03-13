#include <chrono>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <SFML/Audio/SoundBuffer.hpp>
#include <utilities/shader.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <utilities/timeutils.h>
#include <utilities/mesh.h>
#include <utilities/shapes.h>
#include <utilities/glutils.h>
#include <SFML/Audio/Sound.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fmt/format.h>
#include "gamelogic.h"
#include "sceneGraph.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "utilities/imageLoader.hpp"
#include "utilities/glfont.h"

enum KeyFrameAction {
    BOTTOM, TOP
};

#include <timestamps.h>

double padPositionX = 0;
double padPositionZ = 0;

unsigned int currentKeyFrame = 0;
unsigned int previousKeyFrame = 0;

SceneNode* rootNode;
SceneNode* boxNode;
SceneNode* ballNode;
SceneNode* padNode;

// point light scene node declaration
SceneNode* pointLightA;
SceneNode* pointLightB;
SceneNode* pointLightC;

SceneNode* textNode;

double ballRadius = 3.0f;

// These are heap allocated, because they should not be initialised at the start of the program
sf::SoundBuffer* buffer;
Gloom::Shader* gameShader;
Gloom::Shader* uiShader;
sf::Sound* sound;

const glm::vec3 boxDimensions(180, 90, 90);
const glm::vec3 padDimensions(30, 3, 40);

glm::vec3 ballPosition(0, ballRadius + padDimensions.y, boxDimensions.z / 2);
glm::vec3 ballDirection(1, 1, 0.2f);
glm::vec3 cameraPosition = glm::vec3(0, 2, -20);

CommandLineOptions options;

bool hasStarted = false;
bool hasLost = false;
bool jumpedToNextFrame = false;
bool isPaused = false;

bool mouseLeftPressed   = false;
bool mouseLeftReleased  = false;
bool mouseRightPressed  = false;
bool mouseRightReleased = false;

// Modify if you want the music to start further on in the track. Measured in seconds.
const float debug_startTime = 0;
double totalElapsedTime = debug_startTime;
double gameElapsedTime = debug_startTime;

double mouseSensitivity = 1.0;
double lastMouseX = windowWidth / 2;
double lastMouseY = windowHeight / 2;

void mouseCallback(GLFWwindow* window, double x, double y) {
    double deltaX = x - lastMouseX;
    double deltaY = y - lastMouseY;

    padPositionX -= mouseSensitivity * deltaX / windowWidth;
    padPositionZ -= mouseSensitivity * deltaY / windowHeight;

    if (padPositionX > 1) padPositionX = 1;
    if (padPositionX < 0) padPositionX = 0;
    if (padPositionZ > 1) padPositionZ = 1;
    if (padPositionZ < 0) padPositionZ = 0;

    glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
}

//// A few lines to help you if you've never used c++ structs
// struct LightSource {
//     bool a_placeholder_value;
// };
// LightSource lightSources[/*Put number of light sources you want here*/];

void initGame(GLFWwindow* window, CommandLineOptions gameOptions) {
    buffer = new sf::SoundBuffer();
    if (!buffer->loadFromFile("../res/Hall of the Mountain King.ogg")) {
        return;
    }

    options = gameOptions;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetCursorPosCallback(window, mouseCallback);

    gameShader = new Gloom::Shader();
    uiShader = new Gloom::Shader();
    gameShader->makeBasicShader("../res/shaders/simple.vert", "../res/shaders/simple.frag");
    uiShader->makeBasicShader("../res/shaders/ui.vert", "../res/shaders/ui.frag");

    // task 2-1b
    PNGImage image = loadPNGFile("../res/textures/charmap.png");
    // task 2-1c
    unsigned int textureCharMap = generateTexture(image);

    // task 2-1b
    image = loadPNGFile("../res/textures/Brick03_col.png");
    unsigned int textureDiffuse = generateTexture(image);

    image = loadPNGFile("../res/textures/Brick03_nrm.png");
    unsigned int textureNormalMap = generateTexture(image);
    
    // Create meshes
    Mesh pad = cube(padDimensions, glm::vec2(30, 40), true);
    Mesh box = cube(boxDimensions, glm::vec2(90), true, true);
    std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

    computeTangentBasis(box.indices, box.vertices, box.textureCoordinates, box.normals, tangents, bitangents);
    box.tangents = tangents;
    box.bitangents = bitangents;
    std::cout << glm::to_string(box.tangents.at(0)) << std::endl;

    Mesh sphere = generateSphere(1.0, 40, 40);
    // task 2-1g
    Mesh text = generateTextGeometryBuffer("hello", 39.0 / 29.0, 5 * 29.0);

    // Fill buffers
    unsigned int ballVAO = generateBuffer(sphere);
    unsigned int boxVAO  = generateBuffer(box);
    unsigned int padVAO  = generateBuffer(pad);

    // task 2-1g
    unsigned int textVAO = generateBuffer(text);

    // Construct scene
    rootNode = createSceneNode();
    // task 2-3a
    boxNode  = createSceneNode(GEOMETRY_NORMAL_MAP);
    boxNode->textureId = textureDiffuse;
    boxNode->normalMapTextureId = textureNormalMap;
    padNode  = createSceneNode();
    ballNode = createSceneNode();

    //task 1-1a - point light scene nodes with ids for referencing
    pointLightA = createSceneNode(POINT_LIGHT);
    pointLightA->position = glm::vec3(0.0, 10.0, 0.0);
    pointLightB = createSceneNode(POINT_LIGHT);
    pointLightB->position = glm::vec3(-60.0, 10.0, -120.0);
    pointLightC = createSceneNode(POINT_LIGHT);
    pointLightC->position = glm::vec3(60.0, 10.0, -120.0);

    // task 2-1h
    textNode = createSceneNode(GEOMETRY_2D);
    textNode->textureId = textureCharMap;
    textNode->position = glm::vec3(windowWidth/2.0f, windowHeight/2.0f, 0.0f);

    rootNode->children.push_back(boxNode);
    rootNode->children.push_back(padNode);
    rootNode->children.push_back(ballNode);
    
    // add light node A to the pad 
    padNode->children.push_back(pointLightA);
    rootNode->children.push_back(pointLightB);
    rootNode->children.push_back(pointLightC);
    rootNode->children.push_back(textNode);

    boxNode->vertexArrayObjectID = boxVAO;
    boxNode->VAOIndexCount = box.indices.size();

    padNode->vertexArrayObjectID = padVAO;
    padNode->VAOIndexCount = pad.indices.size();

    ballNode->vertexArrayObjectID = ballVAO;
    ballNode->VAOIndexCount = sphere.indices.size();

    textNode->vertexArrayObjectID = textVAO;
    textNode->VAOIndexCount = text.indices.size();

    getTimeDeltaSeconds();

    std::cout << fmt::format("Initialized scene with {} SceneNodes.", totalChildren(rootNode)) << std::endl;

    std::cout << "Ready. Click to start!" << std::endl;
}

void updateFrame(GLFWwindow* window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    double timeDelta = getTimeDeltaSeconds();

    const float ballBottomY = boxNode->position.y - (boxDimensions.y/2) + ballRadius + padDimensions.y;
    const float ballTopY    = boxNode->position.y + (boxDimensions.y/2) - ballRadius;
    const float BallVerticalTravelDistance = ballTopY - ballBottomY;

    const float cameraWallOffset = 30; // Arbitrary addition to prevent ball from going too much into camera

    const float ballMinX = boxNode->position.x - (boxDimensions.x/2) + ballRadius;
    const float ballMaxX = boxNode->position.x + (boxDimensions.x/2) - ballRadius;
    const float ballMinZ = boxNode->position.z - (boxDimensions.z/2) + ballRadius;
    const float ballMaxZ = boxNode->position.z + (boxDimensions.z/2) - ballRadius - cameraWallOffset;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
        mouseLeftPressed = true;
        mouseLeftReleased = false;
    } else {
        mouseLeftReleased = mouseLeftPressed;
        mouseLeftPressed = false;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
        mouseRightPressed = true;
        mouseRightReleased = false;
    } else {
        mouseRightReleased = mouseRightPressed;
        mouseRightPressed = false;
    }
    
    if(!hasStarted) {
        if (mouseLeftPressed) {
            if (options.enableMusic) {
                sound = new sf::Sound();
                sound->setBuffer(*buffer);
                sf::Time startTime = sf::seconds(debug_startTime);
                sound->setPlayingOffset(startTime);
                sound->play();
            }
            totalElapsedTime = debug_startTime;
            gameElapsedTime = debug_startTime;
            hasStarted = true;
        }

        ballPosition.x = ballMinX + (1 - padPositionX) * (ballMaxX - ballMinX);
        ballPosition.y = ballBottomY;
        ballPosition.z = ballMinZ + (1 - padPositionZ) * ((ballMaxZ+cameraWallOffset) - ballMinZ);
    } else {
        totalElapsedTime += timeDelta;
        if(hasLost) {
            if (mouseLeftReleased) {
                hasLost = false;
                hasStarted = false;
                currentKeyFrame = 0;
                previousKeyFrame = 0;
            }
        } else if (isPaused) {
            if (mouseRightReleased) {
                isPaused = false;
                if (options.enableMusic) {
                    sound->play();
                }
            }
        } else {
            gameElapsedTime += timeDelta;
            if (mouseRightReleased) {
                isPaused = true;
                if (options.enableMusic) {
                    sound->pause();
                }
            }
            // Get the timing for the beat of the song
            for (unsigned int i = currentKeyFrame; i < keyFrameTimeStamps.size(); i++) {
                if (gameElapsedTime < keyFrameTimeStamps.at(i)) {
                    continue;
                }
                currentKeyFrame = i;
            }

            jumpedToNextFrame = currentKeyFrame != previousKeyFrame;
            previousKeyFrame = currentKeyFrame;

            double frameStart = keyFrameTimeStamps.at(currentKeyFrame);
            double frameEnd = keyFrameTimeStamps.at(currentKeyFrame + 1); // Assumes last keyframe at infinity

            double elapsedTimeInFrame = gameElapsedTime - frameStart;
            double frameDuration = frameEnd - frameStart;
            double fractionFrameComplete = elapsedTimeInFrame / frameDuration;

            double ballYCoord = 0;

            KeyFrameAction currentOrigin = keyFrameDirections.at(currentKeyFrame);
            KeyFrameAction currentDestination = keyFrameDirections.at(currentKeyFrame + 1);

            // Synchronize ball with music
            if (currentOrigin == BOTTOM && currentDestination == BOTTOM) {
                ballYCoord = ballBottomY;
            } else if (currentOrigin == TOP && currentDestination == TOP) {
                ballYCoord = ballBottomY + BallVerticalTravelDistance;
            } else if (currentDestination == BOTTOM) {
                ballYCoord = ballBottomY + BallVerticalTravelDistance * (1 - fractionFrameComplete);
            } else if (currentDestination == TOP) {
                ballYCoord = ballBottomY + BallVerticalTravelDistance * fractionFrameComplete;
            }

            // Make ball move
            const float ballSpeed = 60.0f;
            ballPosition.x += timeDelta * ballSpeed * ballDirection.x;
            ballPosition.y = ballYCoord;
            ballPosition.z += timeDelta * ballSpeed * ballDirection.z;

            // Make ball bounce
            if (ballPosition.x < ballMinX) {
                ballPosition.x = ballMinX;
                ballDirection.x *= -1;
            } else if (ballPosition.x > ballMaxX) {
                ballPosition.x = ballMaxX;
                ballDirection.x *= -1;
            }
            if (ballPosition.z < ballMinZ) {
                ballPosition.z = ballMinZ;
                ballDirection.z *= -1;
            } else if (ballPosition.z > ballMaxZ) {
                ballPosition.z = ballMaxZ;
                ballDirection.z *= -1;
            }

            if(options.enableAutoplay) {
                padPositionX = 1-(ballPosition.x - ballMinX) / (ballMaxX - ballMinX);
                padPositionZ = 1-(ballPosition.z - ballMinZ) / ((ballMaxZ+cameraWallOffset) - ballMinZ);
            }

            // Check if the ball is hitting the pad when the ball is at the bottom.
            // If not, you just lost the game! (hehe)
            if (jumpedToNextFrame && currentOrigin == BOTTOM && currentDestination == TOP) {
                double padLeftX  = boxNode->position.x - (boxDimensions.x/2) + (1 - padPositionX) * (boxDimensions.x - padDimensions.x);
                double padRightX = padLeftX + padDimensions.x;
                double padFrontZ = boxNode->position.z - (boxDimensions.z/2) + (1 - padPositionZ) * (boxDimensions.z - padDimensions.z);
                double padBackZ  = padFrontZ + padDimensions.z;

                if (   ballPosition.x < padLeftX
                    || ballPosition.x > padRightX
                    || ballPosition.z < padFrontZ
                    || ballPosition.z > padBackZ) {
                    hasLost = true;
                    if (options.enableMusic) {
                        sound->stop();
                        delete sound;
                    }
                }
            }
        }
    }

    glm::mat4 projection = glm::perspective(glm::radians(80.0f), float(windowWidth) / float(windowHeight), 0.1f, 350.f);

    // Some math to make the camera move in a nice way
    float lookRotation = -0.6 / (1 + exp(-5 * (padPositionX-0.5))) + 0.3;
    glm::mat4 cameraTransform = 
                    glm::rotate(0.3f + 0.2f * float(-padPositionZ*padPositionZ), glm::vec3(1, 0, 0)) *
                    glm::rotate(lookRotation, glm::vec3(0, 1, 0)) *
                    glm::translate(-cameraPosition);

    // projection matrix * view matrix
    glm::mat4 VP = projection * cameraTransform;

    // Move and rotate various SceneNodes
    boxNode->position = { 0, -10, -80 };

    ballNode->position = ballPosition;
    ballNode->scale = glm::vec3(ballRadius);
    ballNode->rotation = { 0, totalElapsedTime*2, 0 };

    padNode->position  = { 
        boxNode->position.x - (boxDimensions.x/2) + (padDimensions.x/2) + (1 - padPositionX) * (boxDimensions.x - padDimensions.x), 
        boxNode->position.y - (boxDimensions.y/2) + (padDimensions.y/2), 
        boxNode->position.z - (boxDimensions.z/2) + (padDimensions.z/2) + (1 - padPositionZ) * (boxDimensions.z - padDimensions.z)
    };

    updateNodeTransformations(rootNode, VP, glm::mat4(1.0));
}

// pass model matrix of parent to calculate the correct model matrix for all children
void updateNodeTransformations(SceneNode* node, glm::mat4 VP, glm::mat4 modelMatrix) {
    
    // task 1-1b - calculate model matrix separately
    // model matrix = parentModelMatrix * own transformations
    node->modelMatrix = modelMatrix *
              glm::translate(node->position)
            * glm::translate(node->referencePoint)
            * glm::rotate(node->rotation.y, glm::vec3(0,1,0))
            * glm::rotate(node->rotation.x, glm::vec3(1,0,0))
            * glm::rotate(node->rotation.z, glm::vec3(0,0,1))
            * glm::scale(node->scale)
            * glm::translate(-node->referencePoint);

    // vp * parent model matrix * node model matrix
    node->currentTransformationMatrix = VP * node->modelMatrix;

    switch(node->nodeType) {
        case GEOMETRY: break;
        case POINT_LIGHT:break;
        case SPOT_LIGHT: break;
    }

    for(SceneNode* child : node->children) {
        updateNodeTransformations(child, VP, node->modelMatrix);
    }
}

void renderNode(SceneNode* node) {
    if (node->nodeType == SceneNodeType::GEOMETRY_2D) return;
    
    unsigned int MVP = gameShader->getUniformFromName("MVP");
    glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(node->currentTransformationMatrix));

    // task 1-1b - pass matrix separately as uniform to shaders
    unsigned int model = gameShader->getUniformFromName("model");
    glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(node->modelMatrix));

    // task 1-1d - compute transpose of inverse of model matrix and take top 3x3 part of it
    glm::mat3 normal = glm::mat3(glm::transpose(glm::inverse(node->modelMatrix)));
    
    unsigned int normalMat = gameShader->getUniformFromName("normal_mat");
    glUniformMatrix3fv(normalMat, 1, GL_FALSE, glm::value_ptr(normal));

    switch(node->nodeType) {
        case GEOMETRY:
            if(node->vertexArrayObjectID != -1) {
                glBindVertexArray(node->vertexArrayObjectID);
                glDrawElements(GL_TRIANGLES, node->VAOIndexCount, GL_UNSIGNED_INT, nullptr);
            }
            break;
        case POINT_LIGHT:
            break;
        case SPOT_LIGHT: 
            break;
        case GEOMETRY_NORMAL_MAP:
            if(node->vertexArrayObjectID != -1) {
                unsigned int useTexture = gameShader->getUniformFromName("useTexture");
                glUniform1f(useTexture, 1.0f);

                glActiveTexture(GL_TEXTURE0);
                glUniform1i(glGetUniformLocation(gameShader->get(), "textureSampler"), 0);
                glBindTexture(GL_TEXTURE_2D, node->textureId);

                glActiveTexture(GL_TEXTURE1);
                glUniform1i(glGetUniformLocation(gameShader->get(), "normalMapSampler"), 1);
                glBindTexture(GL_TEXTURE_2D, node->normalMapTextureId);

                glBindVertexArray(node->vertexArrayObjectID);
                glDrawElements(GL_TRIANGLES, node->VAOIndexCount, GL_UNSIGNED_INT, nullptr);
                glUniform1f(useTexture, 0.0f);
            }
    }

    for(SceneNode* child : node->children) {
        renderNode(child);
    }
}

void renderFrame() {
    // task 2-1i enabling and disabling specific shaders
    gameShader->activate();

    // pass view position to shader
    unsigned int cameraPos = gameShader->getUniformFromName("viewPos");
    glUniform3fv(cameraPos, 1, glm::value_ptr(cameraPosition));

    // pass ball position to shader
    unsigned int ballPos = gameShader->getUniformFromName("ballPos");
    glUniform3fv(ballPos, 1, glm::value_ptr(ballPosition));

    // task 1-1c - calculate positions of all point lights and pass
    // light[0]
    glm::vec3 lightPos = pointLightA->modelMatrix * glm::vec4(0,0,0,1);
    unsigned int pointLight = gameShader->getUniformFromName("lights[0].position");
    glUniform3fv(pointLight, 1, glm::value_ptr(lightPos));
    unsigned int color = gameShader->getUniformFromName("lights[0].color");
    glUniform3fv(color, 1, glm::value_ptr(glm::vec3(1.0, 0.0, 0.0)));

    // light[1]
    lightPos = pointLightB->modelMatrix * glm::vec4(0,0,0,1);
    pointLight = gameShader->getUniformFromName("lights[1].position");
    glUniform3fv(pointLight, 1, glm::value_ptr(lightPos));
    color = gameShader->getUniformFromName("lights[1].color");
    glUniform3fv(color, 1, glm::value_ptr(glm::vec3(0.0, 1.0, 0.0)));

    // light[2]
    lightPos = pointLightC->modelMatrix * glm::vec4(0,0,0,1);
    pointLight = gameShader->getUniformFromName("lights[2].position");
    glUniform3fv(pointLight, 1, glm::value_ptr(lightPos));
    color = gameShader->getUniformFromName("lights[2].color");
    glUniform3fv(color, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 1.0)));

    renderNode(rootNode);
    gameShader->deactivate();

    renderUi();
}

void renderUi() {
    // task 2-1i enabling and disabling specific shaders
    uiShader->activate();

    glm::mat4 modelMatrix =
        glm::translate(textNode->position)
        * glm::translate(textNode->referencePoint)
        * glm::rotate(textNode->rotation.y, glm::vec3(0,1,0))
        * glm::rotate(textNode->rotation.x, glm::vec3(1,0,0))
        * glm::rotate(textNode->rotation.z, glm::vec3(0,0,1))
        * glm::scale(textNode->scale)
        * glm::translate(-textNode->referencePoint);

    glm::mat4 proj = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, -1.0f, 1.0f);
    unsigned int uiProj = uiShader->getUniformFromName("uiProj");
    unsigned int model = uiShader->getUniformFromName("model");
    glUniformMatrix4fv(uiProj, 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textNode->textureId);
    glBindVertexArray(textNode->vertexArrayObjectID);

    glDrawElements(GL_TRIANGLES, textNode->VAOIndexCount, GL_UNSIGNED_INT, nullptr);

    uiShader->deactivate();

    //unsigned int textureCoords = gameShader->getUniformFromName("textureCoords");
    //glUniform3fv(textureCoords, 1, glm::value_ptr(textNode.));
}
