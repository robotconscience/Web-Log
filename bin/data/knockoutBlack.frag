#version 120

uniform sampler2DRect	tex;

void main(){
	//this is the fragment shader
	//this is where the pixel level drawing happens

	vec2 v2 = gl_TexCoord[0].st;	

	//gl_FragCoord gives us the x and y of the current pixel its drawing
	
	//we grab the x and y and store them in an int
	float xVal = gl_FragCoord.x;
	float yVal = gl_FragCoord.y;
	vec4 col = gl_Color;
	vec4 texColor = texture2DRect(tex, v2);
	float dist = distance(gl_Color, texColor);

	if( dist < .5 ){
		gl_FragColor = texColor * gl_Color;    
    }else{
		gl_FragColor.rgb = texture2DRect(tex, v2).rgb * gl_Color.rgb;    
		gl_FragColor.a = 0.0;//dist * .1;
	}
	
}