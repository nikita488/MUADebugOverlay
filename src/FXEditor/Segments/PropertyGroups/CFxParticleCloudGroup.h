#pragma once

#include "IFxPropertyGroup.h"
#include "../SegmentDataTypes.h"

class CFxParticleCloudGroup : public IFxPropertyGroup
{
private:
	int mNumParticles;

	CSegmentFloat mParticleTravel;
	CSegmentFloat mParticleSpawn;
	CSegmentVector mTransformRotation;

	bool mNoSync;
	int mNewLifeMode;

public:
	CFxParticleCloudGroup();

	virtual const char* GetName() const;
	virtual void Initialize(IFxSegment* segment);
	virtual bool Draw(IFxSegment* segment);
};
