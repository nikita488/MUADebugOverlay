#pragma once

#include "IFxEditor.h"

#include <string>
#include <list>
#include <filesystem>

#include <FX/CFxTemplate.h>
#include <FX/CFxTemplateManager.h>
#include <FX/IFxManager.h>
#include <FX/CFxPrimitiveTemplatePool.h>
#include <FX/CRangedComponentPool.h>
#include <Misc/Common.h>
#include <Misc/ResourceSpecifier.h>

#if !defined(RATL_STRING_VS_INC)
	#include <Common/Ratl/string_vs.h>
#endif

#include <igGapMath.h>
#include <igMath/igVec4f.h>
#include <igMath/igMatrix44f.h>

#include "CFxFile.h"
#include "CFxCamera.h"
#include "CFxPlayback.h"
#include "CFxSegmentTable.h"
#include "CFxSpawn.h"

#include "CTextureAssetCache.h"
#include "CModelAssetCache.h"
#include "CFxAssetCache.h"

class IFxSegment;

using namespace Gap;

enum EFxEditorAction
{
	ACTION_NONE = -1,
	ACTION_FILE_NEW,
	ACTION_FILE_OPEN,
	ACTION_FILE_SAVE,
	ACTION_FILE_SAVE_AS,
	ACTION_FILE_EXIT,
};

class CFxEditor : public IFxEditor
{
private:
	bool mActive;

	CFxFile mFile;
	CFxPlayback mPlayback;
	CFxSegmentFilter mSegmentFilter;
	CFxCamera mCamera;

	IFxSegment* mSelectedSegment;

	float mTimeScale;
	bool mDrawGrid;
	bool mDrawAxes;
	igVec4f mBackgroundColor;
	igVec4f mFloorColor;

	float mLastTimeScale;
	igVec4f mLastClearColor;
	bool mLastFullscreenFXEnabled;

	float mNextPlayTime;
	int mPersistFXHandle;

	bool mIsPlaying;
	bool mIsPaused;
	float mStopTime;

	bool mWorldWindowVisible;
	bool mPlaybackWindowVisible;
	bool mStatusWindowVisible;

	igVec3f mSpawnOrigin;
	float mAnimateSpawnPointTime;

	std::filesystem::path mFilePath;
	int mPendingActionId;

	CFxSegmentTable mSegmentTable;

	CFxSpawn mSpawn;

	bool mShowError;
	std::string mErrorMessage;

public:
	CTextureAssetCache mTextureCache;
	CModelAssetCache mModelCache;
	CFxAssetCache mFxCache;

private:
	void Reset();
	void ResetPlayState(bool flag = true);

private:
	inline virtual void SetSelectedSegment(IFxSegment* selected)
	{
		mSelectedSegment = selected;

		if (selected)
		{
			selected->Reset();
		}
	}

	inline virtual IFxSegment* GetSelectedSegment() const
	{
		return mSelectedSegment;
	}

	inline bool HasSelectedSegment() const
	{
		return mSelectedSegment != NULL;
	}

	void DeleteSelectedSegment();
	void CloneSelectedSegment();
	
	void SetTimeScale(float scale);
	void SetPaused(bool paused);

	void FileNewAction();
	void FileOpenAction();
	void FileSaveAction();
	void FileSaveAsAction();
	void FileExitAction();

	void ExecuteAction(int actionId);
	void ExecutePendingAction();
	void ResetPendingAction();

	bool SaveFile(const char* fileName = NULL);

	void ProcessSaveChangesPopup();
	void ProcessOpenFileDialog();
	void ProcessSaveFileDialog();
	void ProcessErrorPopup();

	virtual void ShowError(const char* message);

	void MainMenuBar();

	int FileMenu();
	int EditMenu();
	int ViewMenu();

	void PlaybackBar();
	void StatusBar();

	void EffectSegmentsWindow();
	void SegmentPropertiesWindow();
	void WorldSettings();

	void PlayEffect(int handle);
	void UpdatePlayState();
	void UpdateGameCamera();

	bool CanBeStopped() const;
	void UpdateStopTime();

public:
	CFxEditor();

	virtual void Initialize();

	virtual void Shutdown();

	virtual void RunFrame();

	virtual void Draw();

	inline virtual bool IsActive() const
	{
		return mActive;
	}

	inline virtual void SetActive(bool active)
	{
		mActive = active;
	}

	inline virtual bool DrawGrid() const
	{
		return mDrawGrid;
	}

	inline virtual bool DrawAxis() const
	{
		return mDrawAxes;
	}

	inline virtual const igVec3f& GetFloorColor() const
	{
		return *(igVec3f*)mFloorColor.vec;
	}

	virtual void UpdateTable();

	inline virtual CFxFile& GetFile()
	{
		return mFile;
	}

	inline virtual CFxCamera& GetCamera()
	{
		return mCamera;
	}

	inline virtual CFxPlayback& GetPlayback()
	{
		return mPlayback;
	}

	inline virtual CFxSegmentFilter& GetFilter()
	{
		return mSegmentFilter;
	}

	inline virtual IAssetCache& TextureCache()
	{
		return mTextureCache;
	}

	inline virtual IAssetCache& ModelCache()
	{
		return mModelCache;
	}

	inline virtual IAssetCache& FxCache()
	{
		return mFxCache;
	}

	inline CFxTemplate* GetFxTemplate()
	{
		return mFile.GetTemplate();
	}
};

