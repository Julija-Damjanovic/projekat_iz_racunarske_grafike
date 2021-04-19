#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
#include <rg/Texture2D.h>


void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);

const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 700;

//kamera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

//vreme
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 position;

//svetlost
struct PointLight {
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

int main()
{
    // glfw: inicijalizacija i konfiguracija
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //pravimo prozor
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Projekat", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // sklanjamo misa
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ucitavanje opengl funkcija
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);


    Shader pozadinaShader("resources/shaders/pozadina.vs", "resources/shaders/pozadina.fs");
    Shader refleksijaShader("resources/shaders/refleksijaKocka.vs", "resources/shaders/refleksijaKocka.fs");

    Shader objekatShader("resources/shaders/objekatKocka.vs", "resources/shaders/objekatKocka.fs");
    Shader lightCubeShader("resources/shaders/svetloKocka.vs", "resources/shaders/svetloKocka.fs");

    Shader modelShader("resources/shaders/model.vs", "resources/shaders/model.fs");
    // load models
    Model modelPtica(FileSystem::getPath("resources/objects/ptica/12213_Bird_v1_l3.obj"));
    modelPtica.SetShaderTextureNamePrefix("material.");



    float kockaVertices[] = {
            // tacke            // tekstura    //normale
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
    };



    float pozadinaVertices[] = {
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f
    };

    unsigned int indices[] = {
            1, 3, 5,
            5, 6, 1,
            2, 3, 1,
            1, 0, 2,
            5, 4, 7,
            7, 6, 5,
            2, 0, 7,
            7, 4, 2,
            1, 6, 7,
            7, 0, 1,
            3, 2, 5,
            5, 2, 4
    };




