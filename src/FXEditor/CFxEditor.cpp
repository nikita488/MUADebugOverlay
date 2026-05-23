#include "CFxEditor.h"

#include <Misc/Common.h>
#include <FX/CFxManager.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx9.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <Game/CMarvelCamera.h>
#include <FX/IFxManager.h>
#include <Scene/CPlayfield.h>
#include <Display/IAlchemyDisplay.h>
#include <UI/IMenuSystem.h>

#include <Misc/CConfig.h>
#include <Misc/ICommand.h>

#include "FxTemplateHelpers.h"
#include "Segments/PropertyGroups/FxImGuiWidgets.h"

#include <filesystem>

namespace fs = std::filesystem;

using namespace Gap;

#define DEFAULT_BACKGROUND_COLOR (0x808080)
#define DEFAULT_FLOOR_COLOR (0xC0C0C0)

//const char* FILE_OPEN_DIALOG = "FxFileOpen";
//const char* FILE_SAVE_DIALOG = "FxFileSave";

float GetTime()
{
	return TheMenuMgr().GetMenuTime();
}

float GetTimeDelta()
{
	return TheMenuMgr().GetMenuTimeDelta();
}

float GetTimeScale()
{
	return TheMenuMgr().GetTimeScale();
}

void _SetTimeScale(float scale)
{
	TheMenuMgr().SetTimeScale(scale);
}

struct EffectOpenDialog : public ImGuiFileDialogContext
{
	EffectOpenDialog()
	{
		dialogId = "FxFileOpen";
		title = "Choose Effect File";
		filters = "Effect (*.xmlb){.xmlb}";
		root = "effects/";
		SetupDefaultFlags();
	}
};

struct EffectSaveDialog : public ImGuiFileDialogContext
{
	EffectSaveDialog()
	{
		dialogId = "FxFileSave";
		title = "Save File";
		filters = "Effect (*.xmlb){.xmlb}";
		root = "effects/";
		SetupDefaultFlags();
		AddFlags(ImGuiFileDialogFlags_ConfirmOverwrite);
	}
};

static EffectOpenDialog effectOpenDlg;
static EffectSaveDialog effectSaveDlg;

static bool& g_fullscreenFXEnabled = *(bool*)0xD4029C;

CFxEditor::CFxEditor()
{
	Reset();
}

void CFxEditor::Reset()
{
	mActive = false;

	mFile.Clear();
	mPlayback.Reset();
	mSegmentFilter.Reset();
	mCamera.Reset();

	mSelectedSegment = NULL;

	mTimeScale = 1.0F;
	mDrawGrid = true;
	mDrawAxes = true;

	mLastTimeScale = 1.0F;
	mLastFullscreenFXEnabled = false;

	mNextPlayTime = 0.0F;
	mPersistFXHandle = 0;

	mIsPlaying = false;
	mIsPaused = false;
	mStopTime = 0.0F;

	mWorldWindowVisible = false;
	mPlaybackWindowVisible = true;
	mStatusWindowVisible = true;

	mSpawnOrigin = igVec3f::ZeroVector;
	mAnimateSpawnPointTime = 0.0F;

	mFilePath.clear();
	mPendingActionId = ACTION_NONE;

	mSegmentTable.Clear();

	mSpawn.Reset();

	mShowError = false;
	mErrorMessage.clear();
}

void CFxEditor::ResetPlayState(bool flag)
{
	SetPaused(false);
	TheMenuFxManager().FlushFx(-1);
	mPersistFXHandle = 0;

	mSpawnOrigin = igVec3f::ZeroVector;
	mIsPlaying = false;
	
	if (flag)
	{
		SetSelectedSegment(NULL);
		mFilePath.clear();
	}


	ResetPendingAction();

	mNextPlayTime = 0.0F;
	mStopTime = 0.0F;
	mAnimateSpawnPointTime = 0.0F;
}

