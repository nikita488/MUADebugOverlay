#include "CFxParticleCloudSegment.h"

#include "PropertyGroups/CFxOriginSizeGroup.h"
#include "PropertyGroups/CFxMotionGroup.h"
#include "PropertyGroups/CFxMotion2Group.h"
#include "PropertyGroups/CFxColorGroup.h"
#include "PropertyGroups/CFxParticleCloudGroup.h"

void CFxParticleCloudSegment::InitializePropertyGroups()
{
	CFxPrimitiveTemplate* primTemplate = GetTemplate();
	
	AddPropertyGroup<CFxOriginSizeGroup>();
	AddPropertyGroup<CFxMotionGroup>();
	
	if (primTemplate->field_4 == FXPRIM_TYPE_PARTICLE_CLOUD)
	{
		AddPropertyGroup<CFxMotion2Group>();
	}

	AddPropertyGroup<CFxParticleCloudGroup>();
	AddPropertyGroup<CFxColorGroup>();
	CFxSegment::InitializePropertyGroups();
}
