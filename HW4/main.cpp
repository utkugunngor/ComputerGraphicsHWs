#include "helper.h"
//#include "solution.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include <vector>

static GLFWwindow* win = NULL;
int widthWindow = 1000, heightWindow = 1000;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idHeightTexture;
GLuint idMVPMatrix;

// Buffers
GLuint idVertexBuffer;
GLuint idIndexBuffer;

int textureWidth, textureHeight;
float heightFactor=10.0;

GLFWmonitor* primary_monitor;
const GLFWvidmode* vidmode;


//Camera camera;
//Camera initialCamera;

glm::vec3 lightPos;
GLuint depthMapFBO;
GLuint depthCubemap;
bool lightPosFlag = false;
bool viewportFlag=false;
bool fullscreen_mode = false;


glm::vec3 gaze = glm::vec3(0.0, 0.0, 1.0);
glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
glm::vec3 camera_v = cross(up, gaze);
GLfloat speed = 0.0f;
glm::vec3 camPos;
GLfloat angle = 45; 
GLfloat aspect_ratio = 1;
GLfloat near = 0.1;
GLfloat far = 1000;
glm::vec3 light_pos;


int vertex_cnt;
//glm::vec3* vertices;
int buffer_size = 18*textureHeight*textureWidth;
vector<float> vertices;


static void setCamera(){

    camPos += gaze * speed;

    glm::mat4 P = glm::perspective(angle, aspect_ratio, near, far);

    glm::vec3 center = camPos + gaze * (GLfloat) 0.1;

    glm::mat4 V = glm::lookAt(camPos, center, up);

    glm::mat4 M = glm::mat4(1.0f);

    glm::mat4 MVP = P * V * M;

    glm::mat4 N = glm::inverseTranspose(V);

  
    glUniformMatrix4fv(glGetUniformLocation(idProgramShader, "MV"), 1, GL_FALSE, &V[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(idProgramShader, "MVP"), 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(idProgramShader, "G_norm"), 1, GL_FALSE, &N[0][0]);
    glUniform3fv(glGetUniformLocation(idProgramShader, "cameraPosition"), 1, &camPos[0]);
    glViewport(0,0, widthWindow, heightWindow);

}


static void make_triangles(){

    vertices.resize(buffer_size); 

    for(int i=0;i<textureHeight;i++){
    
        for(int j=0;j<textureWidth;j++){

            int index = 18*(i*textureWidth+j);
        
            vertices[index] = j;
            vertices[index+1] = 0;
            vertices[index+2] = i;
      
            vertices[index+3] = j+1;
            vertices[index+4] = 0;
            vertices[index+5] = i;
      
            vertices[index+6] = j;
            vertices[index+7] = 0;
            vertices[index+8] = i+1;
      
            vertices[index+9] = j+1;
            vertices[index+10] = 0;
            vertices[index+11] = i;
      
            vertices[index+12] = j+1;
            vertices[index+13] = 0;
            vertices[index+14] = i+1;
      
            vertices[index+15] = j;
            vertices[index+16] = 0;
            vertices[index+17] = i+1;
        
        
        }
    
    
    }
    
      glGenBuffers(1, &idVertexBuffer);
      glGenVertexArrays(1, &idIndexBuffer);
      glBindBuffer(GL_ARRAY_BUFFER, idVertexBuffer);
      glBindVertexArray(idIndexBuffer);
      glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);

}


