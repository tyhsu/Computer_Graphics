#version 150 compatibility

layout(triangles) in;
layout(line_strip, max_vertices=6) out;

in Vertex{
    vec3 normal;
}vertex[];

void main(){
    float length = 2.0f;

    for(int i = 0; i < gl_in.length(); i++){
        //start point
        gl_Position = gl_ProjectionMatrix * gl_in[i].gl_Position;
        EmitVertex();

        //end point
        gl_Position = gl_ProjectionMatrix * (gl_in[i].gl_Position + vec4(vertex[i].normal, 0.0f) * length);
        EmitVertex();

        EndPrimitive();
    }
}
