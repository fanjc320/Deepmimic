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
	/*矩阵的块操作：有两种使用方法：
	matrix.block(i, j, p, q) : 表示返回从矩阵(i, j)开始，每行取p个元素，每列取q个元素所组成的临时新矩阵对象，原矩阵的元素不变；
	matrix.block<p, q>(i, j) : <p, q>可理解为一个p行q列的子矩阵，该定义表示从原矩阵中第(i, j)开始，获取一个p行q列的子矩阵，返回该子矩阵组成的临时矩阵对象，原矩阵的元素不变*/

	/*向量的块操作：
	获取向量的前n个元素：vector.head(n);
	获取向量尾部的n个元素：vector.tail(n);
	获取从向量的第i个元素开始的n个元素：vector.segment(i, n);*/
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