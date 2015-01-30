#include <H3D/TotalRenderState.h>
#include <H3D/RenderProperties.h>
#include <H3D/X3DGeometryNode.h>
#include <H3D/Appearance.h>
#include <H3D/RenderCommandBuffer.h>
#include <H3D/OGLUtil.h> //For OGL error reporting
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


void H3D::TotalRenderState::setBlendState(BlendState val) {
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

bool H3D::TotalRenderState::getBlendingEnabled() {
	return blendState.blendingEnabled;
}

void H3D::TotalRenderState::setBlendingEnabled(bool val) {
	if(blendState.blendingEnabled != val) {
		blendState.blendingEnabled = val;
		stateChanges.insert(BlendingEnabled);
	}
}

GLenum H3D::TotalRenderState::getSrcFactorRGB() {
	return blendState.srcFactorRGB;
}

void H3D::TotalRenderState::setSrcFactorRGB(GLenum val) {
	if(blendState.srcFactorRGB != val){
		blendState.srcFactorRGB = val;
		stateChanges.insert(BlendFunctionChange);
	}
}

GLenum H3D::TotalRenderState::getDstFactorRGB() {
	return blendState.dstFactorRGB;
}

void H3D::TotalRenderState::setDstFactorRGB(GLenum val) {
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

void H3D::TotalRenderState::setWindingOrder(GLenum val) {
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
	if(shader.programID != val.programID || shader.uniforms != val.uniforms) {
		shader = val;
		stateChanges.insert(ShaderChange);
	}
}


/************************************************************************/
/* VertexLayoutDescription get&set	                                    */
/************************************************************************/

unsigned int H3D::TotalRenderState::getVertexLayoutDescription() {
	return vertexLayoutDescription;
}

void H3D::TotalRenderState::setVertexLayoutDescription(unsigned int val) {
	vertexLayoutDescription = val;
}

void H3D::TotalRenderState::InsertRenderCommands(RenderCommandBuffer* buffer, TotalRenderState* previousRenderState) {
	/* For each custom state change. */
	for(std::set<StateChangeType>::iterator it = stateChanges.begin(); it != stateChanges.end(); ++it) {
		bool previousStateHasMatch = false;
		bool stateChangeIsValid = false;

		/* Compare against previous render state... */
		if(previousRenderState) {
			for(std::set<StateChangeType>::iterator prevIt = previousRenderState->stateChanges.begin(); prevIt != previousRenderState->stateChanges.end(); ++prevIt) {
				//If these match it means that both the previous state and the current state have a non-default state of the same type, 
				// and we need to compare to make sure that we don't insert two identical state changes.
				if(*it == *prevIt) {
					previousStateHasMatch = true;
				}
			}
		}

		auto_ptr<StateChangeValue> stateChangeVal;

		//Act accordingly...
		switch(*it)
		{
			//////////////////////////////////////////////////////////////////////////
		case DepthTestingEnable: {
			BoolStateValue* depthTest = new BoolStateValue;
			if(previousStateHasMatch) {
				if(getDepthTestingEnabled() != previousRenderState->getDepthTestingEnabled()) {

					depthTest->value = getDepthTestingEnabled(); 
				}
			} else {
				stateChangeIsValid = true;
				depthTest->value = getDepthTestingEnabled(); 
			}
			stateChangeVal.reset(depthTest); }
			break;

			//////////////////////////////////////////////////////////////////////////
		case DepthBufferWriteEnable: {
			BoolStateValue* depthBufferWrite = new BoolStateValue;
			if(previousStateHasMatch) {
				if(getDepthBufferWriteEnabled() != previousRenderState->getDepthBufferWriteEnabled()) {
					depthBufferWrite->value = getDepthBufferWriteEnabled();
				}
			} else {
				stateChangeIsValid = true;
				depthBufferWrite->value = getDepthBufferWriteEnabled();
			}
			stateChangeVal.reset(depthBufferWrite); }
			break;

			//////////////////////////////////////////////////////////////////////////
		case DepthFunctionChange: {
			UintStateValue* depthFunc = new UintStateValue;
			if(previousStateHasMatch) {
				if(getDepthFunction() != previousRenderState->getDepthFunction()){
					stateChangeIsValid = true;
					depthFunc->value = getDepthFunction();
				}
			} else {
				stateChangeIsValid = true;
				depthFunc->value = getDepthFunction();
			}
			stateChangeVal.reset(depthFunc); }
			break;

			//////////////////////////////////////////////////////////////////////////
		case BlendingEnabled: {
			BoolStateValue* blendEnable = new BoolStateValue;
			if(previousStateHasMatch) {
				if(getBlendingEnabled() != previousRenderState->getBlendingEnabled()){
					stateChangeIsValid = true;
					blendEnable->value = getBlendingEnabled();
				}
			} else {
				stateChangeIsValid = true;
				blendEnable->value = getBlendingEnabled();
			}
			stateChangeVal.reset(blendEnable); }
			break;

			//////////////////////////////////////////////////////////////////////////
		case BlendFunctionChange: {
			BlendFunctionStateBundle* blendFuncBundle = new BlendFunctionStateBundle;
			if(previousStateHasMatch) { 
				if(	getSrcFactorRGB() != previousRenderState->getSrcFactorRGB() ||
					getDstFactorRGB() != previousRenderState->getDstFactorRGB() ||
					getSrcFactorAlpha() != previousRenderState->getSrcFactorAlpha() ||
					getDstFactorAlpha() != previousRenderState->getDstFactorAlpha()) {
						stateChangeIsValid = true;			
						blendFuncBundle->srcRGB = getSrcFactorRGB();
						blendFuncBundle->dstRGB = getDstFactorRGB();
						blendFuncBundle->srcAlpha = getSrcFactorAlpha();
						blendFuncBundle->dstAlpha = getDstFactorAlpha();
				}
			} else {
				stateChangeIsValid = true;
				blendFuncBundle->srcRGB = getSrcFactorRGB();
				blendFuncBundle->dstRGB = getDstFactorRGB();
				blendFuncBundle->srcAlpha = getSrcFactorAlpha();
				blendFuncBundle->dstAlpha = getDstFactorAlpha();
			}
			stateChangeVal.reset(blendFuncBundle); }
			break;

			//////////////////////////////////////////////////////////////////////////
		case BlendEquationChange: {
			BlendEquationStateBundle* blendEqBundle = new BlendEquationStateBundle;
			if(previousStateHasMatch) {
				if(	getBlendEquationRGB() != previousRenderState->getBlendEquationRGB() ||
					getBlendEquationAlpha() != previousRenderState->getBlendEquationAlpha()) {
						stateChangeIsValid = true;
						blendEqBundle->rgbEquation = getBlendEquationRGB();
						blendEqBundle->alphaEquation = getBlendEquationAlpha();
				}
			} else {
				stateChangeIsValid = true;
				blendEqBundle->rgbEquation = getBlendEquationRGB();
				blendEqBundle->alphaEquation = getBlendEquationAlpha();
			}
			stateChangeVal.reset(blendEqBundle); }
			break;

			//////////////////////////////////////////////////////////////////////////
		case AlphaFunctionChange: {
			AlphaFuncStateBundle* alphaFuncBundle = new AlphaFuncStateBundle;
			if(previousStateHasMatch) {
				if(getAlphaTestFunc() != previousRenderState->getAlphaTestFunc() ||
					getAlphaFuncValue() != previousRenderState->getAlphaFuncValue()) {
						stateChangeIsValid = true;
						alphaFuncBundle->alphaTestFunc = getAlphaTestFunc();
						alphaFuncBundle->testValue = getAlphaFuncValue();
				}
			} else {
				stateChangeIsValid = true;
				alphaFuncBundle->alphaTestFunc = getAlphaTestFunc();
				alphaFuncBundle->testValue = getAlphaFuncValue();
			}
			stateChangeVal.reset(alphaFuncBundle); }
			break;

			//////////////////////////////////////////////////////////////////////////
		case ColorMaskChange: {
			ColorMaskStateValue* colorMask = new ColorMaskStateValue;
			if(previousStateHasMatch) {
				if(!(getColorMask() == previousRenderState->getColorMask())) {
					stateChangeIsValid = true;
					colorMask->value = getColorMask();
				}
			} else {
				stateChangeIsValid = true;
				colorMask->value = getColorMask();
			}
			stateChangeVal.reset(colorMask); }
			break;

			//////////////////////////////////////////////////////////////////////////
		case BlendColorChange: {
			BlendColorStateValue* blendColor = new BlendColorStateValue;
			if(previousStateHasMatch) {
				if(!(getBlendColor() == previousRenderState->getBlendColor())) {
					stateChangeIsValid = true;			
					blendColor->value = getBlendColor();
				}
			} else {
				stateChangeIsValid = true;			
				blendColor->value = getBlendColor();
			}
			stateChangeVal.reset(blendColor); }
			break;

			//////////////////////////////////////////////////////////////////////////
		case FaceCullingEnable: {
			BoolStateValue* faceCull = new BoolStateValue;
			if(previousStateHasMatch) {
				if(getFaceCullingEnabled() != previousRenderState->getFaceCullingEnabled()) {
					stateChangeIsValid = true;
					faceCull->value = getFaceCullingEnabled();
				}
			} else {
				stateChangeIsValid = true;
				faceCull->value = getFaceCullingEnabled();
			}
			stateChangeVal.reset(faceCull); }
			break;

			//////////////////////////////////////////////////////////////////////////
		case CullFaceChanged: {
			UintStateValue* cullFace = new UintStateValue;
			if(previousStateHasMatch) {
				if(getCullFace() != previousRenderState->getCullFace()){
					stateChangeIsValid = true;
					cullFace->value = getCullFace();
				}
			} else {
				stateChangeIsValid = true;
				cullFace->value = getCullFace();
			}
			stateChangeVal.reset(cullFace); }
			break;

			//////////////////////////////////////////////////////////////////////////
		case WindingOrderChange: {
			UintStateValue* windingOrder = new UintStateValue;
			if(previousStateHasMatch) {
				if(getWindingOrder() != previousRenderState->getWindingOrder()) {
					stateChangeIsValid = true;
					windingOrder->value = getWindingOrder();
				}
			} else {
				stateChangeIsValid = true;
				windingOrder->value = getWindingOrder();
			}
			stateChangeVal.reset(windingOrder);
			}
			break;

			//////////////////////////////////////////////////////////////////////////
		case ScissorTestEnable: {
			BoolStateValue* scissorTest = new BoolStateValue;
			if(previousStateHasMatch) {
				if(getScissorTestEnabled() != previousRenderState->getScissorTestEnabled()) {
					stateChangeIsValid = true;
					scissorTest->value = getScissorTestEnabled();
				}
			} else {
				stateChangeIsValid = true;
				scissorTest->value = getScissorTestEnabled();
			}
			stateChangeVal.reset(scissorTest); }
			break;

			//////////////////////////////////////////////////////////////////////////
		case ShaderChange: {
			UintStateValue* shaderValue = new UintStateValue;
			if(previousStateHasMatch) {
				const ShaderData& prevShader = previousRenderState->getShaderData();

				if(getShaderData().programID != prevShader.programID ||
					getShaderData().uniforms != prevShader.uniforms) {
						stateChangeIsValid = true;
						shaderValue->value = getShaderData().programID;
				}
			} else {
				stateChangeIsValid = true;
				shaderValue->value = getShaderData().programID;
			}
			stateChangeVal.reset(shaderValue); }
			break;
		}
		//////////////////////////////////////////////////////////////////////////

		if(stateChangeIsValid) {
			//After we've new-ed the stateChangeVal, remember to add the type.
			stateChangeVal->type = *it;

			buffer->InsertNewCommand(new GenericStateChangeCommand(stateChangeVal));

			//If it happens to be a shader, we also want to insert some bindUniform commands!
			if(*it == ShaderChange){
				for(unsigned int i = 0; i < getShaderData().uniforms.size(); ++i) {
					buffer->InsertNewCommand(new BindUniformCommand(getShaderData().uniforms[i]));
				}
			}
		}
	}
}


/*The stuff below: 
We need setters too.

Also: I need to make a new std::set that contains all of the deviating values. 
Whenever we call a get, we look through the set to see if there's a deviating value floating around here, 
else we do an actual glGet to fetch the default value. */
GLboolean H3D::TotalRenderState::getGLBoolean(GLenum target, GLuint index/*= 0*/) {
	GLboolean val;
	glGetBooleani_v(target, index, &val);
	LogOGLErrors("TotalRenderState::getGLBoolean");
	return val;
}

//void H3D::TotalRenderState::setGLBoolean(GLenum target, GLboolean val, GLuint index /*= 0*/) {
//	//.. Hm
//}

GLint H3D::TotalRenderState::getGLInteger(GLenum target, GLuint index/*= 0*/) {
	GLint val;
	glGetIntegeri_v(target, index, &val);
	LogOGLErrors("TotalRenderState::getGLInteger");
	return val;
}

//void H3D::TotalRenderState::setGLInteger(GLenum target, GLint val, GLuint index /*= 0*/) {
//
//}

GLint64 H3D::TotalRenderState::getGLInteger64(GLenum target, GLuint index/*= 0*/) {
	GLint64 val;
	glGetInteger64i_v(target, index, &val);
	LogOGLErrors("TotalRenderState::getGLInteger64");
	return val;
}

//void H3D::TotalRenderState::setGLInteger64(GLenum target, GLint64 val, GLuint index /*= 0*/) {
//
//}

GLfloat H3D::TotalRenderState::getGLFloat(GLenum target, GLuint index/*= 0*/) {
	GLfloat val;
	glGetFloati_v(target, index, &val);
	LogOGLErrors("TotalRenderState::getGLFloat");
	return val;
}

//void H3D::TotalRenderState::setGLFloat(GLenum target, GLfloat val, GLuint index /*= 0*/) {
//
//}

GLdouble H3D::TotalRenderState::getGLDouble(GLenum target, GLuint index/*= 0*/) {
	GLdouble val;
	glGetDoublei_v(target, index, &val);
	LogOGLErrors("TotalRenderState::getGLDouble");
	return val;
}

//void H3D::TotalRenderState::setGLDouble(GLenum target, GLdouble val, GLuint index /*= 0*/) {
//
//}

H3DUtil::Vec2f H3D::TotalRenderState::getGLFloat2(GLenum target/*= 0*/) {
	Vec2f val;
	glGetFloati_v(target, 0, &val.x);
	LogOGLErrors("TotalRenderState::getGLFloat2");
	return val;
}

//void H3D::TotalRenderState::setGLFloat2(GLenum target, Vec2f val)
//{
//}

H3DUtil::Vec3f H3D::TotalRenderState::getGLFloat3(GLenum target/*= 0*/) {
	Vec3f val;
	glGetFloati_v(target, 0, &val.x);
	LogOGLErrors("TotalRenderState::getGLFloat3");
	return val;
}

//void H3D::TotalRenderState::setGLFloat3(GLenum target, Vec3f val) {
//
//}

H3DUtil::Vec4f H3D::TotalRenderState::getGLFloat4(GLenum target/*= 0*/) {
	Vec4f val;
	glGetFloati_v(target, 0, &val.x);
	LogOGLErrors("TotalRenderState::getGLFloat4");
	return val;
}

//void H3D::TotalRenderState::setGLFloat4(GLenum target, Vec4f val) {
//
//}



