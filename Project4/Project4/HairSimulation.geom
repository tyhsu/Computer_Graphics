#version 150 compatibility

uniform float segmentLen;
uniform int segmentNum;
uniform vec4 gravity;
uniform mat4 projectMatrix;

layout(triangles) in;
layout(line_strip, max_vertices=256) out;

in Vertex {
    vec3 normal;
}vertex[];
out float segmentIndex;

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
