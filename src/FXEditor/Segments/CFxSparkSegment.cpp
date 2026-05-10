#include "CFxSparkSegment.h"

#include "PropertyGroups/CFxOriginSizeGroup.h"
#include "PropertyGroups/CFxMotionGroup.h"
#include "PropertyGroups/CFxColorGroup.h"
#include "PropertyGroups/CFxSparkGroup.h"

void CFxSparkSegment::InitializePropertyGroups()
{
	AddPropertyGroup<CFxOriginSizeGroup>();
	AddPropertyGroup<CFxMotionGroup>();
	AddPropertyGroup<CFxSparkGroup>();
	AddPropertyGroup<CFxColorGroup>();
	CFxSegment::InitializePropertyGroups();
}
