#version 150 compatibility

uniform float segmentLen;
uniform int segmentNum;
uniform vec3 gravity;
uniform mat4 projectMatrix;

layout(triangles) in;
layout(line_strip, max_vertices=256) out;

in Vertex {
    vec3 normal;
}vertex[];
out float segmentIndex;

void main() {
    for (int i = 0; i < gl_in.length(); i++) {
		vec3 curNormal = vertex[i].normal;
		float normalLen = length(curNormal);
		vec4 startPos = gl_in[i].gl_Position;
		vec4 endPos = startPos + normalize( vec4(curNormal + gravity, 0.0f) ) * segmentLen;

		segmentIndex = 0.0;

		for (int j = 0; j < segmentNum; j++) {
			//start point
			gl_Position = projectMatrix * startPos;
			EmitVertex();

			//end point
			gl_Position =  projectMatrix * endPos;
			EmitVertex();

			EndPrimitive();
			curNormal = normalize((endPos - startPos).xyz) * normalLen;
			startPos = endPos;
			endPos = startPos + normalize( vec4(curNormal + gravity, 0.0f) ) * segmentLen;
			segmentIndex += 1;
		}
    }
}
