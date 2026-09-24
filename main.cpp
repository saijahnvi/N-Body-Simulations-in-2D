#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp> 
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <cmath>

class ball {
public:
    std::vector<float> Position;
    std::vector<float> velocity;
    float mass;
    float RADIUS = 0.2f;

    ball(std::vector<float> velocity, std::vector<float> Position,float mass, float RADIUS) {
        this->Position = Position;
        this->velocity = velocity;
        this->mass=mass;
        this->RADIUS = RADIUS;
    }

    void updatevelocity(float dt) {
        this->Position[0] += this->velocity[0] * dt;
        this->Position[1] += this->velocity[1] * dt;

        /*if (this->Position[0] + this->RADIUS > 1.0f) {
            this->Position[0] = 1.0f - this->RADIUS;
            velocity[0] = -velocity[0] * 0.8f;
        } else if (this->Position[0] - this->RADIUS < -1.0f) {
            this->Position[0] = -1.0f + this->RADIUS;
            velocity[0] = -velocity[0] * 0.8f;
        }

        if (this->Position[1] + this->RADIUS > 1.0f) {
            this->Position[1] = 1.0f - this->RADIUS;
            velocity[1] = -velocity[1] * 0.75f;
        } else if (this->Position[1] - this->RADIUS < -1.0f) {
            this->Position[1] = -1.0f + this->RADIUS;
            velocity[1] = -velocity[1] * 0.75f;
            velocity[0] *= 0.98f;
        }*/
    }

    void accelerate(float dt, glm::vec2 direction, float gravity) {
        this->velocity[0] += direction.x * gravity * dt;
        this->velocity[1] += direction.y * gravity * dt;
    }

    float acceleratefreespace(ball& a, ball& b, glm::vec2& outDir) {
        glm::vec2 posA(a.Position[0], a.Position[1]);
        glm::vec2 posB(b.Position[0], b.Position[1]);
        glm::vec2 delta = posB - posA;
        float distance = glm::length(delta);
        
        if (distance < 0.05f) return 0.0f; // Avoid division by zero
        
        outDir = delta / distance; // Direction vector from A to B
        double G = 1; // Scaled constant for screen coordinates
        float g = (G * b.mass) / (distance * distance);
        return g;
    }

    std::vector<glm::vec3> drawcircle(float x0,float y0,int ncount,float radius){
        float degree = 360.0f/ncount;
        std::vector<glm::vec3> temp;
        std::vector<glm::vec3> vertices;
        glm::vec3 center(0.0f, 0.0f, 0.0f);
        //assigning positions
        for(int i=0;i<=ncount;i++){
            float theta= i*degree;
            float x = radius*cos(glm::radians(theta));
            float y= radius*sin(glm::radians(theta));
            float z=0.0f;
            temp.push_back(glm::vec3(x, y, z));
        }
        //triangles vertices
        for(int i=0;i< ncount;i++){
            vertices.push_back(center);
            vertices.push_back(temp[i]);
            vertices.push_back(temp[i+1]);
        }

        return vertices;
    }
};

void collision(ball& a ,ball& b){
    glm::vec2 posA(a.Position[0],a.Position[1]);
    glm::vec2 posB(b.Position[0],b.Position[1]);
    glm::vec2 delta=posA-posB;
    float distance=glm::length(delta);
    float mindist=a.RADIUS+b.RADIUS;
    if(distance<mindist && distance > 0.0001f){
        glm::vec2 normal=delta/distance;
        float overlap=mindist-distance;
        float m=overlap*(0.5f);
        
        a.Position[0]+=normal.x*(m);
        a.Position[1]+=normal.y*(m);
        b.Position[0]-=normal.x*(m);
        b.Position[1]-=normal.y*(m);

        glm::vec2 velA(a.velocity[0],a.velocity[1]);
        glm::vec2 velB(b.velocity[0],b.velocity[1]);
        glm::vec2 relvel=velA-velB;
        float velAlongNormal = glm::dot(relvel, normal);
        if(velAlongNormal>0) return;
        float e =1.0f;
        float j=-(1.0f+e)*(velAlongNormal)/((1/a.mass) + (1/b.mass));
        glm::vec2 impulse = j * normal;
        a.velocity[0] += (1.0f / a.mass) * impulse.x;
        a.velocity[1] += (1.0f / a.mass) * impulse.y;
        b.velocity[0] -= (1.0f / b.mass) * impulse.x;
        b.velocity[1] -= (1.0f / b.mass) * impulse.y;
    }
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type); 
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);

        std::cout << "Failed to compile "
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main() {
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(680, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Error initializing glew" << std::endl;
        return -1;
    }
    {
        unsigned int vao; //making a vao and binding it 
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        std::vector<ball> objs = {
    ball({-0.824f, -0.57f}, {0.0f, 0.0f}, 1.0f, 0.08f),
    ball({0.412f, 0.282f}, {1.0f, 0.0f}, 1.0f, 0.08f), 
    ball({0.412f, 0.283f}, {-1.0f, 0.0f}, 1.0f, 0.08f) 

        };

        // Shaders
        std::string vertexShader =
            "#version 330 core\n"
            "layout(location = 0) in vec3 position;\n"
            "uniform vec2 u_Offset;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = vec4(position.x + u_Offset.x, position.y + u_Offset.y, position.z, 1.0);\n"
            "}\n";

        std::string fragmentShader =
            "#version 330 core\n"
            "layout(location = 0) out vec4 color;\n"
            "void main()\n"
            "{\n"
            "   color = vec4(1.0, 1.0, 1.0, 1.0);\n" 
            "}\n";

        unsigned int shader = CreateShader(vertexShader, fragmentShader);
        int location = glGetUniformLocation(shader, "u_Offset");

        float lastFrame = (float)glfwGetTime();

        while (!glfwWindowShouldClose(window)) {
            float currentFrame = (float)glfwGetTime();
            float dt = currentFrame - lastFrame;
            lastFrame = currentFrame;

            glClear(GL_COLOR_BUFFER_BIT);

            // Bind shader and VAO to draw
            glUseProgram(shader);
            glBindVertexArray(vao);

            for (size_t i = 0; i < objs.size(); ++i) {
                for (size_t j = i + 1; j < objs.size(); ++j) {
                    collision(objs[i], objs[j]);
                }
            }

            for (size_t i = 0; i < objs.size(); ++i) {
                for (size_t j = 0; j < objs.size(); ++j) {
                    if (i != j) {
                        glm::vec2 dir;
                        float gravity = objs[i].acceleratefreespace(objs[i], objs[j], dir);
                        objs[i].accelerate(dt, dir, gravity);
                    }
                }
            }

            for(auto&obj: objs){
                obj.updatevelocity(dt);

                std::vector<glm::vec3> verticesot = obj.drawcircle(obj.Position[0], obj.Position[1], 32, obj.RADIUS);

                unsigned int vbo;
                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, verticesot.size() * sizeof(glm::vec3), verticesot.data(), GL_DYNAMIC_DRAW);

                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (const void*)0); //links the vao and vertex buffer

                glUniform2f(location, obj.Position[0], obj.Position[1]);

                glDrawArrays(GL_TRIANGLES, 0, (GLsizei)verticesot.size());

                glDeleteBuffers(1, &vbo);
            }

            // Unbind VAO and VBO 
            glBindVertexArray(0); //default vao
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glDeleteProgram(shader);
        glDeleteVertexArrays(1, &vao);
    }
    glfwTerminate();
    return 0;
}