#version 460 core

layout (location = 0) in vec3 iPos;

layout (std140, binding = 0) uniform camera {
	vec3 camPos;
	mat4 view;
	mat4 projection;
	mat4 viewProjection;
};

out vec3 texCoords;

void main() {
	texCoords = vec3(iPos.xy, -iPos.z); 
	mat4 newView = mat4(mat3(view));

	vec4 pos = projection * newView * vec4(iPos, 1.0f);
	gl_Position = pos.xyww;
}
