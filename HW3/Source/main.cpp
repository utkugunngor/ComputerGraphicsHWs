#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <sstream>
#include <chrono>
#include "parser.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

#define PI 3.14159265358979323846
//////-------- Global Variables -------/////////

GLuint gpuVertexBuffer;
GLuint gpuNormalBuffer;
GLuint gpuIndexBuffer;

// Sample usage for reading an XML scene file
parser::Scene scene;

static GLFWwindow *win = NULL;
char gRendererInfo[512] = { 0 };
char gWindowTitle[512] = { 0 };

static void errorCallback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
  
}


parser::Vec3f getVertexValue(int vertexId, const parser::Scene &scene) {
    return scene.vertex_data[vertexId - 1];
}

static void updateLightSources() {
  const std::vector<parser::PointLight> point_lights = scene.point_lights;
  for (size_t i = 0; i < point_lights.size(); i++) {
    const parser::PointLight light = point_lights[i];
    GLfloat light_position[4] = {light.position.x, light.position.y,
                                 light.position.z, 1.};
    glLightfv(GL_LIGHT0 + i, GL_POSITION, light_position);
  }
}

void startLights() {
    //glEnable(GL_LIGHTING);
    const GLfloat ambient[4] = {scene.ambient_light.x, scene.ambient_light.y,
                              scene.ambient_light.z, 1.};
  // glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
  const std::vector<parser::PointLight> point_lights = scene.point_lights;
  for (size_t i = 0; i < point_lights.size(); i++) {
    const parser::PointLight light = point_lights[i];
    const GLfloat intensity[4] = {light.intensity.x, light.intensity.y,
                                  light.intensity.z, 1.};
    glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, intensity);
    glLightfv(GL_LIGHT0 + i, GL_SPECULAR, intensity);
    glEnable(GL_LIGHT0 + i);
  }
}
double flength( parser::Vec3f v)
{
	return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}



static parser::Vec3f fnormalize(parser::Vec3f v)
{
	parser::Vec3f tmp;
	double d;
	
	d=flength(v);
	tmp.x = v.x/d;
	tmp.y = v.y/d;
	tmp.z = v.z/d;
	
	return tmp;
}

static parser::Vec3f fadd(parser::Vec3f a,parser::Vec3f b)
{
	parser::Vec3f tmp;
	tmp.x = a.x+b.x;
	tmp.y = a.y+b.y;
	tmp.z = a.z+b.z;
	
	return tmp;
}

parser::Vec3f fmult(parser::Vec3f a, double c)
{
	parser::Vec3f tmp;
	tmp.x = a.x*c;
	tmp.y = a.y*c;
	tmp.z = a.z*c;
	
	return tmp;
}

static parser::Vec3f fcross(parser::Vec3f a, parser::Vec3f b)
{
	parser::Vec3f tmp;
	
	tmp.x = a.y*b.z-b.y*a.z;
	tmp.y = b.x*a.z-a.x*b.z;
	tmp.z = a.x*b.y-b.x*a.y;
	
	return tmp;
}

void initScene(){
   glClearColor(scene.background_color.x, scene.background_color.y,
               scene.background_color.z, 1);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    if(scene.culling_enabled==1){
      glEnable(GL_CULL_FACE);
      if(scene.culling_face==1){
       glCullFace(GL_FRONT);
      }
      else{
        glCullFace(GL_BACK);
      }
      
      
    
    }
    
    glEnable(GL_LIGHTING);
    /*for (int i = 0; i < scene.point_lights.size(); i++) {
        glEnable(GL_LIGHT0 + i);
    }*/
    startLights();
    updateLightSources();  
    parser::Camera camera = scene.camera;
    glViewport(0, 0,camera.image_width,camera.image_height); // Set viewport
  
 
     //gluPerspective(60,1,camera.near_distance,camera.far_distance);            
    
    glMatrixMode(GL_MODELVIEW);
   
    glLoadIdentity();
    gluLookAt(camera.position.x,camera.position.y,camera.position.z,(camera.gaze.x * camera.near_distance + camera.position.x),
              (camera.gaze.y * camera.near_distance + camera.position.y),
              (camera.gaze.z * camera.near_distance + camera.position.z),
              camera.up.x,
              camera.up.y,
              camera.up.z);
            
  
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(camera.near_plane.x, camera.near_plane.y,camera.near_plane.z, camera.near_plane.w, camera.near_distance, camera.far_distance);  
       
}
parser::Vec3f doOperationOver(const parser::Vec3f v1, const float val) {
    parser::Vec3f result;

    result.x = v1.x / val;
    result.y = v1.y / val;
    result.z = v1.z / val;

    return result;
}
parser::Vec3f doUnitVector(const parser::Vec3f &v) {
    parser::Vec3f result;
    float magnitude = 0;
    magnitude += v.x * v.x;
    magnitude += v.y * v.y;
    magnitude += v.z * v.z;

    magnitude = sqrtf(magnitude);

    result = doOperationOver(v, magnitude);

    return result;

}