void CFxEditor::Initialize()
{
	igVec4f defaultBackgroundColor;
	defaultBackgroundColor.unpackColor(IG_MATH_COLORSPACE_FORMAT_RGBA, DEFAULT_BACKGROUND_COLOR);

	igVec4f defaultFloorColor;
	defaultFloorColor.unpackColor(IG_MATH_COLORSPACE_FORMAT_RGBA, DEFAULT_FLOOR_COLOR);

	XMenIni().Get("FXEDITOR", "backgroundColor", &mBackgroundColor, defaultBackgroundColor);
	XMenIni().Get("FXEDITOR", "floorColor", &mFloorColor, defaultFloorColor);

	mBackgroundColor[3] = 0.0F;
	mFloorColor[3] = 0.0F;

	mTextureCache.Initialize(MP_MENU);
	mModelCache.Initialize(MP_MENU);
	mFxCache.Initialize(MP_MENU);
	
	mFile.Create();
	mCamera.Initialize();
	mActive = true;

	TheAlchemyDisplay().GetVC()->getClearColor(mLastClearColor);
	mLastTimeScale = GetTimeScale();
	mLastFullscreenFXEnabled = g_fullscreenFXEnabled;

	SetTimeScale(mTimeScale);
}

void CFxEditor::Shutdown()
{
	mTextureCache.Shutdown(MP_MENU);
	mModelCache.Shutdown(MP_MENU);
	mFxCache.Shutdown(MP_MENU);
	
	TheAlchemyDisplay().GetVC()->setClearColor(mLastClearColor);
	_SetTimeScale(mLastTimeScale);
	g_fullscreenFXEnabled = mLastFullscreenFXEnabled;
	//ImGui::GetIO().MouseDrawCursor = false;

	Reset();
	ResetPlayState();
}

void CFxEditor::DeleteSelectedSegment()
{
	if (!HasSelectedSegment())
	{
		return;
	}
	
	mFile.DeleteSegment(GetSelectedSegment());
	SetSelectedSegment(NULL);
}

void CFxEditor::CloneSelectedSegment()
{
	if (!HasSelectedSegment())
	{
		return;
	}

	IFxSegment* clonedSegment = mFile.CloneSegment(GetSelectedSegment());

	if (clonedSegment)
	{
		SetSelectedSegment(clonedSegment);
	}
}

void CFxEditor::SetTimeScale(float scale)
{
	mTimeScale = scale;
	_SetTimeScale(scale);
}

void CFxEditor::SetPaused(bool paused)
{
	mIsPaused = paused;
	
	if (paused)
	{
		_SetTimeScale(0.0F);
	}
	else
	{
		_SetTimeScale(mTimeScale);
	}
}

//void OpenFileDialog(const char* str_id, const char* title, const char* filters, const fs::path& currentPath)
//{
//	fs::path folderPath = currentPath.parent_path();
//	
//	IGFD::FileDialogConfig config;
//
//	config.path = folderPath.empty() ? "effects" : folderPath.string();
//	config.flags |= ImGuiFileDialogFlags_HideColumnType;
//	config.flags |= ImGuiFileDialogFlags_HideColumnSize;
//	config.flags |= ImGuiFileDialogFlags_HideColumnDate;
//	config.flags |= ImGuiFileDialogFlags_Modal;
//	config.flags |= ImGuiFileDialogFlags_DisablePlaceMode;
//	config.flags |= ImGuiFileDialogFlags_DisableThumbnailMode;
//	//config.flags |= ImGuiFileDialogFlags_DisableQuickPathSelection;
//
//	ImGuiFileDialog::Instance()->OpenDialog(str_id, title, filters, config);
//}

void CFxEditor::FileNewAction()
{
	//TODO: Check if file is currently fresh -> don't do anything
	ResetPlayState();

	if (!mFile.Create())
	{
		ShowError("Failed to create new file");
	}
}

