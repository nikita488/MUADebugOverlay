#include "CConversationWindow.h"

#include <string>
#include <imgui/imgui.h>

#include <Game/Conversation/CConversationSystem.h>

void AddLineBlock(CLineBlock* line, CConversationFile* file)
{
	if (ImGui::TreeNodeEx(line->mConversationAtom.mText, ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto responseIt = line->mBlocks.begin(); responseIt != line->mBlocks.end(); responseIt++)
		{
			int responseHandle = *responseIt;
			CResponseBlock* response = file->field_468[responseHandle];

			if (ImGui::TreeNodeEx(response->mConversationAtom.mText, ImGuiTreeNodeFlags_DefaultOpen))
			{
				for (auto lineIt = response->mBlocks.begin(); lineIt != response->mBlocks.end(); lineIt++)
				{
					int lineHandle = *lineIt;
					CLineBlock* line = file->field_1100[lineHandle];

					AddLineBlock(line, file);
				}

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
}

void CConversationWindow::Draw()
{
	if (ImGui::Begin("Conversation"))
	{
		CConversationSystem& convSystem = TheCConversationSystem();

		if (theConvStringPool)
		{
			ImGui::Text("String Pool (Small): %d / %d", theConvStringPool->size(), theConvStringPool->capacity());
		}

		if (theConvStringPoolBig)
		{
			ImGui::Text("String Pool (Big): %d / %d", theConvStringPoolBig->size(), theConvStringPoolBig->capacity());
		}

		ImGui::Text("Files: %d / %d", convSystem.mFiles.size(), convSystem.mFiles.capacity());

		for (auto it = convSystem.mFiles.begin(); it != convSystem.mFiles.end(); it++)
		{
			CConversationFile* file = it.value();

			if (ImGui::TreeNode(it.key().c_str()))
			{
				ImGui::Text("Conversations: %d / %d", file->field_0.size(), file->field_0.capacity());
				ImGui::Text("Response Blocks: %d / %d", file->field_468.size(), file->field_468.capacity());
				ImGui::Text("Line Blocks: %d / %d", file->field_1100.size(), file->field_1100.capacity());

				//if (file->field_1616 > 0 && file->field_1628)
				//{
				//	AddLineBlock(file->field_1628, file);
				//}

				//if (false && ImGui::TreeNode(line->mConversationAtom.mText))
				//{
				//	for (auto responseIt = line->mBlocks.begin(); responseIt != line->mBlocks.end(); responseIt++)
				//	{
				//		int responseHandle = *responseIt;
				//		CResponseBlock* response = file->field_468[responseHandle];
				//
				//		if (ImGui::TreeNode(response->mConversationAtom.mText))
				//		{
				//			
				//			for (auto lineIt = response->mBlocks.begin(); lineIt != response->mBlocks.end(); lineIt++)
				//			{
				//				int lineHandle = *lineIt;
				//				CLineBlock* response = file->field_1100[lineHandle];
				//
				//				if (ImGui::TreeNode(response->mConversationAtom.mText))
				//				{
				//
				//					ImGui::TreePop();
				//				}
				//			}
				//
				//			ImGui::TreePop();
				//		}
				//	}
				//}

				if (false && ImGui::TreeNode("Response Blocks"))
				{
					CResponseBlock* block = file->field_1632;

					for (int i = 0; i < file->field_1612; i++, block++)
					{
						const CConversationAtom& atom = block->mConversationAtom;

						std::string characterName = atom.mCharName.c_str();

						//igStringObjRef label = NewInstanceRef<igStringObj>(kIGMemoryPoolTemporary);
						//
						//if (!characterName.empty())
						//{
						//	label->printf("%s: %s", characterName, atom.mText.c_str());
						//}
						//else
						//{
						//	label->set(atom.mText.c_str());
						//}

						//if (ImGui::TreeNode(label->getBuffer()))
						if (ImGui::TreeNode(atom.mText.c_str()))
						{
							ImGui::Text("field_152: %d", block->field_152);

							ImGui::Text("text: %s", atom.mText.c_str());
							ImGui::Text("scriptFile: %s", atom.mScriptFile.c_str());//8 scriptFile
							ImGui::Text("chosenScriptFile: %s", atom.mRespChosenScript.c_str());//16 chosenScriptFile
							ImGui::Text("scriptCommand: %s", atom.mScriptCommand.c_str());//24 scriptCommand
							ImGui::Text("conditionScript: %s", atom.mConditionScript.c_str());//32 conditionScript
							ImGui::Text("tagIndex: %s", atom.mTagIndex.c_str());//40 tagIndex
							ImGui::Text("tagJump: %s", atom.mTagJump.c_str());//48 tagJump
							ImGui::Text("includeCharacter: %s", atom.mIncludeCharacters.c_str());//56 includeCharacter
							ImGui::Text("excludeCharacter: %s", atom.mExcludeCharacters.c_str());//64 excludeCharacter
							ImGui::Text("soundToPlay: %s", atom.mSoundToPlay.c_str());//72 soundToPlay
							ImGui::Text("Character Name: %s", atom.mCharName.c_str());//80 ?
							ImGui::Text("File Name: %s", atom.mFileName.c_str());//88 CConversationSystem::mExpandedFileName
							ImGui::Text("actorAnimation: %s", atom.mActorAnimation.c_str());//96 actorAnimation

							bool noTalkAnim = atom.mNoTalkAnim;
							bool conversationEnd = atom.mConversationEnd;

							ImGui::Checkbox("noTalkAnim", &noTalkAnim);//104|1 noTalkAnim
							ImGui::Checkbox("conversationEnd", &conversationEnd);//104|2 conversationEnd

							ImGui::Text("field_106: %d", atom.field_106);
							ImGui::Text("mTimeDelay: %f", atom.mTimeDelay);//108 timeDelay
							ImGui::Text("field_112: %d", atom.mHandle);

							ImGui::Text("Blocks: %d / %d", block->mBlocks.size(), block->mBlocks.capacity());

							ImGui::TreePop();
						}
					}

					ImGui::TreePop();
				}

				if (false && ImGui::TreeNode("Line Blocks"))
				{
					CLineBlock* block = file->field_1628;

					for (int i = 0; i < file->field_1616; i++, block++)
					{
						const CConversationAtom& atom = block->mConversationAtom;



						std::string characterName = atom.mCharName.c_str();

						//igStringObjRef label = NewInstanceRef<igStringObj>(kIGMemoryPoolTemporary);
						//
						//if (!characterName.empty())
						//{
						//	label->printf("%s: %s", characterName, atom.mText.c_str());
						//}
						//else
						//{
						//	label->set(atom.mText.c_str());
						//}

						//if (ImGui::TreeNode(label->getBuffer()))
						if (ImGui::TreeNode(atom.mText.c_str()))
						{
							ImGui::Text("field_152: %d", block->field_152);

							ImGui::Text("text: %s", atom.mText.c_str());
							ImGui::Text("scriptFile: %s", atom.mScriptFile.c_str());//8 scriptFile
							ImGui::Text("chosenScriptFile: %s", atom.mRespChosenScript.c_str());//16 chosenScriptFile
							ImGui::Text("scriptCommand: %s", atom.mScriptCommand.c_str());//24 scriptCommand
							ImGui::Text("conditionScript: %s", atom.mConditionScript.c_str());//32 conditionScript
							ImGui::Text("tagIndex: %s", atom.mTagIndex.c_str());//40 tagIndex
							ImGui::Text("tagJump: %s", atom.mTagJump.c_str());//48 tagJump
							ImGui::Text("includeCharacter: %s", atom.mIncludeCharacters.c_str());//56 includeCharacter
							ImGui::Text("excludeCharacter: %s", atom.mExcludeCharacters.c_str());//64 excludeCharacter
							ImGui::Text("soundToPlay: %s", atom.mSoundToPlay.c_str());//72 soundToPlay
							ImGui::Text("Character Name: %s", atom.mCharName.c_str());//80 ?
							ImGui::Text("File Name: %s", atom.mFileName.c_str());//88 CConversationSystem::mExpandedFileName
							ImGui::Text("actorAnimation: %s", atom.mActorAnimation.c_str());//96 actorAnimation

							bool noTalkAnim = atom.mNoTalkAnim;
							bool conversationEnd = atom.mConversationEnd;

							ImGui::Checkbox("noTalkAnim", &noTalkAnim);//104|1 noTalkAnim
							ImGui::Checkbox("conversationEnd", &conversationEnd);//104|2 conversationEnd

							ImGui::Text("field_106: %d", atom.field_106);
							ImGui::Text("mTimeDelay: %f", atom.mTimeDelay);//108 timeDelay
							ImGui::Text("field_112: %d", atom.mHandle);

							ImGui::Text("Blocks: %d / %d", block->mBlocks.size(), block->mBlocks.capacity());

							ImGui::TreePop();
						}
					}

					ImGui::TreePop();
				}

				ImGui::TreePop();
			}
		}
	}

	ImGui::End();
}