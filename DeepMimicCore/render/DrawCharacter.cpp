#include "DrawCharacter.h"
//#include "DrawKinTree.h"
#include "render/DrawUtil.h"
#include <iostream>

void cDrawCharacter::Draw(const cCharacter& character, double link_width, const tVector& fill_col, const tVector& line_col)
{
	if (character.HasDrawShapes())
	{
		DrawCharShapes(character, tVector::Ones(), line_col);
	}
	else
	{
		/*const Eigen::MatrixXd& joint_mat = character.GetJointMat();
		const Eigen::VectorXd& pose = character.GetPose();
		cDrawKinTree::Draw(joint_mat, pose, link_width, fill_col, line_col);*/
	}
}

void cDrawCharacter::DrawPose(const cCharacter& character, const Eigen::VectorXd& pose, const tVector& fill_tint, const tVector& line_col)
{
	/*if (character.HasDrawShapes())
	{
		DrawPoseCharShapes(character, pose, tVector::Ones(), line_col);
	}
	else
	{
		const Eigen::MatrixXd& joint_mat = character.GetJointMat();
		cDrawKinTree::Draw(joint_mat, pose, 0, fill_tint, line_col);
	}*/
}

void cDrawCharacter::DrawShape(const cKinTree::tDrawShapeDef& def, const tMatrix& parent_world_trans,
	const tVector& fill_tint, const tVector& line_col)
{
	cShape::eShape shape = static_cast<cShape::eShape>((int)def[cKinTree::eDrawShapeShape]);
	switch (shape)
	{
	case cShape::eShapeBox:
		DrawShapeBox(def, parent_world_trans, fill_tint, line_col);
		break;
	case cShape::eShapeCapsule:
		//DrawShapeCapsule(def, parent_world_trans, fill_tint, line_col);
		break;
	case cShape::eShapeSphere:
		//DrawShapeSphere(def, parent_world_trans, fill_tint, line_col);
		break;
	case cShape::eShapeCylinder:
		//DrawShapeCylinder(def, parent_world_trans, fill_tint, line_col);
		break;
	case cShape::eShapeNull:
		break;
	default:
		assert(false); // unsupported draw shape
		break;
	}
}

void cDrawCharacter::DrawHeading(const cCharacter& character, double arrow_size, const tVector& arrow_col, const tVector& offset)
{
	/*tMatrix origin_trans = character.BuildOriginTrans();
	tMatrix heading_trans = cMathUtil::InvRigidMat(origin_trans);
	tVector root_pos = character.GetRootPos();
	cDrawUtil::SetColor(arrow_col);

	tVector start = tVector(0.25, root_pos[1], 0, 0);
	tVector end = tVector(1, root_pos[1], 0, 0);

	cDrawUtil::PushMatrixView();
	cDrawUtil::MultMatrixView(heading_trans);
	cDrawUtil::DrawArrow2D(start, end, arrow_size);
	cDrawUtil::PopMatrixView();*/
}

void cDrawCharacter::DrawShapeMesh(const cKinTree::tDrawShapeDef& def, const tMatrix& parent_world_trans,
	cDrawMesh& mesh, const tVector& fill_tint)
{
	/*double theta = 0;
	tVector euler = cKinTree::GetDrawShapeAttachTheta(def);
	int parent_joint = cKinTree::GetDrawShapeParentJoint(def);
	tVector attach_pt = cKinTree::GetDrawShapeAttachPt(def);
	tVector col = cKinTree::GetDrawShapeColor(def);
	tVector scale = tVector(def[cKinTree::eDrawShapeParam0], def[cKinTree::eDrawShapeParam1], def[cKinTree::eDrawShapeParam2], 0);
	col = col.cwiseProduct(fill_tint);

	cDrawUtil::PushMatrixView();
	cDrawUtil::MultMatrixView(parent_world_trans);
	cDrawUtil::Translate(attach_pt);
	cDrawUtil::Rotate(euler);
	cDrawUtil::Scale(scale);
	cDrawUtil::SetColor(col);
	mesh.Draw();
	cDrawUtil::PopMatrixView();*/
}


