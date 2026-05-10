#include "CFxSpriteSegment.h"

#include "PropertyGroups/CFxOriginSizeGroup.h"
#include "PropertyGroups/CFxMotionGroup.h"
#include "PropertyGroups/CFxColorGroup.h"
#include "PropertyGroups/CFxTransform2Group.h"

void CFxSpriteSegment::InitializePropertyGroups()
{
	AddPropertyGroup<CFxOriginSizeGroup>();
	AddPropertyGroup<CFxMotionGroup>();
	AddPropertyGroup<CFxTransform2Group>();
	AddPropertyGroup<CFxColorGroup>();
	CFxSegment::InitializePropertyGroups();
}
