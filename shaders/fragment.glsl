#version 330 core

out vec4 FragColor;


in vec2 TexCoord;
in vec2 TexOffset;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D texture1;

void main()
{
    vec2 coord = (TexCoord - floor(TexCoord) + TexOffset)/vec2(27.0f, -27.0f);
    // ambient
    vec4 tex = texture(texture1, coord);
    if (tex.a < 0.1)
    {
        discard; // yes: discard this fragment
    }
    vec3 ambient = vec3(0.9, 0.9, 0.9) * tex.rgb;
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(0.1, 0.1, 0.1) * diff;  
    
    // attenuation
    float distance    = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.045 * distance + 0.0075 * (distance * distance));    

    //ambient  *= attenuation;  
    //diffuse *= attenuation;
    vec3 result = ambient  + diffuse;
    FragColor = vec4(result, 1.0);
} 