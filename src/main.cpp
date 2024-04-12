#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
#include "learnopengl/filesystem.h"

#define GROUND_DIMENSION (20)

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const *path);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    //glEnable(GL_CULL_FACE);

    // ground texture

    float groundVertices[] = {
            // positions          // normals        //texCoords
            0.5f,  0.5f,  0.0f,   0.0f, 0.0f, -1.0f, 1.0f,  1.0f,     // top right
            0.5f, -0.5f,  0.0f,   0.0f, 0.0f, -1.0f, 1.0f,  0.0f,     // bottom right
            -0.5f, -0.5f,  0.0f,  0.0f, 0.0f, -1.0f, 0.0f,  0.0f,     // bottom left
            -0.5f,  0.5f,  0.0f,  0.0f, 0.0f, -1.0f, 0.0f,  1.0f      // top left
    };

    unsigned int groundIndices[] = {
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
    };

    unsigned int groundVAO, groundVBO, groundEBO;
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glGenBuffers(1, &groundEBO);

    glBindVertexArray(groundVAO);

    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int diffuseGround = loadTexture(FileSystem::getPath("resources/textures/tough_grass.jpg").c_str());
    //unsigned int specularGround = loadTexture(FileSystem::getPath("resources/textures/Pebbles_Height.png").c_str());

    // build and compile shaders
    // -------------------------
    Shader lightingShader("resources/shaders/lightingShader.vs", "resources/shaders/lightingShader.fs");
    // load models
    // -----------
    Model t10mModel(FileSystem::getPath("resources/objects/tank_t10m/tank_t10m.obj"));
    t10mModel.SetShaderTextureNamePrefix("material.");

    Model ammoBoxModel(FileSystem::getPath("resources/objects/ammo_box/ammo_box.obj"));
    ammoBoxModel.SetShaderTextureNamePrefix("material.");

    Model watchtowerModel(FileSystem::getPath("resources/objects/watchtower/watchtower.obj"));
    watchtowerModel.SetShaderTextureNamePrefix("material.");

    Model cratesAndBarrelsModel(FileSystem::getPath("resources/objects/crates_and_barrels/crates_and_barrels.obj"));
    cratesAndBarrelsModel.SetShaderTextureNamePrefix("material.");

    Model redwoodTreeModel(FileSystem::getPath("resources/objects/redwood_tree/redwood_tree.obj"));
    redwoodTreeModel.SetShaderTextureNamePrefix("material.");

    Model pineTreeModel(FileSystem::getPath("resources/objects/pine_tree/pine_tree.obj"));
    pineTreeModel.SetShaderTextureNamePrefix("material.");

    Model pineTreeLowpolyModel(FileSystem::getPath("resources/objects/pine_tree_low-poly/pine_tree_low-poly.obj"));
    pineTreeLowpolyModel.SetShaderTextureNamePrefix("material.");

    Model kv2Model(FileSystem::getPath("resources/objects/kv2/kv2.obj"));
    kv2Model.SetShaderTextureNamePrefix("material.");

    Model oilDrumsModel(FileSystem::getPath("resources/objects/oil_drums/oil_drums.obj"));
    oilDrumsModel.SetShaderTextureNamePrefix("material.");

    Model rustyOilBarrelsModel(FileSystem::getPath("resources/objects/rusty_oil_barrels/rusty_oil_barrels.obj"));
    rustyOilBarrelsModel.SetShaderTextureNamePrefix("material.");

    Model reflectorModel(FileSystem::getPath("resources/objects/reflector/reflector.obj"));
    reflectorModel.SetShaderTextureNamePrefix("material.");

    Model forestModel(FileSystem::getPath("resources/objects/forest/forest.obj"));
    forestModel.SetShaderTextureNamePrefix("material.");

    Model challenger2Model(FileSystem::getPath("resources/objects/challenger2_shooting_range/challenger2_shooting_range.obj"));
    challenger2Model.SetShaderTextureNamePrefix("material.");

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        lightingShader.use();
        lightingShader.setVec3("viewPosition", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);

        // reflector spotlight
        glm::vec3 reflector1LightPos = glm::vec3(-10.0f, 5.5f, -3.0f);
        lightingShader.setVec3("spotLight1.position", reflector1LightPos);
        lightingShader.setVec3("spotLight1.direction", 11.0f, -5.5f, -11.0f);
        lightingShader.setVec3("spotLight1.ambient", 0.1f, 0.1f, 0.f);
        lightingShader.setVec3("spotLight1.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight1.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight1.constant", 1.0f);
        lightingShader.setFloat("spotLight1.linear", 0.007f);
        lightingShader.setFloat("spotLight1.quadratic", 0.0002f);
        lightingShader.setFloat("spotLight1.cutOff", glm::cos(glm::radians(28.0f)));
        lightingShader.setFloat("spotLight1.outerCutOff", glm::cos(glm::radians(30.0f)));

        glm::vec3 reflector2LightPos = glm::vec3(10.0f, 5.5f, -3.0f);
        lightingShader.setVec3("spotLight2.position", reflector2LightPos);
        lightingShader.setVec3("spotLight2.direction", -11.0f, -5.5f, -11.0f);
        lightingShader.setVec3("spotLight2.ambient", 0.1f, 0.1f, 0.f);
        lightingShader.setVec3("spotLight2.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight2.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight2.constant", 1.0f);
        lightingShader.setFloat("spotLight2.linear", 0.007f);
        lightingShader.setFloat("spotLight2.quadratic", 0.0002f);
        lightingShader.setFloat("spotLight2.cutOff", glm::cos(glm::radians(28.0f)));
        lightingShader.setFloat("spotLight2.outerCutOff", glm::cos(glm::radians(30.0f)));


        // directional Light
        lightingShader.setVec3("dirLight.direction", -1.0f, -1.0f, 0.0f);
        lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        lightingShader.use();
        // render tank t10m
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, -10.0f)); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(1.0f, 0.3f, 0.3f));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);
        t10mModel.Draw(lightingShader);

        // render tank kv2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-8.0f, -2.0f, -25.0f)); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(1.0f, 0.3f, 0.3f));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);
        kv2Model.Draw(lightingShader);

        // render tank challenger2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(10.0f, -2.0f, -25.0f)); // translate it down so it's at the center of the scene
        //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);
        challenger2Model.Draw(lightingShader);

        // render ammo boxes
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-7.0f, -2.0f, -12.0f)); // translate it down so it's at the center of the scene
        //model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);
        ammoBoxModel.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-6.32f, -2.0f, -11.83f)); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);
        ammoBoxModel.Draw(lightingShader);

        // render watchtower
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-9.0f, -2.0f, -17.0f)); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);
        watchtowerModel.Draw(lightingShader);

        // render crates and barrels
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(7.0f, -2.0f, -17.0f)); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);
        cratesAndBarrelsModel.Draw(lightingShader);

        // render oil drums
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(10.0f, -2.0f, -7.0f)); // translate it down so it's at the center of the scene
       //model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(1.0f, 0.3f, 0.3f));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);
        oilDrumsModel.Draw(lightingShader);

        // render rusty oil barrels
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(10.0f, -2.0f, -10.0f)); // translate it down so it's at the center of the scene
        //model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.004f, 0.004f, 0.004f));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);
        rustyOilBarrelsModel.Draw(lightingShader);

        // render reflector
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-10.0f, -2.0f, -3.0f)); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(1.0f, 0.3f, 0.3f));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);
        reflectorModel.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(10.0f, -2.0f, -3.0f)); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(-135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(1.0f, 0.3f, 0.3f));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);
        reflectorModel.Draw(lightingShader);

        // render forest
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-37.0f, -2.0f, -10.0f)); // translate it down so it's at the center of the scene
        //model = glm::rotate(model, glm::radians(-135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(1.0f, 0.3f, 0.3f));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);
        forestModel.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-15.0f, -2.0f, -50.0f)); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(1.0f, 0.3f, 0.3f));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);
        forestModel.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(32.0f, -2.0f, -50.0f)); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(1.0f, 0.3f, 0.3f));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);
        forestModel.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(38.0f, -2.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(1.0f, 0.3f, 0.3f));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);
        forestModel.Draw(lightingShader);



        // render ground texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseGround);

        for(int i = 0; i < GROUND_DIMENSION; i ++) {
            for(int j = 0; j < GROUND_DIMENSION; j ++) {
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(((float)i - GROUND_DIMENSION / 2.0f) * 2.0f, -2.0f, j * (-2.0f)));
                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(2.0f));
                lightingShader.setMat4("model", model);
                lightingShader.setMat4("view", view);
                lightingShader.setMat4("projection", projection);

                glBindVertexArray(groundVAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            }
        }




        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const *path){
    unsigned textureId;
    glGenTextures(1, &textureId);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

    if(data)
    {
        GLenum format = GL_RED;
        if(nrChannels == 1)
            format = GL_RED;
        else if(nrChannels == 3)
            format = GL_RGB;
        else if(nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureId);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else{
        cout << "Texture failed to load!" << path << "\n";
        stbi_image_free(data);
    }

    return textureId;
}