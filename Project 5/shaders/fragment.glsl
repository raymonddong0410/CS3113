#version 330


const float LINEAR_TERM    = 0.00035; 
const float QUADRATIC_TERM = 0.00015; 

const float MIN_BRIGHTNESS = 0.03;

uniform sampler2D texture0;
uniform vec2 lightPosition;

in vec2 fragTexCoord;
in vec2 fragPosition;

out vec4 finalColor;

float attenuate(float distance, float linearTerm, float quadraticTerm)
{
    float attenuation = 1.0 / (1.0 + 
                               linearTerm * distance + 
                               quadraticTerm * distance * distance);
                               
    return max(attenuation, MIN_BRIGHTNESS);
}

void main()
{
    float dist = distance(lightPosition, fragPosition);
    float brightness = attenuate(dist, LINEAR_TERM, QUADRATIC_TERM);
    vec4 color = texture(texture0, fragTexCoord);
    finalColor = vec4(color.rgb * brightness, color.a);
}