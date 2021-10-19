/* Copyright (c) Russell Gillette
* December 2013
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
* documentation files (the "Software"), to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
* to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <fstream>
#include <algorithm>
#include "render/DrawMesh.h"
#include "render/DrawUtil.h"
#include <iostream>

cDrawMesh::cDrawMesh() : mNumElem(0), mVbos(0)
{
}

cDrawMesh::~cDrawMesh()
{
}

void cDrawMesh::Init(int num_buffers)
{
	mState.BindVAO();

	mVbos.resize(num_buffers);

	// set the buffer IDs that the buffer objects will use
	mIbo.SetRender(0, mState);
	for (int i = 0; i < num_buffers; i++)
		mVbos[i].SetRender(i + 1, mState);
}

void cDrawMesh::Draw(GLenum primitive)//GL_TRIANGLES
{
	Draw(primitive, 0);
}

//void glDrawElements(GLenum  	mode,
//	GLsizei  	count,
//	GLenum  	type,
//	const GLvoid* indices);
//Parameters
//mode
//Specifies what kind of primitives to render.Symbolic constants GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCYand GL_TRIANGLES_ADJACENCY are accepted.
//count
//Specifies the number of elements to be rendered.
//type
//Specifies the type of the values in indices.Must be one of GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT.
//indices
//Specifies an offset of the first index in the array in the data store of the buffer currently bound to the GL_ELEMENT_ARRAY_BUFFER target

void cDrawMesh::Draw(GLenum primitive, int start_idx)
{
	mState.BindVAO();
	SyncGPU(0, 0);
	cDrawUtil::LoadShaderUniforms();

	std::cout << "cDrawMesh::Draw primitive(GL_TRIANGLES):" << primitive << " start:" << start_idx << " mNumElem:" << mNumElem << std::endl;

	glDrawElements(primitive, mNumElem - start_idx, GL_UNSIGNED_INT, (void*)(start_idx * sizeof(GLuint)));
}

void cDrawMesh::Draw(GLenum primitive, int idx_start, int idx_end)
{
	mState.BindVAO();
	SyncGPU(0, 0);
	int num_elem = std::min(idx_end, mNumElem) - idx_start;
	cDrawUtil::LoadShaderUniforms();

	std::cout << "cDrawMesh::Draw primitive(GL_TRIANGLES):" << primitive << " start:" << idx_start << " end:" << idx_end << std::endl;
	glDrawElements(primitive, num_elem, GL_UNSIGNED_INT, (void*)(idx_start * sizeof(GLuint)));
}

void cDrawMesh::AddBuffer(int buff_num)
{
	mVbos.push_back(cVertexBuffer(buff_num, mState));
	mVbos[buff_num].mRenderID = mVbos[buff_num].mRenderID + 1;
}

//attr_info.mAttribNumber = eAttributePosition
//out_mesh->LoadVBuffer(attr_info.mAttribNumber, sizeof(float)* pos_size, (GLubyte*)pos_data, 0, 1, &attr_info);
void cDrawMesh::LoadVBuffer(unsigned int buffer_num, int data_size, GLubyte *data, int data_offset, int num_attr, tAttribInfo *attr_info)
{
	if (buffer_num >= mVbos.size())
		AddBuffer(buffer_num);

	mVbos[buffer_num].LoadBuffer(data_size, data, data_offset, num_attr, attr_info);
}

void cDrawMesh::LoadIBuffer(int num_elem, int elem_size, int *data)
{
	// no need to bind vertex array buffer, since this is an index buffer
	mIbo.LoadBuffer(num_elem, elem_size, data);
	mNumElem = num_elem;
}

// by having a range, we can choose to only update the buffers we have changed
// extent is the number of buffers after the base index to update
void cDrawMesh::SyncGPU(unsigned int base, size_t extent)
{
	if (base + extent > mVbos.size() ||
		extent == 0)
		extent = mVbos.size() - base;

	// bind the vertex array object to store all the vertex settings
	mState.BindVAO();
	for (unsigned int i = base; i < extent; i++)
		mVbos[i].SyncBuffer();

	mIbo.SyncBuffer();
}

int cDrawMesh::GetNumFaces() const
{
	const int verts_per_face = 3;
	int num_faces = mIbo.GetNumElems() / verts_per_face;
	return num_faces;
}

int cDrawMesh::GetNumVerts() const
{
	int num_verts = 0;
	if (mVbos.size() > 0)
	{
		const cVertexBuffer& vbos = mVbos[0];
		int comp = vbos.mAttrInfo->mNumComp;
		int attr_size = vbos.mAttrInfo->mAttribSize;
		int size = vbos.mSize;
		num_verts = size / (comp * attr_size);
	}
	return num_verts;
}