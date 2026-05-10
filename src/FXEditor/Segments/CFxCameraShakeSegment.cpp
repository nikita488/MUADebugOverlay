#include "CFxCameraShakeSegment.h"

#include "PropertyGroups/CFxCameraShakeGroup.h"

void CFxCameraShakeSegment::InitializePropertyGroups()
{
	AddPropertyGroup<CFxCameraShakeGroup>();
	CFxSegment::InitializePropertyGroups();
}