void render() {

  



    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glClearDepth(1.0f);
   
   
   
   
    for(parser::Mesh &mesh : scene.meshes){
    
    //std::cout<<scene.vertex_data.size()<<std::endl;
      parser::Vec3f normals[scene.vertex_data.size()];
    int count[scene.vertex_data.size()];
     for(int i = 0; i < scene.vertex_data.size(); i++){
      normals[i].x = 0;
      normals[i].y = 0;
      normals[i].z = 0;
      count[i]=0;
    }
    
    parser::Material meshMaterial = scene.materials[mesh.material_id-1];
        GLfloat ambColor[4] = {meshMaterial.ambient.x,meshMaterial.ambient.y,meshMaterial.ambient.z,1.0};
        GLfloat difColor[4] = {meshMaterial.diffuse.x,meshMaterial.diffuse.y,meshMaterial.diffuse.z,1.0};
        GLfloat specColor[4] = {meshMaterial.specular.x,meshMaterial.specular.y,meshMaterial.specular.z,1.0};
        GLfloat specExp[1] = {meshMaterial.phong_exponent};
    
    
    for(parser::Face m_face: mesh.faces){
         parser::Vec3f normal;
         parser::Vec3f v0=scene.vertex_data[m_face.v0_id-1];
         parser::Vec3f v1=scene.vertex_data[m_face.v1_id-1];
         parser::Vec3f v2=scene.vertex_data[m_face.v2_id-1];
            
            const parser::Vec3f ba = fadd(v1,fmult(v0,-1));
            const parser::Vec3f ca = fadd(v2,fmult(v0,-1));
            
            normal = fcross(ba,ca);
            normal = doUnitVector(normal);
            // std::cout<<"Face normal "<<normal.x<<","<<normal.y<<","<<normal.z<<std::endl;
            //if(m_face.v0_id==15385||m_face.v1_id==15385||m_face.v2_id==15385) std::cout<<"Face normal "<<normal.x<<","<<normal.y<<","<<normal.z<<count[15384]<<std::endl;
            normals[m_face.v0_id-1].x+=(normal.x); normals[m_face.v0_id-1].y+=(normal.y); normals[m_face.v0_id-1].z+=(normal.z); count[m_face.v0_id-1]+=1;
            normals[m_face.v1_id-1].x+=(normal.x); normals[m_face.v1_id-1].y+=(normal.y); normals[m_face.v1_id-1].z+=(normal.z); count[m_face.v1_id-1]+=1;
            normals[m_face.v2_id-1].x+=(normal.x); normals[m_face.v2_id-1].y+=(normal.y); normals[m_face.v2_id-1].z+=(normal.z); count[m_face.v2_id-1]+=1;   
    }
    //std::cout<<"-------------------------------------------------------------------------------------------------------------"<<std::endl;
    for(int i = 0; i < scene.vertex_data.size(); i++){
      //std::cout<<"     |      "<<vec.x<<"     |    "<<vec.y<<"    |    "<<vec.z<<"   |   "<<vec.w<<"   |"   <<std::endl;
     // std::cout<<normals[i].x<<" "<<normals[i].y<<" "<<normals[i].z<<std::endl;
    if(count[i]>0){
        parser::Vec3f normal;
         
          normals[i].x/=count[i];
          normals[i].y/=count[i];
           normals[i].z/=count[i];
         // std::cout<<"w: "<<vec.w<<std::endl;
         
        
         
        // std::cout<<"before normalize "<<normal.x<<","<<normal.y<<","<<normal.z<<std::endl;
        normals[i]=doUnitVector(normals[i]);
      //  std::cout<<"after normalize "<<normal.x<<","<<normal.y<<","<<normal.z<<std::endl;
        
       } 
    }
      glPushMatrix();
      for(int i=mesh.transformations.size()-1;i>-1;i--){
      
        parser::Transformation current = mesh.transformations[i];
        
        if(current.transformation_type=="Scaling"){
           parser::Vec3f &transformation = scene.scalings[current.id-1];
           glScalef(transformation.x,transformation.y,transformation.z);
        
        }
        else if(current.transformation_type=="Translation"){
           parser::Vec3f &transformation = scene.translations[current.id-1];
           glTranslatef(transformation.x,transformation.y,transformation.z);
        
        }
        else{
           parser::Vec4f &transformation = scene.rotations[current.id-1];
           glRotatef(transformation.x,transformation.y,transformation.z,transformation.w);
        
        }
        }
        
        if(mesh.mesh_type == "Solid") {
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        }  
        
        

        glMaterialfv(GL_FRONT,GL_AMBIENT,ambColor);
        glMaterialfv(GL_FRONT,GL_DIFFUSE,difColor);
        glMaterialfv(GL_FRONT,GL_SPECULAR,specColor);
        glMaterialfv(GL_FRONT,GL_SHININESS,specExp);
        
       // startLights();
        //updateLightSources();
        
         
     
       //DRAW
       glBegin(GL_TRIANGLES);
        
       for(int j=0;j<mesh.faces.size();j++){
         parser::Vec3f normal;
         parser::Face m_face=mesh.faces[j];
         parser::Vec3f v0=scene.vertex_data[m_face.v0_id-1];
         parser::Vec3f v1=scene.vertex_data[m_face.v1_id-1];
         parser::Vec3f v2=scene.vertex_data[m_face.v2_id-1];
          const parser::Vec3f ba = fadd(v1,fmult(v0,-1));
            const parser::Vec3f ca = fadd(v2,fmult(v0,-1));
            
            normal = fcross(ba,ca);
            normal = doUnitVector(normal);
        // if( m_face.v1_id==15385)   std::cout<<"normal of :"<< m_face.v1_id<<" "<<normals[m_face.v1_id-1].x<<" "<<normals[m_face.v1_id-1].y<<"  "<<normals[m_face.v1_id-1].z<<"hi"<<std::endl;
         // std::cout<<"normal of :"<< m_face.v0_id<<" "<<normals[m_face.v0_id-1].x<<" "<<normals[m_face.v0_id-1].y<<"  "<<normals[m_face.v0_id-1].z<<std::endl;
         //std::cout<<"normal of :"<< m_face.v1_id<<" "<<normals[m_face.v1_id-1].x<<" "<<normals[m_face.v1_id-1].y<<"  "<<normals[m_face.v1_id-1].z<<std::endl;
          // std::cout<<"normal of :"<< m_face.v2_id<<" "<<normals[m_face.v2_id-1].x<<" "<<normals[m_face.v2_id-1].y<<"  "<<normals[m_face.v2_id-1].z<<std::endl;
         glNormal3f(normals[m_face.v0_id-1].x,normals[m_face.v0_id-1].y,normals[m_face.v0_id-1].z);
       //  glNormal3f(normal.x,normal.y,normal.z);
         glVertex3f(v0.x,v0.y,v0.z);
         
         glNormal3f(normals[m_face.v1_id-1].x,normals[m_face.v1_id-1].y,normals[m_face.v1_id-1].z);
         glVertex3f(v1.x,v1.y,v1.z);
         
         glNormal3f(normals[m_face.v2_id-1].x,normals[m_face.v2_id-1].y,normals[m_face.v2_id-1].z);
         glVertex3f(v2.x,v2.y,v2.z);    
       
       
       }
       glEnd();
       glPopMatrix();
    
    
    }
}

