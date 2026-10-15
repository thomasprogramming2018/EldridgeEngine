
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 WorldPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform float uTime;

// Pseudorandom value
float random(vec2 p)
{
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

// Smooth 2D value noise
float noise(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);

    f = f * f * (3.0 - 2.0 * f);

    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    return mix(
        mix(a, b, f.x),
        mix(c, d, f.x),
        f.y
    );
}

// Animated ocean height
float waves(vec2 p)
{
    vec2 movement = vec2(0.4, 0.2) * uTime;

    float large = noise(p * 0.35 + movement) * 1.2;
    float small = noise(p * 1.2 + movement * 1.5) * 0.25;

    return large + small;
}

void main()
{
    vec3 pos = aPos;

    pos.y += waves(pos.xz);

    // Approximate slopes for dynamic normals
    float e = 0.05;

    float dx = (
        waves(aPos.xz + vec2(e, 0.0)) -
        waves(aPos.xz - vec2(e, 0.0))
    ) / (2.0 * e);

    float dz = (
        waves(aPos.xz + vec2(0.0, e)) -
        waves(aPos.xz - vec2(0.0, e))
    ) / (2.0 * e);

    vec3 N = normalize(vec3(-dx, 1.0, -dz));

    Normal = normalize(
        mat3(transpose(inverse(uModel))) * N
    );

    vec4 world = uModel * vec4(pos, 1.0);

    WorldPos = world.xyz;
    TexCoords = aTexCoords;

    gl_Position = uProjection * uView * world;
}
