#version 460

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec3 FragPos; // 월드 좌표계
out vec2 TexCoord;
out vec3 FragNormal;
out mat3 TBN;

uniform mat4 modelMat;
uniform mat4 PVM;
uniform mat3 normalMat;

void main()
{
    // 본 변환이 없으므로 단순히 정적인 위치만 사용
    vec4 totalPosition = vec4(pos, 1.0);

    // 모델의 회전, 스케일 변환을 적용
    mat3 normalMatrix = transpose(inverse(mat3(modelMat))) * normalMat;

    gl_Position = PVM * totalPosition;

    FragPos = (modelMat * totalPosition).xyz;
    TexCoord = tex;

    // 법선, 탄젠트, 비탄젠트 벡터 변환
    vec3 T = normalize(normalMatrix * tangent);
    vec3 B = normalize(normalMatrix * bitangent);
    vec3 N = normalize(normalMatrix * normal);

    TBN = mat3(T, B, N);
}