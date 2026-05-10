#pragma once

#include "IFxPropertyGroup.h"
#include "../SegmentDataTypes.h"

#if !defined(RATL_ARRAY_VS)
	#include "../Common/Ratl/array_vs.h"
#endif

#if !defined(RATL_STRING_VS_INC)
	#include <Common/Ratl/string_vs.h>
#endif

#define MAX_BLEND_TYPES 5
#define MAX_TEXTURES 6

typedef ratl::string_vs<64> TAssetString;

enum EFxShader
{
	SHADER_LIT,
	SHADER_POSTFX,
	SHADER_CUSTOM,
};

//TODO: Rename to CFxMaterialGroup
class CFxColorGroup : public IFxPropertyGroup
{
private:
	TAssetString mTexture;

	bool mApplyShader;
	int mShaderType;
	TAssetString mShader;
	ratl::array_vs<TAssetString, MAX_TEXTURES> mShaderTextures;

	CSegmentColor mColor;
	CSegmentQuadratic mAlpha;

	bool mTurnUV;
	CSegmentQuadratic mUVScale;
	CSegmentQuadratic mUVScroll;

private:
	const char* ShaderTextureName(int unitId);

public:
	CFxColorGroup();

	virtual const char* GetName() const;
	virtual void Initialize(IFxSegment* segment);
	virtual bool Draw(IFxSegment* segment);
};
