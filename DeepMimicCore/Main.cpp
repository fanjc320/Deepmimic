#include <iostream>

#include "DeepMimicCore.h"

#include "util/FileUtil.h"

#include "render/DrawUtil.h"
#include "render/TextureDesc.h"

// Dimensions of the window we are drawing into.
int gWinWidth = 800;
int gWinHeight = static_cast<int>(gWinWidth * 9.0 / 16.0);
//int gWinWidth = 1055;
//int gWinHeight = 450;
bool gReshaping = false;

// intermediate frame buffers
std::unique_ptr<cTextureDesc> gDefaultFrameBuffer;

// anim
const double gFPS = 60.0;
const double gAnimStep = 1.0 / gFPS;
const int gDisplayAnimTime = static_cast<int>(1000 * gAnimStep);
bool gAnimating = true;

int gSampleCount = 0;

double gPlaybackSpeed = 1;
const double gPlaybackDelta = 0.05;

// FPS counter
int gPrevTime = 0;
double gUpdatesPerSec = 0;

std::vector<std::string> gArgs;
std::unique_ptr<cDeepMimicCore> gCore;

void SetupDeepMimicCore()
{
	bool enable_draw = true;
	gCore = std::unique_ptr<cDeepMimicCore>(new cDeepMimicCore(enable_draw));
	if (gArgs.size() < 1)
	{
		std::cout << "gArgs0:" << gArgs[0] << std::endl;

	}
	else
	{
		std::cout << "gArgs1:" << gArgs[1] << std::endl;
	}
	gCore->ParseArgs(gArgs);
	gCore->Init();

	int num_agents = gCore->GetNumAgents();
	for (int id = 0; id < num_agents; ++id)
	{
		int action_space = gCore->GetActionSpace(id);
		int state_size = gCore->GetStateSize(id);
		int goal_size = gCore->GetGoalSize(id);
		int action_size = gCore->GetActionSize(id);
		int num_actions = gCore->GetNumActions(id);

		auto s_offset = gCore->BuildStateOffset(id);
		auto s_scale = gCore->BuildStateScale(id);
		auto g_offset = gCore->BuildGoalOffset(id);
		auto g_scale = gCore->BuildGoalScale(id);
		auto a_offset = gCore->BuildActionOffset(id);
		auto a_scale = gCore->BuildActionScale(id);

		auto action_min = gCore->BuildActionBoundMin(id);
		auto action_max = gCore->BuildActionBoundMax(id);

		auto state_norm_groups = gCore->BuildStateNormGroups(id);
		auto goal_norm_groups = gCore->BuildGoalNormGroups(id);

		double reward_min = gCore->GetRewardMin(id);
		double reward_max = gCore->GetRewardMax(id);
		double reward_fail = gCore->GetRewardFail(id);
		double reward_succ = gCore->GetRewardSucc(id);

		int xx = 0;
		++xx;
	}
}

void FormatArgs(int argc, char** argv, std::vector<std::string>& out_args)
{
	out_args.resize(argc);
	for (int i = 0; i < argc; ++i)
	{
		out_args[i] = std::string(argv[i]);
	}
}

void UpdateFrameBuffer()
{
	if (!gReshaping)
	{
		if (gWinWidth != gCore->GetWinWidth() || gWinHeight != gCore->GetWinHeight())
		{
			gCore->Reshape(gWinWidth, gWinHeight);
		}
	}
}

void Update(double time_elapsed)
{
	int num_substeps = gCore->GetNumUpdateSubsteps();
	double timestep = time_elapsed / num_substeps;
	num_substeps = (time_elapsed == 0) ? 1 : num_substeps;

	for (int i = 0; i < num_substeps; ++i)//10
	{
		for (int id = 0; id < gCore->GetNumAgents(); ++id)
		{
			if (gCore->NeedNewAction(id))
			{
				auto s = gCore->RecordState(id);//size-197
				auto g = gCore->RecordGoal(id);//size=0
				double r = gCore->CalcReward(id);//0.99890062977706384
				++gSampleCount;

				std::vector<double> action = std::vector<double>(gCore->GetActionSize(id), 0);
				gCore->SetAction(id, action);//0, size:36 Ö»ÊÇinitialize?
			}
		}

		gCore->Update(timestep);

		if (gCore->IsRLScene())
		{
			bool end_episode = gCore->IsEpisodeEnd();
			bool valid_episode = gCore->CheckValidEpisode();
			if (end_episode || !valid_episode)
			{
				for (int id = 0; id < gCore->GetNumAgents(); ++id)
				{
					int terminated = gCore->CheckTerminate(id);
					if (terminated)
					{
						printf("Agent %i terminated\n", id);
					}
				}
				gCore->SetSampleCount(gSampleCount);//34
				gCore->Reset();
			}
		}
	}
}

void Draw(void)
{
	UpdateFrameBuffer();
	gCore->Draw();

	glutSwapBuffers();
	gReshaping = false;
}

void Reshape(int w, int h)
{
	gReshaping = true;

	gWinWidth = w;
	gWinHeight = h;

	gDefaultFrameBuffer->Reshape(w, h);
	glViewport(0, 0, gWinWidth, gWinHeight);
	glutPostRedisplay();
}

void StepAnim(double time_step)
{
	Update(time_step);
	gAnimating = false;
	glutPostRedisplay();
}

void Reload()
{
	SetupDeepMimicCore();
}

void Reset()
{
	gCore->Reset();
}

int GetNumTimeSteps()
{
	int num_steps = static_cast<int>(gPlaybackSpeed);
	if (num_steps == 0)
	{
		num_steps = 1;
	}
	num_steps = std::abs(num_steps);
	return num_steps;
}

