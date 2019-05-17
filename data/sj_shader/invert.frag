#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DRect Org;
uniform sampler2DRect mask;

void main(){
	/********************
	********************/
	vec2 tex_pos = gl_TexCoord[0].xy;
	
	vec4 color = texture2DRect(Org, tex_pos);
	vec4 color_mask = texture2DRect(mask, tex_pos);

	if(0 < color_mask.a){
		color.r = 1.0 - color.r;
		color.g = 1.0 - color.g;
		color.b = 1.0 - color.b;
	}
	
	/********************
	********************/
	gl_FragColor = color;
}
