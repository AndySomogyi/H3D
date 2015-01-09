#include <H3D/OGLRenderStates.h>
#include <H3D/RenderProperties.h>

#include <H3D/X3DGeometryNode.h>
#include <H3D/Appearance.h>

#include <H3D/RenderCommandBuffer.h>

#include <limits> //for std::numeric_limits::epsilon that I use when comparing two floats

using namespace H3D;

/************************************************************************/
/*                   DepthState definitions                             */
/************************************************************************/

H3D::DepthState::DepthState()
{
	defaultInit();
}

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
/*					OGLBlendColor definitions	                        */
/************************************************************************/

H3D::OGLBlendColor::OGLBlendColor() {
	color[0] = 0.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;
	color[3] = 1.0f;
}

H3D::OGLBlendColor::OGLBlendColor(GLclampf redBlend, GLclampf greenBlend, GLclampf blueBlend, GLclampf alphaBlend) {
	color[0] = redBlend;
	color[1] = greenBlend;
	color[2] = blueBlend;
	color[3] = alphaBlend;
}

bool H3D::OGLBlendColor::operator==(const OGLBlendColor& rhs) {
	return (rhs.color[0] == color[0] &&
		rhs.color[1] ==	color[1] && 
		rhs.color[2] ==	color[2] && 
		rhs.color[3] ==	color[3] );
}

/************************************************************************/
/*					OGLColorMask definitions	                        */
/************************************************************************/

H3D::OGLColorMask::OGLColorMask() {
	mask[0]	= true;
	mask[1]	= true;
	mask[2]	= true;
	mask[3]	= true;
}

H3D::OGLColorMask::OGLColorMask(bool redMask, bool greenMask, bool blueMask, bool alphaMask) {
	mask[0] = redMask;
	mask[1] = greenMask;
	mask[2] = blueMask;
	mask[3] = alphaMask;
}

bool H3D::OGLColorMask::operator==(const OGLColorMask& rhs) {
	return (mask[0] ==	rhs.mask[0] &&
		mask[1] ==	rhs.mask[1] && 
		mask[2] ==	rhs.mask[2] && 
		mask[3] ==	rhs.mask[3] );
}

/************************************************************************/
/*                   BlendState definitions                             */
/************************************************************************/

H3D::BlendState::BlendState()
{
	defaultInit();
}

H3D::BlendState::BlendState(RenderProperties* const rp) {
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

		blendColor.color[0] = 0.0f;
		blendColor.color[1] = 0.0f;
		blendColor.color[2] = 0.0f;
		blendColor.color[3] = 1.0f;

		colorMask.mask[0] = redMask;
		colorMask.mask[1] = greenMask;
		colorMask.mask[2] = blueMask;
		colorMask.mask[3] = alphaMask;
	}
}

H3D::BlendState::BlendState(bool _blendingEnabled /*= GL_TRUE*/, GLenum _alphaTestFunc /*= GL_ALWAYS*/, GLenum _srcFactorRGB /*= GL_SRC_ALPHA*/, GLenum _dstFactorRGB /*= GL_ONE_MINUS_SRC_ALPHA*/, GLenum _equationRGB /*= GL_FUNC_ADD*/, GLenum _srcFactorAlpha /*= GL_SRC_ALPHA*/, GLenum _dstFactorAlpha /*= GL_ONE_MINUS_SRC_ALPHA*/, GLenum _equationAlpha /*= GL_FUNC_ADD*/) : blendingEnabled(_blendingEnabled), alphaTestFunc(_alphaTestFunc), 
	srcFactorRGB(_srcFactorRGB), dstFactorRGB(_dstFactorRGB),
	blendEquationRGB(_equationRGB), srcFactorAlpha(_srcFactorAlpha),
	dstFactorAlpha(_dstFactorAlpha), blendEquationAlpha(_equationAlpha),
	blendColor(0.0f, 0.0f, 0.0f, 1.0f), colorMask(true, true, true, true) {
		alphaFuncValue = 0.0f;

		//Clamp to [0.0f, 1.0f]
		if(alphaFuncValue > 1.0f) {
			alphaFuncValue = 1.0f;
		} else if(alphaFuncValue < 0.0f) {
			alphaFuncValue = 0.0f;
		}
}

bool H3D::BlendState::operator!=(const H3D::BlendState& rhs) {
	return !(*this == rhs);
}