void CFxEditor::FileOpenAction()
{
	//OpenFileDialog(FILE_OPEN_DIALOG, "Choose Effect File", ".xmlb", mFilePath);
	effectOpenDlg.SetInitialPath(mFilePath.string().c_str(), "effects/");
	effectOpenDlg.OpenDialog();
}

void CFxEditor::FileSaveAction()
{
	if (mFile.IsDirty())
	{
		SaveFile();
	}
}

void CFxEditor::FileSaveAsAction()
{
	//OpenFileDialog(FILE_SAVE_DIALOG, "Save File", ".xmlb", mFilePath);
	effectSaveDlg.SetInitialPath(mFilePath.string().c_str(), "effects/");
	effectSaveDlg.OpenDialog();
}

void CFxEditor::FileExitAction()
{
	ratl::string_vs<64> startMenu;
	XMenIni().Get("INIT", "startMenu", startMenu.c_str(), startMenu.capacity(), TheMenuMgr().GetStartMenu());
	
	if (stricmp(startMenu.c_str(), "fxeditor") == 0)
	{
		bool& quit = *(bool*)0x835D8F;
		quit = true;
	}
	else
	{
		Command().ExecuteCommandString("closemenu fxeditor");
	}
}

void CFxEditor::ExecuteAction(int actionId)
{
	switch (actionId)
	{
	case ACTION_FILE_NEW:
		FileNewAction();
		break;
	case ACTION_FILE_OPEN:
		FileOpenAction();
		break;
	case ACTION_FILE_SAVE:
		FileSaveAction();
		break;
	case ACTION_FILE_SAVE_AS:
		FileSaveAsAction();
		break;
	case ACTION_FILE_EXIT:
		FileExitAction();
		break;
	default:
		break;
	}
}

void CFxEditor::ProcessSaveChangesPopup()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5F, 0.5F));

	bool saveChangesOpen = true;

	if (ImGui::BeginPopupModal("Save Changes?###SaveChanges", &saveChangesOpen, ImGuiWindowFlags_AlwaysAutoResize))
	{
		const char* fileName = Filename_FixFilename(mFile.GetName(), NULL, ".xmlb");

		ImGui::Text("Save changes to %s?", fileName);
		ImGui::Separator();

		if (ImGui::Button("Yes", ImVec2(80, 0)))
		{
			ImGui::CloseCurrentPopup();
			SaveFile();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();

		if (ImGui::Button("No", ImVec2(80, 0)))
		{
			ImGui::CloseCurrentPopup();
			ExecutePendingAction();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(80, 0)))
		{
			ImGui::CloseCurrentPopup();
			ResetPendingAction();
		}

		ImGui::EndPopup();
	}

	//printf("Save Changes closed\n");//TODO: Handle close button?
	if (!saveChangesOpen)
	{

	}
}

void CFxEditor::ProcessOpenFileDialog()
{
	//if (ImGuiFileDialog::Instance()->Display(FILE_OPEN_DIALOG))
	//{
	//	bool showError = false;
	//	
	//	if (ImGuiFileDialog::Instance()->IsOk())
	//	{
	//		fs::path absolutePath = ImGuiFileDialog::Instance()->GetFilePathName();
	//		fs::path relativePath = fs::relative(absolutePath);
	//
	//		if (relativePath != mFilePath)//TODO: Check dirty flag?
	//		{
	//			std::string filePath = relativePath.string();
	//
	//			//TODO: Check if relative path is not below cwd, otherwise show error and don't set file path
	//
	//			if (mFile.LoadFromFile(filePath.c_str()))
	//			{
	//				ResetPlayState();
	//				mFilePath = relativePath;
	//
	//				std::string fileName = relativePath.filename().string();
	//				mFile.SetName(fileName.c_str());
	//				mPlayback.Initialize(mFile.GetTemplate());
	//			}
	//			else
	//			{
	//				showError = true;
	//			}
	//		}
	//	}
	//
	//	ImGuiFileDialog::Instance()->Close();
	//
	//	if (showError)
	//	{
	//		ShowError("Failed to open file");
	//	}
	//}

	if (effectOpenDlg.Display())
	{		
		if (effectOpenDlg.IsConfirmed())
		{
			fs::path relativePath;
			bool success = effectOpenDlg.RelativePath(effectOpenDlg.GetFilePath().c_str(), relativePath);

			if (success)
			{
				if (mFile.IsDirty() || relativePath != mFilePath)//TODO: Check dirty flag?
				{
					std::string filePath = relativePath.string();

					if (mFile.LoadFromFile(filePath.c_str()))
					{
						ResetPlayState();
						mFilePath = relativePath;

						std::string fileName = relativePath.filename().string();
						mFile.SetName(fileName.c_str());
						mPlayback.Initialize(mFile.GetTemplate());
					}
					else
					{
						ShowError("Failed to open file");
					}
				}
			}
		}
	
		effectOpenDlg.CloseDialog();
	}
}

