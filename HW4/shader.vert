#version 330

layout(location = 0) in vec3 position;

// Data from CPU 
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform vec4 cameraPosition;
uniform float heightFactor;
uniform vec3 light_pos;

// Texture-related data
uniform sampler2D rgbTexture;
uniform sampler2D depthTexture;
uniform int widthTexture;
uniform int heightTexture;

uniform mat4 G_norm;

// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;



void main()
{

    // get texture value, compute height
    
    vec3 m_pos= position;
    
    textureCoordinate = vec2(1 - (float(m_pos.x)/(widthTexture+1)) , 1-(float(m_pos.z)/(heightTexture+1)));
    
    vec4 tex_color = texture(depthTexture,textureCoordinate);
    vec3 depth = tex_color.xyz;
    
    m_pos.y = heightFactor * (depth.x);
    
    
    // compute normal vector using also the heights of neighbor vertices
    
     vec2 neighbor_coord;
     vec3 norm;
     vec3 neighbor0, neighbor1, neighbor2, neighbor3, neighbor4, neighbor5;
     vec3 vertex0, vertex1, vertex2, vertex3, vertex4, vertex5;
     vec4 texture_color;
     vec3 rgb_neighbor;
    if(position.x == 0 && position.z == 0 ){
    
        neighbor0 = vec3(position.x + 1, position.y, position.z);
        neighbor1 = vec3(position.x, position.y, position.z + 1);
        
        neighbor_coord.x = abs(neighbor0.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor0.z - heightTexture) / heightTexture;
    
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor0.y = heightFactor * rgb_neighbor.x;
        
        neighbor_coord.x = abs(neighbor1.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor1.z - heightTexture) / heightTexture;
    
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor1.y = heightFactor * rgb_neighbor.x;
        
        vertex0 = m_pos - neighbor0;
        vertex1 = neighbor1 - neighbor0;
        
        norm = normalize(cross(vertex0, vertex1));
    
    }
    else if(position.x == 0 && position.z == heightTexture-1){
        
        neighbor0 = vec3(position.x + 1, 0, position.z);
        neighbor1 = vec3(position.x + 1, 0, position.z - 1);
        neighbor2 = vec3(position.x, 0,position.z - 1);
    
        neighbor_coord.x = abs(neighbor0.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor0.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor0.y = heightFactor * rgb_neighbor.x;
        
        neighbor_coord.x = abs(neighbor1.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor1.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor1.y = heightFactor * rgb_neighbor.x;
        
        neighbor_coord.x = abs(neighbor2.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor2.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor2.y = heightFactor * rgb_neighbor.x;
        
        vertex0 = neighbor0 - m_pos;
        vertex1 = neighbor1 - m_pos;
        vertex2 = neighbor2 - m_pos;

        norm = normalize(cross(vertex1, vertex0) + cross(vertex2, vertex1));

    
    
    }
    else if(position.x == widthTexture-1 && position.z == 0 ){
     
        neighbor0 = vec3(position.x - 1, position.y, position.z);
        neighbor1 = vec3(position.x - 1, position.y, position.z + 1);
        neighbor2 = vec3(position.x, 0, position.z + 1);

        
        neighbor_coord.x = abs(neighbor0.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor0.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor0.y = heightFactor * rgb_neighbor.x;
        
        neighbor_coord.x = abs(neighbor1.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor1.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor1.y = heightFactor * rgb_neighbor.x;
        
        neighbor_coord.x = abs(neighbor2.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor2.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor2.y = heightFactor * rgb_neighbor.x;
        
        vertex0 = neighbor0 - m_pos;
        vertex1 = neighbor1 - m_pos;
        vertex2 = neighbor2 - m_pos;
        
        norm = normalize(cross(vertex0, vertex1) + cross(vertex1, vertex2));

    
    
    }
    else if(position.x == widthTexture-1 && position.z == heightTexture-1 ){
    
        neighbor0 = vec3(position.x - 1, 0, position.z);
        neighbor1 = vec3(position.x, 0, position.z - 1);
        
        neighbor_coord.x = abs(neighbor0.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor0.z - heightTexture) / heightTexture;
    
        vec4 texture_color = texture(depthTexture, neighbor_coord);
        vec3 rgb_neighbor = texture_color.xyz;
        neighbor0.y = heightFactor * rgb_neighbor.x;
        
        neighbor_coord.x = abs(neighbor1.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor1.z - heightTexture) / heightTexture;
    
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor1.y = heightFactor * rgb_neighbor.x;
        
        vertex0 = m_pos - neighbor0;
        vertex1 = neighbor1 - neighbor0;
    
        norm = normalize(cross(vertex0, vertex1));
    
    }
    else if(position.x == 0 && position.z > 0 && position.z < heightTexture-1){
    
        neighbor0 = vec3(position.x, 0, position.z - 1);
        neighbor1 = vec3(position.x + 1, 0, position.z - 1);
        neighbor2 = vec3(position.x + 1, 0, position.z);
        neighbor3 = vec3(position.x, 0, position.z + 1);
    
        neighbor_coord.x = abs(neighbor0.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor0.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor0.y = heightFactor * rgb_neighbor.x;
        
        neighbor_coord.x = abs(neighbor1.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor1.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor1.y = heightFactor * rgb_neighbor.x;
        
        neighbor_coord.x = abs(neighbor2.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor2.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor2.y = heightFactor * rgb_neighbor.x;

        neighbor_coord.x = abs(neighbor3.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor3.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor3.y = heightFactor * rgb_neighbor.x;
        
        vertex0 = neighbor0 - m_pos;
        vertex1 = neighbor1 - m_pos;
        vertex2 = neighbor2 - m_pos;
        vertex3 = neighbor3 - m_pos;
        
         norm = normalize(cross(vertex1, vertex0) + cross(vertex2, vertex1) + cross(vertex3, vertex2));
    
    }
    else if(position.x == widthTexture-1 && position.z > 0 && position.z < heightTexture-1){
    
         neighbor0 = vec3(position.x, 0, position.z + 1);
         neighbor1 = vec3(position.x - 1, 0, position.z + 1);
         neighbor2 = vec3(position.x - 1, 0, position.z);
         neighbor3 = vec3(position.x, 0, position.z - 1);
    
    
         neighbor_coord.x = abs(neighbor0.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor0.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor0.y = heightFactor * rgb_neighbor.x;
        
        neighbor_coord.x = abs(neighbor1.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor1.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor1.y = heightFactor * rgb_neighbor.x;
        
        neighbor_coord.x = abs(neighbor2.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor2.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor2.y = heightFactor * rgb_neighbor.x;

        neighbor_coord.x = abs(neighbor3.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor3.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor3.y = heightFactor * rgb_neighbor.x;
        
        vertex0 = neighbor0 - m_pos;
        vertex1 = neighbor1 - m_pos;
        vertex2 = neighbor2 - m_pos;
        vertex3 = neighbor3 - m_pos;
        
         norm = normalize(cross(vertex1, vertex0) + cross(vertex2, vertex1) + cross(vertex3, vertex2));
    
    
    
    }
    else if(position.z == 0 && position.x > 0 && position.x < widthTexture-1){
            neighbor0 = vec3(position.x + 1, 0, position.z);
            neighbor1 = vec3(position.x, 0, position.z + 1);
            neighbor2 = vec3(position.x - 1, 0, position.z + 1);
            neighbor3 = vec3(position.x - 1, 0, position.z);
    
    
        neighbor_coord.x = abs(neighbor0.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor0.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor0.y = heightFactor * rgb_neighbor.x;
        
        neighbor_coord.x = abs(neighbor1.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor1.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor1.y = heightFactor * rgb_neighbor.x;
        
        neighbor_coord.x = abs(neighbor2.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor2.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor2.y = heightFactor * rgb_neighbor.x;

        neighbor_coord.x = abs(neighbor3.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor3.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor3.y = heightFactor * rgb_neighbor.x;
        
        vertex0 = neighbor0 - m_pos;
        vertex1 = neighbor1 - m_pos;
        vertex2 = neighbor2 - m_pos;
        vertex3 = neighbor3 - m_pos;
        
         norm = normalize(cross(vertex1, vertex0) + cross(vertex2, vertex1) + cross(vertex3, vertex2));
    
    }
    else if(position.z == heightTexture-1 && position.x > 0 && position.x < widthTexture-1){
    
          neighbor0 = vec3(position.x - 1, 0, position.z);
          neighbor1 = vec3(position.x, 0, position.z - 1);
          neighbor2 = vec3(position.x + 1, 0, position.z - 1);
          neighbor3 = vec3(position.x + 1, 0, position.z);
    
           neighbor_coord.x = abs(neighbor0.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor0.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor0.y = heightFactor * rgb_neighbor.x;
        
        neighbor_coord.x = abs(neighbor1.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor1.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor1.y = heightFactor * rgb_neighbor.x;
        
        neighbor_coord.x = abs(neighbor2.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor2.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor2.y = heightFactor * rgb_neighbor.x;

        neighbor_coord.x = abs(neighbor3.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor3.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor3.y = heightFactor * rgb_neighbor.x;
        
        vertex0 = neighbor0 - m_pos;
        vertex1 = neighbor1 - m_pos;
        vertex2 = neighbor2 - m_pos;
        vertex3 = neighbor3 - m_pos;
        
         norm = normalize(cross(vertex1, vertex0) + cross(vertex2, vertex1) + cross(vertex3, vertex2));
    
    }
    else{
    
        neighbor0 = vec3(position.x - 1, 0, position.z);
        neighbor1 = vec3(position.x - 1, 0, position.z + 1);
        neighbor2 = vec3(position.x, 0, position.z + 1);
        neighbor3 = vec3(position.x + 1, 0, position.z);
        neighbor4 = vec3(position.x + 1, 0, position.z - 1);
        neighbor5 = vec3(position.x, 0, position.z - 1);
        
        neighbor_coord.x = abs(neighbor0.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor0.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor0.y = heightFactor * rgb_neighbor.x;
        
        neighbor_coord.x = abs(neighbor1.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor1.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor1.y = heightFactor * rgb_neighbor.x;
        
        neighbor_coord.x = abs(neighbor2.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor2.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor2.y = heightFactor * rgb_neighbor.x;

        neighbor_coord.x = abs(neighbor3.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor3.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor3.y = heightFactor * rgb_neighbor.x;

        neighbor_coord.x = abs(neighbor4.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor4.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor4.y = heightFactor * rgb_neighbor.x;
        
        neighbor_coord.x = abs(neighbor5.x - widthTexture) / widthTexture;
        neighbor_coord.y = abs(neighbor5.z - heightTexture) / heightTexture;   
        texture_color = texture(depthTexture, neighbor_coord);
        rgb_neighbor = texture_color.xyz;
        neighbor5.y = heightFactor * rgb_neighbor.x;
        
        vertex0 = neighbor0 - m_pos;
        vertex1 = neighbor1 - m_pos;
        vertex2 = neighbor2 - m_pos;
        vertex3 = neighbor3 - m_pos;
        vertex4 = neighbor4 - m_pos;
        vertex5 = neighbor5 - m_pos;

        norm = normalize(cross(vertex0, vertex1) + cross(vertex1, vertex2) + cross(vertex2, vertex3) + cross(vertex3, vertex4)
                            + cross(vertex4, vertex5) + cross(vertex5, vertex0));
    
    
    
    }
    // compute toLight vector vertex coordinate in VCS
       vertexNormal = normalize(vec3(G_norm * vec4(norm, 0)));
       ToCameraVector = normalize(vec3(MV * (vec4(vec3(cameraPosition) - m_pos, 0))));
       ToLightVector = normalize(vec3(MV * vec4(light_pos - m_pos, 0)));
   
   
   
   // set gl_Position variable correctly to give the transformed vertex position

  


   gl_Position=MVP*vec4(m_pos, 1.0);
    
}