static void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
    
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        
    if(key ==  GLFW_KEY_R && action == GLFW_PRESS ) {
      heightFactor += 0.5;
      GLint loc_heightFactor = glGetUniformLocation(idProgramShader, "heightFactor");
      glUniform1f(loc_heightFactor, heightFactor);
      return;
    }
    if(key ==  GLFW_KEY_F && action == GLFW_PRESS )  {
      heightFactor -= 0.5;
      GLint loc_heightFactor = glGetUniformLocation(idProgramShader, "heightFactor");
      glUniform1f(loc_heightFactor, heightFactor);
      return;
    }
    if(key ==  GLFW_KEY_Q && action == GLFW_PRESS )
    if(key ==  GLFW_KEY_E && action == GLFW_PRESS )
    if(key ==  GLFW_KEY_T && action == GLFW_PRESS ) { 
    
     light_pos.y+=5;
     glUniform3fv(glGetUniformLocation(idProgramShader, "light_pos"),1, &light_pos[0]);
     
     }
    if(key ==  GLFW_KEY_G && action == GLFW_PRESS )  { 
    
      light_pos.y-=5; 
      glUniform3fv(glGetUniformLocation(idProgramShader, "light_pos"),1, &light_pos[0]);
      
      }
    if(key ==  GLFW_KEY_UP && action == GLFW_PRESS )  { 
    
      light_pos.z-=5; 
      glUniform3fv(glGetUniformLocation(idProgramShader, "light_pos"),1, &light_pos[0]);
      
      } 
    if(key ==  GLFW_KEY_DOWN && action == GLFW_PRESS )  { 
    
      light_pos.z+=5; 
      glUniform3fv(glGetUniformLocation(idProgramShader, "light_pos"),1, &light_pos[0]);
      
      } 
    
    if(key ==  GLFW_KEY_LEFT && action == GLFW_PRESS )  { 
    
      light_pos.x-=5; 
      glUniform3fv(glGetUniformLocation(idProgramShader, "light_pos"),1, &light_pos[0]);
      
      }   
      
    if(key ==  GLFW_KEY_RIGHT && action == GLFW_PRESS )  { 
    
      light_pos.x+=5; 
      glUniform3fv(glGetUniformLocation(idProgramShader, "light_pos"),1, &light_pos[0]);
      
      }        
      
      
      
      
      
    if(key ==  GLFW_KEY_A && action == GLFW_PRESS ) {
    
      camera_v = glm::rotate(camera_v, 0.05f, up);
       gaze = glm::rotate(gaze, 0.05f, up);
      return;
      
    }
    if(key ==  GLFW_KEY_D && action == GLFW_PRESS ) {
    
      camera_v = glm::rotate(camera_v, -0.05f, up);
      gaze = glm::rotate(gaze, -0.05f, up);
      return;
        
    }
    if(key ==  GLFW_KEY_W && action == GLFW_PRESS ) {
    
      up = glm::rotate(up, -0.05f, camera_v);
      gaze = glm::rotate( gaze, -0.05f, camera_v);
      return;
      
    
    }
    if(key ==  GLFW_KEY_S && action == GLFW_PRESS ) {
    
      up = glm::rotate(up, 0.05f, camera_v);
      gaze = glm::rotate( gaze, 0.05f, camera_v);
      return;
    
    
    }
    if(key ==  GLFW_KEY_Y && action == GLFW_PRESS ) speed+=0.01;
    if(key ==  GLFW_KEY_H && action == GLFW_PRESS ) speed-=0.01;
    if(key ==  GLFW_KEY_X && action == GLFW_PRESS ) speed =0;
    if(key ==  GLFW_KEY_I && action == GLFW_PRESS ) {
    
      camPos = glm::vec3(textureWidth / 2, textureWidth / 10, - textureWidth / 4);
      speed=0;
       glm::vec3 gaze = glm::vec3(0.0, 0.0, 1.0);
      glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
      GLint loc_camera_pos = glGetUniformLocation(idProgramShader, "cameraPosition");
      glUniform3fv(loc_camera_pos, 1, &camPos[0]);
     
    
    }
    if(key ==  GLFW_KEY_P && action == GLFW_PRESS ){
       
         viewportFlag = true;

        if(fullscreen_mode){

            fullscreen_mode = false;

            glfwSetWindowMonitor(win, nullptr, 0, 0,widthWindow,heightWindow, 0);

        }
        else{

            fullscreen_mode = true;

           

            glfwSetWindowMonitor(win, primary_monitor,0,0,vidmode->width, vidmode->height, vidmode->refreshRate);

        }
    
    }
    
}

int main(int argc, char *argv[]) {

  if (argc != 3) {
    printf("Please provide height and texture image files!\n");
    exit(-1);
  }

  glfwSetErrorCallback(errorCallback);

  if (!glfwInit()) {
    exit(-1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); // This is required for remote
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); // This might be used for local

  win = glfwCreateWindow(widthWindow, heightWindow, "CENG477 - HW4", NULL, NULL);

  if (!win) {
      glfwTerminate();
      exit(-1);
  }
  glfwMakeContextCurrent(win);

  GLenum err = glewInit();
  if (err != GLEW_OK) {
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

      glfwTerminate();
      exit(-1);
  }


  /*idProgramShader = glCreateProgram();
  idVertexShader = initVertexShader("shader.vert");
  idFragmentShader = initFragmentShader("shader.frag");
  glAttachShader(idProgramShader, idVertexShader);
  glAttachShader(idProgramShader, idFragmentShader);
  glLinkProgram(idProgramShader);    
  glUseProgram(idProgramShader);    */
  

  string vert = "shader.vert";
  string frag = "shader.frag";

  initShaders(idProgramShader, vert, frag);
  glUseProgram(idProgramShader);

  glfwSetKeyCallback(win, keyCallback);

  initTexture(argv[1], argv[2], &textureWidth, &textureHeight);
  
  make_triangles();
  
  glEnable(GL_DEPTH_TEST);
  
  camPos = glm::vec3(textureWidth / 2, textureWidth / 10, - textureWidth / 4);
  
  setCamera();

   light_pos = glm::vec3(textureWidth / 2, 100, textureHeight / 2);
  
  
   glUniform3fv(glGetUniformLocation(idProgramShader, "light_pos"), 1,&light_pos[0]);
   glUniform1i(glGetUniformLocation(idProgramShader, "rgbTexture"), 0);
   glUniform1i(glGetUniformLocation(idProgramShader, "depthTexture"), 1);
   glUniform1i(glGetUniformLocation(idProgramShader, "widthTexture"), textureWidth);
   glUniform1i(glGetUniformLocation(idProgramShader, "heightTexture"), textureHeight);
   glUniform1f(glGetUniformLocation(idProgramShader, "heightFactor"), heightFactor);
   
 
 
    primary_monitor = glfwGetPrimaryMonitor();
    vidmode = glfwGetVideoMode(primary_monitor);
    glfwGetWindowSize(win, &widthWindow, &heightWindow);
 
  while(!glfwWindowShouldClose(win)) {

     if( viewportFlag == true){

          viewportFlag = false;
          glViewport(0, 0, widthWindow, heightWindow);

      }


    glClearStencil(0);
    glClearDepth(1.0f);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    setCamera();
    glDrawArrays(GL_TRIANGLES, 0, buffer_size);

  
   
    glfwSwapBuffers(win);
    glfwPollEvents();
  }

   glDeleteVertexArrays(1, &idIndexBuffer);
   glDeleteBuffers(1, &idVertexBuffer);

  glfwDestroyWindow(win);
  glfwTerminate();
  return 0;
}