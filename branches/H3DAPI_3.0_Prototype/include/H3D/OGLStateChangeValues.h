#ifndef __OGLSTATECHANGEVALUES_H__
#define __OGLSTATECHANGEVALUES_H__

#include <GL/glew.h>
#include <H3DUtil/LinAlgTypes.h>

using namespace H3DUtil;

namespace H3D {
	namespace OGLValueTypes {
		typedef unsigned int TypedValue;
		enum {
			BlendFactorBundle,
			BlendEquationBundle,
			AlphaFuncBundle,
			BoolValue,
			FloatValue,
			Vec2fValue,
			Vec3fValue,
			Vec4fValue,
			IntValue,
			UIntValue,
			DoubleValue,
			BlendColorValue,
			ColorMaskValue
		};
	}


	struct OGLBlendColor {
		OGLBlendColor() {
			color[0] = 0.0f;
			color[1] = 0.0f;
			color[2] = 0.0f;
			color[3] = 1.0f;
		}

		OGLBlendColor(GLclampf redBlend, GLclampf greenBlend, GLclampf blueBlend, GLclampf alphaBlend) {
			color[0] = redBlend;
			color[1] = greenBlend;
			color[2] = blueBlend;
			color[3] = alphaBlend;
		}

		bool operator==(const OGLBlendColor& rhs) {
			return (rhs.color[0] == color[0] &&
				rhs.color[1] ==	color[1] && 
				rhs.color[2] ==	color[2] && 
				rhs.color[3] ==	color[3] );
		}

		GLclampf color[4];
	};

	struct OGLColorMask {
		OGLColorMask() {
			mask[0]	= true;
			mask[1]	= true;
			mask[2]	= true;
			mask[3]	= true;
		}

		OGLColorMask(bool redMask, bool greenMask, bool blueMask, bool alphaMask) {
			mask[0] = redMask;
			mask[1] = greenMask;
			mask[2] = blueMask;
			mask[3] = alphaMask;
		}

		bool operator==(const OGLColorMask& rhs) {
			return (mask[0] ==	rhs.mask[0] &&
				mask[1] ==	rhs.mask[1] && 
				mask[2] ==	rhs.mask[2] && 
				mask[3] ==	rhs.mask[3] );
		}

		GLboolean mask[4];
	};

	struct StateChangeValue {
		StateChangeValue(){}
		virtual ~StateChangeValue(){};

		OGLValueTypes::TypedValue type; // <-- Based on this we just do a static_cast to a child type and fetch the value of the child.
	};

	//Might need more specific naming...

	struct BlendFunctionStateBundle : public StateChangeValue {
		GLenum srcRGB, srcAlpha, dstRGB, dstAlpha;
	};

	struct BlendEquationStateBundle : public StateChangeValue {
		GLenum rgbEquation, alphaEquation;
	};

	struct AlphaFuncStateBundle : public StateChangeValue {
		GLenum alphaTestFunc;
		GLclampf testValue;
	};

	struct BoolStateValue : public StateChangeValue {
		GLboolean value;
	};

	struct Vec2fStateValue : public StateChangeValue {
		Vec2f value;
	};

	struct Vec3fStateValue : public StateChangeValue {
		Vec3f value;
	};

	struct Vec4fStateValue : public StateChangeValue {
		Vec4f value;
	};

	struct UintStateValue : public StateChangeValue {
		GLuint value;
	};

	struct IntStateValue : public StateChangeValue {
		GLint value;
	};

	struct FloatStateValue : public StateChangeValue {
		float value;
	};

	struct DoubleStateValue : public StateChangeValue {
		double value;
	};

	struct BlendColorStateValue : public StateChangeValue {
		OGLBlendColor value;
	};

	struct ColorMaskStateValue : public StateChangeValue {
		OGLColorMask value;
	};
}

#endif