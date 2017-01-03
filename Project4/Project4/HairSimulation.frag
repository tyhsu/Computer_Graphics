#version 150 compatibility

uniform int segmentNum;

//in int segmentIndex;

vec4 firstColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
vec4 finalColor = vec4(0.3f, 0.3f, 0.7f, 1.0f);

void main(){
	//gl_FragColor = mix( finalColor, firstColor, (float)segmentIndex/(segmentNum-1) );
	gl_FragColor =  vec4(0.3f, 0.3f, 0.7f, 1.0f);
}
