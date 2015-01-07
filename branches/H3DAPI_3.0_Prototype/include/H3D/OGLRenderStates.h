#ifndef __OGLRENDERSTATES_H__
#define	__OGLRENDERSTATES_H__

#include <H3D/VertexAttributeDescription.h>
#include <string>
#include <vector>

namespace H3D 
{
	// Forward declarations...
	class RenderProperties;
	class X3DGeometryNode;
	class Appearance;

	//////////////////////////////////////////////////////////////////////////

	struct DepthState {
	private:
		void defaultInit();

	public:
		DepthState(bool _depthTestingEnabled = true, bool _depthBufferWriteEnabled = true, GLenum _depthFunction = GL_LESS);
		DepthState(RenderProperties* const rp);

		bool operator==(const DepthState& rhs);
		bool operator!=(const DepthState& rhs);

		void reset();

		bool depthTestingEnabled;
		bool depthBufferWriteEnabled;
		GLenum depthFunction;
	};

	//////////////////////////////////////////////////////////////////////////

	struct BlendState {
	private:
		void defaultInit();

	public:

		//Yes, writing this did in fact hurt my soul.
		BlendState(bool _blendingEnabled = GL_TRUE, GLenum _alphaTestFunc = GL_ALWAYS, 
			GLenum _srcFactorRGB = GL_SRC_ALPHA, GLenum _dstFactorRGB = GL_ONE_MINUS_SRC_ALPHA, 
			GLenum	_equationRGB = GL_FUNC_ADD, GLenum	_srcFactorAlpha = GL_SRC_ALPHA, 
			GLenum	_dstFactorAlpha = GL_ONE_MINUS_SRC_ALPHA, GLenum _equationAlpha = GL_FUNC_ADD);

		//Much easier
		BlendState(RenderProperties* const rp);
		bool operator==(const BlendState& rhs);
		bool operator!=(const BlendState& rhs);

		void reset();

		bool	blendingEnabled;
		GLenum	srcFactorRGB;
		GLenum  dstFactorRGB;
		GLenum	blendEquationRGB;

		GLenum	alphaTestFunc;
		GLenum	srcFactorAlpha;
		GLenum	dstFactorAlpha;
		GLenum	blendEquationAlpha;

		GLclampf	alphaFuncValue;
		GLclampf	blendColor[4]; //0=R, 1=G, 2=B, 3=A
		GLboolean	colorMask[4]; //0=R, 1=G, 2=B, 3=A
	};

	//////////////////////////////////////////////////////////////////////////

	struct RasterizerState {
	private:
		void defaultInit();

	public:
		RasterizerState(bool _faceCullingEnabled = true, bool _scissorTestEnabled = false, 
			GLenum _cullFace = GL_FRONT, GLenum _windingOrder = GL_CCW);

		bool operator==(const RasterizerState& rhs);
		bool operator!=(const RasterizerState& rhs);

		void reset();

		bool faceCullingEnabled;
		bool scissorTestEnabled;
		GLenum cullFace; //GL_FRONT or GL_BACK
		GLenum windingOrder; //GL_CW or GL_CCW
		//Potentially keep stuff like stippling and fill patterns here...?
	};

	//////////////////////////////////////////////////////////////////////////

	struct ShadowState {
		ShadowState(bool useDefaultShadows = true, float darkness = 0.4f, 
			float depthOffset = 6.0f, std::string geometryAlgorithm = "GEOMETRY_SHADER");

		void reset();

		bool useDefaultShadows;
		float defaultShadowDarkness;
		float defaultShadowDepthOffset;
		std::string defaultShadowGeometryAlgorithm;
	};

	//////////////////////////////////////////////////////////////////////////

	struct UniformDescription
	{
		enum UniformType {
			f1 = 0,
			i1,
			fv1,
			fv2,
			fv3,
			fv4,
			iv1,
			iv2,
			iv3,
			iv4,
			fmat3x3,
			fmat4x4,
			dmat3x3,
			dmat4x4
		};

		UniformDescription(UniformType _type = fv1, std::string _uniformName = "", GLint _location = 0, GLsizei _count = 0, GLboolean _normalize = GL_FALSE, GLvoid* _ptr = 0);

		UniformType type;
		std::string name;
		GLint location;
		GLsizei count;
		GLboolean normalize;
		GLvoid* ptr;
	};

	//////////////////////////////////////////////////////////////////////////

	struct ShaderData
	{
		ShaderData();

		GLuint programID;
		std::vector<UniformDescription> uniforms;
	};

	//////////////////////////////////////////////////////////////////////////

	//Complete render state setup.
	//They're all auto_ptrs because they can be null. If they're null, 
	//it means they're completely default and we don't need to change the state.
	struct TotalRenderState {
		DepthState depthState;
		BlendState blendState;
		RasterizerState rasterizerState;
		//ShadowState shadowState;

		//Vertex layout handle. Actual vertex layout stored in Renderer
		unsigned int vertexLayoutDescription;

		//Current shader.
		ShaderData shader;

		//Material.
		 
		TotalRenderState();
		
		void ApplyChanges(Appearance* const appearance);
		void ApplyChanges(X3DGeometryNode* const geometryNode);

		bool operator==(TotalRenderState& rhs);
		bool operator!=(TotalRenderState& rhs);

		//Set all values to default
		void reset();
	};

	//////////////////////////////////////////////////////////////////////////


}

#endif