void CFxEditor::ProcessSaveFileDialog()
{
	//if (ImGuiFileDialog::Instance()->Display(FILE_SAVE_DIALOG))
	//{
	//	if (ImGuiFileDialog::Instance()->IsOk())
	//	{
	//		fs::path absolutePath = ImGuiFileDialog::Instance()->GetFilePathName();
	//		fs::path relativePath = fs::relative(absolutePath);
	//		std::string filePath = relativePath.string();
	//
	//		//TODO: Check if relative path is not below cwd, otherwise show error and don't set file path
	//		if (SaveFile(filePath.c_str()))
	//		{
	//			mFilePath = relativePath;
	//
	//			std::string fileName = relativePath.filename().string();
	//			mFile.SetName(fileName.c_str());
	//		}
	//	}
	//
	//	ResetPendingAction();
	//	ImGuiFileDialog::Instance()->Close();
	//}

	if (effectSaveDlg.Display())
	{
		if (effectSaveDlg.IsConfirmed())
		{
			fs::path relativePath;
			bool success = effectSaveDlg.RelativePath(effectSaveDlg.GetFilePath().c_str(), relativePath, true);

			if (success)
			{
				std::string filePath = relativePath.string();

				if (SaveFile(filePath.c_str()))
				{
					mFilePath = relativePath;

					std::string fileName = relativePath.filename().string();
					mFile.SetName(fileName.c_str());
				}
			}
		}

		ResetPendingAction();
		effectSaveDlg.CloseDialog();
	}
}

void CFxEditor::ProcessErrorPopup()
{
	if (mShowError)
	{
		ImGui::OpenPopup("###ErrorMessage");
		mShowError = false;
	}
	
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5F, 0.5F));

	bool errorOpen = true;

	if (ImGui::BeginPopupModal("Error###ErrorMessage", &errorOpen, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextWrapped("%s", mErrorMessage.c_str());
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(80, 0)))
		{
			ImGui::CloseCurrentPopup();
			mErrorMessage.clear();
		}

		ImGui::SetItemDefaultFocus();

		ImGui::EndPopup();
	}

	//TODO: Handle close button?
	if (!errorOpen)
	{

	}
}

void CFxEditor::ShowError(const char* message)
{
	mShowError = true;
	mErrorMessage = message;
}

