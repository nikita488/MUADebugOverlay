#pragma once

#include "IFxPropertyGroup.h"

#if !defined(RATL_STRING_VS_INC)
	#include <Common/Ratl/string_vs.h>
#endif

class CFxModelGroup : public IFxPropertyGroup
{
private:
	ratl::string_vs<64> mModel;

public:
	virtual const char* GetName() const;
	virtual void Initialize(IFxSegment* segment);
	virtual bool Draw(IFxSegment* segment);
};
