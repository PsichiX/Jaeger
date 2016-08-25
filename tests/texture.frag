uniform sampler2D sBase;
varying vec2 vCoord;

void main()
{
	vec4 col = texture2D(sBase, vCoord);
	gl_FragColor = gl_Color * col;
}
