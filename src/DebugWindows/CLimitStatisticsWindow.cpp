#include "CLimitStatisticsWindow.h"

#include <imgui/imgui.h>

#include <Game/Conversation/CConversationSystem.h>
#include <Game/CCharacters.h>
#include <Game/Earthworm.h>
#include <Game/CZoneInfo.h>
#include <Game/CExtractionPointSystem.h>
#include <Game/CTalentSystem.h>
#include <Game/CTalentValues.h>
#include <Game/CGame.h>
#include <Game/CSmartEnt.h>
#include <Game/CMultipartManager.h>
#include <Game/CRequirements.h>
#include <Game/CWorld.h>

#include <FX/CFxManager.h>
#include <FX/CFxPrimitiveTemplatePool.h>
#include <FX/CFxTemplateManager.h>
#include <FX/CRangedComponentPool.h>

#include <Game/ICombat.h>
#include <Game/CombatNode/CCombatNode.h>
#include <Game/CombatNode/CCombatNodeEventPool.h>
#include <Game/CombatNode/CCombatStyles.h>

#include <Game/Powerups/CPowerupPool.h>
#include <Game/Powerups/CAffecter.h>
#include <Game/Powerups/CFxCoreData.h>
#include <Game/Powerups/CPowerupBoltOn.h>
#include <Game/Powerups/CPowerupScope.h>
#include <Game/Powerups/CVirtualFunctionList.h>
#include <Game/Powerups/CAttachedPowerup.h>
#include <Game/Powerups/CAttachedPrimaryFx.h>

#include <Scene/CModelMgr.h>
#include <Scene/CIGBInfoCache2.h>
#include <Scene/CAnimMotionCache.h>
#include <Scene/CXmlCache.h>
#include <Scene/CMotionPathCache.h>
#include <Scene/CTextureCache.h>
#include <Scene/CShaderCache.h>
#include <Scene/CNodeCaches.h>
#include <Misc/CCacheHandlePool.h>

void ShowCharacterStats()
{
	CCharacters& characters = reinterpret_cast<CCharacters&>(TheCharacters());
	CMultipartManager& multipartMgr = reinterpret_cast<CMultipartManager&>(TheMultipartManager());

	ImGui::SeparatorText("Characters");
	ImGui::Text("Stats: %d / %d", characters.mStats.size(), characters.mStats.capacity());
	ImGui::Text("Characters: %d / %d", characters.mCharacterLookup.size(), characters.mCharacterLookup.capacity());
	ImGui::Text("Hero: %d / %d", characters.mStatDataHero.size(), characters.mStatDataHero.capacity());
	ImGui::Text("NPC: %d / %d", characters.mStatDataNPC.size(), characters.mStatDataNPC.capacity());
	ImGui::Text("Talent Start Info: %d / %d", staticTalentStartInfo.size(), staticTalentStartInfo.capacity());
	ImGui::Text("Stat Effect: %d / %d", statEffectInfoPool.size(), statEffectInfoPool.capacity());
	ImGui::Text("BoltOn: %d / %d", boltOnInfoPool.size(), boltOnInfoPool.capacity());
	ImGui::Text("Multipart: %d / %d", multipartMgr.field_4.size(), multipartMgr.field_4.capacity());
}

