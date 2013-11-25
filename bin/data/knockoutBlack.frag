#version 120

uniform sampler2DRect	tex1;
uniform sampler2DRect	tex2;
uniform float minDist;
uniform float time;

void main(){
	//this is the fragment shader
	//this is where the pixel level drawing happens

	vec2 v2 = gl_TexCoord[0].st;	

	//gl_FragCoord gives us the x and y of the current pixel its drawing
	
	//we grab the x and y and store them in an int
	vec4 col = gl_Color;
	vec4 texColor = texture2DRect(tex1, v2);
    vec4 texColor2 = texture2DRect(tex2, v2);
    vec4 mixed      = mix( texColor2, texColor, time);
	float dist = distance(gl_Color, texColor);
    
    gl_FragColor = mixed * gl_Color;
    
    if( dist <= minDist ){
		gl_FragColor = mixed * gl_Color;
    }else{
		gl_FragColor.rgb = mixed.rgb * gl_Color.rgb;
		gl_FragColor.a = 0.0;//dist * .1;
	}
	
}