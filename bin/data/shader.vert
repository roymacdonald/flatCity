#version 120
#extension GL_EXT_gpu_shader4 : require

uniform float thickness;
uniform float heightMult;

//varying vec4 color;

void main(){
   // color = vec4(1.0,1.0,1.0,1.0);
    //*
    vec4 c = gl_Color;
    int ind = 1;
    if (mod(gl_VertexID ,2) == 0) {
      ind = -1;
    }//*/
    vec4 pos;
    pos.x = gl_Vertex.x + ind * c.x * thickness;
    pos.y = gl_Vertex.y * heightMult,
    pos.z = gl_Vertex.z + ind * c.z * thickness;
    pos.w = 1.0;
//    gl_Position = gl_ModelViewProjectionMatrix *gl_Vertex;
        gl_Position = gl_ModelViewProjectionMatrix * pos ;
}