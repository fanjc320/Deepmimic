#include "DrawSceneSimChar.h"

#include "sim/SimBox.h"

#include "render/DrawUtil.h"
#include "render/DrawGround.h"
#include "render/DrawCharacter.h"
#include "render/DrawSimCharacter.h"
#include "render/DrawObj.h"
#include "render/DrawWorld.h"
//#include "render/DrawPerturb.h"

const tVector gCamFocus0 = tVector(0, 0.75, 0, 0);

const tVector gLineColor = tVector(0, 0, 0, 1);
const size_t gInitGroundUpdateCount = std::numeric_limits<size_t>::max();

const std::string gOutputCharFile = "output/char_state.txt";

cDrawSceneSimChar::cDrawSceneSimChar()
{
 
}

cDrawSceneSimChar::~cDrawSceneSimChar()
{
}

void cDrawSceneSimChar::Init()
{
	BuildScene(mScene);
	SetupScene(mScene);
	
	cDrawScene::Init();

	InitTracer();
	BuildGroundDrawMesh();
}

void cDrawSceneSimChar::Clear()
{
	cDrawScene::Clear();
	mScene->Clear();
	mTracer.Clear();
	mPrevGroundUpdateCount = gInitGroundUpdateCount;
}

void cDrawSceneSimChar::Update(double time_elapsed)
{
 
	UpdateScene(time_elapsed);//×¢ÊÍµôÔò»­Ãæ¾²Ö¹
	 
}

const std::shared_ptr<cSceneSimChar>& cDrawSceneSimChar::GetScene() const
{
	return mScene;
}

void cDrawSceneSimChar::MouseClick(int button, int state, double x, double y)
{
	 
}

void cDrawSceneSimChar::MouseMove(double x, double y)
{
	 
}

void cDrawSceneSimChar::Keyboard(unsigned char key, double device_x, double device_y)
{

}

double cDrawSceneSimChar::GetTime() const
{
	return mScene->GetTime();
}

bool cDrawSceneSimChar::IsEpisodeEnd() const
{
	return mScene->IsEpisodeEnd();
}

bool cDrawSceneSimChar::CheckValidEpisode() const
{
	return mScene->CheckValidEpisode();
}

std::string cDrawSceneSimChar::GetName() const
{
	return mScene->GetName();
}

void cDrawSceneSimChar::BuildScene(std::shared_ptr<cSceneSimChar>& out_scene) const
{
	out_scene = std::shared_ptr<cSceneSimChar>(new cSceneSimChar());
}

void cDrawSceneSimChar::SetupScene(std::shared_ptr<cSceneSimChar>& out_scene)
{
	out_scene->ParseArgs(mArgParser);
	out_scene->Init();
}

void cDrawSceneSimChar::UpdateScene(double time_elapsed)
{
	//ApplyUIForce(time_elapsed);
	mScene->Update(time_elapsed);//mScene->cSceneSimChar
}

void cDrawSceneSimChar::ResetScene()
{
	cDrawScene::ResetScene();
	mScene->Reset();
	mTracer.Reset();
	BuildGroundDrawMesh();
}

tVector cDrawSceneSimChar::GetCamTrackPos() const
{
	const auto& character = mScene->GetCharacter();
	return character->CalcCOM();
}

tVector cDrawSceneSimChar::GetCamStillPos() const
{
	return tVector();
}

tVector cDrawSceneSimChar::GetDefaultCamFocus() const
{
	return gCamFocus0;
}

void cDrawSceneSimChar::ResetParams()
{
	cDrawScene::ResetParams();
	ResetUI();
}

void cDrawSceneSimChar::ToggleTrace()
{

}

void cDrawSceneSimChar::InitTracer()
{

}

void cDrawSceneSimChar::AddTraces()
{

}

void cDrawSceneSimChar::AddCharTrace(const std::shared_ptr<cSimCharacter>& character,
									const tVectorArr& cols)
{

}

void cDrawSceneSimChar::UpdateTracer(double time_elapsed)
{
	//mTracer.Update(time_elapsed);
}

void cDrawSceneSimChar::SpawnProjectile()
{
	mScene->SpawnProjectile();
}

void cDrawSceneSimChar::SpawnBigProjectile()
{
	mScene->SpawnBigProjectile();
}

void cDrawSceneSimChar::OutputCharState(const std::string& out_file) const
{
	mScene->OutputCharState(out_file);
}

std::string cDrawSceneSimChar::GetOutputCharFile() const
{
	return gOutputCharFile;
}


void cDrawSceneSimChar::ResetUI()
{
	
}

void cDrawSceneSimChar::RayTest(const tVector& start, const tVector& end, cWorld::tRayTestResult& out_result)
{
	//return GetScene()->RayTest(start, end, out_result);
}

bool cDrawSceneSimChar::ObjectSelected() const
{
	return mSelectedObj != nullptr;
}

void cDrawSceneSimChar::HandleRayTest(const cWorld::tRayTestResult& result)
{
	
}

void cDrawSceneSimChar::ApplyUIForce(double time_step)
{
	
}

void cDrawSceneSimChar::DrawObjs() const
{
	
}

void cDrawSceneSimChar::DrawMisc() const
{

}

void cDrawSceneSimChar::DrawCoM() const
{
	
}

void cDrawSceneSimChar::DrawTorque() const
{
	
}

void cDrawSceneSimChar::DrawBodyVel() const
{
	
}

void cDrawSceneSimChar::DrawHeading() const
{
	for (int i = 0; i < mScene->GetNumChars(); ++i)
	{
		const auto& character = mScene->GetCharacter(i);
		double arrow_size = 0.2;
		tVector arrow_col = tVector(0, 0.8, 0, 0.5);
		cDrawCharacter::DrawHeading(*(character.get()), arrow_size, arrow_col, GetVisOffset());
	}
}

void cDrawSceneSimChar::DrawTrace() const
{
	
}


void cDrawSceneSimChar::DrawPerturbs() const
{
	
}

void cDrawSceneSimChar::DrawGround() const
{
	
}

void cDrawSceneSimChar::DrawCharacters() const
{
	int num_chars = mScene->GetNumChars();
	for (int i = 0; i < num_chars; ++i)
	{
		const auto& curr_char = mScene->GetCharacter(i);
		DrawCharacter(curr_char);
	}
}

void cDrawSceneSimChar::DrawCharacter(const std::shared_ptr<cSimCharacter>& character) const
{
	const tVector fill_tint = tVector(1, 1, 1, 1);
	bool enable_draw_shape = true;
	//cDrawSimCharacter::Draw(*(character.get()), fill_tint, GetLineColor(), enable_draw_shape);
	cDrawCharacter::Draw(*(character.get()), 0, fill_tint, GetLineColor());
}

void cDrawSceneSimChar::UpdateGroundDrawMesh()
{
	
}

void cDrawSceneSimChar::BuildGroundDrawMesh()
{
	
}

void cDrawSceneSimChar::DrawInfo() const
{
	//DrawPoliInfo();
}

void cDrawSceneSimChar::DrawPoliInfo() const
{
	
}