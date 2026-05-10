#include "CMenuFxEditor.h"

#include "../IDebugMenuMgr.h"
#include "IFxEditor.h"

#define CAMERA_FOV (90.0F)
#define CAMERA_NEAR (0.01F)
#define CAMERA_FAR (10000.0F)

CMenuFxEditor::CMenuFxEditor() : 
	mGrid(NULL),
	mAxis(NULL)
{
	mMode = -1;
}

CMenuFxEditor::~CMenuFxEditor()
{
	TheFxEditor().Shutdown();
}

bool CMenuFxEditor::Initialize(TiXmlElement* menuElement, TiXmlElement* genericNode)
{	
	if (!CMenu::Initialize(menuElement, genericNode))
	{
		return false;
	}

	TheFxEditor().Initialize();

	CMenuItem* axis = FindMenuItem("axis");
	CMenuItem* grid = FindMenuItem("grid");

	if (axis && grid)
	{
		axis->SetModelPosition(igVec3f::ZeroVector);
		grid->SetModelPosition(igVec3f::ZeroVector);
		
		const IModelPtr& gridModel = grid->GetModel();
		
		if (gridModel.Validate())
		{
			gridModel->AddRenderFx(IModel::RFX_TINT);
			gridModel->SetRenderFxTint(TheFxEditor().GetFloorColor());
		}

		const IModelPtr& axisModel = axis->GetModel();

		if (axisModel.Validate())
		{
			axisModel->AddRenderFx(IModel::RFX_BLEND_USING_DEPTH);
		}
		
		mGrid = grid;
		mAxis = axis;
	}

	IPlayfieldPtr& pf = TheMenuMgr().GetPlayfieldPtr(MENU_PF_BACK);

	if (pf.Validate())
	{
		pf->SetCameraFOV(CAMERA_FOV);
		pf->SetCameraClipping(CAMERA_NEAR, CAMERA_FAR);
	}

	return true;
}

void CMenuFxEditor::HandleInput()
{
	CMenu::HandleInput();
}

EMenuExec CMenuFxEditor::OnCancel()
{
	return EXEC_NONE;
}

void CMenuFxEditor::Draw()
{
	CMenu::Draw();

	bool hideAxis = !TheFxEditor().DrawAxis();

	if (mAxis)
	{
		if (mAxis->GetHide() != hideAxis)
		{
			mAxis->SetHide(hideAxis);
		}
	}

	bool hideGrid = !TheFxEditor().DrawGrid();

	if (mGrid)
	{
		if (mGrid->GetHide() != hideGrid)
		{
			mGrid->SetHide(hideGrid);
		}

		const IModelPtr& model = mGrid->GetModel();

		if (model.Validate())
		{
			model->SetRenderFxTint(TheFxEditor().GetFloorColor());
		}
	}

	TheFxEditor().Draw();
}
