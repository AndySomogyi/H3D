#ifndef __OGLRENDERSTATES_H__
#define	__OGLRENDERSTATES_H__

#include <string>
#include <vector>
#include <set>
#include <memory>
#include <H3D/OGLStateChangeValues.h>

namespace H3D {
	// Forward declarations...
	class RenderProperties;
	class X3DGeometryNode;
	class Appearance;
	class RenderCommandBuffer;

	//////////////////////////////////////////////////////////////////////////

	struct DepthState {
	private:
		void defaultInit();

	public:
		DepthState();
		DepthState(bool _depthTestingEnabled, bool _depthBufferWriteEnabled, GLenum _depthFunction);
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
		BlendState();

		//Yes, writing this did in fact hurt my soul.
		BlendState(bool _blendingEnabled, GLenum _alphaTestFunc, 
			GLenum _srcFactorRGB, GLenum _dstFactorRGB, 
			GLenum	_equationRGB, GLenum	_srcFactorAlpha, 
			GLenum	_dstFactorAlpha, GLenum _equationAlpha);

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
		OGLBlendColor	blendColor; //0=R, 1=G, 2=B, 3=A
		OGLColorMask	colorMask; //0=R, 1=G, 2=B, 3=A
	};

	//////////////////////////////////////////////////////////////////////////

	struct RasterizerState {
	private:
		void defaultInit();

	public:
		RasterizerState();

		RasterizerState(bool _faceCullingEnabled, bool _scissorTestEnabled, 
			GLenum _cullFace, GLenum _windingOrder);

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
		const bool operator==(const UniformDescription& rhs) const;

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
	public:
		//Enumerations of each kind of state change type we could have.
		//We use these to identify what type of changes have occurred, and act according to these types.
		enum StateChangeType
		{
			//Depth state
			DepthTestingEnable = 0, //Probably not necessary, but just doing it for clarity and as a sanity check
			DepthBufferWriteEnable,
			DepthFunctionChange,
			//Blend state
			BlendingEnabled,
			BlendFunctionChange,
			BlendEquationChange,
			AlphaFunctionChange,
			ColorMaskChange,
			BlendColorChange,
			//Rasterizer state
			FaceCullingEnable,
			CullFaceChanged,
			WindingOrderChange,
			ScissorTestEnable,
			//Shader state
			ShaderChange,
			STATE_CHANGE_TYPE_COUNT //Always keep this last! That way we have an automatic counter for how many enum definitions are contained within this list.
		};

		/*union StateChangeValue
		{
		struct BlendFactorBundle
		{
		GLenum srcRGB, srcAlpha, dstRGB, dstAlpha;
		} blendFactorBundle;

		struct BlendEquationBundle{
		GLenum rgbEquation;
		GLenum alphaEquation;
		} blendEquationBundle;

		struct AlphaFuncBundle
		{
		GLenum alphaTestFunc;
		GLclampf testValue;
		} alphaFuncBundle;

		GLenum enumVal;
		GLboolean colorMaskVal[4];
		GLclampf blendColorVal[4];
		GLclampf clampVal;
		bool boolVal;
		unsigned int uintVal;
		int	intVal;
		};*/

	public: 
		TotalRenderState();

		//Set all values to default
		void reset();

		void applyChanges(Appearance* const appearance);
		void applyChanges(X3DGeometryNode* const geometryNode);

		bool operator==(TotalRenderState& rhs);
		bool operator!=(TotalRenderState& rhs);

		/*
		Right so I could use some form of Dictionary with GLenums and a union for each.. value. That'd use up an immense amount of memory once I start getting many different instances of totalrenderstate.
		Problem with this entire thing is that I don't use the glSet and stuff in traverseSG because I queue it all up in totalRenderState right. 
		So I need to do my own thing with storing every single value? Or at least the ones that deviate...?


		//GL_INVALID_ENUM is generated if pname is not an accepted value.
		//GL_INVALID_VALUE is generated on any of glGetBooleani_v, glGetIntegeri_v, or glGetInteger64i_v if index is outside of the valid range for the indexed state target. 
		*/

		/* Generic get&set functions to expose OGL functionality */
		GLboolean getGLBoolean(GLenum target, GLuint index = 0);
		//void setGLBoolean(GLenum target, GLboolean val, GLuint index = 0);

