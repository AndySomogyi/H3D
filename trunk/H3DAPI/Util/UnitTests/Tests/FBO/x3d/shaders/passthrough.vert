uniform sampler2D tex_sampler;
void main() {
      gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
      gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
      gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}