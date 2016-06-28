// frag shader
#version 430

out vec4 finalColor; //color fragment output

// vertex color
uniform vec4 vertexColor;
// texture stuff
in vec2 gTexCoord; //uv texture coordinates
uniform sampler2D tex; //the texture sampler
// lighting stuff
in vec3 v_normal;
in vec3 FragPos;
uniform vec4 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main()
{
	// ambient lighting
	float ambientStrength = 0.1f;
	vec4 ambient = ambientStrength * vec4(lightColor, 1.0f);
	
	// diffuse lighting calculations
	vec3 norm = normalize(v_normal);
	vec3 lightDir = normalize(vec3(lightPos) - FragPos);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = diff * vec4(lightColor, 1.0f);

	// specular highlights
	float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	int shininess = 32;
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = specularStrength * spec * lightColor;
	
	// final calculations
	//vec4 colorCalc = (ambient + diffuse + specular) * vertexColor;
	vec4 lightCalc = (ambient + diffuse + vec4(specular, 0.0f));
	//vec4 colorCalc = ambient * vertexColor;
	finalColor = texture(tex, gTexCoord) * lightCalc;
	//color = vertexColor;
}