uniform sampler2D texture;
uniform int nr_samples;
void main() {
      gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
      gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
      gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}