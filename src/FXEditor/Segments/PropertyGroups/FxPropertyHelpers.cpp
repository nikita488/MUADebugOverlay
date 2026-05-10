#include "FxPropertyHelpers.h"

#include <unordered_map>

#include <FX/CFxPrimitiveTemplate.h>

#define LINE_FLAGS LINE | LIGHTNING | CYLINDER | BEZIER_CURVE

#define PROC_PRIMITIVE_FLAGS LINE_FLAGS | SPRITE | ORIENTED_SPRITE | SPARK | SCREEN_FLASH | TRAIL

#define PARTICLE_CLOUD_FLAGS PARTICLE_CLOUD | WEATHER_FX

#define TEXTURE_FLAGS PROC_PRIMITIVE_FLAGS | PARTICLE_CLOUD_FLAGS

#define MODEL_FLAGS MODEL

#define SHADER_FLAGS PROC_PRIMITIVE_FLAGS

#define ROTATION_RADIUS_FLAGS SPRITE | ORIENTED_SPRITE | SPARK | CYLINDER | MODEL | TRAIL

#define ROTATION_FLAGS PARTICLE_CLOUD_FLAGS | ROTATION_RADIUS_FLAGS | BEZIER_CURVE

#define LENGTH_FLAGS SPARK | TRAIL | BEZIER_CURVE

#define BLEND_FLAGS PROC_PRIMITIVE_FLAGS | PARTICLE_CLOUD_FLAGS

#define COLOR_FLAGS PROC_PRIMITIVE_FLAGS | PARTICLE_CLOUD_FLAGS | MODEL | LIGHT

#define ALPHA_FLAGS COLOR_FLAGS

#define NUM_SEGMENTS_FLAGS PARTICLE_CLOUD_FLAGS | CYLINDER | BEZIER_CURVE

#define ARC_FLAGS CYLINDER

#define VEC_FLAGS BEZIER_CURVE

#define CHAOS_FLAGS LIGHTNING | BEZIER_CURVE

#define CAMERA_SHAKE_FLAGS CAMERA_SHAKE

#define OFFSET_FLAGS TRAIL

#define ROTATION_AXIS_FLAGS SPRITE | ORIENTED_SPRITE | MODEL

#define ORIENT_AXIS_FLAGS ORIENTED_SPRITE | MODEL

#define UV_SCROLL_FLAGS LINE_FLAGS | SCREEN_FLASH

#define UV_SCALE_FLAGS LINE_FLAGS | SCREEN_FLASH | TRAIL

#define SIZE_FLAGS PROC_PRIMITIVE_FLAGS | PARTICLE_CLOUD_FLAGS | MODEL

#define SIZE2_FLAGS CYLINDER | BEZIER_CURVE

#define ORIGIN_FLAGS PROC_PRIMITIVE_FLAGS | PARTICLE_CLOUD_FLAGS | EMPTY | MODEL | LIGHT

#define ORIGIN2_FLAGS LINE_FLAGS | SPRITE | ORIENTED_SPRITE | MODEL

#define LIGHT_FLAGS LIGHT

#define MOTION_FLAGS PROC_PRIMITIVE_FLAGS | PARTICLE_CLOUD_FLAGS | EMPTY | MODEL | LIGHT
#define MOTION2_FLAGS LINE_FLAGS | PARTICLE_CLOUD

#define DRAG_FLAGS PROC_PRIMITIVE_FLAGS | EMPTY | MODEL | LIGHT

#define GRAVITY_FLAGS PROC_PRIMITIVE_FLAGS | EMPTY | MODEL | LIGHT

#define VIEWOFFSET_FLAGS PROC_PRIMITIVE_FLAGS | PARTICLE_CLOUD | EMPTY | MODEL | LIGHT

