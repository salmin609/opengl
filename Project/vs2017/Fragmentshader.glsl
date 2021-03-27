#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 normal_vec;
in vec3 frag_pos;
in vec4 light;

out vec4 frag_color;

uniform vec3 color_val;
uniform vec3 view_pos;
uniform Light light_;
uniform Material material;


vec3 Get_Color_Foamshading()
{
    vec3 ambient = light_.ambient * material.ambient;

    vec3 norm = normalize(normal_vec);
    vec3 lightDir = normalize(vec3(light.x, light.y, light.z) - frag_pos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse_color = (diff * material.diffuse) * light_.diffuse;

    vec3 view_dir = normalize(view_pos - frag_pos);
    vec3 reflect_dir = reflect(-lightDir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular = (material.specular * spec) * light_.specular;

    vec3 result = (ambient + diffuse_color + specular);
    return result;
}

void main()
{
    vec3 result = Get_Color_Foamshading();
    frag_color = vec4(result , 1.0);
};