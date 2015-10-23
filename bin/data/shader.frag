#version 120
#extension GL_EXT_gpu_shader4 : require

uniform vec4 fogColor;
uniform float fogStart;
uniform float fogEnd;

varying vec4 color;
varying float zPos;

void main() {
    float c = clamp(zPos/fogStart, 0.0,1.0);
//    gl_FragColor = vec4(c,c,c,1.0);
    gl_FragColor = mix(color, fogColor, (clamp((zPos-fogStart)/(fogEnd - fogStart), 0.0,1.0)));
//    gl_FragColor = vec4(1.0,1.0,1.0,1.0);//color;
}
