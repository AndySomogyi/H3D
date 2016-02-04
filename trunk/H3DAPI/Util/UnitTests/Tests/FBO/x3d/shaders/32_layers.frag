#version 150 compatibility
#extension GL_ARB_fragment_layer_viewport : enable

void main(void ) {
  if(gl_Layer < 10) {
	float col_val = 0.1 + (gl_Layer / 10.0);
  	gl_FragColor = vec4(col_val,0,0,1);
  } else if(gl_Layer < 20) {
  	float col_val = 0.1 + ((gl_Layer-10) / 10.0);
    gl_FragColor = vec4(0,col_val,0,1);
  } else if(gl_Layer < 30) {
  	float col_val = 0.1 + ((gl_Layer-20) / 10.0);
    gl_FragColor = vec4(0,0,col_val,1);
  } else {
	float col_val = 0.1 + ((gl_Layer-30) / 10.0);
	gl_FragColor = vec4(0,col_val,col_val,1);
  }
}