bool H3D::BlendState::operator==(const H3D::BlendState& rhs) {
	return (rhs.blendingEnabled		== blendingEnabled &&
		rhs.srcFactorRGB			== srcFactorRGB &&
		rhs.dstFactorRGB			== dstFactorRGB &&
		rhs.blendEquationRGB		== blendEquationRGB &&
		rhs.alphaTestFunc			== alphaTestFunc &&
		colorMask					== rhs.colorMask &&
		blendColor					== rhs.blendColor &&
		rhs.srcFactorAlpha			== srcFactorAlpha &&
		rhs.blendEquationAlpha		== blendEquationAlpha &&
		(rhs.alphaFuncValue-alphaFuncValue) < std::numeric_limits<float>::epsilon()*10);
}

void H3D::BlendState::defaultInit() {
	blendingEnabled = GL_TRUE; 
	alphaTestFunc = GL_ALWAYS; 
	srcFactorRGB = GL_SRC_ALPHA;
	dstFactorRGB = GL_ONE_MINUS_SRC_ALPHA;
	blendEquationRGB = GL_FUNC_ADD; 
	srcFactorAlpha = GL_SRC_ALPHA;
	dstFactorAlpha = GL_ONE_MINUS_SRC_ALPHA;
	blendEquationAlpha = GL_FUNC_ADD;

	alphaFuncValue = 0.0f;

	blendColor.color[0] = 0.0f;
	blendColor.color[1] = 0.0f;
	blendColor.color[2] = 0.0f;
	blendColor.color[3] = 1.0f;

	colorMask.mask[0] = GL_TRUE;
	colorMask.mask[1] = GL_TRUE;
	colorMask.mask[2] = GL_TRUE;
	colorMask.mask[3] = GL_TRUE;
}

void H3D::BlendState::reset()
{
	defaultInit();
}

/************************************************************************/
/*                 RasterizerState definitions                          */
/************************************************************************/

H3D::RasterizerState::RasterizerState()
{
	defaultInit();
}

H3D::RasterizerState::RasterizerState(bool _faceCullingEnabled /*= true*/, bool _scissorTestEnabled /*= false*/, 
	GLenum _cullFace /*= GL_FRONT*/, GLenum _windingOrder /*= GL_CCW*/) : faceCullingEnabled(_faceCullingEnabled), scissorTestEnabled(_scissorTestEnabled), 
	cullFace(_cullFace), windingOrder(_windingOrder)
{

}

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
/*		UniformDescription definitions                                  */
/************************************************************************/

H3D::UniformDescription::UniformDescription(UniformType _type /*= fv1*/, std::string _uniformName /*= ""*/, GLint _location /*= 0*/, 
	GLsizei _count /*= 0*/, GLboolean _normalize /*= GL_FALSE*/, GLvoid* _ptr /*= 0*/) 
	: type(_type), name(_uniformName), location(_location), count(_count), normalize(_normalize), ptr(_ptr)
{}

const bool H3D::UniformDescription::operator==(const UniformDescription& rhs) const
{
	//Not doing any comparison on the void* ptr... which btw can be a potential leak...
	return (count == rhs.count &&
		type == rhs.type &&
		name == rhs.name &&
		location == rhs.location &&
		normalize == rhs.normalize &&
		ptr == rhs.ptr //Compare actual ptr location...
		);
}

/************************************************************************/
/*                 ShaderData definitions                              */
/************************************************************************/

H3D::ShaderData::ShaderData()
	: programID(0), uniforms(0){

}

/************************************************************************/
/*                    TotalRenderState definitions                      */
/************************************************************************/
H3D::TotalRenderState::TotalRenderState(){
	//Default init everything.
	reset();
}

void H3D::TotalRenderState::reset(){
	stateChanges.clear();
	depthState.reset();
	blendState.reset();
	rasterizerState.reset();
	vertexLayoutDescription = 0;
	shader.programID = 0;
	shader.uniforms.resize(0);
}

