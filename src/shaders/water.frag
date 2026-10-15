#version 330 core

in vec3 WorldPos;
in vec3 Normal;

out vec4 FragColor;

uniform float uTime;
uniform vec3 uAlbedo;

struct Camera {
    vec3 Pos;
};
uniform Camera camera;

struct DLight {
    vec3 dir;
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    vec3 normal;
};
uniform DLight dlight;

uniform sampler2D sceneColor;
uniform sampler2D sceneDepth;

uniform mat4 uView;
uniform mat4 uProjection;




void main()
{
    float waterDepth = 10.0;
    float maxDepth = 10.0;
    float depth = waterDepth / maxDepth;

    depth = 1.0 - clamp(depth, 0.0, 1.0);



    vec3 L = normalize(dlight.dir);

    vec3 N = normalize(Normal);

    vec3 R = reflect(-L, N);

    vec3 V = normalize(camera.Pos - WorldPos);

    float specular = pow(max(dot(R, V), 0.0), 32.0);

    vec3 spec = dlight.specular * specular;

    float diffuse = max(dot(L, N), 0.0);

    float angle = max(dot(N, V), 0.0);
    float fresnel = 1.0 - angle;
    fresnel = pow(fresnel,1.0);

    vec3 diff = dlight.diffuse * diffuse * uAlbedo;
    vec3 reflectionColor = vec3(0.2, 0.5, 0.9);

    vec3 deep = vec3(0.01, 0.05, 0.15);
    vec3 shallow = vec3(0.0, 0.5, 0.6);

    vec3 waterColor = mix(deep, shallow, depth);
    vec3 fresnelCol = mix(waterColor, reflectionColor, fresnel);

    vec3 ambient = dlight.ambient * uAlbedo;
    vec3 color = ambient + spec + diff + fresnelCol;


    vec2 uv = gl_FragCoord.xy / vec2(textureSize(sceneColor, 0));



    vec3 rayDir = normalize(reflect(-V, N));

    vec3 rayPos = WorldPos;
    float stepSize = 0.2;

    for (int i = 0; i < 50; i++)
    {
        rayPos += rayDir * stepSize;

        vec4 clip = uProjection * uView * vec4(rayPos, 1.0);

        if (clip.w <= 0.0)
            break;

        vec2 rayUV = (clip.xy / clip.w) * 0.5 + 0.5;

        if (any(lessThan(rayUV, vec2(0.0))) ||
            any(greaterThan(rayUV, vec2(1.0))))
            break;
        
        float storedDepth = texture(sceneDepth, rayUV).r;

        float rayDepth = (clip.z / clip.w) * 0.5 + 0.5;

        if (rayDepth >= storedDepth &&
            rayDepth - storedDepth < 0.001)
        {
            reflectionColor = texture(sceneColor, rayUV).rgb;
            break;
        }
    }




    
    FragColor = vec4(
        mix(waterColor, reflectionColor, fresnel) + ambient + diff + spec,
        1.0
    );
}








    // 1. Surface normal
    vec3 N = normalize(Normal);

    // 2. Calculate tangent space
    vec3 dp1 = dFdx(WorldPos);
    vec3 dp2 = dFdy(WorldPos);

    vec2 duv1 = dFdx(TexCoords);
    vec2 duv2 = dFdy(TexCoords);

    float det = duv1.x * duv2.y - duv1.y * duv2.x;

    if (abs(det) > 0.00001)
    {
        vec3 tangent = (dp1 * duv2.y - dp2 * duv1.y) / det;
        vec3 bitangent = (dp2 * duv1.x - dp1 * duv2.x) / det;

        vec3 T = normalize(tangent - N * dot(N, tangent));
        vec3 B = normalize(cross(N, T)) *
                 sign(dot(cross(N, T), bitangent));

        mat3 TBN = mat3(T, B, N);

        // 3. Read normal map
        vec3 mappedNormal = normalize(
            texture(material.normal, TexCoords).rgb * 2.0 - 1.0
        );

        N = normalize(TBN * mappedNormal);
    }

    // 4. Light directions
    vec3 L = normalize(light.Pos - WorldPos);
    vec3 DL = normalize(dlight.dir);
    vec3 V = normalize(camera.Pos - WorldPos);

    // 5. Diffuse
    float diff = max(dot(N, L), 0.0);
    float dDiff = max(dot(N, DL), 0.0);

    // 6. Specular
    float spec = pow(max(dot(V, reflect(-L, N)), 0.0), 32.0);
    float dSpec = pow(max(dot(V, reflect(-DL, N)), 0.0), 32.0);

    // 7. Textures
    vec3 albedo = texture(material.diffuse, TexCoords).rgb;
    vec3 specMap = texture(material.specular, TexCoords).rgb;

    // 8. Combine lights
    vec3 ambient = dlight.ambient * albedo;

    vec3 diffuse =
        (light.diffuse * diff +
         dlight.diffuse * dDiff) * albedo;

    vec3 specular =
        (light.specular * spec +
         dlight.specular * dSpec) * specMap;