int CalcDisplayAnimTime(int num_timesteps)
{
	int anim_time = static_cast<int>(gDisplayAnimTime * num_timesteps / gPlaybackSpeed);
	anim_time = std::abs(anim_time);
	return anim_time;
}

void Shutdown()
{
	gCore->Shutdown();
	exit(0);
}

int GetCurrTime()
{
	return glutGet(GLUT_ELAPSED_TIME);
}

void InitTime()
{
	gPrevTime = GetCurrTime();
	gUpdatesPerSec = 0;
}

void Animate(int callback_val)
{
	const double counter_decay = 0;

	if (gAnimating)
	{
		int num_steps = GetNumTimeSteps();
		int curr_time = GetCurrTime();//389164
		int time_elapsed = curr_time - gPrevTime;//62
		gPrevTime = curr_time;

		double timestep = (gPlaybackSpeed < 0) ? -gAnimStep : gAnimStep;//0.016666
		for (int i = 0; i < num_steps; ++i)//1
		{
			Update(timestep);
		}

		// FPS counting
		double update_count = num_steps / (0.001 * time_elapsed);//16.129
		if (std::isfinite(update_count))
		{
			gUpdatesPerSec = counter_decay * gUpdatesPerSec + (1 - counter_decay) * update_count;
			gCore->SetUpdatesPerSec(gUpdatesPerSec);//16.129
		}

		int timer_step = CalcDisplayAnimTime(num_steps);
		int update_dur = GetCurrTime() - curr_time;//190298
		timer_step -= update_dur;
		timer_step = std::max(timer_step, 0);//0

		glutTimerFunc(timer_step, Animate, 0);
		glutPostRedisplay();
	}

	if (gCore->IsDone())
	{
		Shutdown();
	}
}

void ToggleAnimate()
{
	gAnimating = !gAnimating;
	if (gAnimating)
	{
		glutTimerFunc(gDisplayAnimTime, Animate, 0);
	}
}

void ChangePlaybackSpeed(double delta)
{
	double prev_playback = gPlaybackSpeed;
	gPlaybackSpeed += delta;
	gCore->SetPlaybackSpeed(gPlaybackSpeed);

	if (std::abs(prev_playback) < 0.0001 && std::abs(gPlaybackSpeed) > 0.0001)
	{
		glutTimerFunc(gDisplayAnimTime, Animate, 0);
	}
}

void Keyboard(unsigned char key, int x, int y)
{
	gCore->Keyboard(key, x, y);

	switch (key) {
	case 27: // escape
		Shutdown();
		break;
	case ' ':
		ToggleAnimate();
		break;
	case '>':
		StepAnim(gAnimStep);
		break;
	case '<':
		StepAnim(-gAnimStep);
		break;
	case ',':
		ChangePlaybackSpeed(-gPlaybackDelta);
		break;
	case '.':
		ChangePlaybackSpeed(gPlaybackDelta);
		break;
	case '/':
		ChangePlaybackSpeed(-gPlaybackSpeed + 1);
		break;
	case 'l':
		Reload();
		break;
	case 'r':
		Reset();
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void MouseClick(int button, int state, int x, int y)
{
	gCore->MouseClick(button, state, x, y);
	glutPostRedisplay();
}

void MouseMove(int x, int y)
{
	gCore->MouseMove(x, y);
	glutPostRedisplay();
}

void InitFrameBuffers(void)
{
	gDefaultFrameBuffer = std::unique_ptr<cTextureDesc>(new cTextureDesc(0, 0, 0, gWinWidth, gWinHeight, 1, GL_RGBA, GL_RGBA));
}

void InitDraw(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(gWinWidth, gWinHeight);
	glutCreateWindow("DeepMimic");
}

void SetupDraw()
{
	glutDisplayFunc(Draw);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseClick);
	glutMotionFunc(MouseMove);
	glutTimerFunc(gDisplayAnimTime, Animate, 0);

	InitFrameBuffers();
	Reshape(gWinWidth, gWinHeight);
	gCore->Reshape(gWinWidth, gWinHeight);
}

void DrawMainLoop()
{
	InitTime();
	glutMainLoop();
}

int testBlock();
bool bTest = false;
int main(int argc, char** argv)
{
	if (bTest)
	{
		testBlock();
		return 0;
	}

	FormatArgs(argc, argv, gArgs);
	std::cout << "c++ gArgs0:" << gArgs[0] << std::endl;
	InitDraw(argc, argv);
	SetupDeepMimicCore();
	SetupDraw();

	DrawMainLoop();

	return EXIT_SUCCESS;
}

#include <Eigen/Core>
#include <iostream>

template<typename Derived>
Eigen::Block<Derived>
topLeftCorner(Eigen::MatrixBase<Derived>& m, int rows, int cols)
{
	return Eigen::Block<Derived>(m.derived(), 0, 0, rows, cols);
}

template<typename Derived>
const Eigen::Block<const Derived>
topLeftCorner(const Eigen::MatrixBase<Derived>& m, int rows, int cols)
{
	return Eigen::Block<const Derived>(m.derived(), 0, 0, rows, cols);
}

int testBlock()
{
	Eigen::Matrix4d m = Eigen::Matrix4d::Identity();
	std::cout << "topLeftCorner m:" << topLeftCorner(4 * m, 2, 3) << std::endl; // calls the const version
	topLeftCorner(m, 2, 3) *= 5;              // calls the non-const version
	std::cout << "topLeftCorner: Now the matrix m is:" << std::endl << m << std::endl;
	return 0;
}