void CFxEditor::MainMenuBar()
{
	int actionId = ACTION_NONE;

	if (ImGui::BeginMainMenuBar())
	{
		actionId = FileMenu();
		EditMenu();
		ViewMenu();
		ImGui::EndMainMenuBar();
	}

	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_N, ImGuiInputFlags_RouteGlobal))
	{
		actionId = ACTION_FILE_NEW;
	}

	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_O, ImGuiInputFlags_RouteGlobal))
	{
		actionId = ACTION_FILE_OPEN;
	}

	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S, ImGuiInputFlags_RouteGlobal))
	{
		actionId = ACTION_FILE_SAVE;
	}

	if (ImGui::Shortcut(ImGuiKey_Escape, ImGuiInputFlags_RouteGlobal))
	{
		actionId = ACTION_FILE_EXIT;
	}

	if (actionId != ACTION_NONE)
	{
		bool executeAction = true;
		
		if (actionId == ACTION_FILE_NEW || 
			actionId == ACTION_FILE_OPEN || 
			actionId == ACTION_FILE_EXIT)
		{
			if (mFile.IsDirty())
			{
				mPendingActionId = actionId;
				ImGui::OpenPopup("###SaveChanges");
				executeAction = false;
			}
		}

		if (executeAction)
		{
			ExecuteAction(actionId);
		}
	}

	//TODO: If we are opening same file, don't do anything?
	//TODO: Set dialog current path to mFilePath folder, otherwise to "effects/" folder

	ProcessSaveChangesPopup();
	ProcessOpenFileDialog();
	ProcessSaveFileDialog();
	ProcessErrorPopup();
}

void CFxEditor::ExecutePendingAction()
{
	if (mPendingActionId != ACTION_NONE)
	{
		ExecuteAction(mPendingActionId);
		ResetPendingAction();
	}
}

void CFxEditor::ResetPendingAction()
{
	mPendingActionId = ACTION_NONE;
}

bool CFxEditor::SaveFile(const char* fileName)
{	
	if (fileName || !mFilePath.empty())
	{
		std::string savePath = fileName ? fileName : mFilePath.string();

		if (mFile.SaveToFile(savePath.c_str()))
		{
			ExecutePendingAction();
			return true;
		}
		else
		{
			ShowError("Failed to save file");
			ResetPendingAction();
		}
	}
	else
	{
		//OpenFileDialog(FILE_SAVE_DIALOG, "Save File", ".xmlb", mFilePath);
		effectSaveDlg.SetInitialPath(mFilePath.string().c_str(), "effects/");
		effectSaveDlg.OpenDialog();
	}

	return false;
}

int CFxEditor::FileMenu()
{
	int id = ACTION_NONE;
	
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New", "Ctrl+N"))
		{
			id = ACTION_FILE_NEW;
		}

		if (ImGui::MenuItem("Open...", "Ctrl+O"))
		{
			id = ACTION_FILE_OPEN;
		}

		bool saveEnabled = mFile.IsDirty();

		if (ImGui::MenuItem("Save", "Ctrl+S", (bool*)NULL, saveEnabled))
		{
			id = ACTION_FILE_SAVE;
		}

		if (ImGui::MenuItem("Save As..."))
		{
			id = ACTION_FILE_SAVE_AS;
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Exit", "Esc"))
		{
			id = ACTION_FILE_EXIT;
		}

		ImGui::EndMenu();
	}

	return id;
}

int CFxEditor::EditMenu()
{
	if (ImGui::BeginMenu("Edit"))
	{
		bool canMofidy = HasSelectedSegment();
		
		if (ImGui::MenuItem("Clone", "Ctrl+C", (bool*)NULL, canMofidy))
		{
			CloneSelectedSegment();
		}

		if (ImGui::MenuItem("Delete", "Del", (bool*)NULL, canMofidy))
		{
			DeleteSelectedSegment();
		}

		ImGui::EndMenu();
	}
	
	return 0;
}

int CFxEditor::ViewMenu()
{
	if (ImGui::BeginMenu("View"))
	{
		ImGui::MenuItem("Playback", NULL, &mPlaybackWindowVisible);

		bool spawnWindowVisible = mSpawn.IsVisible();

		if (ImGui::MenuItem("Spawn", NULL, &spawnWindowVisible))
		{
			mSpawn.SetVisible(spawnWindowVisible);
		}

		ImGui::MenuItem("World", NULL, &mWorldWindowVisible);
		ImGui::MenuItem("Status", NULL, &mStatusWindowVisible);

		ImGui::Separator();

		ImGui::MenuItem("Fullscreen Effects", NULL, &g_fullscreenFXEnabled);

		ImGui::Separator();

		if (ImGui::MenuItem("Reset/zero view position"))
		{
			mCamera.Reset();
			mCamera.Initialize();
		}

		ImGui::EndMenu();
	}
	
	return 0;
}