void H3D::TotalRenderState::applyChanges(Appearance* const appearance){
	RenderProperties* rp = appearance->renderProperties->getValue();

	if(rp){
		/************************************************************************/
		/* BlendState assignment                                                */
		/************************************************************************/

		setBlendingEnabled(rp->blendEnabled->getValue());

		rp->getAlphaFunc(rp->alphaFunc->getValue(), blendState.alphaTestFunc);

		GLenum tempVal;
		rp->getBlendEquation(rp->blendEquationRGB->getValue(), tempVal);
		setBlendEquationRGB(tempVal);

		rp->getBlendEquation(rp->blendEquationAlpha->getValue(), tempVal);
		setBlendEquationAlpha(tempVal);

		rp->getSFactor(rp->blendFuncSrcFactorRGB->getValue(), tempVal);
		setSrcFactorRGB(tempVal);

		rp->getDFactor(rp->blendFuncDstFactorRGB->getValue(), tempVal);
		setDstFactorRGB(tempVal);

		rp->getSFactor(rp->blendFuncSrcFactorAlpha->getValue(), tempVal);
		setSrcFactorAlpha(tempVal);

		rp->getDFactor(rp->blendFuncDstFactorAlpha->getValue(), tempVal);
		setDstFactorAlpha(tempVal);

		OGLColorMask colorMask(
			rp->colorBufferRedWriteEnabled->getValue(),
			rp->colorBufferGreenWriteEnabled->getValue(),
			rp->colorBufferBlueWriteEnabled->getValue(),
			rp->colorBufferAlphaWriteEnabled->getValue());

		setColorMask(colorMask);

		//Clamp to [0.0f, 1.0f]
		if(blendState.alphaFuncValue > 1.0f) {
			blendState.alphaFuncValue = 1.0f;
		} else if(blendState.alphaFuncValue < 0.0f) {
			blendState.alphaFuncValue = 0.0f;
		}

		setAlphaFuncValue(rp->alphaFuncValue->getValue());

		/************************************************************************/
		/* DepthState assignment                                                */
		/************************************************************************/
		rp->getDepthFunc(rp->depthFunc->getValue(), tempVal);

		setDepthFunction(tempVal);
		setDepthTestingEnabled(rp->depthTestEnabled->getValue()); 
		setDepthBufferWriteEnabled(rp->depthBufferWriteEnabled->getValue());
	}
}

void H3D::TotalRenderState::applyChanges(X3DGeometryNode* const geometryNode){
	if(geometryNode)
	{
		setFaceCullingEnabled(geometryNode->usingCulling());
		setCullFace(geometryNode->getCullFace());
	}
}

bool H3D::TotalRenderState::operator==(TotalRenderState& rhs){
	return (	rhs.vertexLayoutDescription == vertexLayoutDescription &&
		rhs.rasterizerState			== rasterizerState &&
		rhs.depthState				== depthState &&
		rhs.blendState				== blendState &&
		rhs.shader.programID		== shader.programID
		);
}

bool H3D::TotalRenderState::operator!=(TotalRenderState& rhs){
	return !(*this == rhs);
}

/************************************************************************/
/* DepthState get&set                                                   */
/************************************************************************/
DepthState& H3D::TotalRenderState::getDepthState() {
	return depthState;
}

void H3D::TotalRenderState::setDepthState(DepthState val) {
	setDepthTestingEnabled(val.depthTestingEnabled);
	setDepthBufferWriteEnabled(val.depthBufferWriteEnabled);
	setDepthFunction(val.depthFunction);
}

bool H3D::TotalRenderState::getDepthTestingEnabled(){
	return depthState.depthTestingEnabled;
}

void H3D::TotalRenderState::setDepthTestingEnabled(bool val){
	if(depthState.depthTestingEnabled != val) {
		depthState.depthTestingEnabled = val;
		stateChanges.insert(DepthTestingEnable);
	}
}

bool H3D::TotalRenderState::getDepthBufferWriteEnabled(){
	return depthState.depthBufferWriteEnabled;
}

void H3D::TotalRenderState::setDepthBufferWriteEnabled(bool val){
	if(depthState.depthBufferWriteEnabled != val) {
		depthState.depthBufferWriteEnabled = val;
		stateChanges.insert(DepthBufferWriteEnable);
	}
}

GLenum H3D::TotalRenderState::getDepthFunction(){
	return depthState.depthFunction;
}

void H3D::TotalRenderState::setDepthFunction(GLenum val){
	if(depthState.depthFunction != val){
		depthState.depthFunction = val;
		stateChanges.insert(DepthFunctionChange);
	}
}

/************************************************************************/
/* BlendState get&set                                                   */
/************************************************************************/

BlendState& H3D::TotalRenderState::getBlendState()
{
	return blendState;
}


