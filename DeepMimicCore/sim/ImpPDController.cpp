#include "ImpPDController.h"
#include <iostream>

#include "sim/SimCharacter.h"
#include "sim/RBDUtil.h"

#include "util/FileUtil.h"
#include "util/easylogging++.h"

cImpPDController::cImpPDController()
{
	mExternRBDModel = true;

#if defined(IMP_PD_CTRL_PROFILER)
	mPerfSolveTime = 0;
	mPerfTotalTime = 0;
	mPerfSolveCount = 0;
	mPerfTotalCount = 0;
#endif // IMP_PD_CTRL_PROFILER
}

cImpPDController::~cImpPDController()
{
}

void cImpPDController::Init(cSimCharacter* character, const Eigen::MatrixXd& pd_params, const tVector& gravity)
{
	std::shared_ptr<cRBDModel> model = BuildRBDModel(*character, gravity);
	Init(character, model, pd_params, gravity);
	mExternRBDModel = false;
}

void cImpPDController::Init(cSimCharacter* character, const std::shared_ptr<cRBDModel>& model, const Eigen::MatrixXd& pd_params, const tVector& gravity)
{
	cExpPDController::Init(character, pd_params);
	mGravity = gravity;
	mRBDModel = model;
	InitGains();
}

void cImpPDController::Clear()
{
	cExpPDController::Clear();
	mExternRBDModel = true;
	mRBDModel.reset();
}

void cImpPDController::UpdateControlForce(double time_step, Eigen::VectorXd& out_tau)
{
	cController::Update(time_step);

#if defined(IMP_PD_CTRL_PROFILER)
	TIMER_RECORD_BEG(Update_Ctrl_Force)
#endif

	if (time_step > 0)
	{
		if (!mExternRBDModel)
		{
			UpdateRBDModel();
		}

		CalcControlForces(time_step, out_tau);
	}

#if defined(IMP_PD_CTRL_PROFILER)
	TIMER_RECORD_END(Update_Ctrl_Force, mPerfTotalTime, mPerfTotalCount)
#endif

#if defined(IMP_PD_CTRL_PROFILER)
	printf("Solve Time: %.5f\n", mPerfSolveTime);
	printf("Total Time: %.5f\n", mPerfTotalTime);
#endif
}

void cImpPDController::SetKp(int joint_id, double kp)
{
	cExpPDController::SetKp(joint_id, kp);

	int param_offset = mChar->GetParamOffset(joint_id);
	int param_size = mChar->GetParamSize(joint_id);

	auto curr_kp = mKp.segment(param_offset, param_size);
	curr_kp.setOnes();
	curr_kp *= kp;
}

void cImpPDController::SetKd(int joint_id, double kd)
{
	cExpPDController::SetKd(joint_id, kd);

	int param_offset = mChar->GetParamOffset(joint_id);
	int param_size = mChar->GetParamSize(joint_id);

	auto curr_kd = mKd.segment(param_offset, param_size);
	curr_kd.setOnes();
	curr_kd *= kd;
}

void cImpPDController::InitGains()
{
	int num_dof = GetNumDof();
	mKp = Eigen::VectorXd::Zero(num_dof);//43
	mKd = Eigen::VectorXd::Zero(num_dof);//43

	for (int j = 0; j < GetNumJoints(); ++j)
	{
		const cPDController& pd_ctrl = GetPDCtrl(j);
		if (pd_ctrl.IsValid())
		{
			int param_offset = mChar->GetParamOffset(j);
			int param_size = mChar->GetParamSize(j);

			double kp = pd_ctrl.GetKp();
			double kd = pd_ctrl.GetKd();

			mKp.segment(param_offset, param_size) = Eigen::VectorXd::Ones(param_size) * kp;
			mKd.segment(param_offset, param_size) = Eigen::VectorXd::Ones(param_size) * kd;
			//humanoid3d_ctrl.txt
			/*{
				"ID": 1,
					"Name" : "chest",
					"Kp" : 1000,
					"Kd" : 100,
					"TargetTheta0" : 0,
					"UseWorldCoord" : 0
			},*/
			/*param_offset:7 param_size : 4 kp : 1000 kd : 100
			param_offset : 11 param_size : 4 kp : 100 kd : 10
			param_offset : 15 param_size : 4 kp : 500 kd : 50
			param_offset : 19 param_size : 1 kp : 500 kd : 50
			param_offset : 20 param_size : 4 kp : 400 kd : 40
			param_offset : 24 param_size : 4 kp : 400 kd : 40
			param_offset : 28 param_size : 1 kp : 300 kd : 30
			param_offset : 29 param_size : 0 kp : 0 kd : 0
			param_offset : 29 param_size : 4 kp : 500 kd : 50
			param_offset : 33 param_size : 1 kp : 500 kd : 50
			param_offset : 34 param_size : 4 kp : 400 kd : 40
			param_offset : 38 param_size : 4 kp : 400 kd : 40
			param_offset : 42 param_size : 1 kp : 300 kd : 30
			param_offset : 43 param_size : 0 kp : 0 kd : 0*/
			//LOG(INFO) << "cImpPDController::InitGains param_offset:" << param_offset << " param_size:" << param_size << " kp:" << kp << " kd:" << kd;
		}
	}

	//LOG(INFO) << "cImpPDController::InitGains mKp:" << mKp;
}

