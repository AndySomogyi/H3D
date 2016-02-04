uniform sampler2D tex_sampler;

void main(void)
{
    vec4 color = texture2D(tex_sampler, gl_TexCoord[0].st);
    gl_FragColor = color;
}

