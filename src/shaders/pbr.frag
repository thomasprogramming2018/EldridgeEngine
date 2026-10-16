
#version 330 core

in vec3 WorldPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

// Camera
struct Camera {
    vec3 Pos;
};
uniform Camera camera;

// Material
struct Material {
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    vec3 shininess;
    sampler2D normal;
};
uniform Material material;

// Point lights
struct Light {
    vec3 ambient;
    vec3 position;
    vec3 diffuse;
    vec3 specular;
};

uniform Light lights[32];
uniform int lightCount;

// Directional light
struct DLight {
    vec3 dir;
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    vec3 normal;
};
uniform DLight dlight;

// Object
struct Object {
    vec3 translate;
};
uniform Object object;


// Convert tangent-space normal map into world space
vec3 calculateNormal()
{
    vec3 N = normalize(Normal);

    vec3 normalColor = texture(material.normal, TexCoords).rgb;
    vec3 mappedNormal = normalColor * 2.0 - 1.0;

    mappedNormal.xy *= 1.0;
    mappedNormal = normalize(mappedNormal);
    vec3 dp1 = dFdx(WorldPos);
    vec3 dp2 = dFdy(WorldPos);

    vec2 duv1 = dFdx(TexCoords);
    vec2 duv2 = dFdy(TexCoords);

    float det = duv1.x * duv2.y - duv2.x * duv1.y;

    if (abs(det) < 0.00001)
        return N;

    vec3 tangent = (dp1 * duv2.y - dp2 * duv1.y) / det;
    vec3 bitangent = (dp2 * duv1.x - dp1 * duv2.x) / det;

    vec3 T = normalize(tangent - N * dot(N, tangent));

    vec3 B = cross(N, T);
    B *= sign(dot(B, bitangent));

    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * mappedNormal);
}


// Calculate point light
vec3 calculatePointLight(
    Light light,
    vec3 N,
    vec3 V,
    vec3 albedo,
    vec3 specMap
)
{
    vec3 L = normalize(light.position - WorldPos);

    float diff = max(dot(N, L), 0.0);

    vec3 R = reflect(-L, N);

    float spec = 0.0;

    if (diff > 0.0)
        spec = pow(max(dot(V, R), 0.0), 32.0);

    vec3 ambient = light.ambient * albedo;
    vec3 diffuse = light.diffuse * diff * albedo;
    vec3 specular = light.specular * spec * specMap;

    return ambient + diffuse + specular;
}


// Calculate directional light
vec3 calculateDirectionalLight(
    vec3 N,
    vec3 V,
    vec3 albedo,
    vec3 specMap
)
{
    // Assumes dlight.dir points along the light rays
    vec3 L = normalize(-dlight.dir);

    float diff = max(dot(N, L), 0.0);

    vec3 R = reflect(-L, N);

    float spec = 0.0;

    if (diff > 0.0)
        spec = pow(max(dot(V, R), 0.0), 32.0);

    vec3 ambient = dlight.ambient * albedo;
    vec3 diffuse = dlight.diffuse * diff * albedo;
    vec3 specular = dlight.specular * spec * specMap;

    return ambient + diffuse + specular;
}


void main()
{
    vec3 N = calculateNormal();

    vec3 V = normalize(camera.Pos - WorldPos);

    vec3 albedo = texture(material.diffuse, TexCoords).rgb;
    vec3 specMap = texture(material.specular, TexCoords).rgb;

    vec3 result = vec3(0.0);

    // Multiple point lights
    for (int i = 0; i < min(lightCount, 32); i++)
    {
        result += calculatePointLight(
            lights[i],
            N,
            V,
            albedo,
            specMap
        );
    }

    // Directional light
    result += calculateDirectionalLight(
        N,
        V,
        albedo,
        specMap
    );

    FragColor = vec4(result, 1.0);
}