void CFxEditor::PlaybackBar()
{
	if (!mPlaybackWindowVisible)
	{
		return;
	}
	
	if (ImGui::Begin("Playback##PlaybackBar", &mPlaybackWindowVisible, ImGuiWindowFlags_AlwaysAutoResize))
	{
		{
			GroupSection sectionA(mFile.HasSegments());
			
			if (ToggleButton("Play", mIsPlaying))
			{
				//TODO: Reset here too?
				mIsPlaying = !mIsPlaying;

				if (mIsPlaying && mPlayback.GetRepeatMode() == REPEAT_FOR_DURATION)
				{
					UpdateStopTime();
				}
			}

			ImGui::SameLine();

			if (ToggleButton("Pause", mIsPaused))
			{
				SetPaused(!mIsPaused);
			}

			ImGui::SameLine();

			{
				GroupSection sectionB(CanBeStopped());

				if (ImGui::Button("Stop"))
				{
					ResetPlayState(false);
				}
			}
		}

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical); ImGui::SameLine();

		if (ImGui::Button("Settings"))
		{
			mPlayback.SetVisible(!mPlayback.IsVisible());
		}
	}

	ImGui::End();
}

void CFxEditor::StatusBar()
{
	if (!mStatusWindowVisible)
	{
		return;
	}
	
	if (ImGui::Begin("Status Bar", &mStatusWindowVisible, ImGuiWindowFlags_AlwaysAutoResize))
	{
		CFxManager& fxManager = reinterpret_cast<CFxManager&>(TheMenuFxManager());
		int activeCount = fxManager.mActivePrimitives.size();
		int scheduledCount = fxManager.mScheduledFxQueue.size();
		int persistentCount = fxManager.mPersistentEffects.size();

		ImGui::Text("Active: %d / Scheduled: %d / Persistent: %d", activeCount, scheduledCount, persistentCount);
	}

	ImGui::End();
}

EFXPrimType fxSegmentTypes[FXPRIM_TYPE_NUM_TYPES - 2] =
{
	FXPRIM_TYPE_BEZIER,
	FXPRIM_TYPE_CAMERA_SHAKE,
	FXPRIM_TYPE_CYLINDER,
	FXPRIM_TYPE_EMPTY,
	FXPRIM_TYPE_LIGHT,
	FXPRIM_TYPE_LIGHTNING,
	FXPRIM_TYPE_LINE,
	FXPRIM_TYPE_MODEL,
	FXPRIM_TYPE_ORIENTED_SPRITE,
	FXPRIM_TYPE_PARTICLE_CLOUD,
	FXPRIM_TYPE_SCREENFLASH,
	FXPRIM_TYPE_SPARK,
	FXPRIM_TYPE_SPRITE,
	FXPRIM_TYPE_TRAIL,
	FXPRIM_TYPE_WEATHERFX,
};