void H3D::TotalRenderState::setBlendState(BlendState val)
{
	//I do this because each individual set function does some checks to make sure that the values are actually different. 
	//If any value is different we set a flag to indicate that something has actually changed.
	setBlendingEnabled(val.blendingEnabled);
	setSrcFactorRGB(val.srcFactorRGB);
	setDstFactorRGB(val.dstFactorRGB);
	setSrcFactorAlpha(val.srcFactorAlpha);
	setDstFactorAlpha(val.dstFactorAlpha);
	setAlphaFuncValue(val.alphaFuncValue);
	setAlphaTestFunc(val.alphaTestFunc);
	setBlendEquationRGB(val.blendEquationRGB);
	setBlendEquationAlpha(val.blendEquationAlpha);
	setBlendColor(val.blendColor);
	setColorMask(val.colorMask);
}

bool H3D::TotalRenderState::getBlendingEnabled(){
	return blendState.blendingEnabled;
}

void H3D::TotalRenderState::setBlendingEnabled(bool val){
	if(blendState.blendingEnabled != val){
		blendState.blendingEnabled = val;
		stateChanges.insert(BlendingEnabled);
	}
}

GLenum H3D::TotalRenderState::getSrcFactorRGB(){
	return blendState.srcFactorRGB;
}

void H3D::TotalRenderState::setSrcFactorRGB(GLenum val){
	if(blendState.srcFactorRGB != val){
		blendState.srcFactorRGB = val;
		stateChanges.insert(BlendFunctionChange);
	}
}

GLenum H3D::TotalRenderState::getDstFactorRGB(){
	return blendState.dstFactorRGB;
}

void H3D::TotalRenderState::setDstFactorRGB(GLenum val){
	if(blendState.dstFactorRGB != val){
		blendState.dstFactorRGB = val;
		stateChanges.insert(BlendFunctionChange);
	}
}

GLenum H3D::TotalRenderState::getBlendEquationRGB(){
	return blendState.blendEquationRGB;
}

void H3D::TotalRenderState::setBlendEquationRGB(GLenum val){
	if(blendState.blendEquationRGB != val){
		blendState.blendEquationRGB = val;
		stateChanges.insert(BlendEquationChange);
	}
}

GLenum H3D::TotalRenderState::getAlphaTestFunc(){
	return blendState.alphaTestFunc;
}

void H3D::TotalRenderState::setAlphaTestFunc(GLenum val){
	if(blendState.alphaTestFunc != val){
		blendState.alphaTestFunc = val;
		stateChanges.insert(AlphaFunctionChange);
	}
}

GLenum H3D::TotalRenderState::getSrcFactorAlpha(){
	return blendState.srcFactorAlpha;
}

void H3D::TotalRenderState::setSrcFactorAlpha(GLenum val){
	if(blendState.srcFactorAlpha != val){
		blendState.srcFactorAlpha = val;
		stateChanges.insert(BlendFunctionChange);
	}
}

GLenum H3D::TotalRenderState::getDstFactorAlpha(){
	return blendState.dstFactorAlpha;
}

void H3D::TotalRenderState::setDstFactorAlpha(GLenum val){
	if(	blendState.dstFactorAlpha != val){
		blendState.dstFactorAlpha = val;
		stateChanges.insert(BlendFunctionChange);
	}
}

GLenum H3D::TotalRenderState::getBlendEquationAlpha(){
	return blendState.blendEquationAlpha;
}

void H3D::TotalRenderState::setBlendEquationAlpha(GLenum val){
	if(blendState.blendEquationAlpha != val){
		blendState.blendEquationAlpha = val;
		stateChanges.insert(BlendEquationChange);
	}
}

GLclampf H3D::TotalRenderState::getAlphaFuncValue(){
	return blendState.alphaFuncValue;
}

void H3D::TotalRenderState::setAlphaFuncValue(GLclampf val){
	if(blendState.alphaFuncValue != val){
		blendState.alphaFuncValue = val;
		stateChanges.insert(AlphaFunctionChange);
	}
}

H3D::OGLBlendColor H3D::TotalRenderState::getBlendColor(){
	return blendState.blendColor;
}

void H3D::TotalRenderState::setBlendColor(OGLBlendColor val){
	if(!(blendState.blendColor == val)){
		blendState.blendColor = val;
		stateChanges.insert(BlendColorChange);
	}
}

H3D::OGLColorMask H3D::TotalRenderState::getColorMask(){
	return blendState.colorMask;
}