void cDrawCharacter::DrawCharShapes(const cCharacter& character, const tVector& fill_tint, const tVector& line_col)
{
	assert(character.HasDrawShapes());
	const auto& shape_defs = character.GetDrawShapeDefs();
	size_t num_shapes = shape_defs.rows();

	cDrawUtil::SetLineWidth(1);
	for (int i = 0; i < num_shapes; ++i)
	{
		cKinTree::tDrawShapeDef curr_def = shape_defs.row(i);
		int parent_joint = cKinTree::GetDrawShapeParentJoint(curr_def);
		tMatrix parent_world_trans = character.BuildJointWorldTrans(parent_joint);

		/*parent_world_trans << 0.999801, -0.0010596, 0.0199322, 0.0119301,
			0.00111006, 0.999996, -0.00252073, 0.0499361,
			-0.0199294, 0.00254235, 0.999798, 0.240212,
			0, 0, 0, 1;*/

		parent_world_trans << 0.995075, -0.00033007, -0.0991241, 0.219024,
			0.000338421, 1, 6.74357e-05, 0.0499651,
			0.099124, -0.000100649, 0.995075, 0.258042,
			0, 0, 0, 1;

		int mesh_id = cKinTree::GetDrawShapeMeshID(curr_def);
		if (mesh_id != gInvalidIdx)
		{
			/*const auto& mesh = character.GetMesh(mesh_id);
			cDrawCharacter::DrawShapeMesh(curr_def, parent_world_trans, *mesh, fill_tint);*/
		}
		else
		{
			cShape::eShape shape = static_cast<cShape::eShape>((int)curr_def[cKinTree::eDrawShapeShape]);
			
			if (shape != cShape::eShapeBox)
			{
				continue;
			}
			int parent_joint = (int)curr_def[cKinTree::eDrawShapeParentJoint];//5,11 11在下面输出为b，不知道为啥

			//std::cout << " parent_joint:" << (int)parent_joint << std::endl; 11也是b
			std::cout << "---DrawCharShapes shape:" << shape << " parent_joint:" << (int)parent_joint << " meshid:" << mesh_id << std::endl;//11->b
			if (parent_joint == 11)
			{
				continue;
			}
			std::cout << "---DrawCharShapes parent_world_trans:" << parent_world_trans << std::endl;
			cDrawCharacter::DrawShape(curr_def, parent_world_trans, fill_tint, line_col);
		}
	}
}

void cDrawCharacter::DrawPoseCharShapes(const cCharacter& character, const Eigen::VectorXd& pose, const tVector& fill_tint, const tVector& line_col)
{
	/*assert(character.HasDrawShapes());
	const auto& shape_defs = character.GetDrawShapeDefs();
	size_t num_shapes = shape_defs.rows();
	const auto& joint_mat = character.GetJointMat();

	cDrawUtil::SetLineWidth(1);
	for (int i = 0; i < num_shapes; ++i)
	{
		cKinTree::tDrawShapeDef curr_def = shape_defs.row(i);
		int parent_joint = cKinTree::GetDrawShapeParentJoint(curr_def);
		tMatrix parent_world_trans = cKinTree::JointWorldTrans(joint_mat, pose, parent_joint);

		int mesh_id = cKinTree::GetDrawShapeMeshID(curr_def);
		if (mesh_id != gInvalidIdx)
		{
			const auto& mesh = character.GetMesh(mesh_id);
			cDrawCharacter::DrawShapeMesh(curr_def, parent_world_trans, *mesh, fill_tint);
		}
		else
		{
			cDrawCharacter::DrawShape(curr_def, parent_world_trans, fill_tint, line_col);
		}
	}*/
}

void cDrawCharacter::DrawShapeBox(const cKinTree::tDrawShapeDef& def, const tMatrix& parent_world_trans,
	const tVector& fill_tint, const tVector& line_col)
{
	double theta = 0;
	tVector euler = cKinTree::GetDrawShapeAttachTheta(def);
	int parent_joint = cKinTree::GetDrawShapeParentJoint(def);
	tVector attach_pt = cKinTree::GetDrawShapeAttachPt(def);
	tVector col = cKinTree::GetDrawShapeColor(def);
	tVector size = tVector(def[cKinTree::eDrawShapeParam0], def[cKinTree::eDrawShapeParam1], def[cKinTree::eDrawShapeParam2], 0);
	col = col.cwiseProduct(fill_tint);

	cDrawUtil::PushMatrixView();
	cDrawUtil::MultMatrixView(parent_world_trans);
	/*cDrawUtil::Translate(attach_pt);
	cDrawUtil::Rotate(euler);*/

	cDrawUtil::SetColor(col);
	//这里的DrawBox其实只是调用了 DrawUtil的 gBoxSolidMesh->Draw(GL_TRIANGLES);
	cDrawUtil::DrawBox(tVector::Zero(), size, cDrawUtil::eDrawSolid);
	

	/*if (line_col[3] > 0)
	{
		cDrawUtil::SetColor(line_col);
		cDrawUtil::DrawBox(tVector::Zero(), size, cDrawUtil::eDrawWireSimple);
	}*/

	cDrawUtil::PopMatrixView();
}