		GLint getGLInteger(GLenum target, GLuint index = 0);
		//void setGLInteger(GLenum target, GLint val, GLuint index = 0);

		GLint64 getGLInteger64(GLenum target, GLuint index = 0);
		//void setGLInteger64(GLenum target, GLint64 val, GLuint index = 0);

		GLfloat getGLFloat(GLenum target, GLuint index = 0);
		//void setGLFloat(GLenum target, GLfloat val, GLuint index = 0);

		GLdouble getGLDouble(GLenum target, GLuint index = 0);
		//void setGLDouble(GLenum target, GLdouble val, GLuint index = 0);

		H3DUtil::Vec2f getGLFloat2(GLenum target);
		//void setGLFloat2(GLenum target, Vec2f val);

		H3DUtil::Vec3f getGLFloat3(GLenum target);
		//void setGLFloat3(GLenum target, Vec3f val);

		H3DUtil::Vec4f getGLFloat4(GLenum target);
		//void setGLFloat4(GLenum target, Vec4f val);

		/* Get and set for DepthState */
		DepthState&	getDepthState();
		void		setDepthState(DepthState val);

		bool		getDepthTestingEnabled();
		void		setDepthTestingEnabled(bool val);

		bool		getDepthBufferWriteEnabled();
		void		setDepthBufferWriteEnabled(bool val);

		GLenum		getDepthFunction();
		void		setDepthFunction(GLenum val);

		/* Get and set for BlendState */
		BlendState&		getBlendState();
		void			setBlendState(BlendState val);

		bool			getBlendingEnabled();
		void			setBlendingEnabled(bool val);

		GLenum			getSrcFactorRGB();
		void			setSrcFactorRGB(GLenum val);

		GLenum			getDstFactorRGB();		
		void			setDstFactorRGB(GLenum val);

		GLenum			getBlendEquationRGB();
		void			setBlendEquationRGB(GLenum val);

		GLenum			getAlphaTestFunc();
		void			setAlphaTestFunc(GLenum val);

		GLenum			getSrcFactorAlpha();
		void			setSrcFactorAlpha(GLenum val);

		GLenum			getDstFactorAlpha();
		void			setDstFactorAlpha(GLenum val);

		GLenum			getBlendEquationAlpha();		
		void			setBlendEquationAlpha(GLenum val);

		GLclampf		getAlphaFuncValue();
		void			setAlphaFuncValue(GLclampf val);

		OGLBlendColor	getBlendColor();
		void			setBlendColor(OGLBlendColor val);

		OGLColorMask	getColorMask();
		void			setColorMask(OGLColorMask val);

		/* Get and set for RasterizerState */
		RasterizerState&	getRasterizerState();
		void				setRasterizerState(RasterizerState val);

		bool			getFaceCullingEnabled();
		void			setFaceCullingEnabled(bool val);

		bool			getScissorTestEnabled();
		void			setScissorTestEnabled(bool val);

		GLenum			getCullFace();
		void			setCullFace(GLenum val);

		GLenum			getWindingOrder();
		void			setWindingOrder(GLenum val);

		/* Get and set for Shader */
		ShaderData&		getShaderData();
		void			setShaderData(ShaderData val);

		/* Get and set for Vertex Layout Description */
		unsigned int	getVertexLayoutDescription();
		void			setVertexLayoutDescription(unsigned int val);

		//It's in here that we actually process the state changes
		void InsertRenderCommands(RenderCommandBuffer* buffer, TotalRenderState* previousRenderState);

	private:
		typedef std::pair<GLenum, auto_ptr<StateChangeValue>> StateKeyValuePair;

		struct KeyValPairComparator {
			bool operator() (const StateKeyValuePair& lhs, const StateKeyValuePair& rhs) const{
				//Just compare their key value.
				return lhs.first < rhs.first;
			}
		};

		// Set of different state changes. Each value in a set is unique, so we don't have to worry about duplicates.
		//std::set<StateKeyValuePair, KeyValPairComparator> stateChanges;
		std::set<StateChangeType> stateChanges;

		DepthState depthState;
		BlendState blendState;
		RasterizerState rasterizerState;
		//ShadowState shadowState;

		//Vertex layout handle. Actual vertex layout stored in Renderer
		unsigned int vertexLayoutDescription;

		//Current shader.
		ShaderData shader;
	};
}

#endif