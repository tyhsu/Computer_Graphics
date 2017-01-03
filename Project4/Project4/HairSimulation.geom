#version 150 compatibility

uniform float segmentLen;
uniform int segmentNum;
uniform float gravityY;
uniform mat4 projectMatrix;

layout(triangles) in;
layout(line_strip, max_vertices=256) out;

in Vertex {
    vec3 normal;
}vertex[];
out float segmentIndex;

vec4 gravity = vec4(0.0f, gravityY, 0.0f, 0.0f);

void main() {
    for (int i = 0; i < gl_in.length(); i++) {
		vec4 preEndpoint = projectMatrix * gl_in[i].gl_Position;
		vec4 normal = projectMatrix * vec4(vertex[i].normal, 0.0f);
		segmentIndex = 0.0;
		for (int j = 0; j < segmentNum; j++) {
			//start point
			gl_Position = preEndpoint;
			EmitVertex();

			//end point
			gl_Position = preEndpoint + normalize(normal + gravity) * segmentLen;
			EmitVertex();

			EndPrimitive();
			preEndpoint = gl_Position;
			normal = normal + gravity;
			segmentIndex += 1;
		}
    }
}
