#include "MatrixStack.h"

cMatrixStack::cMatrixStack(size_t capacity)
{
	Clear();
	mMatStack.resize(capacity);
}

cMatrixStack::~cMatrixStack()
{
}

void cMatrixStack::Clear()
{
	mStackHead = 0;
	SetIdentity();
}

const tMatrix& cMatrixStack::GetMatrix() const
{
	return mCurrMat;
}

void cMatrixStack::Push()
{
	assert(mStackHead < mMatStack.size());

	mMatStack[mStackHead] = mCurrMat;
	mStackHead = std::min(mStackHead + 1, static_cast<int>(mMatStack.size()));
}

void cMatrixStack::Push(const tMatrix& mat)
{
	Push();
	SetMatrix(mat);
}

void cMatrixStack::Pop()
{
	assert(mStackHead > 0);

	mCurrMat = mMatStack[mStackHead - 1];
	mStackHead = std::max(mStackHead - 1, 0);
}

size_t cMatrixStack::GetCapacity() const
{
	return mMatStack.size();
}

size_t cMatrixStack::GetSize() const
{
	return mStackHead;
}

void cMatrixStack::SetIdentity()
{
	mCurrMat.setIdentity();
}

void cMatrixStack::SetMatrix(const tMatrix& mat)
{
	mCurrMat = mat;
}

void cMatrixStack::MultMatrix(const tMatrix& mat)//Eigen::Matrix<double,4,4,0,4,4>Matrix4d
{
	mCurrMat = mCurrMat * mat;//16=16*16
}

void cMatrixStack::Translate(const tVector& trans)
{
	/*����Ŀ������������ʹ�÷�����
	matrix.block(i, j, p, q) : ��ʾ���شӾ���(i, j)��ʼ��ÿ��ȡp��Ԫ�أ�ÿ��ȡq��Ԫ������ɵ���ʱ�¾������ԭ�����Ԫ�ز��䣻
	matrix.block<p, q>(i, j) : <p, q>�����Ϊһ��p��q�е��Ӿ��󣬸ö����ʾ��ԭ�����е�(i, j)��ʼ����ȡһ��p��q�е��Ӿ��󣬷��ظ��Ӿ�����ɵ���ʱ�������ԭ�����Ԫ�ز���*/

	/*�����Ŀ������
	��ȡ������ǰn��Ԫ�أ�vector.head(n);
	��ȡ����β����n��Ԫ�أ�vector.tail(n);
	��ȡ�������ĵ�i��Ԫ�ؿ�ʼ��n��Ԫ�أ�vector.segment(i, n);*/
	mCurrMat.block(0, 3, 3, 1) += mCurrMat.block(0, 0, 3, 3) * trans.segment(0, 3);
}

void cMatrixStack::Scale(const tVector& scale)
{
	mCurrMat.col(0) *= scale[0];
	mCurrMat.col(1) *= scale[1];
	mCurrMat.col(2) *= scale[2];
}

void cMatrixStack::Rotate(double theta, const tVector& axis)
{
	tMatrix rot_mat = cMathUtil::RotateMat(axis, theta);
	mCurrMat.block(0, 0, 3, 3) = mCurrMat.block(0, 0, 3, 3) * rot_mat.block(0, 0, 3, 3);
}