std::shared_ptr<cRBDModel> cImpPDController::BuildRBDModel(const cSimCharacter& character, const tVector& gravity) const
{
	std::shared_ptr<cRBDModel> model = std::shared_ptr<cRBDModel>(new cRBDModel());
	model->Init(character.GetJointMat(), character.GetBodyDefs(), gravity);
	return model;
}

void cImpPDController::UpdateRBDModel()
{
	const Eigen::VectorXd& pose = mChar->GetPose();
	const Eigen::VectorXd& vel = mChar->GetVel();
	mRBDModel->Update(pose, vel);
}

void cImpPDController::CalcControlForces(double time_step, Eigen::VectorXd& out_tau)
{
	double t = time_step;

	const Eigen::VectorXd& pose = mChar->GetPose();
	const Eigen::VectorXd& vel = mChar->GetVel();
	Eigen::VectorXd tar_pose;
	Eigen::VectorXd tar_vel;
	BuildTargetPose(tar_pose);
	BuildTargetVel(tar_vel);

	Eigen::DiagonalMatrix<double, Eigen::Dynamic> Kp_mat = mKp.asDiagonal();
	Eigen::DiagonalMatrix<double, Eigen::Dynamic> Kd_mat = mKd.asDiagonal();
	//LOG(INFO) << "cImpPDController::CalcControlForces mKp:" << mKp;// << " vel:" << vel;

	for (int j = 0; j < GetNumJoints(); ++j)
	{
		const cPDController& pd_ctrl = GetPDCtrl(j);
		if (!pd_ctrl.IsValid() || !pd_ctrl.IsActive()) //！valid
		{
			int param_offset = mChar->GetParamOffset(j);
			int param_size = mChar->GetParamSize(j);
			Kp_mat.diagonal().segment(param_offset, param_size).setZero();
			Kd_mat.diagonal().segment(param_offset, param_size).setZero();
			//cImpPDController::CalcControlForces param_offset : 0 param_size : 7 前七个元素设置为0, 从下面Kp_mat的日志中可以看出来,的确被置零了
			//LOG(INFO) << "cImpPDController::CalcControlForces param_offset:" << param_offset << " param_size:" << param_size;
		}
	}

	Eigen::MatrixXd M = mRBDModel->GetMassMat();
	const Eigen::VectorXd& C = mRBDModel->GetBiasForce();
	M.diagonal() += t * mKd;

	Eigen::VectorXd pose_inc;
	const Eigen::MatrixXd& joint_mat = mChar->GetJointMat();
	cKinTree::VelToPoseDiff(joint_mat, pose, vel, pose_inc);

	pose_inc = pose + t * pose_inc;
	cKinTree::PostProcessPose(joint_mat, pose_inc);

	Eigen::VectorXd pose_err;
	cKinTree::CalcVel(joint_mat, pose_inc, tar_pose, 1, pose_err);
	Eigen::VectorXd vel_err = tar_vel - vel;
	Eigen::VectorXd acc = Kp_mat * pose_err + Kd_mat * vel_err - C;
	
	
	//pose : 0.0376539 0.848213 0.122245 0.858526 - 0.256094 0.0786001 - 0.437232 0.997923 0.0163822 0.0593358 - 0.0190122  0.95286 0.0686819 0.0711306 - 0.286848 0.852271 0.0995521 0.143628 0.493046  1.70478e-05 0.980617 0.0317739 - 0.193309 - 0.00337033 0.706614 - 0.643463 - 0.293779 0.0186043 0.00377271 0.935225 0.0870577 0.259251 0.224865  3.54744e-06 0.983591 - 0.10014 - 0.0190355 0.148858 0.735646 0.562505 - 0.312716 0.211238 0.00493754 
	//vel : 0.19307 - 0.139257 - 0.361912 - 2.01958 - 0.437488 - 0.84093 0 0.068953 0.437826 - 0.0209946 0 0.00436782 - 0.00199577 - 0.314112 0 0.969889 0.200974  1.34792 0  5.33909e-05 0.451502 0.164664  0.29481 0 0.167277 0.0327958 0.107338 0 - 0.0256499  0.0161 0.361465 0.523126 0 - 0.00238016 - 0.0919742 - 0.0399371 0.0194374 0 - 0.0762852 - 0.0192729 0.0683982 0 - 0.0348174
	//LOG(INFO) << "cImpPDController::CalcControlForces pose:" << pose << " vel:" << vel;
	//LOG(INFO) << "cImpPDController::CalcControlForces tar_pose:" << tar_pose << " tar_vel:" << tar_vel;//全部是0,这就是刚开始有动作，动作渐渐消失的原因吧?
#if defined(IMP_PD_CTRL_PROFILER)
	TIMER_RECORD_BEG(Solve)
#endif

	//int root_size = cKinTree::gRootDim;
	//int num_act_dofs = static_cast<int>(acc.size()) - root_size;
	//auto M_act = M.block(root_size, root_size, num_act_dofs, num_act_dofs);
	//auto acc_act = acc.segment(root_size, num_act_dofs);
	//acc_act = M_act.ldlt().solve(acc_act);
	
	acc = M.ldlt().solve(acc);//213.  rows:43   //https://blog.csdn.net/wonengguwozai/article/details/125029383 M*X = acc, 求X

#if defined(IMP_PD_CTRL_PROFILER)
	TIMER_RECORD_END(Solve, mPerfSolveTime, mPerfSolveCount)
#endif

 
	Eigen::VectorXd res = Kp_mat * pose_err + Kd_mat * (vel_err - t * acc);
	Eigen::VectorXd res1 = Kp_mat * pose_err;
	out_tau += Kp_mat * pose_err + Kd_mat * (vel_err - t * acc);//rowa43=rows43 *rows43+rows43*(rows43-0.0016*rows43)!!!!!!!!!!!!!, Kp_mat, Kd_mat好像都是0
	//out_tau : 0     0 0 0 0 0 0   28.7558 - 29.4256   2.04277 0 - 18.3157   2.18059    18.711 0   1.85058 - 30.3524 - 33.3619 0    290.27 - 0.851167 - 12.3723 - 8.36034 0   95.1215   49.2287 - 105.318 0 - 142.382   5.32291   16.1997 - 32.3052 0   163.944   2.13277   5.87534   1.71974 0 - 78.5635 - 30.9121 - 91.181 0 - 188.185

	//Eigen::MatrixXi V(Kp_mat);
	//LOG(INFO) << "cImpPDController::CalcControlForces Kp_mat:" << V；//报错
	// 
	//Diagonal Matrix并没有 << 方法，需要转换为Matrix3d或者别的进行打印，这里不能用Matrix3d或Matrix4d,因为维度不够 或MatrixXi，会报错,只能用diagonal,只打印对角线。
	//Kp_mat:rows43 0 0 0 0 0 0 0 1000 1000 1000 1000 100 100 100 100 500 500 500 500 500 400 400 400 400 400 400 400 400 300 500 500 500 500 500 400 400 400 400 400 400 400 400 300
	/*LOG(INFO) << "cImpPDController::CalcControlForces Kp_mat:" << " Kp_mat:" << Kp_mat.diagonal();
	LOG(INFO) << "cImpPDController::CalcControlForces pose_err:" << pose_err;
	LOG(INFO) << "cImpPDController::CalcControlForces out_tau kp_mat*pose_err:" << Kp_mat * pose_err;
	LOG(INFO) << "cImpPDController::CalcControlForces out_tau res1:" << res1;
	LOG(INFO) << "cImpPDController::CalcControlForces out_tau res:" << res;*/
}