void H3D::TotalRenderState::setColorMask(OGLColorMask val){
	if(!(blendState.colorMask == val)){
		blendState.colorMask = val;
		stateChanges.insert(ColorMaskChange);
	}
}

/************************************************************************/
/* RasterizerState get&set                                             */
/************************************************************************/

RasterizerState& H3D::TotalRenderState::getRasterizerState()
{
	return rasterizerState;
}

void H3D::TotalRenderState::setRasterizerState(RasterizerState val)
{
	setFaceCullingEnabled(val.faceCullingEnabled);
	setScissorTestEnabled(val.scissorTestEnabled);
	setWindingOrder(val.windingOrder);
	setCullFace(val.cullFace);
}

bool H3D::TotalRenderState::getFaceCullingEnabled(){
	return rasterizerState.faceCullingEnabled;
}

void H3D::TotalRenderState::setFaceCullingEnabled(bool val){
	if(rasterizerState.faceCullingEnabled != val){
		rasterizerState.faceCullingEnabled = val;
		stateChanges.insert(FaceCullingEnable);
	}
}

bool H3D::TotalRenderState::getScissorTestEnabled(){
	return rasterizerState.scissorTestEnabled;
}

void H3D::TotalRenderState::setScissorTestEnabled(bool val){
	if(	rasterizerState.scissorTestEnabled != val){
		rasterizerState.scissorTestEnabled = val;
		stateChanges.insert(ScissorTestEnable);
	}
}

GLenum H3D::TotalRenderState::getCullFace(){
	return rasterizerState.cullFace;
}

void H3D::TotalRenderState::setCullFace(GLenum val){
	if(rasterizerState.cullFace != val){
		rasterizerState.cullFace = val;
		stateChanges.insert(CullFaceChanged);
	}
}

GLenum H3D::TotalRenderState::getWindingOrder(){
	return rasterizerState.windingOrder;
}

void H3D::TotalRenderState::setWindingOrder(GLenum val){
	if(	rasterizerState.windingOrder != val){
		rasterizerState.windingOrder = val;
		stateChanges.insert(WindingOrderChange);
	}
}

/************************************************************************/
/* ShaderData get&set                                                   */
/************************************************************************/

H3D::ShaderData& H3D::TotalRenderState::getShaderData(){
	return shader;
}

void H3D::TotalRenderState::setShaderData(ShaderData val){
	if(shader.programID != val.programID || shader.uniforms != val.uniforms){
		shader = val;
		stateChanges.insert(ShaderChange);
	}
}


/************************************************************************/
/* VertexLayoutDescription get&set	                                    */
/************************************************************************/

unsigned int H3D::TotalRenderState::getVertexLayoutDescription(){
	return vertexLayoutDescription;
}

void H3D::TotalRenderState::setVertexLayoutDescription(unsigned int val){
	vertexLayoutDescription = val;
}

