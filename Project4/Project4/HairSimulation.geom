#version 150 compatibility

uniform float segmentLen;
uniform int segmentNum;
uniform float gravityY;
uniform mat4 projectMatrix;

layout(triangles) in;
layout(line_strip, max_vertices=6) out;

in Vertex{
    vec3 normal;
}vertex[];

void main(){
    for(int i = 0; i < gl_in.length(); i++){
        //start point
        gl_Position = projectMatrix * gl_in[i].gl_Position;
        EmitVertex();

        //end point
        gl_Position = projectMatrix * (gl_in[i].gl_Position + vec4(vertex[i].normal, 0.0f) * segmentLen);
        EmitVertex();

        EndPrimitive();
    }
}
