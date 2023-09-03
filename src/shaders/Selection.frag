#version 460 core

uniform int entityId;

out int fragColor; 

void main() {
	fragColor = entityId;
}
