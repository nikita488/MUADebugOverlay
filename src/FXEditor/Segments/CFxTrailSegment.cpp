#include "CFxTrailSegment.h"

#include "PropertyGroups/CFxOriginSizeGroup.h"
#include "PropertyGroups/CFxMotionGroup.h"
#include "PropertyGroups/CFxColorGroup.h"
#include "PropertyGroups/CFxTrailGroup.h"

void CFxTrailSegment::InitializePropertyGroups()
{
	AddPropertyGroup<CFxOriginSizeGroup>();
	AddPropertyGroup<CFxMotionGroup>();
	AddPropertyGroup<CFxTrailGroup>();
	AddPropertyGroup<CFxColorGroup>();
	CFxSegment::InitializePropertyGroups();
}
