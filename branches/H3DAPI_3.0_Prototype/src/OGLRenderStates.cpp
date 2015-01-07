#include <H3D/OGLRenderStates.h>
#include <H3D/RenderProperties.h>

#include <H3D/X3DGeometryNode.h>
#include <H3D/Appearance.h>

#include <limits>

using namespace H3D;

/************************************************************************/
/*                   DepthState definitions                             */
/************************************************************************/

bool H3D::DepthState::operator!=(const DepthState& rhs)
{
	return !(*this == rhs);
}

bool H3D::DepthState::operator==(const DepthState& rhs)
{
	return (rhs.depthTestingEnabled	== depthTestingEnabled &&
		rhs.depthBufferWriteEnabled	== depthBufferWriteEnabled &&
		rhs.depthFunction == depthFunction);
}

H3D::DepthState::DepthState(RenderProperties* const rp)
{
	if(rp == NULL) {
		defaultInit();
	} else {
		rp->getDepthFunc(rp->depthFunc->getValue(), depthFunction);
		depthTestingEnabled = rp->depthTestEnabled->getValue(); 
		depthBufferWriteEnabled = rp->depthBufferWriteEnabled->getValue();
	}
}

H3D::DepthState::DepthState(bool _depthTestingEnabled /*= true*/, bool _depthBufferWriteEnabled /*= true*/, GLenum _depthFunction /*= GL_LESS*/) : depthTestingEnabled(_depthTestingEnabled), depthBufferWriteEnabled(_depthBufferWriteEnabled),
	depthFunction(_depthFunction)
{

}

void H3D::DepthState::defaultInit()
{
	depthTestingEnabled = true;
	depthBufferWriteEnabled = true;
	depthFunction = GL_LESS;
}

void H3D::DepthState::reset()
{
	defaultInit();
}

/************************************************************************/
/*                   BlendState definitions                             */
/************************************************************************/

bool H3D::BlendState::operator!=(const H3D::BlendState& rhs)
{
	return !(*this == rhs);
}

bool H3D::BlendState::operator==(const H3D::BlendState& rhs)
{
	return (rhs.blendingEnabled		== blendingEnabled &&
		rhs.srcFactorRGB			== srcFactorRGB &&
		rhs.dstFactorRGB			== dstFactorRGB &&
		rhs.blendEquationRGB		== blendEquationRGB &&
		rhs.alphaTestFunc			== alphaTestFunc &&
		rhs.srcFactorAlpha			== srcFactorAlpha &&
		rhs.blendEquationAlpha		== blendEquationAlpha &&
		(rhs.alphaFuncValue-alphaFuncValue) < std::numeric_limits<float>::epsilon()*10);
}

H3D::BlendState::BlendState(RenderProperties* const rp)
{
	if(rp == NULL) {
		defaultInit();
	} else {
		blendingEnabled = rp->blendEnabled->getValue();
		rp->getAlphaFunc(rp->alphaFunc->getValue(), alphaTestFunc);

		rp->getBlendEquation(rp->blendEquationRGB->getValue(), blendEquationRGB);
		rp->getBlendEquation(rp->blendEquationAlpha->getValue(), blendEquationAlpha);

		rp->getSFactor(rp->blendFuncSrcFactorRGB->getValue(), srcFactorRGB);
		rp->getDFactor(rp->blendFuncDstFactorRGB->getValue(), dstFactorRGB);
		rp->getSFactor(rp->blendFuncSrcFactorAlpha->getValue(), srcFactorAlpha);
		rp->getDFactor(rp->blendFuncDstFactorAlpha->getValue(), dstFactorAlpha);


		GLboolean redMask = rp->colorBufferRedWriteEnabled->getValue();
		GLboolean greenMask = rp->colorBufferGreenWriteEnabled->getValue();
		GLboolean blueMask = rp->colorBufferBlueWriteEnabled->getValue();
		GLboolean alphaMask = rp->colorBufferAlphaWriteEnabled->getValue();

		alphaFuncValue = rp->alphaFuncValue->getValue();

		//Clamp to [0.0f, 1.0f]
		if(alphaFuncValue > 1.0f) {
			alphaFuncValue = 1.0f;
		} else if(alphaFuncValue < 0.0f) {
			alphaFuncValue = 0.0f;
		}

		//Can't figure out member initialization syntax for arrays in pre-C++11 so I just do it here.
		blendColor[0] = 0.0f;
		blendColor[1] = 0.0f;
		blendColor[2] = 0.0f;
		blendColor[3] = 1.0f;

		colorMask[0] = redMask;
		colorMask[1] = greenMask;
		colorMask[2] = blueMask;
		colorMask[3] = alphaMask;
	}
}

