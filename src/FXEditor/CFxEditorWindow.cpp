#include "CFxEditorWindow.h"

#include <imgui/imgui.h>

#include <Client/CClient.h>
#include <FX/IFxManager.h>
#include <UI/IMenuSystem.h>
#include <Scene/CPlayfield.h>
#include <Scene/SceneCommon.h>
#include "CFxEditor.h"
#include "CTextureAssetCache.h"
#include "CModelAssetCache.h"
#include "CFxAssetCache.h"

#include <Windows.h>
#include <commdlg.h>
#include <Display/IAlchemyDisplay.h>
#include <Display/CDisplay.h>

#include <igGapDisplay.h>
#include <igDisplay/igWin32Window.h>

#include <igGapGfx.h>
#include <igGfx/igDx9VisualContext.h>

#include <thread>
#include <atomic>
#include <mutex>
#include <string>

#include <Game/CActor.h>
#include <Game/ICamera.h>
#include <Input/IInput.h>
#include <Scene/ITextureCache.h>
#include <Scene/CTexture.h>
#include <Scene/CModel.h>
#include <Scene/IScene.h>
#include <Scene/CZonePlayfield.h>

#include <igGapGfx.h>
#include <igGfx/igDx9VisualContext.h>

#include "Segments/PropertyGroups/FxImGuiWidgets.h"

#include <UI/CHud.h>

#include <Client/ILoadThread.h>

using namespace Gap;

bool g_gammaEnabled = true;

std::mutex pathMutex;
std::string path;
std::atomic<bool> ready(false);

void OpenFileFunc(HWND hWnd)
{
	ShowCursor(true);
	
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[MAX_PATH];       // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0Text\0*.XMLB\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn) == TRUE)
	{
		std::lock_guard<std::mutex> lock(pathMutex);
		path = ofn.lpstrFile;
		ready = true;
	}
}

PDIRECT3DTEXTURE9 ptr = NULL;
int width = -1;
int height = -1;

#include <UI/CMenuSystem.h>

