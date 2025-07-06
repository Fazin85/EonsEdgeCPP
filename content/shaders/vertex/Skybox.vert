#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 pos;
out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vec4 p = projection * view * vec4(aPos, 1.0f);
    // Having z equal w will always result in a depth of 1.0f
    pos = aPos;
    gl_Position = vec4(p.x, p.y, p.w, p.w);
    // We want to flip the z axis due to the different coordinate systems (left hand vs right hand)
    texCoords = vec3(aPos.x, aPos.y, -aPos.z);
}    