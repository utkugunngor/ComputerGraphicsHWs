#include <iostream>
#include <math.h>
#include <time.h>
#include <string.h>
#include <vector>
#include "parser.h"
#include "ppm.h"

#define ABS(a) ((a)>0?(a):-1*(a))

typedef unsigned char RGB[3];
using namespace parser;

typedef struct
{
	Vec3f a;
	Vec3f b;
} Ray;



Vec3i icross(Vec3i a, Vec3i b)
{
	Vec3i tmp;
	
	tmp.x = a.y*b.z-b.y*a.z;
	tmp.y = b.x*a.z-a.x*b.z;
	tmp.z = a.x*b.y-b.x*a.y;
	
	return tmp;
}

Vec3f fcross(Vec3f a, Vec3f b)
{
	Vec3f tmp;
	
	tmp.x = a.y*b.z-b.y*a.z;
	tmp.y = b.x*a.z-a.x*b.z;
	tmp.z = a.x*b.y-b.x*a.y;
	
	return tmp;
}

double idot(Vec3i a, Vec3i b)
{
		return a.x*b.x+a.y*b.y+a.z*b.z;
}

double fdot(Vec3f a, Vec3f b)
{
		return a.x*b.x+a.y*b.y+a.z*b.z;
}

double ilength2(Vec3i v)
{
	return (v.x*v.x+v.y*v.y+v.z*v.z);
}

double flength2(Vec3f v)
{
	return (v.x*v.x+v.y*v.y+v.z*v.z);
}

double ilength(Vec3i v)
{
	return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}

double flength(Vec3f v)
{
	return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}

Vec3i inormalize(Vec3i v)
{
	Vec3i tmp;
	double d;
	
	d=ilength(v);
	tmp.x = v.x/d;
	tmp.y = v.y/d;
	tmp.z = v.z/d;
	
	return tmp;
}

Vec3f fnormalize(Vec3f v)
{
	Vec3f tmp;
	double d;
	
	d=flength(v);
	tmp.x = v.x/d;
	tmp.y = v.y/d;
	tmp.z = v.z/d;
	
	return tmp;
}

Vec3i iadd(Vec3i a, Vec3i b)
{
	Vec3i tmp;
	tmp.x = a.x+b.x;
	tmp.y = a.y+b.y;
	tmp.z = a.z+b.z;
	
	return tmp;
}

Vec3f fadd(Vec3f a, Vec3f b)
{
	Vec3f tmp;
	tmp.x = a.x+b.x;
	tmp.y = a.y+b.y;
	tmp.z = a.z+b.z;
	
	return tmp;
}

Vec3i imult(Vec3i a, double c)
{
	Vec3i tmp;
	tmp.x = a.x*c;
	tmp.y = a.y*c;
	tmp.z = a.z*c;
	
	return tmp;
}

Vec3f fmult(Vec3f a, double c)
{
	Vec3f tmp;
	tmp.x = a.x*c;
	tmp.y = a.y*c;
	tmp.z = a.z*c;
	
	return tmp;
}

double idistance(Vec3i a, Vec3i b)
{
    return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)+(a.z-b.z)*(a.z-b.z));
}

double fdistance(Vec3f a, Vec3f b)
{
    return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)+(a.z-b.z)*(a.z-b.z));
}

int iequal(Vec3i a, Vec3i b)
{
	double e = 0.000000001;
	
	//printf("%lf %lf %f ----",ABS((a.x-b.x)),ABS((a.y-b.y)),ABS((a.z-b.z)));
	if ((ABS((a.x-b.x))<e) && (ABS((a.y-b.y))<e) && (ABS((a.z-b.z))<e))
		{ return 1;}
	else { return 0;}
}


int fequal(Vec3f a, Vec3f b)
{
	double e = 0.000000001;
	
	//printf("%lf %lf %f ----",ABS((a.x-b.x)),ABS((a.y-b.y)),ABS((a.z-b.z)));
	if ((ABS((a.x-b.x))<e) && (ABS((a.y-b.y))<e) && (ABS((a.z-b.z))<e))
		{ return 1;}
	else { return 0;}
}