void ShowCombatNodeEventStats()
{
	auto& combatNodePool = TheCombatNodePool();

	if (ImGui::TreeNode("Node/Event Statistics"))
	{
		CCombatStyles& combatStyles = reinterpret_cast<CCombatStyles&>(TheCombatStyles());
		
		int nodes[5] = { 0 };
		int events[5] = { 0 };

		for (auto it = combatStyles.mFightStyleMap.begin(); it != combatStyles.mFightStyleMap.end(); it++)
		{
			CFightStyle& fightStyle = it.value();

			if (&fightStyle)
			{
				if (fightStyle.mMemoryPool == MP_ZONE)
				{
					fightStyle.DebugDump(false, nodes[0], events[0]);
				}
				else if (fightStyle.mMemoryPool >= MP_HERO0 && fightStyle.mMemoryPool <= MP_HERO3)
				{
					fightStyle.DebugDump(false, nodes[fightStyle.mMemoryPool - MP_SCENE_GRAPH], events[fightStyle.mMemoryPool - MP_SCENE_GRAPH]);
				}
			}
		}

		CGame& game = reinterpret_cast<CGame&>(TheGame());

		ImGui::BulletText("Zone: %d nodes, %d events", nodes[0], events[0]);
		ImGui::BulletText("Hero: %d nodes, %d events (%s)", nodes[1], events[1], game.mHeroNames[0].c_str());
		ImGui::BulletText("Hero: %d nodes, %d events (%s)", nodes[2], events[2], game.mHeroNames[1].c_str());
		ImGui::BulletText("Hero: %d nodes, %d events (%s)", nodes[3], events[3], game.mHeroNames[2].c_str());
		ImGui::BulletText("Hero: %d nodes, %d events (%s)", nodes[4], events[4], game.mHeroNames[3].c_str());
		ImGui::BulletText("Total: %d nodes, %d events", combatNodePool.size(), combatStyles.mEventPool.mPool.size());
		ImGui::TreePop();
	}
}

void ShowCombatStats()
{
	auto& requirePool = CRequirements::mRequirePool;
	auto& combatNodePool = TheCombatNodePool();
	auto& cnTriggerPool = TheCombatNodeTriggerPool();
	auto& cnChainsPool = TheCombatNodeChainsPool();
	auto& cnRareDataPool = TheCombatNodeRareDataPool();
	auto& combatStyles = reinterpret_cast<CCombatStyles&>(TheCombatStyles());;
	
	ImGui::SeparatorText("Combat");
	ImGui::Text("Fight Styles: %d / %d", combatStyles.mFightStyleMap.size(), combatStyles.mFightStyleMap.capacity());
	ImGui::Text("Nodes: %d / %d", combatNodePool.size(), combatNodePool.capacity());
	ImGui::Text("Events: %d / %d", combatStyles.mEventPool.mPool.size(), combatStyles.mEventPool.mPool.capacity());
	ImGui::Text("Triggers: %d / %d", cnTriggerPool.size(), cnTriggerPool.capacity());
	ImGui::Text("Chains: %d / %d", cnChainsPool.size(), cnChainsPool.capacity());
	ImGui::Text("Rares: %d / %d", cnRareDataPool.size(), cnRareDataPool.capacity());
	ImGui::Text("Requires: %d / %d", requirePool.size(), requirePool.capacity());

	ShowCombatNodeEventStats();
}

void ShowTalentStats()
{
	CTalentSystem& talentSystem = reinterpret_cast<CTalentSystem&>(TheTalentSystem());
	CTalentValues& talentValues = reinterpret_cast<CTalentValues&>(TheTalentValues());
	
	ImGui::SeparatorText("Talents");
	ImGui::Text("Talent Files: %d / %d", talentSystem.mXmlDocs.size(), talentSystem.mXmlDocs.capacity());
	ImGui::Text("Talents: %d / %d", talentSystem.mTalentPool.size(), talentSystem.mTalentPool.capacity());
	ImGui::Text("Talent Value Definitions: %d / %d", talentValues.mValueDefs.size(), talentValues.mValueDefs.capacity());
	ImGui::Text("Talent Values: %d / %d", talentValues.mValues.size(), talentValues.mValues.capacity());
}