void CFxEditor::EffectSegmentsWindow()
{
	//TODO: When saving unsaved file, file name is not updated
	
	ratl::string_vs<64> label;
	FormatToStringBuffer(label.c_str(), label.capacity(), "%s###EffectSegments", mFile.GetName());
	
	ImGuiWindowFlags flags = 0;

	if (mFile.IsDirty())
	{
		flags |= ImGuiWindowFlags_UnsavedDocument;
	}

	if (ImGui::Begin(label.c_str(), NULL, flags))
	{
		if (HasSelectedSegment())
		{
			if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_C))
			{
				CloneSelectedSegment();
			}

			if (ImGui::Shortcut(ImGuiKey_Delete))
			{
				DeleteSelectedSegment();
			}
		}
		
		ImGui::SeparatorText("Segments");

		{
			GroupSection sectionA(mFile.IsValid());

			{
				GroupSection sectionB(mFile.CanAddSegment());
				
				if (ImGui::Button("New"))
				{
					ImGui::OpenPopup("SegmentTypes");
				}
			}

			if (ImGui::BeginPopup("SegmentTypes"))
			{
				for (int i = 0; i < IM_ARRAYSIZE(fxSegmentTypes); i++)
				{
					EFXPrimType type = fxSegmentTypes[i];
					const char* name = GetPrimitiveDisplayName(type);

					if (ImGui::Selectable(name))
					{
						IFxSegment* segment = mFile.NewSegment(EFXPrimType(type));

						if (segment)
						{
							SetSelectedSegment(segment);
						}
					}
				}

				ImGui::EndPopup();
			}

			ImGui::SameLine();

			{
				GroupSection sectionB(HasSelectedSegment());

				if (ImGui::Button("Delete"))
				{
					DeleteSelectedSegment();
				}

				ImGui::SameLine();

				if (ImGui::Button("Clone"))
				{
					CloneSelectedSegment();
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Filter"))
			{
				ImGui::OpenPopup("SegmentFilter");
			}

			if (ImGui::BeginPopup("SegmentFilter"))
			{
				if (mSegmentFilter.Update())
				{
					mFile.UpdateTemplateLinks();

					if (HasSelectedSegment() && !mSegmentFilter.Matches(GetSelectedSegment()))
					{
						SetSelectedSegment(NULL);
					}
				}

				ImGui::EndPopup();
			}

			mSegmentTable.Update();
		}
	}

	ImGui::End();
}

void CFxEditor::SegmentPropertiesWindow()
{
	if (ImGui::Begin("Segment Properties"))
	{
		//TODO: Always select first tab
		
		if (HasSelectedSegment())
		{
			mSelectedSegment->Draw();
		}
		else
		{
			ImGui::Text("No Segment selected");
		}
	}

	ImGui::End();
}