Vec3f normalize(Vec3f v)
{
	Vec3f tmp;
	double d;
	
	d=flength(v);
	tmp.x = v.x/d;
	tmp.y = v.y/d;
	tmp.z = v.z/d;
	
	return tmp;
}

/*void initImage(int sizeX, int sizeY)
{
	int i,j;

    for (i=0;i<sizeX;i++)
		for (j=0;j<sizeY;j++)
		{
			image[i][j].r = 255;
			image[i][j].g = 255;
			image[i][j].b = 255;
		}
}
*/

Ray generateRay(int i, int j, Camera cam,double pixelW, double pixelH)
{
	Ray tmp;
	
	Vec3f su,sv,s,u;
	double halfPixelW= pixelW * 0.5;
	double halfPixelH= pixelH * 0.5;
	tmp.a = cam.position;
	u=fcross(cam.gaze,cam.up);
	su = fmult(u,cam.near_plane.x+(j*pixelW)+halfPixelW);
	sv = fadd(fmult(fmult(cam.up,-1),+(i*pixelH)+halfPixelH),fmult(cam.up,cam.near_plane.w));
	
	s = fadd(su,sv);
	
	tmp.b = fadd(fmult(cam.gaze,cam.near_distance),s);
	
	return tmp;
}



double intersectSphere(Ray r, Sphere s, Vec3f scenter)
{
	double A,B,C; //constants for the quadratic function
	
	double delta;
	
	//Vec3f scenter;
    double sradius;
    
    Vec3f p;
		
	double t,t1,t2;
    int i;
	
    //scenter = vertex_data[s.center_vertex_id-1];
    sradius = s.radius;
    
    
	C = (r.a.x-scenter.x)*(r.a.x-scenter.x)+(r.a.y-scenter.y)*(r.a.y-scenter.y)+(r.a.z-scenter.z)*(r.a.z-scenter.z)-sradius*sradius;

	B = 2*r.b.x*(r.a.x-scenter.x)+2*r.b.y*(r.a.y-scenter.y)+2*r.b.z*(r.a.z-scenter.z);
	
	A = r.b.x*r.b.x+r.b.y*r.b.y+r.b.z*r.b.z;
	
	delta = B*B-4*A*C;
	
	if (delta<0) return -1;
	else if (delta==0)
	{
		t = -B / (2*A);
	}
	else
	{
		double tmp;
		delta = sqrt(delta);
		A = 2*A;
		t1 = (-B + delta) / A;
		t2 = (-B - delta) / A;
	
		if (t2<t1){
			tmp = t2;
			t2 = t1;
			t1 = tmp;
		}	
		
		if (t1>=1.0) t = t1;
		else t = -1;
	}
	return t;
}



double intersectTriangle(Ray r, Vec3f tra, Vec3f trb, Vec3f trc)
{
	
	double  a,b,c,d,e,f,g,h,i,j,k,l;
	double beta,gamma,t;
	
	double eimhf,gfmdi,dhmeg,akmjb,jcmal,blmkc;

	double M;
	
	double dd;
	Vec3f ma,mb,mc;

	ma = tra;
	mb = trb;
	mc = trc;
	
	a = ma.x-mb.x;
	b = ma.y-mb.y;
	c = ma.z-mb.z;

	d = ma.x-mc.x;
	e = ma.y-mc.y;
	f = ma.z-mc.z;
	
	g = r.b.x;
	h = r.b.y;
	i = r.b.z;
	
	j = ma.x-r.a.x;
	k = ma.y-r.a.y;
	l = ma.z-r.a.z;
	
	eimhf = e*i-h*f;
	gfmdi = g*f-d*i;
	dhmeg = d*h-e*g;
	akmjb = a*k-j*b;
	jcmal = j*c-a*l;
	blmkc = b*l-k*c;

	M = a*eimhf+b*gfmdi+c*dhmeg;
    if (M==0) return -1;
	
	t = -(f*akmjb+e*jcmal+d*blmkc)/M;
	
	if (t<1.0) return -1;
	
	gamma = (i*akmjb+h*jcmal+g*blmkc)/M;
	
	if (gamma<0 || gamma>1) return -1;
	
	beta = (j*eimhf+k*gfmdi+l*dhmeg)/M;
	
	if (beta<0 || beta>(1-gamma)) return -1;
	
	return t;

}

