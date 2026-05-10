#include "CFxModelSegment.h"

#include "PropertyGroups/CFxOriginSizeGroup.h"
#include "PropertyGroups/CFxMotionGroup.h"
#include "PropertyGroups/CFxColorGroup.h"
#include "PropertyGroups/CFxModelGroup.h"
#include "PropertyGroups/CFxTransform2Group.h"

void CFxModelSegment::InitializePropertyGroups()
{
	AddPropertyGroup<CFxOriginSizeGroup>();
	AddPropertyGroup<CFxMotionGroup>();
	AddPropertyGroup<CFxTransform2Group>();
	AddPropertyGroup<CFxModelGroup>();
	AddPropertyGroup<CFxColorGroup>();
	CFxSegment::InitializePropertyGroups();
}