H3D::BlendState::BlendState(bool _blendingEnabled /*= GL_TRUE*/, GLenum _alphaTestFunc /*= GL_ALWAYS*/, GLenum _srcFactorRGB /*= GL_SRC_ALPHA*/, GLenum _dstFactorRGB /*= GL_ONE_MINUS_SRC_ALPHA*/, GLenum _equationRGB /*= GL_FUNC_ADD*/, GLenum _srcFactorAlpha /*= GL_SRC_ALPHA*/, GLenum _dstFactorAlpha /*= GL_ONE_MINUS_SRC_ALPHA*/, GLenum _equationAlpha /*= GL_FUNC_ADD*/) : blendingEnabled(_blendingEnabled), alphaTestFunc(_alphaTestFunc), 
	srcFactorRGB(_srcFactorRGB), dstFactorRGB(_dstFactorRGB),
	blendEquationRGB(_equationRGB), srcFactorAlpha(_srcFactorAlpha),
	dstFactorAlpha(_dstFactorAlpha), blendEquationAlpha(_equationAlpha)
{
	alphaFuncValue = 0.0f;

	//Clamp to [0.0f, 1.0f]
	if(alphaFuncValue > 1.0f) {
		alphaFuncValue = 1.0f;
	} else if(alphaFuncValue < 0.0f) {
		alphaFuncValue = 0.0f;
	}

	//Can't figure out member initialization syntax for arrays in pre-C++11 so I just do it here.
	blendColor[0] = 0.0f;
	blendColor[1] = 0.0f;
	blendColor[2] = 0.0f;
	blendColor[3] = 1.0f;

	colorMask[0] = GL_TRUE;
	colorMask[1] = GL_TRUE;
	colorMask[2] = GL_TRUE;
	colorMask[3] = GL_TRUE;
}

void H3D::BlendState::defaultInit()
{
	blendingEnabled = GL_TRUE; 
	alphaTestFunc = GL_ALWAYS; 
	srcFactorRGB = GL_SRC_ALPHA;
	dstFactorRGB = GL_ONE_MINUS_SRC_ALPHA;
	blendEquationRGB = GL_FUNC_ADD; 
	srcFactorAlpha = GL_SRC_ALPHA;
	dstFactorAlpha = GL_ONE_MINUS_SRC_ALPHA;
	blendEquationAlpha = GL_FUNC_ADD;

	alphaFuncValue = 0.0f;

	blendColor[0] = 0.0f;
	blendColor[1] = 0.0f;
	blendColor[2] = 0.0f;
	blendColor[3] = 1.0f;

	colorMask[0] = GL_TRUE;
	colorMask[1] = GL_TRUE;
	colorMask[2] = GL_TRUE;
	colorMask[3] = GL_TRUE;
}

void H3D::BlendState::reset()
{
	defaultInit();
}

/************************************************************************/
/*                 RasterizerState definitions                          */
/************************************************************************/


bool H3D::RasterizerState::operator!=(const RasterizerState& rhs)
{
	return !(*this == rhs);
}

bool H3D::RasterizerState::operator==(const RasterizerState& rhs)
{
	return (rhs.faceCullingEnabled	== faceCullingEnabled &&
		rhs.scissorTestEnabled	== scissorTestEnabled &&
		rhs.cullFace == cullFace &&
		rhs.windingOrder == windingOrder);
}

H3D::RasterizerState::RasterizerState(bool _faceCullingEnabled /*= true*/, bool _scissorTestEnabled /*= false*/, 
	GLenum _cullFace /*= GL_FRONT*/, GLenum _windingOrder /*= GL_CCW*/) : faceCullingEnabled(_faceCullingEnabled), scissorTestEnabled(_scissorTestEnabled), 
	cullFace(_cullFace), windingOrder(_windingOrder)
{

}

void H3D::RasterizerState::defaultInit()
{
	faceCullingEnabled = true; 
	scissorTestEnabled = false;
	cullFace = GL_FRONT; 
	windingOrder = GL_CCW;
}

void H3D::RasterizerState::reset()
{
	defaultInit();
}

/************************************************************************/
/*                    Shadowstate definitions                           */
/************************************************************************/

H3D::ShadowState::ShadowState(bool useDefaultShadows /*= true*/, float darkness /*= 0.4f*/, 
	float depthOffset /*= 6.0f*/, std::string geometryAlgorithm /*= "GEOMETRY_SHADER"*/) : useDefaultShadows(useDefaultShadows), defaultShadowDarkness(darkness), 
	defaultShadowDepthOffset(depthOffset), defaultShadowGeometryAlgorithm(geometryAlgorithm)
{

}