Vec3f reflect(const Vec3f &I, const Vec3f &N) 
{ 
    return fadd(fmult(fmult(N, fdot(I, N)), 2), fmult(I, -1)); 
} 

float clamp(const float &lo, const float &hi, const float &v) 
{ return std::max(lo, std::min(hi, v)); } 


Vec3f trace(Vec3i background,int dep,double epsilon,std::vector<Vec3f> vertex_data,std::vector<Sphere> spheres,std::vector<Triangle> triangles,std::vector<Mesh> meshes,std::vector<Material> materials,std::vector<PointLight> lights,Vec3f ambient,Camera camera1,Ray r,const int &depth){
		//std::cout << depth<<std::endl;
		
    	
		
		Vec3f position_of_cam1 = camera1.position;
		Vec3f gaze_of_cam1 = camera1.gaze;
		Vec3f up_of_cam1= camera1.up;
		
		float neardist_of_cam1= camera1.near_distance;
		
		
		
		int numLight=lights.size();
		int height_of_cam1=camera1.image_height;
   		int width_of_cam1= camera1.image_width;
		int numMesh=meshes.size();
		int numSphere= spheres.size();
		int numTriangle= triangles.size();
		int i = 0,mi,n,l,f;
		Vec3f ret;
		Material m;
		Vec3f view , a,b,c,at,bt,ct;
			double tmin = 40000;
			int closestObj = -1;
			int tri_index;
			double t;
			int close_type;
			//FIND
			for(mi = 0; mi < numSphere; mi++){

				Vec3f center = vertex_data[(spheres[mi].center_vertex_id)-1];
				t = intersectSphere(r, spheres[mi], center);
				
				if(t>=1)
				{
					if(t<tmin){
						tmin=t;
						close_type=0;
						closestObj=mi;
					}
				}
			}
			for(n = 0; n < numTriangle; n++){

				Triangle tr = triangles[n];
				a = vertex_data[(tr.indices.v0_id)-1];
				b = vertex_data[(tr.indices.v1_id)-1];
				c = vertex_data[(tr.indices.v2_id)-1];
				t = intersectTriangle(r, a, b, c);
				if(t>=1)
				{
					if(t<tmin){
						tmin=t;
						close_type=1;
						closestObj=n;
					}
				}

			}
			
			for(l = 0; l < numMesh; l++){
				Mesh mesh = meshes[l];
				int face_number = mesh.faces.size();
				for(int la = 0; la < face_number ; la++){
					Face f = mesh.faces[la];
					a = vertex_data[(f.v0_id)-1];
					b = vertex_data[(f.v1_id)-1];
					c = vertex_data[(f.v2_id)-1];
					
					t = intersectTriangle(r, a, b, c);//BU HEP -1 DÖNÜYOOOOOOOOOO
					
					if(t>=0)
					{
						if(t<tmin){
							tmin=t;
							close_type=2;
							closestObj=l;
							 tri_index=la;
						}
					}
				}
			}
			
				double r_val=(float) background.x;
				double g_val=(float) background.y;
				double b_val=(float) background.z;
			
			if(closestObj != -1){
					//COMPUTE AMBIENT LIGHT
					Vec3f p;
					Vec3f normal;
					Vec3f toLight;
					double diffuse;
					
					p = fadd(r.a,fmult(r.b,tmin));
					Ray shadowRay ;
					 r_val=0;
					 g_val=0;
					 b_val=0;
					
					if(close_type==0){
						normal = fadd(p,fmult(vertex_data[(spheres[closestObj].center_vertex_id)-1],-1));
						normal = normalize(normal);	
						m = materials[(spheres[closestObj].material_id)-1];	
						view= normalize(fadd(r.a,fmult(p,-1)));
						
						r_val += (m.ambient.x)*(ambient.x);
						g_val += (m.ambient.y)*(ambient.y);
						b_val += (m.ambient.z)*(ambient.z);
						if(depth<dep){
										
									  	if(m.mirror.x != 0.f || m.mirror.y != 0.f || m.mirror.z!=0.f){
											Vec3f reflectionDirection = normalize(reflect(view, normal));
											Ray second;
											second.a=p;
											second.b=fmult(reflectionDirection,epsilon);
											Vec3f c_ref= trace(background,dep,epsilon,vertex_data,spheres,triangles,meshes,materials,lights,ambient,camera1,second,depth+1);
											r_val+= m.mirror.x* c_ref.x;
											g_val+=m.mirror.y*c_ref.y;
											b_val+=m.mirror.z*c_ref.z;
									   
									   }

								}
						
					}
					else if(close_type==1){
						Triangle tr = triangles[closestObj];
						a = vertex_data[(tr.indices.v0_id)-1];
						b = vertex_data[(tr.indices.v1_id)-1];
						c = vertex_data[(tr.indices.v2_id)-1];
						Vec3f edge1 = fadd(b,fmult(a,-1));
						Vec3f edge2 = fadd(c,fmult(a,-1));
						normal = fcross(edge1,edge2);
						normal = normalize(normal);		
						m = materials[(triangles[closestObj].material_id)-1];
						view= normalize(fadd(r.a,fmult(p,-1)));
						
						r_val += (m.ambient.x)*(ambient.x);
						g_val += (m.ambient.y)*(ambient.y);
						b_val += (m.ambient.z)*(ambient.z);
						if(depth<dep){
										//std::cout << depth<<std::endl;
										if(m.mirror.x != 0.f || m.mirror.y != 0.f || m.mirror.z!=0.f ){
										Vec3f reflectionDirection = normalize(reflect(view, normal));
										Ray second;
										second.a=p;
										second.b=fmult(reflectionDirection,epsilon);
										Vec3f c_ref= trace(background,dep,epsilon,vertex_data,spheres,triangles,meshes,materials,lights,ambient,camera1,second,depth+1);
										r_val+= m.mirror.x* c_ref.x;
										g_val+=m.mirror.y*c_ref.y;
										b_val+=m.mirror.z*c_ref.z;
										}

									}
					}
					else if(close_type==2){
						int g=tri_index;
									
						Triangle tr;
						tr.indices.v0_id = meshes[closestObj].faces[g].v0_id;
						tr.indices.v1_id = meshes[closestObj].faces[g].v1_id;
						tr.indices.v2_id = meshes[closestObj].faces[g].v2_id;
						a = vertex_data[(tr.indices.v0_id)-1];
						b = vertex_data[(tr.indices.v1_id)-1];
						c = vertex_data[(tr.indices.v2_id)-1];
						Vec3f edge1 = fadd(b,fmult(a,-1));
						Vec3f edge2 = fadd(c,fmult(a,-1));
						normal = fcross(edge1,edge2);
						normal = normalize(normal);
						m = materials[(meshes[closestObj].material_id)-1];			
						view= normalize(fadd(r.a,fmult(p,-1)));
									
						r_val += (m.ambient.x)*(ambient.x);
						g_val += (m.ambient.y)*(ambient.y);
						b_val += (m.ambient.z)*(ambient.z);
							if( depth<dep){
										if(m.mirror.x != 0.f || m.mirror.y != 0.f || m.mirror.z!=0.f){
											
										Vec3f reflectionDirection = normalize(reflect(view, normal));
										Ray second;
										second.a=p;
										second.b=fmult(reflectionDirection,epsilon);
										Vec3f c_ref= trace(background,dep,epsilon,vertex_data,spheres,triangles,meshes,materials,lights,ambient,camera1,second,depth+1);
										
										r_val+= m.mirror.x* c_ref.x;
										g_val+=m.mirror.y*c_ref.y;
										b_val+=m.mirror.z*c_ref.z;
										}

									}

					}
				
				//TRAVERSE EACH LIGHT
				for(f=0;f<numLight;f++){
					
					
					PointLight p_light= lights[f];
					Vec3f light= p_light.position;
					toLight = fadd(light,fmult(p,-1));
					toLight = normalize(toLight);
					shadowRay.a= p;
					shadowRay.b= fmult(toLight,epsilon);
					bool isShadow = false;
					//CHECK IFIN SHADOW
					double dist = fdistance(shadowRay.a, light);
					for(int me=0; me<numMesh+numSphere+numTriangle;me++){
						
						int object_index;
						if(me<numSphere){
							double inter = intersectSphere(shadowRay,spheres[me],vertex_data[(spheres[me].center_vertex_id)-1]);
							Vec3f p2 = fadd(shadowRay.a,fmult(shadowRay.b,inter));
								if(inter >-1 &&  dist > fdistance(shadowRay.a, p2) ){
											isShadow=true;
											//std::cout<<"aaa"<<std::endl;
											break;
								}

							}

							else if(me<numSphere+numTriangle){
								object_index=me-numSphere;
								Triangle tr = triangles[object_index];
								a = vertex_data[(tr.indices.v0_id)-1];
								b = vertex_data[(tr.indices.v1_id)-1];
								c = vertex_data[(tr.indices.v2_id)-1];
								double inter = intersectTriangle(shadowRay,a,b,c);
								Vec3f p2 = fadd(shadowRay.a,fmult(toLight,inter));
								if(inter>-1 && dist > fdistance(shadowRay.a, p2)){
									isShadow=true;
									//std::cout<<"bbb"<<std::endl;
									break;
								}

							}
							else if(me<numMesh+numSphere+numTriangle)
							{
								object_index=me-numSphere-numTriangle;
								int face_number = meshes[object_index].faces.size();
								for(int ut=0;ut<face_number;ut++){
									Face f1 = meshes[object_index].faces[ut];
									at = vertex_data[(f1.v0_id)-1];
									bt = vertex_data[(f1.v1_id)-1];
									ct = vertex_data[(f1.v2_id)-1];
									double inter = intersectTriangle(shadowRay,at,bt,ct);
									Vec3f p2 = fadd(shadowRay.a,fmult(shadowRay.b,inter));
									if( inter >-1 && dist > fdistance(shadowRay.a, p2)){
										isShadow=true;
										//std::cout<<"ccc"<<std::endl;
										break;
									}
								}
							}
							

					}

					
					//INTERSECTS WITH ONE OF THE LIGHT SOURCE
				
							if(close_type == 0){
								
								
								
								if(isShadow==false){

									toLight = fadd(light,fmult(p,-1));
									double distance= flength(toLight);
									toLight = normalize(toLight);
									double phong = m.phong_exponent;
									
									Vec3f hw = fadd(toLight,view);
									hw=normalize(hw);
									double spec= std::max((double)0 ,fdot(hw,normal));
									spec=pow(spec,phong);			
								
									//double distance = fdistance(light,fmult(p,-1));
									diffuse = fdot(normal,toLight);
									if (diffuse<0) diffuse=0;

									r_val += (m.diffuse.x)*(p_light.intensity.x/(distance*distance))*diffuse;								
									r_val += (m.specular.x)*(p_light.intensity.x/(distance*distance))*spec;
									g_val += (m.diffuse.y)*(p_light.intensity.y/(distance*distance))*diffuse;	
									g_val += (m.specular.y)*(p_light.intensity.y/(distance*distance))*spec;
									b_val += (m.diffuse.z)*(p_light.intensity.z/(distance*distance))*diffuse;
									b_val += (m.specular.z)*(p_light.intensity.z/(distance*distance))*spec;
								}

							}
							else if(close_type == 1){
								
									if(isShadow==false){

										toLight = fadd(light,fmult(p,-1));
										double distance= flength(toLight);
										toLight = normalize(toLight);
										double phong = m.phong_exponent;
										Vec3f hw = fadd(toLight,view);
										hw=normalize(hw);
										double spec= std::max((double)0 ,fdot(hw,normal));
										spec=pow(spec,phong);
										diffuse = fdot(normal,toLight);
										if (diffuse<0) diffuse=0;


										r_val += (m.diffuse.x)*(p_light.intensity.x/(distance*distance))*diffuse;
										r_val += (m.specular.x)*(p_light.intensity.x/(distance*distance))*spec;
										g_val += (m.diffuse.y)*(p_light.intensity.y/(distance*distance))*diffuse;
										g_val += (m.specular.y)*(p_light.intensity.y/(distance*distance))*spec;
										b_val += (m.diffuse.z)*(p_light.intensity.z/(distance*distance))*diffuse;
										b_val += (m.specular.z)*(p_light.intensity.z/(distance*distance))*spec;
									}
									
								}
							else if(close_type==2){
									if(isShadow==false){
										toLight = fadd(light,fmult(p,-1));
										double distance= flength(toLight);
										toLight = normalize(toLight);
										double phong = m.phong_exponent;
										Vec3f hw = fadd(toLight,view);
										hw=normalize(hw);
										double spec= std::max((double)0 ,fdot(hw,normal));
										spec=pow(spec,phong);
										diffuse = fdot(normal,toLight);
										if (diffuse<0) diffuse=0;

										r_val += (m.diffuse.x)*(p_light.intensity.x/(distance*distance))*diffuse;
										r_val += (m.specular.x)*(p_light.intensity.x/(distance*distance))*spec;
										g_val += (m.diffuse.y)*(p_light.intensity.y/(distance*distance))*diffuse;	
										g_val += (m.specular.y)*(p_light.intensity.y/(distance*distance))*spec;
										b_val += (m.diffuse.z)*(p_light.intensity.z/(distance*distance))*diffuse;
										b_val += (m.specular.z)*(p_light.intensity.z/(distance*distance))*spec;
									}
					
							}

				}

				


			}

			ret.x=clamp(0,255,r_val);
			ret.y=clamp(0,255,g_val);
			ret.z=clamp(0,255,b_val);
									

			return ret;
			
	

}


