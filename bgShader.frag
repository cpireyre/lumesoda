uniform float time;
uniform vec2  player;
uniform vec2  resolution;
void main()
{
    vec3 col;
    float normalizedPosition = gl_FragCoord.x / player.x;

    col.r = cos(time);
    col.g = 1. - normalizedPosition;
    col.b = normalizedPosition;

    // Output to screen
    gl_FragColor = vec4(col,1.0);
}
