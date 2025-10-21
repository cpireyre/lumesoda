uniform float time;
uniform vec2  player;
uniform vec2  resolution;
void main()
{
    vec3 col;
    float normalizedPosition = gl_FragCoord.x / player.x;

    col.r = 0.;
    col.g = 0.;
    col.b = 0.;

    // Output to screen
    gl_FragColor = vec4(col,1.0);
}
