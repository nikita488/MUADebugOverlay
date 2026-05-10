#pragma once

#include <igGapMath.h>
#include <igMath/igVec3f.h>

#include "CFxFile.h"
#include "CFxCamera.h"
#include "CFxPlayback.h"
#include "CFxSegmentFilter.h"

#include "IAssetCache.h"

class IFxEditor
{
public:
	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;
	virtual void RunFrame() = 0;
	virtual void Draw() = 0;
	virtual bool IsActive() const = 0;
	virtual void SetActive(bool active) = 0;

	virtual void SetSelectedSegment(IFxSegment* selected) = 0;
	virtual IFxSegment* GetSelectedSegment() const = 0;

	virtual bool DrawGrid() const = 0;
	virtual bool DrawAxis() const = 0;
	virtual const igVec3f& GetFloorColor() const = 0;

	virtual void UpdateTable() = 0;

	virtual void ShowError(const char* message) = 0;

	virtual CFxFile& GetFile() = 0;
	virtual CFxCamera& GetCamera() = 0;
	virtual CFxPlayback& GetPlayback() = 0;
	virtual CFxSegmentFilter& GetFilter() = 0;

	virtual IAssetCache& TextureCache() = 0;
	virtual IAssetCache& ModelCache() = 0;
	virtual IAssetCache& FxCache() = 0;
};

extern IFxEditor& TheFxEditor();