int main(int argc, char* argv[])
{
    // Sample usage for reading an XML scene file
    Scene scene;

    scene.loadFromXml(argv[1]);
	std::vector<Camera> cams=scene.cameras;
	
	double epsilon = scene.shadow_ray_epsilon;
	std::vector<Sphere> spheres = scene.spheres;
	std::vector<Triangle> triangles= scene.triangles;
	std::vector<Material> materials= scene.materials;
	std::vector<Mesh> meshes= scene.meshes;
	std::vector<PointLight> lights= scene.point_lights;
	std::vector<Vec3f> vertex_data= scene.vertex_data;
	Vec3f ambient = scene.ambient_light;
	int dep = scene.max_recursion_depth;
	Vec3i background = scene.background_color;
	int numberofcam=cams.size();
	for(int no_c =0; no_c<numberofcam;no_c++){

			Camera camera1 = cams[no_c];
			
			Vec4f nearplane_of_cam1 = camera1.near_plane;
			int height_of_cam1=camera1.image_height;
			int width_of_cam1= camera1.image_width;
			std::string const name_of_cam1 = camera1.image_name;
		
			double pixelw = (nearplane_of_cam1.y-nearplane_of_cam1.x)/(double)width_of_cam1;
			double pixelh = (nearplane_of_cam1.w-nearplane_of_cam1.z)/(double)height_of_cam1;
			

			unsigned char* image = new unsigned char [width_of_cam1 * height_of_cam1 * 3];

			int i=0,j,k;	
			for(j=0;j<height_of_cam1; j++){
				
				for(k=0; k<width_of_cam1;k++){
					
					Ray r = generateRay(j,k,camera1,pixelw,pixelh);
					
					Vec3f color=trace(background,dep,epsilon,vertex_data,spheres,triangles,meshes,materials,lights,ambient,camera1,r,0);
					//int ind=3*(k*height_of_cam1+j);
					image[i++]=int(round(color.x));
					image[i++]=int(round(color.y));
					image[i++]=int(round(color.z));
					
				}
			}	


			write_ppm(name_of_cam1.c_str(), image, width_of_cam1, height_of_cam1);
	}

}
	
