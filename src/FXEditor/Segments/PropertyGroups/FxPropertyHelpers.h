#pragma once

#define EMPTY 1 << FXPRIM_TYPE_EMPTY
#define PARTICLE_CLOUD 1 << FXPRIM_TYPE_PARTICLE_CLOUD
#define SPRITE 1 << FXPRIM_TYPE_SPRITE
#define WEATHER_FX 1 << FXPRIM_TYPE_WEATHERFX
#define ORIENTED_SPRITE 1 << FXPRIM_TYPE_ORIENTED_SPRITE
#define SPARK 1 << FXPRIM_TYPE_SPARK
#define LINE 1 << FXPRIM_TYPE_LINE
#define LIGHTNING 1 << FXPRIM_TYPE_LIGHTNING
#define CYLINDER 1 << FXPRIM_TYPE_CYLINDER
#define MODEL 1 << FXPRIM_TYPE_MODEL
#define LIGHT 1 << FXPRIM_TYPE_LIGHT
#define SCREEN_FLASH 1 << FXPRIM_TYPE_SCREENFLASH
#define CAMERA_SHAKE 1 << FXPRIM_TYPE_CAMERA_SHAKE
#define TRAIL 1 << FXPRIM_TYPE_TRAIL
#define BEZIER_CURVE 1 << FXPRIM_TYPE_BEZIER

enum EFXPrimType;

bool IsPrimitiveSupported(unsigned int flags, EFXPrimType primType);

bool IsPrimitiveAttributeSupported(const char* name, EFXPrimType primType);

bool IsProcPrimitive(EFXPrimType primType);

bool IsLineBasedPrimitive(EFXPrimType primType);

bool IsParticleBasedPrimitive(EFXPrimType primType);

bool IsBlendSupported(EFXPrimType primType);

bool IsTextureSupported(EFXPrimType primType);

bool IsShaderSupported(EFXPrimType primType);

bool IsRotationSupported(EFXPrimType primType);

bool IsRotationRadiusSupported(EFXPrimType primType);

bool IsRotationAxisSupported(EFXPrimType primType);

bool IsOrientAxisSupported(EFXPrimType primType);

bool IsLengthSupported(EFXPrimType primType);

bool IsOrigin2Supported(EFXPrimType primType);

bool IsSizeSupported(EFXPrimType primType);

bool IsSize2Supported(EFXPrimType primType);

bool IsColorSupported(EFXPrimType primType);

bool IsAlphaSupported(EFXPrimType primType);

bool IsNumSegmentsSupported(EFXPrimType primType);

bool IsChaosSupported(EFXPrimType primType);

bool IsUVScaleSupported(EFXPrimType primType);

bool IsUVScrollSupported(EFXPrimType primType);

bool IsDragSupported(EFXPrimType primType);

bool IsGravitySupported(EFXPrimType primType);

bool IsViewOffsetSupported(EFXPrimType primType);
