#version 330 core

void main()
{
    /*if (gl_FragCoord.x < -1
        || gl_FragCoord.y < -1
        || gl_FragCoord.x > 1
        || gl_FragCoord.y > 1)
        discard;*/
    /*if (gl_FragCoord.x < 0
        || gl_FragCoord.x > 780)
        discard;*/
    gl_FragDepth = gl_FragCoord.z;
}