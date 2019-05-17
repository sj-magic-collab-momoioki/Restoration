#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DRect Org;
uniform sampler2DRect mask_0;
uniform sampler2DRect mask_1;
uniform sampler2DRect mask_2;

void main(){
	/********************
	********************/
	vec2 tex_pos = gl_TexCoord[0].xy;
	
	vec4 color0 = texture2DRect(mask_0, tex_pos);
	vec4 color1 = texture2DRect(mask_1, tex_pos);
	vec4 color2 = texture2DRect(mask_2, tex_pos);
	
	float alpha = max(max(color0.a, color1.a), color2.a);
	
	/********************
	********************/
	vec4 color = texture2DRect(Org, tex_pos);
	color.a = alpha;
	
	/********************
	********************/
	gl_FragColor = color;
}