void ShowFXStats()
{
	CFxManager& fxManager = reinterpret_cast<CFxManager&>(TheFxManager());
	CFxTemplateManager& fxTemplateManager = TheFxTemplateManager();
	CFxPrimitiveTemplatePool& primitiveTemplatePool = TheFxPrimitiveTemplatePool();
	TQuadPool& quadComponentPool = TheQuadPool();
	TVectorPool& vectorComponentPool = TheVectorPool();
	
	ImGui::SeparatorText("FX");
	ImGui::Text("FX Templates: %d / %d", fxTemplateManager.field_19416.size(), fxTemplateManager.field_19416.CAPACITY);
	ImGui::Text("FX Primitive Templates: %d / %d", primitiveTemplatePool.field_0.size(), primitiveTemplatePool.field_0.CAPACITY);
	ImGui::Text("Quad Components: %d / %d", quadComponentPool.field_0.size(), quadComponentPool.field_0.CAPACITY);
	ImGui::Text("Vector Components: %d / %d", vectorComponentPool.field_0.size(), vectorComponentPool.field_0.CAPACITY);
	ImGui::Text("Active Primitives: %d / %d", fxManager.mActivePrimitives.size(), fxManager.mActivePrimitives.capacity());
	ImGui::Text("Scheduled Primitives: %d / %d", fxManager.mScheduledFxQueue.size(), fxManager.mScheduledFxQueue.CAPACITY);
	ImGui::Text("Persistent Effects: %d / %d", fxManager.mPersistentEffects.size(), fxManager.mPersistentEffects.capacity());
	ImGui::Text("Handle Effects: %d / %d", fxManager.mHandleEffects.size(), fxManager.mHandleEffects.capacity());
}

void ShowCacheStats()
{
	CIGBInfoCache2& igbCache = reinterpret_cast<CIGBInfoCache2&>(TheIGBInfoCache2());
	CAnimMotionCache& animMotionCache = reinterpret_cast<CAnimMotionCache&>(TheAnimMotionCache());
	CXmlCache& xmlCache = reinterpret_cast<CXmlCache&>(TheXmlCache());
	CMotionPathCache& motionPathCache = reinterpret_cast<CMotionPathCache&>(TheMotionPathCache());
	CTextureCache& textureCache = reinterpret_cast<CTextureCache&>(TheTextureCache());
	CShaderCache& shaderCache = reinterpret_cast<CShaderCache&>(TheShaderCache());
	CCacheHandlePool& cacheHandlePool = reinterpret_cast<CCacheHandlePool&>(TheCacheHandlePool());
	
	ImGui::SeparatorText("Caches");
	ImGui::Text("Resources: %d / %d", cacheHandlePool.mCacheHandleLookup.size(), cacheHandlePool.mCacheHandleLookup.capacity());
	ImGui::Text("Xmls: %d / %d", xmlCache.mXmls.size(), xmlCache.mXmls.capacity());
	ImGui::Text("IGBs: %d / %d", igbCache.mIGBs.size(), igbCache.mIGBs.capacity());
	ImGui::Text("Anim Motions: %d / %d", animMotionCache.mAnimMotions.size(), animMotionCache.mAnimMotions.capacity());
	ImGui::Text("Textures: %d / %d", textureCache.mTextures.size(), textureCache.mTextures.capacity());
	ImGui::Text("Shaders: %d / %d", shaderCache.mShaders.size(), shaderCache.mShaders.capacity());
	ImGui::Text("Motion Paths: %d / %d", motionPathCache.mMotionPaths.size(), motionPathCache.mMotionPaths.capacity());
}