void CFxEditor::WorldSettings()
{
	if (!mWorldWindowVisible)
	{
		return;
	}
	
	if (ImGui::Begin("World Settings", &mWorldWindowVisible, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (ImGui::SliderFloat("Time Scale", &mTimeScale, 0.1F, 10.0F, "%g", ImGuiSliderFlags_AlwaysClamp))
		{
			SetTimeScale(mTimeScale);
		}

		ImGui::Checkbox("Draw Grid", &mDrawGrid);
		ImGui::Checkbox("Draw Axes", &mDrawAxes);

		ImGui::ColorEdit4("Background Color", mBackgroundColor.vec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
		ImGui::ColorEdit4("Floor Color", mFloorColor.vec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
	}

	ImGui::End();
}

void CFxEditor::RunFrame()
{
	if (!mActive)
	{
		return;
	}
	
	//ImGui::GetIO().MouseDrawCursor = true;

	//TODO: Bug: Sometimes if i hold mouse button before goint into FX Editor, it will move camera even if i currently doesn't hold it anymore
	mCamera.RunFrame();
	mPlayback.Update();
	mSpawn.Update();

	MainMenuBar();
	PlaybackBar();
	StatusBar();

	EffectSegmentsWindow();
	SegmentPropertiesWindow();
	WorldSettings();
}

void CFxEditor::PlayEffect(int handle)
{	
	if (TheMenuFxManager().IsEffectPersistent(handle))
	{
		CPlayFx playFx(handle, mSpawnOrigin, mSpawn.GetForwardAxis());

		playFx.OverrideFxLife(mSpawn.GetLife());
		playFx.ScaleFxTime(mSpawn.GetTimeScale());
		playFx.ScaleFxSize(mSpawn.GetSizeScale());
		playFx.SetFxLevel(mSpawn.GetFxLevel());
		playFx.SetColor(mSpawn.GetColor());
		TheMenuFxManager().PlayPersistent(playFx, mPersistFXHandle);
	}
	else
	{
		CPlayFx playFx(handle, mSpawnOrigin, mSpawn.GetForwardAxis());
		
		playFx.OverrideFxLife(mSpawn.GetLife());
		playFx.ScaleFxTime(mSpawn.GetTimeScale());
		playFx.ScaleFxSize(mSpawn.GetSizeScale());
		playFx.SetFxLevel(mSpawn.GetFxLevel());
		playFx.SetColor(mSpawn.GetColor());
		TheMenuFxManager().Play(playFx);
	}
}

void CFxEditor::UpdatePlayState()
{
	if (!mIsPlaying || !mFile.IsValid())
	{
		return;
	}
	
	int handle = mFile.GetTemplateHandle();
	ERepeatMode repeatMode = mPlayback.GetRepeatMode();

	if (repeatMode == REPEAT_ONCE)
	{
		PlayEffect(handle);
		mIsPlaying = false;
	}
	else
	{
		float time = GetTime();

		if (repeatMode == REPEAT_FOR_DURATION && time >= mStopTime)
		{
			mIsPlaying = false;
			//TODO: maybe reset vars?
			mStopTime = 0.0F;
			return;
		}

		if (mPersistFXHandle)
		{
			return;
		}

		//TODO: Fix
		if (mPlayback.IsAnimateSpawnPoint())
		{
			float timeDelta = GetTimeDelta();

			mSpawnOrigin += mPlayback.GetSpawnPointVelocity() * timeDelta;

			if (time >= mAnimateSpawnPointTime)
			{
				mSpawnOrigin = mSpawn.GetOrigin();
				mAnimateSpawnPointTime = time + mPlayback.GetSpawnPointResetTime();
			}
		}
		else
		{
			//TODO: Why?
			mSpawnOrigin = mSpawn.GetOrigin();
		}

		if (mPlayback.IsRespawnEveryFrame())
		{
			PlayEffect(handle);
		}
		else if (time >= mNextPlayTime)
		{
			PlayEffect(handle);

			float loopTime = TheMenuFxManager().GetLoopTime(handle, false);
			mNextPlayTime = time + loopTime;
		}
	}
}

bool HasFx()
{
	CFxManager& manager = reinterpret_cast<CFxManager&>(TheMenuFxManager());
	return manager.mActivePrimitives.size() > 0 ||
		manager.mScheduledFxQueue.size() > 0 || 
		manager.mPersistentEffects.size() > 0;
}

bool CFxEditor::CanBeStopped() const
{
	return mIsPlaying || HasFx();
}

void CFxEditor::UpdateGameCamera()
{
	const igMatrix44f& mat = mCamera.GetCameraMatrix();

	igVec3f cameraPos;
	mat.getTranslation(cameraPos);

	igVec3f axes[3];
	mat.getRow(2, axes[0]);
	mat.getRow(0, axes[1]);
	mat.getRow(1, axes[2]);

	CMarvelCamera& camera = reinterpret_cast<CMarvelCamera&>(TheCamera());

	camera.mCurrentViewpoint = cameraPos;
	camera.mCameraForward = axes[0];
	camera.mCameraRight = axes[1];
	camera.mCameraUp = axes[2];
	camera.mCameraForward.negate();

	//SetCameraFOV
}

void CFxEditor::Draw()
{
	TheAlchemyDisplay().GetVC()->setClearColor(mBackgroundColor);
	TheMenuMgr().SetPlayfieldCameraMtx(1, mCamera.GetCameraMatrix());
	UpdatePlayState();
	UpdateGameCamera();
}

void CFxEditor::UpdateTable()
{
	if (mFile.IsValid())
	{
		mSegmentTable.MarkDirty();
	}
}

void CFxEditor::UpdateStopTime()
{
	mStopTime = GetTime() + mPlayback.GetPlayDuration();
}

IFxEditor& TheFxEditor()
{
	static CFxEditor fxEditor;
	return fxEditor;
}
