#version 150 compatibility

out vec3 vertexEyeSpace;
out vec3 normal;

void main(){
	// Phong shader
    vertexEyeSpace = vec3(gl_ModelViewMatrix * gl_Vertex);
    normal = normalize(gl_NormalMatrix * gl_Normal);

	// texture mapping shader
    gl_TexCoord[0] = gl_MultiTexCoord0;

    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}