void ShowEarthwormStats()
{
	CEarthwormManager& earthwormManager = TheEarthwormManager();
	
	ImGui::SeparatorText("Earthworm");
	ImGui::Text("String Buffer Size: %d / %d", earthwormManager.mStringPool.size(), earthwormManager.mStringPool.capacity());
	ImGui::Text("Variables: %d / %d", earthwormManager.mVariablePool.size(), earthwormManager.mVariablePool.CAPACITY);
	ImGui::Text("Zone Variables: %d / %d", earthwormManager.mZoneVariablePool.size(), earthwormManager.mZoneVariablePool.CAPACITY);
	ImGui::Text("Game Variables: %d / %d", earthwormManager.mGameVariablePool.size(), earthwormManager.mGameVariablePool.CAPACITY);
	ImGui::Text("Commands: %d / %d", earthwormManager.mScriptCommandPool.size(), earthwormManager.mScriptCommandPool.CAPACITY);
	ImGui::Text("Scripts: %d / %d", earthwormManager.mScriptPool.size(), earthwormManager.mScriptPool.CAPACITY);
	ImGui::Text("Wait Count: %d / %d", earthwormManager.mWaitMap.size(), earthwormManager.mWaitMap.CAPACITY);
	ImGui::Text("Script Instances: %d / %d", earthwormManager.mScriptInstancePool.size(), earthwormManager.mScriptInstancePool.CAPACITY);
	ImGui::Text("Script Instances Queue: %d / %d", earthwormManager.mScriptInstanceQueue.size(), earthwormManager.mScriptInstanceQueue.CAPACITY);
}

void ShowWorldStats()
{
	CWorld& world = reinterpret_cast<CWorld&>(TheWorld());
	ImGui::SeparatorText("World");
	ImGui::Text("Entities: %d / %d", world.mEnts.size(), world.mEnts.capacity());

	CSmartEntMgr& smartEntMgr = reinterpret_cast<CSmartEntMgr&>(TheSmartEntMgr());
	ImGui::Text("Smart Entities: %d / %d", smartEntMgr.mSmartEnts.size(), smartEntMgr.mSmartEnts.capacity());
}

void ShowModelMgrStats()
{
	CModelMgr& modelMgr = TheModelMgr();
	
	ImGui::SeparatorText("Model Components");
	ImGui::Text("Models: %d / %d", modelMgr.mModelPool.CurrSize(), 3472);
	ImGui::Text("Transforms: %d / %d", modelMgr.mTransformPool.mPool.size(), modelMgr.mTransformPool.mPool.capacity());
	ImGui::Text("Attributes: %d / %d", modelMgr.mAttributesPool.mPool.size(), modelMgr.mAttributesPool.mPool.capacity());
	ImGui::Text("Shader FXs: %d / %d", modelMgr.mShaderFxPool.mPool.size(), modelMgr.mShaderFxPool.mPool.capacity());
	ImGui::Text("IGBs: %d / %d", modelMgr.mIGBPool.mPool.size(), modelMgr.mIGBPool.mPool.capacity());
	ImGui::Text("Actors: %d / %d", modelMgr.mActorPool.mPool.size(), modelMgr.mActorPool.mPool.capacity());
	ImGui::Text("Procedural Geometries: %d / %d", modelMgr.mProceduralGeometryPool.mPool.size(), modelMgr.mProceduralGeometryPool.mPool.capacity());
	ImGui::Text("Particle Clouds: %d / %d", modelMgr.mParticleCloudPool.mPool.size(), modelMgr.mParticleCloudPool.mPool.capacity());
	ImGui::Text("Movies: %d / %d", modelMgr.mMoviePool.mPool.size(), modelMgr.mMoviePool.mPool.capacity());
}