double lastTime;
int nbFrames;

void showFPS(GLFWwindow *pWindow)
{
    // Measure speed
     double currentTime = glfwGetTime();
     double delta = currentTime - lastTime;
	 char ss[500] = {};
     nbFrames++;
     if ( delta >= 1.0 ){ // If last cout was more than 1 sec ago
         //cout << 1000.0/double(nbFrames) << endl;

         double fps = ((double)(nbFrames)) / delta;
          
         sprintf(ss,"CENG477 - HW3 [%.2lf FPS]",fps);

         glfwSetWindowTitle(pWindow, ss);

         nbFrames = 0;
         lastTime = currentTime;
     }
}

int main(int argc, char *argv[]) {
    

    scene.loadFromXml(argv[1]);
    scene.camera.gaze = fnormalize(scene.camera.gaze);

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    win = glfwCreateWindow(scene.camera.image_width, scene.camera.image_height, "CENG477", NULL, NULL);
    if (!win) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(win);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(win, keyCallback);
    
    initScene();
    
    lastTime = glfwGetTime();
	  nbFrames = 0;
    
    while (!glfwWindowShouldClose(win)) {
      
          
          render();
          showFPS(win);
        glfwSwapBuffers(win);
        glfwWaitEvents();
         
    }
        
        glfwDestroyWindow(win);
        glfwTerminate();
    
        exit(EXIT_SUCCESS);

    return 0;
}