void cImpPDController::BuildTargetPose(Eigen::VectorXd& out_pose) const
{
	out_pose = Eigen::VectorXd::Zero(GetNumDof());

	//const auto& joint_mat = mChar->GetJointMat();
	//tVector root_pos = mChar->GetRootPos();
	//tQuaternion root_rot = mChar->GetRootRotation();
	//cKinTree::SetRootPos(joint_mat, root_pos, out_pose);
	//cKinTree::SetRootRot(joint_mat, root_rot, out_pose);

	//以下代码注释掉没有影响
	for (int j = 0; j < GetNumJoints(); ++j)
	{
		const cPDController& pd_ctrl = GetPDCtrl(j);
		if (pd_ctrl.IsValid())
		{
			Eigen::VectorXd curr_pose;
			pd_ctrl.GetTargetTheta(curr_pose);//注释掉会崩溃，这里起到了初始化的作用
			int param_offset = mChar->GetParamOffset(j);
			int param_size = mChar->GetParamSize(j);
			out_pose.segment(param_offset, param_size) = curr_pose;
		}
	}
}

void cImpPDController::BuildTargetVel(Eigen::VectorXd& out_vel) const
{
	out_vel = Eigen::VectorXd::Zero(GetNumDof());

	//const auto& joint_mat = mChar->GetJointMat();
	//tVector root_vel = mChar->GetRootVel();
	//tVector root_ang_vel = mChar->GetRootAngVel();
	//cKinTree::SetRootVel(joint_mat, root_vel, out_vel);
	//cKinTree::SetRootAngVel(joint_mat, root_ang_vel, out_vel);
	
	for (int j = 0; j < GetNumJoints(); ++j)
	{
		const cPDController& pd_ctrl = GetPDCtrl(j);
		if (pd_ctrl.IsValid())
		{
			Eigen::VectorXd curr_vel;
			pd_ctrl.GetTargetVel(curr_vel);
			int param_offset = mChar->GetParamOffset(j);
			int param_size = mChar->GetParamSize(j);
			out_vel.segment(param_offset, param_size) = curr_vel;
		}
	}
}