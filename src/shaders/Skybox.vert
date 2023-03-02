#version 460 core

layout (location = 0) in vec3 iPos;

uniform mat4 view;
uniform mat4 projection;

out vec3 texCoords;

void main() {
	texCoords = vec3(iPos.xy, -iPos.z); 
	mat4 newView = mat4(mat3(view));

	vec4 pos = projection * newView * vec4(iPos, 1.0f);
	gl_Position = pos.xyww;
}
