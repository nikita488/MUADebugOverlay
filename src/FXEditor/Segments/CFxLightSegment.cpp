#include "CFxLightSegment.h"

#include "PropertyGroups/CFxOriginSizeGroup.h"
#include "PropertyGroups/CFxMotionGroup.h"
#include "PropertyGroups/CFxColorGroup.h"
#include "PropertyGroups/CFxLightGroup.h"

void CFxLightSegment::InitializePropertyGroups()
{
	AddPropertyGroup<CFxOriginSizeGroup>();
	AddPropertyGroup<CFxMotionGroup>();
	AddPropertyGroup<CFxColorGroup>();
	AddPropertyGroup<CFxLightGroup>();
	CFxSegment::InitializePropertyGroups();
}
