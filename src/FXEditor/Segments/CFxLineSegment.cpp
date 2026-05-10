#include "CFxLineSegment.h"

#include "PropertyGroups/CFxOriginSizeGroup.h"
#include "PropertyGroups/CFxMotionGroup.h"
#include "PropertyGroups/CFxColorGroup.h"
#include "PropertyGroups/CFxCylinderGroup.h"
#include "PropertyGroups/CFxTempGroup.h"
#include "PropertyGroups/CFxTransform2Group.h"
#include "PropertyGroups/CFxMotion2Group.h"
#include "PropertyGroups/CFxLineGroup.h"

void CFxLineSegment::InitializePropertyGroups()
{
	AddPropertyGroup<CFxOriginSizeGroup>();
	AddPropertyGroup<CFxMotionGroup>();
	AddPropertyGroup<CFxTransform2Group>();
	AddPropertyGroup<CFxMotion2Group>();
	AddPropertyGroup<CFxLineGroup>();
	AddPropertyGroup<CFxColorGroup>();
	CFxSegment::InitializePropertyGroups();
}
