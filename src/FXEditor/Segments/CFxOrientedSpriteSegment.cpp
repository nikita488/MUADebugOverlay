#include "CFxOrientedSpriteSegment.h"

#include "PropertyGroups/CFxOriginSizeGroup.h"
#include "PropertyGroups/CFxMotionGroup.h"
#include "PropertyGroups/CFxColorGroup.h"
#include "PropertyGroups/CFxOrientedSpriteGroup.h"
#include "PropertyGroups/CFxTransform2Group.h"

void CFxOrientedSpriteSegment::InitializePropertyGroups()
{
	AddPropertyGroup<CFxOriginSizeGroup>();
	AddPropertyGroup<CFxMotionGroup>();
	AddPropertyGroup<CFxTransform2Group>();
	AddPropertyGroup<CFxOrientedSpriteGroup>();
	AddPropertyGroup<CFxColorGroup>();
	CFxSegment::InitializePropertyGroups();
}
