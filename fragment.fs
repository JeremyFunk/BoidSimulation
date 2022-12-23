#version 330 core
out vec4 fragColor;

flat in int predator;

//in vec2 TexCoord;

// texture samplers
//uniform sampler2D texture1;
//uniform sampler2D texture2;

void main()
{
    // linearly interpolate between both textures (80% container, 20% awesomeface)
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
    if(predator == 1){
        fragColor = vec4(1.0,0.2,0.2,1.0);
    }else{
        fragColor = vec4(1.0);
    }
}