void cDrawCharacter::DrawShapeCapsule(const cKinTree::tDrawShapeDef& def, const tMatrix& parent_world_trans,
	const tVector& fill_tint, const tVector& line_col)
{
	double theta = 0;
	tVector axis = tVector(0, 0, 1, 0);
	cKinTree::GetDrawShapeRotation(def, axis, theta);
	int parent_joint = cKinTree::GetDrawShapeParentJoint(def);
	tVector attach_pt = cKinTree::GetDrawShapeAttachPt(def);
	tVector col = cKinTree::GetDrawShapeColor(def);
	tVector size = tVector(def[cKinTree::eDrawShapeParam0], def[cKinTree::eDrawShapeParam1], 0, 0);
	col = col.cwiseProduct(fill_tint);

	double r = 0.5 * size[0];
	double h = size[1];

	cDrawUtil::PushMatrixView();
	cDrawUtil::MultMatrixView(parent_world_trans);
	cDrawUtil::Translate(attach_pt);
	cDrawUtil::Rotate(theta, axis);

	cDrawUtil::SetColor(col);
	cDrawUtil::DrawCapsule(r, h, cDrawUtil::eDrawSolid);

	if (line_col[3] > 0)
	{
		cDrawUtil::SetColor(line_col);
		cDrawUtil::DrawCapsule(r, h, cDrawUtil::eDrawWireSimple);
	}

	cDrawUtil::PopMatrixView();
}

void cDrawCharacter::DrawShapeSphere(const cKinTree::tDrawShapeDef& def, const tMatrix& parent_world_trans,
	const tVector& fill_tint, const tVector& line_col)
{
	double theta = 0;
	tVector axis = tVector(0, 0, 1, 0);
	cKinTree::GetDrawShapeRotation(def, axis, theta);
	int parent_joint = cKinTree::GetDrawShapeParentJoint(def);
	tVector attach_pt = cKinTree::GetDrawShapeAttachPt(def);
	tVector col = cKinTree::GetDrawShapeColor(def);
	tVector size = tVector(def[cKinTree::eDrawShapeParam0], def[cKinTree::eDrawShapeParam1], 0, 0);
	col = col.cwiseProduct(fill_tint);

	double r = 0.5 * size[0];

	cDrawUtil::PushMatrixView();
	cDrawUtil::MultMatrixView(parent_world_trans);
	cDrawUtil::Translate(attach_pt);
	cDrawUtil::Rotate(theta, axis);

	cDrawUtil::SetColor(col);
	cDrawUtil::DrawSphere(r, cDrawUtil::eDrawSolid);

	/*if (line_col[3] > 0)
	{
		cDrawUtil::SetColor(line_col);
		cDrawUtil::DrawSphere(r, cDrawUtil::eDrawWireSimple);
	}*/

	cDrawUtil::PopMatrixView();
}

void cDrawCharacter::DrawShapeCylinder(const cKinTree::tDrawShapeDef& def, const tMatrix& parent_world_trans,
	const tVector& fill_tint, const tVector& line_col)
{
	/*double theta = 0;
	tVector axis = tVector(0, 0, 1, 0);
	cKinTree::GetDrawShapeRotation(def, axis, theta);
	int parent_joint = cKinTree::GetDrawShapeParentJoint(def);
	tVector attach_pt = cKinTree::GetDrawShapeAttachPt(def);
	tVector col = cKinTree::GetDrawShapeColor(def);
	tVector size = tVector(def[cKinTree::eDrawShapeParam0], def[cKinTree::eDrawShapeParam1], 0, 0);
	col = col.cwiseProduct(fill_tint);

	double r = 0.5 * size[0];
	double h = size[1];

	cDrawUtil::PushMatrixView();
	cDrawUtil::MultMatrixView(parent_world_trans);
	cDrawUtil::Translate(attach_pt);
	cDrawUtil::Rotate(theta, axis);

	cDrawUtil::SetColor(col);
	cDrawUtil::DrawCylinder(r, h, cDrawUtil::eDrawSolid);

	if (line_col[3] > 0)
	{
		cDrawUtil::SetColor(line_col);
		cDrawUtil::DrawCylinder(r, h, cDrawUtil::eDrawWireSimple);
	}

	cDrawUtil::PopMatrixView();*/
}