std::unordered_map<const char*, unsigned int> primTypeFlags =
{
	{ "transformrotation", PARTICLE_CLOUD_FLAGS },
	{ "plife", PARTICLE_CLOUD_FLAGS },
	{ "ptravel", PARTICLE_CLOUD_FLAGS },
	{ "pspawn", PARTICLE_CLOUD_FLAGS },
	{ "origin", ORIGIN_FLAGS },
	{ "velocity", MOTION_FLAGS },
	{ "acceleration", MOTION_FLAGS },
	{ "gravity", GRAVITY_FLAGS },
	{ "drag", DRAG_FLAGS },
	{ "viewoffset", VIEWOFFSET_FLAGS },
	{ "origin2", ORIGIN2_FLAGS },
	{ "velocity2", MOTION2_FLAGS },
	{ "acceleration2", MOTION2_FLAGS },
	{ "gravity2", MOTION2_FLAGS },
	{ "drag2", MOTION2_FLAGS },
	{ "startvec", VEC_FLAGS },
	{ "endvec", VEC_FLAGS },
	{ "radius", ORIGIN_FLAGS },
	{ "radius2", ORIGIN_FLAGS },
	{ "height", ORIGIN_FLAGS },
	{ "startcolor1", COLOR_FLAGS },
	{ "midcolor1", COLOR_FLAGS },
	{ "endcolor1", COLOR_FLAGS },
	{ "startcolor2", COLOR_FLAGS },
	{ "midcolor2", COLOR_FLAGS },
	{ "endcolor2", COLOR_FLAGS },
	{ "alpha", ALPHA_FLAGS },
	{ "size", SIZE_FLAGS },
	{ "size2", SIZE2_FLAGS },
	{ "length", LENGTH_FLAGS },
	{ "offset", OFFSET_FLAGS },
	{ "rotation", ROTATION_FLAGS },
	{ "rotationradius", ROTATION_RADIUS_FLAGS },
	{ "rotationaxis", ROTATION_AXIS_FLAGS },
	{ "orientaxis", ORIENT_AXIS_FLAGS },
	{ "chaos", CHAOS_FLAGS },
	{ "attenuation", LIGHT_FLAGS },
	{ "startarc", ARC_FLAGS },
	{ "endarc", ARC_FLAGS },
	//{ "deathfxfile", 0 },
	//{ "intervalfxfile", 0 },
	//{ "interval", 0 },
	{ "texture", TEXTURE_FLAGS },
	{ "shadertexture0", SHADER_FLAGS },
	{ "shadertexture1", SHADER_FLAGS },
	{ "shadertexture2", SHADER_FLAGS },
	{ "shadertexture3", SHADER_FLAGS },
	{ "shadertexture4", SHADER_FLAGS },
	{ "shadertexture5", SHADER_FLAGS },
	{ "shader", SHADER_FLAGS },
	{ "shaderflags", SHADER_FLAGS },
	{ "modelname", MODEL_FLAGS },
	{ "blend", BLEND_FLAGS },
	{ "uvscroll", UV_SCROLL_FLAGS },
	{ "uvscale", UV_SCALE_FLAGS },
	{ "shaketype", CAMERA_SHAKE_FLAGS },
	{ "shakescale", CAMERA_SHAKE_FLAGS },
	{ "shakespeed", CAMERA_SHAKE_FLAGS },
	{ "numsegments", NUM_SEGMENTS_FLAGS }
};

bool IsPrimitiveSupported(unsigned int flags, EFXPrimType primType)
{
	return (flags & (1 << primType)) != 0;
}

bool IsPrimitiveAttributeSupported(const char* name, EFXPrimType primType)
{
	auto& it = primTypeFlags.find(name);

	if (it != primTypeFlags.end())
	{
		return IsPrimitiveSupported(it->second, primType);
	}

	return true;
}

bool IsProcPrimitive(EFXPrimType primType)
{
	return IsPrimitiveSupported(PROC_PRIMITIVE_FLAGS, primType);
}

bool IsLineBasedPrimitive(EFXPrimType primType)
{
	return IsPrimitiveSupported(LINE_FLAGS, primType);
}

bool IsParticleBasedPrimitive(EFXPrimType primType)
{
	return IsPrimitiveSupported(PARTICLE_CLOUD_FLAGS, primType);
}

bool IsBlendSupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(BLEND_FLAGS, primType);
}

bool IsTextureSupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(TEXTURE_FLAGS, primType);
}

bool IsShaderSupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(SHADER_FLAGS, primType);
}

bool IsRotationSupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(ROTATION_FLAGS, primType);
}

bool IsRotationRadiusSupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(ROTATION_RADIUS_FLAGS, primType);
}

bool IsRotationAxisSupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(ROTATION_AXIS_FLAGS, primType);
}

bool IsOrientAxisSupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(ORIENT_AXIS_FLAGS, primType);
}

bool IsLengthSupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(LENGTH_FLAGS, primType);
}

bool IsOrigin2Supported(EFXPrimType primType)
{
	return IsPrimitiveSupported(ORIGIN2_FLAGS, primType);
}

bool IsSizeSupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(SIZE_FLAGS, primType);
}

bool IsSize2Supported(EFXPrimType primType)
{
	return IsPrimitiveSupported(SIZE2_FLAGS, primType);
}

bool IsColorSupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(COLOR_FLAGS, primType);
}

bool IsAlphaSupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(ALPHA_FLAGS, primType);
}

bool IsNumSegmentsSupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(NUM_SEGMENTS_FLAGS, primType);
}

bool IsChaosSupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(CHAOS_FLAGS, primType);
}

bool IsUVScaleSupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(UV_SCALE_FLAGS, primType);
}

bool IsUVScrollSupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(UV_SCROLL_FLAGS, primType);
}

bool IsDragSupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(DRAG_FLAGS, primType);
}

bool IsGravitySupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(GRAVITY_FLAGS, primType);
}

bool IsViewOffsetSupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(VIEWOFFSET_FLAGS, primType);
}