void ShowPowerupStats()
{
	const CPowerupPool& powerupPool = ThePowerupPool();
	const TAffecterPool& affecterPool = CAffecter::TheAffecterPool();
	const TFxPool& fxPool = CFxCoreData::TheFxPool();
	const TPowerupBoltOnPool& powerupBoltOnPool = CPowerupBoltOn::ThePowerupBoltOnPool();
	const TScopePool& scopePool = CPowerupScope::TheScopePool();
	const TVirtualFunctionPool& vfNodePool = CVirtualFunctionNode::TheVirtualFunctionPool();
	const TAttachedPowerupPool& attachedPowerupPool = TheAttachedPowerupPool();
	const TAttachedPrimaryFxPool& attachedPrimaryFxPool = CAttachedPrimaryFx::TheAttachedPrimaryFxPool();
	const TAttachedPrimaryDataPool& attachedPrimaryDataPool = CAttachedPrimaryData::TheAttachedPrimaryDataPool();
	const TAttachedBoltOnPool& attachedBoltOnPool = CAttachedBoltOn::TheAttachedBoltOnPool();

	ImGui::SeparatorText("Powerup Pools");
	ImGui::Text("Powerup: %d / %d", powerupPool.mPool.size(), powerupPool.mPool.capacity());
	ImGui::Text("Affecter: %d / %d", affecterPool.mPool.size(), affecterPool.mPool.capacity());
	ImGui::Text("FX: %d / %d", fxPool.mPool.size(), fxPool.mPool.capacity());
	ImGui::Text("BoltOn: %d / %d", powerupBoltOnPool.mPool.size(), powerupBoltOnPool.mPool.capacity());
	ImGui::Text("Scope: %d / %d", scopePool.mPool.size(), scopePool.mPool.capacity());
	ImGui::Text("Virtual Function Node: %d / %d", vfNodePool.mPool.size(), vfNodePool.mPool.capacity());
	ImGui::Text("Attached Powerup: %d / %d", attachedPowerupPool.mPool.size(), attachedPowerupPool.mPool.capacity());
	ImGui::Text("Attached Primary Fx: %d / %d", attachedPrimaryFxPool.mPool.size(), attachedPrimaryFxPool.mPool.capacity());
	ImGui::Text("Attached Primary Data: %d / %d", attachedPrimaryDataPool.mPool.size(), attachedPrimaryDataPool.mPool.capacity());
	ImGui::Text("Attached BoltOn: %d / %d", attachedBoltOnPool.mPool.size(), attachedBoltOnPool.mPool.capacity());
}

void ShowZoneStats()
{
	CZoneInfoSystem& zoneInfoSystem = reinterpret_cast<CZoneInfoSystem&>(TheZoneInfoSystem());
	CExtractionPointSystem& extractionPointSystem = reinterpret_cast<CExtractionPointSystem&>(TheExtractionPointSystem());
	
	unsigned int extractionPoints = 0;

	for (auto it = zoneInfoSystem.mZoneInfoVector.begin(); it != zoneInfoSystem.mZoneInfoVector.end(); it++)
	{
		CZoneInfo& zoneInfo = *it;

		if (zoneInfo.mExtraction)
		{
			extractionPoints++;
		}
	}

	ImGui::SeparatorText("Zone");
	ImGui::Text("Zone Info: %d / %d", zoneInfoSystem.mZoneInfoVector.size(), zoneInfoSystem.mZoneInfoVector.capacity());
	ImGui::Text("Extraction Points: %d / %d", extractionPoints, extractionPointSystem.field_4.capacity());
}

void ShowConversationStats()
{
	CConversationSystem& conversation = TheCConversationSystem();

	ImGui::SeparatorText("Conversation System");
	ImGui::Text("Conversation Files: %d / %d", conversation.mFiles.size(), conversation.mFiles.capacity());

	if (theConvStringPool)
	{
		ImGui::Text("Conversation String Pool (Small): %d / %d", theConvStringPool->size(), theConvStringPool->capacity());
	}

	if (theConvStringPoolBig)
	{
		ImGui::Text("Conversation String Pool (Big): %d / %d", theConvStringPoolBig->size(), theConvStringPoolBig->capacity());
	}
}

void CLimitStatisticsWindow::Draw()
{
	if (ImGui::Begin("Limit Statistics"))
	{
		ShowCharacterStats();
		ShowCombatStats();
		ShowTalentStats();
		ShowFXStats();
		ShowCacheStats();
		ShowEarthwormStats();
		ShowWorldStats();
		ShowModelMgrStats();
		ShowPowerupStats();
		ShowZoneStats();
		ShowConversationStats();
	}

	ImGui::End();
}