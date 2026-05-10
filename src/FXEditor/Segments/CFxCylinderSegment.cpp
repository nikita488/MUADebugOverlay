#include "CFxCylinderSegment.h"

#include "PropertyGroups/CFxOriginSizeGroup.h"
#include "PropertyGroups/CFxMotionGroup.h"
#include "PropertyGroups/CFxColorGroup.h"
#include "PropertyGroups/CFxCylinderGroup.h"
#include "PropertyGroups/CFxTempGroup.h"
#include "PropertyGroups/CFxTransform2Group.h"
#include "PropertyGroups/CFxMotion2Group.h"

void CFxCylinderSegment::InitializePropertyGroups()
{
	AddPropertyGroup<CFxOriginSizeGroup>();
	AddPropertyGroup<CFxMotionGroup>();
	AddPropertyGroup<CFxTransform2Group>();
	AddPropertyGroup<CFxMotion2Group>();
	AddPropertyGroup<CFxCylinderGroup>();
	AddPropertyGroup<CFxColorGroup>();
	CFxSegment::InitializePropertyGroups();
}