void H3D::TotalRenderState::InsertRenderCommands(RenderCommandBuffer* buffer, TotalRenderState* previousRenderState)
{
	/* For each custom state change. */
	for(std::set<StateChangeType>::iterator it = stateChanges.begin(); it != stateChanges.end(); ++it)
	{
		bool previousStateHasMatch = false;
		bool stateChangeIsValid = false;

		/* Compare against previous render state... */
		if(previousRenderState)
		{
			for(std::set<StateChangeType>::iterator prevIt = previousRenderState->stateChanges.begin(); prevIt != previousRenderState->stateChanges.end(); ++prevIt) {
				//If these match it means that both the previous state and the current state have a non-default state of the same type, 
				// and we need to compare to make sure that we don't insert two identical state changes.
				if(*it == *prevIt) {
					previousStateHasMatch = true;
				}
			}
		}

		StateChangeValue stateChangeVal;

		//Act accordingly...
		switch(*it)
		{
			//////////////////////////////////////////////////////////////////////////
		case DepthTestingEnable:
			if(previousStateHasMatch){
				if(getDepthTestingEnabled() != previousRenderState->getDepthTestingEnabled()) {
					stateChangeVal.boolVal = getDepthTestingEnabled(); 
				}
			} else {
				stateChangeIsValid = true;
				stateChangeVal.boolVal = getDepthTestingEnabled(); 
			}
			break;

			//////////////////////////////////////////////////////////////////////////
		case DepthBufferWriteEnable:
			if(previousStateHasMatch) {
				if(getDepthBufferWriteEnabled() != previousRenderState->getDepthBufferWriteEnabled()) {
					stateChangeVal.boolVal = getDepthBufferWriteEnabled();
				}
			} else {
				stateChangeIsValid = true;
				stateChangeVal.boolVal = getDepthBufferWriteEnabled();
			}
			break;

			//////////////////////////////////////////////////////////////////////////
		case DepthFunctionChange:
			if(previousStateHasMatch) {
				if(getDepthFunction() != previousRenderState->getDepthFunction()){
					stateChangeIsValid = true;
					stateChangeVal.boolVal = getDepthFunction();
				}
			} else {
				stateChangeIsValid = true;
				stateChangeVal.enumVal = getDepthFunction();
			}
			break;

			//////////////////////////////////////////////////////////////////////////
		case BlendingEnabled:
			if(previousStateHasMatch) {
				if(getBlendingEnabled() != previousRenderState->getBlendingEnabled()){
					stateChangeIsValid = true;
					stateChangeVal.boolVal = getBlendingEnabled();
				}
			} else {
				stateChangeIsValid = true;
				stateChangeVal.boolVal = getBlendingEnabled();
			}
			break;

			//////////////////////////////////////////////////////////////////////////
		case BlendFunctionChange:
			if(previousStateHasMatch){
				if(	getSrcFactorRGB() != previousRenderState->getSrcFactorRGB() ||
					getDstFactorRGB() != previousRenderState->getDstFactorRGB() ||
					getSrcFactorAlpha() != previousRenderState->getSrcFactorAlpha() ||
					getDstFactorAlpha() != previousRenderState->getDstFactorAlpha()) {
					stateChangeIsValid = true;
					stateChangeVal.blendFactorBundle.srcRGB = getSrcFactorRGB();
					stateChangeVal.blendFactorBundle.dstRGB = getDstFactorRGB();
					stateChangeVal.blendFactorBundle.srcAlpha = getSrcFactorAlpha();
					stateChangeVal.blendFactorBundle.dstAlpha = getDstFactorAlpha();
				}
			} else {
				stateChangeIsValid = true;
				stateChangeVal.blendFactorBundle.srcRGB = getSrcFactorRGB();
				stateChangeVal.blendFactorBundle.dstRGB = getDstFactorRGB();
				stateChangeVal.blendFactorBundle.srcAlpha = getSrcFactorAlpha();
				stateChangeVal.blendFactorBundle.dstAlpha = getDstFactorAlpha();
			}
			break;

			//////////////////////////////////////////////////////////////////////////
		case BlendEquationChange:
			if(previousStateHasMatch)
			{
				if(	getBlendEquationRGB() != previousRenderState->getBlendEquationRGB() ||
					getBlendEquationAlpha() != previousRenderState->getBlendEquationAlpha()) {
					stateChangeIsValid = true;
					stateChangeVal.blendEquationBundle.rgbEquation = getBlendEquationRGB();
					stateChangeVal.blendEquationBundle.alphaEquation = getBlendEquationAlpha();
				}
			} 
			else
			{
				stateChangeIsValid = true;
				stateChangeVal.blendEquationBundle.rgbEquation = getBlendEquationRGB();
				stateChangeVal.blendEquationBundle.alphaEquation = getBlendEquationAlpha();
			}
			break;

			//////////////////////////////////////////////////////////////////////////
		case AlphaFunctionChange:
			if(previousStateHasMatch)
			{
				if(getAlphaTestFunc() != previousRenderState->getAlphaTestFunc() ||
					getAlphaFuncValue() != previousRenderState->getAlphaFuncValue()) {
					stateChangeIsValid = true;
					stateChangeVal.alphaFuncBundle.alphaTestFunc = getAlphaTestFunc();
					stateChangeVal.alphaFuncBundle.testValue = getAlphaFuncValue();
				}
			} 
			else
			{
				stateChangeIsValid = true;
				stateChangeVal.alphaFuncBundle.alphaTestFunc = getAlphaTestFunc();
				stateChangeVal.alphaFuncBundle.testValue = getAlphaFuncValue();
			}
			break;

			//////////////////////////////////////////////////////////////////////////
		case ColorMaskChange:
			if(previousStateHasMatch)
			{
				if(!(getColorMask() == previousRenderState->getColorMask())) {
						stateChangeIsValid = true;
						stateChangeVal.colorMaskVal[0] = getColorMask().mask[0];
						stateChangeVal.colorMaskVal[1] = getColorMask().mask[1];
						stateChangeVal.colorMaskVal[2] = getColorMask().mask[2];
						stateChangeVal.colorMaskVal[3] = getColorMask().mask[3];
				}
			} 
			else
			{
				stateChangeIsValid = true;
				stateChangeVal.colorMaskVal[0] = getColorMask().mask[0];
				stateChangeVal.colorMaskVal[1] = getColorMask().mask[1];
				stateChangeVal.colorMaskVal[2] = getColorMask().mask[2];
				stateChangeVal.colorMaskVal[3] = getColorMask().mask[3];
			}
			break;

			//////////////////////////////////////////////////////////////////////////
		case BlendColorChange:
			if(previousStateHasMatch)
			{
				if(!(getBlendColor() == previousRenderState->getBlendColor())) {
					stateChangeIsValid = true;
					stateChangeVal.blendColorVal[0] = getBlendColor().color[0];
					stateChangeVal.blendColorVal[1] = getBlendColor().color[1];
					stateChangeVal.blendColorVal[2] = getBlendColor().color[2];
					stateChangeVal.blendColorVal[3] = getBlendColor().color[3];
				}
			} 
			else
			{
				stateChangeIsValid = true;
				stateChangeVal.blendColorVal[0] = getBlendColor().color[0];
				stateChangeVal.blendColorVal[1] = getBlendColor().color[1];
				stateChangeVal.blendColorVal[2] = getBlendColor().color[2];
				stateChangeVal.blendColorVal[3] = getBlendColor().color[3];
			}
			break;

			//////////////////////////////////////////////////////////////////////////
		case FaceCullingEnable:
			if(previousStateHasMatch)
			{
				if(getFaceCullingEnabled() != previousRenderState->getFaceCullingEnabled()) {
					stateChangeIsValid = true;
					stateChangeVal.boolVal = getFaceCullingEnabled();
				}
			} 
			else
			{
				stateChangeIsValid = true;
				stateChangeVal.boolVal = getFaceCullingEnabled();
			}
			break;

			//////////////////////////////////////////////////////////////////////////
		case CullFaceChanged:
			if(previousStateHasMatch)
			{
				if(getCullFace() != previousRenderState->getCullFace()){
					stateChangeIsValid = true;
					stateChangeVal.enumVal = getCullFace();
				}
			} 
			else
			{
				stateChangeIsValid = true;
				stateChangeVal.enumVal = getCullFace();
			}
			break;

			//////////////////////////////////////////////////////////////////////////
		case WindingOrderChange:
			if(previousStateHasMatch)
			{
				if(getWindingOrder() != previousRenderState->getWindingOrder()) {
				stateChangeIsValid = true;
				stateChangeVal.enumVal = getWindingOrder();
				}
			} 
			else
			{
				stateChangeIsValid = true;
				stateChangeVal.enumVal = getWindingOrder();
			}
			break;

			//////////////////////////////////////////////////////////////////////////
		case ScissorTestEnable:
			if(previousStateHasMatch)
			{
				if(getScissorTestEnabled() != previousRenderState->getScissorTestEnabled()) {
					stateChangeIsValid = true;
					stateChangeVal.boolVal = getScissorTestEnabled();
				}
			} 
			else
			{
				stateChangeIsValid = true;
				stateChangeVal.boolVal = getScissorTestEnabled();
			}
			break;

			//////////////////////////////////////////////////////////////////////////
		case ShaderChange:
			if(previousStateHasMatch)
			{
				const ShaderData& prevShader = previousRenderState->getShaderData();

				if(getShaderData().programID != prevShader.programID ||
					getShaderData().uniforms != prevShader.uniforms) {
					stateChangeIsValid = true;
					stateChangeVal.uintVal = getShaderData().programID;
				}
			} 
			else
			{
				stateChangeIsValid = true;
				stateChangeVal.uintVal = getShaderData().programID;
			}
			break;
		}
		//////////////////////////////////////////////////////////////////////////

		if(stateChangeIsValid)
		{
			buffer->InsertNewCommand(new GenericStateChangeCommand(*it, stateChangeVal));

			//If it happens to be a shader, we also want to insert some bindUniform commands!
			if(*it == ShaderChange){
				for(unsigned int i = 0; i < getShaderData().uniforms.size(); ++i) {
					buffer->InsertNewCommand(new BindUniformCommand(getShaderData().uniforms[i]));
				}
			}
		}
	}
}