    unsigned int svetloVAO, svetloVBO;
    glGenVertexArrays(1, &svetloVAO);
    glGenBuffers(1, &svetloVBO);
    glBindVertexArray(svetloVAO);
    glBindBuffer(GL_ARRAY_BUFFER, svetloVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kockaVertices), &kockaVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    unsigned int svetlecaKockaVAO, svetlecaKockaVBO;
    glGenVertexArrays(1, &svetlecaKockaVAO);
    glGenBuffers(1, &svetloVBO);
    glBindVertexArray(svetloVAO);
    glBindBuffer(GL_ARRAY_BUFFER, svetloVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kockaVertices), &kockaVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int refleksijaVAO, refleksijaVBO;
    glGenVertexArrays(1, &refleksijaVAO);
    glGenBuffers(1, &refleksijaVBO);
    glBindVertexArray(refleksijaVAO);
    glBindBuffer(GL_ARRAY_BUFFER, refleksijaVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kockaVertices), &kockaVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int pozadinaVAO, pozadinaVBO, pozadinaEBO;
    glGenVertexArrays(1, &pozadinaVAO);
    glGenBuffers(1, &pozadinaVBO);
    glGenBuffers(1, &pozadinaEBO);
    glBindVertexArray(pozadinaVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pozadinaVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pozadinaVertices), &pozadinaVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pozadinaEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);


    Texture2D diffuseMap (FileSystem::getPath("resources/textures/Tekstura.jpg").c_str());
    Texture2D specularMap (FileSystem::getPath("resources/textures/SpecularMap.jpg").c_str());

    vector<std::string> faces
            {
                    FileSystem::getPath("resources/textures/priroda/right.jpg"),
                    FileSystem::getPath("resources/textures/priroda/left.jpg"),
                    FileSystem::getPath("resources/textures/priroda/top.jpg"),
                    FileSystem::getPath("resources/textures/priroda/bottom.jpg"),
                    FileSystem::getPath("resources/textures/priroda/front.jpg"),
                    FileSystem::getPath("resources/textures/priroda/back.jpg")
            };
    unsigned int cubemapTexture = loadCubemap(faces);

    refleksijaShader.use();
    refleksijaShader.setInt("refleksija", 0);

    pozadinaShader.use();
    pozadinaShader.setInt("pozadina", 0);

    objekatShader.use();
    objekatShader.setInt("material.diffuse", 0);
    objekatShader.setInt("material.specular", 1);

    PointLight pointLight;
    pointLight.ambient = glm::vec3(0.4, 0.4, 0.2);
    pointLight.diffuse = glm::vec3(0.6, 0.5, 0.6);
    pointLight.specular = glm::vec3(1.0, 1.0, 1.0);
    pointLight.constant = 1.0f;
    pointLight.linear = 0.09f;
    pointLight.quadratic = 0.032f;
    pointLight.position = glm::vec3(2.0, 2.0, 2.0);

    DirLight dirLight;
    dirLight.direction = glm::vec3 (-0.2f, -1.0f, -0.3f);
    dirLight.ambient = glm::vec3 (0.05f, 0.05f, 0.05f);
    dirLight.diffuse = glm::vec3 (0.4f, 0.4f, 0.4f);
    dirLight.specular = glm::vec3 (0.5f, 0.5f, 0.5f);


    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        processInput(window);


        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glCullFace(GL_BACK);

        // refleksijaKocka
        refleksijaShader.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3 (3,0,5));
        model = glm::rotate(model, 30.0f, glm::vec3(1.0, 0, 1.0));
        model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        refleksijaShader.setMat4("model", model);
        refleksijaShader.setMat4("view", view);
        refleksijaShader.setMat4("projection", projection);
        refleksijaShader.setVec3("cameraPos", camera.Position);
        glBindVertexArray(refleksijaVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);



        //tackasto svetlo
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, pointLight.position);
        model = glm::scale(model, glm::vec3(0.5f)); // a smaller cube
        lightCubeShader.setMat4("model", model);
        glBindVertexArray(svetloVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);


        glCullFace(GL_FRONT);

        //objekat kocka
        objekatShader.use();
        objekatShader.setVec3("dirLight.direction", dirLight.direction);
        objekatShader.setVec3("dirLight.ambient", dirLight.ambient);
        objekatShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        objekatShader.setVec3("dirLight.specular", dirLight.specular);

        objekatShader.setVec3("viewPos", camera.Position);
        objekatShader.setFloat("material.shininess", 32.0f);

        objekatShader.setVec3("pointLight.position", pointLight.position);
        objekatShader.setVec3("pointLight.ambient", pointLight.ambient);
        objekatShader.setVec3("pointLight.diffuse", pointLight.diffuse);
        objekatShader.setVec3("pointLight.specular", pointLight.specular);
        objekatShader.setFloat("pointLight.constant", pointLight.constant);
        objekatShader.setFloat("pointLight.linear", pointLight.linear);
        objekatShader.setFloat("pointLight.quadratic", pointLight.quadratic);

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        objekatShader.setMat4("projection", projection);
        objekatShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        objekatShader.setMat4("model", model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap.Id);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap.Id);
        glBindVertexArray(svetloVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);




        glDisable(GL_CULL_FACE);

        modelShader.use();

        modelShader.setVec3("dirLight.direction", dirLight.direction);
        modelShader.setVec3("dirLight.ambient", dirLight.ambient);
        modelShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        modelShader.setVec3("dirLight.specular", dirLight.specular);

        modelShader.setVec3("viewPos", camera.Position);
        modelShader.setFloat("material.shininess", 32.0f);

        modelShader.setVec3("pointLight.position", pointLight.position);
        modelShader.setVec3("pointLight.ambient", pointLight.ambient);
        modelShader.setVec3("pointLight.diffuse", pointLight.diffuse);
        modelShader.setVec3("pointLight.specular", pointLight.specular);
        modelShader.setFloat("pointLight.constant", pointLight.constant);
        modelShader.setFloat("pointLight.linear", pointLight.linear);
        modelShader.setFloat("pointLight.quadratic", pointLight.quadratic);

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f) + position);
        model = glm::rotate(model, (float) -3.14/2, glm::vec3(1.0, 0, 0));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        modelShader.setMat4("model", model);
        modelPtica.Draw(modelShader);

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        pozadinaShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        pozadinaShader.setMat4("view", glm::mat4(glm::mat3 (view)));
        pozadinaShader.setMat4("projection", projection);


        glBindVertexArray(pozadinaVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glBindVertexArray(pozadinaVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteVertexArrays(1, &pozadinaVAO);
    glDeleteVertexArrays(1, &svetloVAO);
    glDeleteVertexArrays(1, &svetlecaKockaVAO);
    glDeleteVertexArrays(1, &refleksijaVAO);
    glDeleteBuffers(1, &svetloVBO);
    glDeleteBuffers(1, &svetlecaKockaVBO);
    glDeleteBuffers(1, &refleksijaVBO);
    glDeleteBuffers(1, &pozadinaVAO);
    glDeleteBuffers(1, &pozadinaEBO);



    glfwTerminate();
    return 0;
}


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

    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        position -= glm::vec3 (0.01, 0, 0);
    }
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        position += glm::vec3 (0.01, 0, 0);
    }
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        position -= glm::vec3 (0.02, 0, 0.02);
    }if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        position += glm::vec3 (0.02, 0, 0.02);
    }if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){
        position += glm::vec3 (0, 0.01, 0);
    }if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
        position -= glm::vec3 (0, 0.01, 0);
    }
}



void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}