void H3D::ShadowState::reset()
{
	useDefaultShadows = true;
	defaultShadowDarkness = 0.4f;
	defaultShadowDepthOffset = 6.0f;
	defaultShadowGeometryAlgorithm = "GEOMETRY_SHADER";
}

/************************************************************************/
/*                 ShaderData definitions                              */
/************************************************************************/

H3D::ShaderData::ShaderData()
: programID(0), uniforms(0)
{
	
}

/************************************************************************/
/*                    TotalRenderState definitions                      */
/************************************************************************/
H3D::TotalRenderState::TotalRenderState()
{
	//Default init everything.
	reset();
}


void H3D::TotalRenderState::reset()
{
	depthState.reset();
	blendState.reset();
	rasterizerState.reset();
	vertexLayoutDescription = 0;
	shader.programID = 0;
	shader.uniforms.resize(0);
}

bool H3D::TotalRenderState::operator==(TotalRenderState& rhs)
{
	return (	rhs.vertexLayoutDescription == vertexLayoutDescription &&
				rhs.rasterizerState			== rasterizerState &&
				rhs.depthState				== depthState &&
				rhs.blendState				== blendState &&
				rhs.shader.programID		== shader.programID
		);
}

bool H3D::TotalRenderState::operator!=(TotalRenderState& rhs)
{
	return !(*this == rhs);
}

void H3D::TotalRenderState::ApplyChanges(Appearance* const appearance)
{
	RenderProperties* rp = appearance->renderProperties->getValue();

	if(rp)
	{
		/************************************************************************/
		/* BlendState assignment                                                */
		/************************************************************************/

		blendState.blendingEnabled = rp->blendEnabled->getValue();
		rp->getAlphaFunc(rp->alphaFunc->getValue(), blendState.alphaTestFunc);

		rp->getBlendEquation(rp->blendEquationRGB->getValue(), blendState.blendEquationRGB);
		rp->getBlendEquation(rp->blendEquationAlpha->getValue(), blendState.blendEquationAlpha);

		rp->getSFactor(rp->blendFuncSrcFactorRGB->getValue(), blendState.srcFactorRGB);
		rp->getDFactor(rp->blendFuncDstFactorRGB->getValue(), blendState.dstFactorRGB);
		rp->getSFactor(rp->blendFuncSrcFactorAlpha->getValue(), blendState.srcFactorAlpha);
		rp->getDFactor(rp->blendFuncDstFactorAlpha->getValue(), blendState.dstFactorAlpha);


		GLboolean redMask = rp->colorBufferRedWriteEnabled->getValue();
		GLboolean greenMask = rp->colorBufferGreenWriteEnabled->getValue();
		GLboolean blueMask = rp->colorBufferBlueWriteEnabled->getValue();
		GLboolean alphaMask = rp->colorBufferAlphaWriteEnabled->getValue();

		blendState.alphaFuncValue = rp->alphaFuncValue->getValue();

		//Clamp to [0.0f, 1.0f]
		if(blendState.alphaFuncValue > 1.0f) {
			blendState.alphaFuncValue = 1.0f;
		} else if(blendState.alphaFuncValue < 0.0f) {
			blendState.alphaFuncValue = 0.0f;
		}

		blendState.colorMask[0] = redMask;
		blendState.colorMask[1] = greenMask;
		blendState.colorMask[2] = blueMask;
		blendState.colorMask[3] = alphaMask;

		/************************************************************************/
		/* DepthState assignment                                                */
		/************************************************************************/
		rp->getDepthFunc(rp->depthFunc->getValue(), depthState.depthFunction);
		depthState.depthTestingEnabled = rp->depthTestEnabled->getValue(); 
		depthState.depthBufferWriteEnabled = rp->depthBufferWriteEnabled->getValue();
	}
}

void H3D::TotalRenderState::ApplyChanges(X3DGeometryNode* const geometryNode)
{
	if(geometryNode)
	{
		rasterizerState.faceCullingEnabled = geometryNode->usingCulling();
		rasterizerState.cullFace = geometryNode->getCullFace();
	}
}

H3D::UniformDescription::UniformDescription(UniformType _type /*= fv1*/, std::string _uniformName /*= ""*/, GLint _location /*= 0*/, 
	GLsizei _count /*= 0*/, GLboolean _normalize /*= GL_FALSE*/, GLvoid* _ptr /*= 0*/) 
: type(_type), name(_uniformName), location(_location), count(_count), normalize(_normalize), ptr(_ptr)
{

}