void CFxEditorWindow::Draw()
{
	if (ImGui::Begin("FX Editor"))
	{
		for (auto it = menuFactoryMenus.begin(); it != menuFactoryMenus.end(); it++)
		{
			auto& key = it.key();
			auto& val = it.value();

			ImGui::Text("%s: %p", key.c_str(), &val);
		}
		
		ImGui::Checkbox("Gamma", &g_gammaEnabled);
		
		IFxEditor& editor = TheFxEditor();
		CTextureAssetCache& textureCache = reinterpret_cast<CTextureAssetCache&>(editor.TextureCache());
		CModelAssetCache& modelCache = reinterpret_cast<CModelAssetCache&>(editor.ModelCache());
		CFxAssetCache& fxCache = reinterpret_cast<CFxAssetCache&>(editor.FxCache());
		
		if (ImGui::TreeNode("Textures"))
		{
			for (auto& it = textureCache.mCachedAssets.begin(); it != textureCache.mCachedAssets.end(); it++)
			{
				ImGui::Text("%s (Count: %d)", it->first.c_str(), it->second.mRefCount);
			}

			ImGui::SeparatorText("Permanent");

			for (auto& it = textureCache.mPermanentCache.begin(); it != textureCache.mPermanentCache.end(); it++)
			{
				ImGui::Text("%s", it->c_str());
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Models"))
		{
			for (auto& it = modelCache.mCachedAssets.begin(); it != modelCache.mCachedAssets.end(); it++)
			{
				ImGui::Text("%s (Count: %d)", it->first.c_str(), it->second.mRefCount);
			}

			ImGui::SeparatorText("Permanent");

			for (auto& it = modelCache.mPermanentCache.begin(); it != modelCache.mPermanentCache.end(); it++)
			{
				ImGui::Text("%s", it->c_str());
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("FX"))
		{
			for (auto& it = fxCache.mCachedAssets.begin(); it != fxCache.mCachedAssets.end(); it++)
			{
				ImGui::Text("%s (Count: %d)", it->first.c_str(), it->second.mRefCount);
			}

			ImGui::SeparatorText("Permanent");

			for (auto& it = fxCache.mPermanentCache.begin(); it != fxCache.mPermanentCache.end(); it++)
			{
				ImGui::Text("%s", it->c_str());
			}

			ImGui::TreePop();
		}
		
		if (ImGui::Button("Dump"))
		{
			CPlayfield* plf = ToConcrete<CPlayfield>(TheMenuMgr().GetPlayfieldPtr(1));

			DumpIGB(plf->field_12, "C:\\menu_plf_1.igb");
		}

		if (ImGui::Button("Unload Menu Effects"))
		{
			TheMenuFxManager().Unload(MP_MENU);
		}

		float scale = TheMenuMgr().GetTimeScale();

		if (ImGui::SliderFloat("TimeScale", &scale, 0.1F, 10.0F))
		{
			TheMenuMgr().SetTimeScale(scale);
		}

		if (ImGui::Button("Open effect"))
		{
			Display::igWin32Window* wnd = Display::igWin32Window::dynamicCast(TheAlchemyDisplay().GetWindow());
			std::thread(OpenFileFunc, wnd->getNativeWindowHandle()).detach();
		}

		if (ImGui::Button("Spawn effect"))
		{
			CEntityID entity = CActor::GroupLeader(TheInput().PrimaryPlayerId());

			if (entity.IsValid())
			{
				const igVec3f& origin = entity->Origin();
				int handle = TheFxManager().Find("test_effect", MP_ZONE);
				CPlayFx playFx(handle, origin, igVec3f::ZeroVector);

				playFx.SetBolt1Info(0, EBOLT_HEAD, estring(""));
				TheFxManager().Play(playFx);
			}
		}

		const igVec3f& focusPoint = TheCamera().GetFocusPoint();

		ImGui::Text("Camera Focus Point: %f %f %f", focusPoint[0], focusPoint[1], focusPoint[2]);

		if (ImGui::Button("Image Test"))
		{
			//ITexturePtr& texture = TheTextureCache().CacheTexture("textures/rock_bottom.png", MP_MENU, false, true);
			//Gfx::igDx9VisualContext* vc = Gfx::igDx9VisualContext::dynamicCast(TheAlchemyDisplay().GetVC());
			//
			//if (vc)
			//{
			//	Gfx::igDxTextureHelper* helper = vc->getTextureHelper();
			//	CTexture* texturee = ToConcrete<CTexture>(texture);
			//	
			//	//texturee->GetTextureAttr()->setup(vc);
			//	texturee->GetTextureAttr()->apply(vc);
			//
			//	int handle = texturee->GetTextureAttr()->getHandle();
			//
			//	vc->loadTexture(handle);
			//	//vc->unloadTexture(handle);
			//	//vc->deleteTexture(handle);
			//
			//	Gfx::Texture* tex = helper->getTexture(handle);
			//	ptr = tex->castTexture9();
			//
			//	width = texturee->GetWidth();
			//	height = texturee->GetHeight();
			//}
		}

		//if (ptr && width != -1 && height != -1)
		//{
		//	ImGui::Image((ImTextureID)(intptr_t)ptr, ImVec2(width, height));
		//}

		CDisplay& display = (CDisplay&)TheDisplay();

		for (int i = 0; i < 8; i++)
		{
			int texHandle = display.field_420[i];
			
			if (texHandle != -1)
			{
				ImGui::Text("Texture Id: %d", i);
				Gfx::igDx9VisualContext* vc = Gfx::igDx9VisualContext::dynamicCast(TheAlchemyDisplay().GetVC());

				if (vc)
				{
					Gfx::igDxTextureHelper* helper = vc->getTextureHelper();

					Gfx::Texture* tex = helper->getTexture(texHandle);
					PDIRECT3DTEXTURE9 texPtr = tex->castTexture9();

					if (texPtr)
					{
						ImGui::Image((ImTextureID)(intptr_t)texPtr, ImVec2(tex->_width, tex->_height));
					}
				}
			}
		}

		char buf[256];
		buf[0] = '\0';
		//InputTextButton("Shader", "..", buf, 256);

		//CImGuiFileDialog dlg;
		//dlg.test();

		static igVec3f forward = igVec3f::ZeroVector;
		static igVec3f right = igVec3f::ZeroVector;
		static igVec3f up = igVec3f::ZeroVector;

		if (ImGui::Button("NFD test"))
		{
			MakeNormalVectors(forward, right, up);
		}

		ImGui::DragFloat3("Forward", forward.vec);
		ImGui::DragFloat3("Right", right.vec);
		ImGui::DragFloat3("Up", up.vec);

		if (ImGui::CollapsingHeader("Lights"))
		{
			IPlayfieldPtr& ptr =  TheScene().GetPlayfieldInstance(PLAYFIELD_INSTANCE_ZONE);

			if (ptr.Validate())
			{
				CZonePlayfield* zone = ToConcrete<CZonePlayfield>(ptr);

				if (zone && zone->IsVisible())
				{
					int worldViewScaleIndex = zone->field_11184;
					igVec4f worldViewScale;

					Gfx::igDx9VisualContext* vc = Gfx::igDx9VisualContextRef::staticCast(TheAlchemyDisplay().GetVC());
					
					vc->getVectorConstant(worldViewScaleIndex, worldViewScale);

					ImGui::Text("World View Scale: %f %f %f %f", worldViewScale[0], worldViewScale[1], worldViewScale[2], worldViewScale[3]);

					ImGui::Text("Omni Lights: %d / %d", zone->field_39652.GetNumLights(), 4);
					
					CLightPicker& picker = zone->field_11392;

					for (int i = 0; i < picker.field_13392.size(); i++)
					{
						SOmniLight& light = picker.field_13392[picker.field_13392.index_to_handle(i)];

						ImGui::PushID(&light);
						ImGui::DragFloat("Bias", &light.field_140);
						ImGui::DragFloat("Near", &light.field_136);
						ImGui::DragFloat("Far", &light.field_68);
						ImGui::DragFloat("Hz", &light.field_64);
						ImGui::PopID();
					}

					ImGui::SeparatorText("First set");

					for (int i = 0; i < picker.field_26504.size(); i++)
					{
						SOmniLight& light = picker.field_26504[i];

						ImGui::Text("Light %d Diffuse: %f %f %f %f", i, light.field_0[0], light.field_0[1], light.field_0[2], light.field_0[3]);
						ImGui::Text("Light %d Position: %f %f %f", i, light.field_16[0], light.field_16[1], light.field_16[2]);
						ImGui::Text("Light %d Attenuation: %f %f %f", i, light.field_28[0], light.field_28[1], light.field_28[2]);
						ImGui::Text("Light %d field_40: %d", i, light.field_40);
						ImGui::Text("Light %d LightStyle: %d", i, light.field_44);
						ImGui::Text("Light %d LightStyle Params: %f %f %f", i, light.field_48[0], light.field_48[1], light.field_48[2]);
						ImGui::Text("Light %d field_60: %d", i, light.field_60);
						ImGui::Text("Light %d field_64: %f", i, light.field_64);
						ImGui::Text("Light %d Far plane: %f", i, light.field_68);
						ImGui::Text("Light %d Near plane: %f", i, light.field_136);
						ImGui::Text("Light %d field_140: %f", i, light.field_140);
					}

					ImGui::SeparatorText("Second set");

					for (int i = 0; i < picker.field_27528.size(); i++)
					{
						SOmniLight& light = picker.field_27528[i];

						ImGui::Text("Light %d Diffuse: %f %f %f %f", i, light.field_0[0], light.field_0[1], light.field_0[2], light.field_0[3]);
						ImGui::Text("Light %d Position: %f %f %f", i, light.field_16[0], light.field_16[1], light.field_16[2]);
						ImGui::Text("Light %d Attenuation: %f %f %f", i, light.field_28[0], light.field_28[1], light.field_28[2]);
						ImGui::Text("Light %d field_40: %d", i, light.field_40);
						ImGui::Text("Light %d LightStyle: %d", i, light.field_44);
						ImGui::Text("Light %d LightStyle Params: %f %f %f", i, light.field_48[0], light.field_48[1], light.field_48[2]);
						ImGui::Text("Light %d field_60: %d", i, light.field_60);
						ImGui::Text("Light %d field_64: %f", i, light.field_64);
						ImGui::Text("Light %d Far plane: %f", i, light.field_68);
						ImGui::Text("Light %d Near plane: %f", i, light.field_136);
						ImGui::Text("Light %d field_140: %f", i, light.field_140);
					}

					if (ImGui::CollapsingHeader("Constants"))
					{
						for (int i = 0; i < zone->field_39652.GetNumLights(); i++)
						{
							CShaderOmniLight& light = zone->field_39652.GetLight(i);

							int enabled = light.field_0->getData();
							const igVec4f& color = light.field_4->getData();
							const igVec4f& position = light.field_8->getData();
							const igVec4f& bounds = light.field_12->getData();

							ImGui::Text("light.omni%d.enabled: %d", i, enabled);
							ImGui::Text("light.omni%d.color: %f %f %f %f", i, color[0], color[1], color[2], color[3]);
							ImGui::Text("light.omni%d.position: %f %f %f %f", i, position[0], position[1], position[2], position[3]);
							ImGui::Text("light.omni%d.bounds: %f %f %f %f", i, bounds[0], bounds[1], bounds[2], bounds[3]);
						}
					}
				}
			}
		}
	}

	ImGui::End();

	if (ready)
	{
		std::string copy;
		
		{
			std::lock_guard<std::mutex> lock(pathMutex);
			copy = path;
			ready = false;
		}
	}
}
