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

#pragma once

#include <GL/glew.h>
#include <vector>

struct tAttribInfo
{
    unsigned int mAttribNumber;  // which attribute within the mesh this info is for
    unsigned int mAttribSize;    // total size in bytes of one attribute
    unsigned int mNumComp;       // the number of components each of these atributes holds
    unsigned int mDataOffset;    // the number of bytes from the start of the array to the first instance of this attribute
    unsigned int mDataStride;    // the number of bytes between instances of this attribute
};

/**
 * A cRenderState object stores a collection of GPU vertex buffer objects to be used to render a
 * single mesh at a time. It also stores all of the associated state setup for
 * those buffers, such as attribute stride and offset within a vertex
 * array object. 
 */
class cRenderState
{
public:
    cRenderState()
    {
        glGenVertexArrays(1, &mVaoID);
    }
    ~cRenderState()
    {
        glDeleteVertexArrays(1, &mVaoID);

        if (!mVboIDs.empty())
            glDeleteBuffers(static_cast<GLsizei>(mVboIDs.size()), &mVboIDs[0]);
    }
    void BindVAO()
    {
        glBindVertexArray(mVaoID);
    }
    void BindVBO(unsigned int i, GLenum target = GL_ARRAY_BUFFER)
    {
        /*void glBindBuffer(GLenum target,
            GLuint buffer);

        Parameters
            target
            Specifies the target to which the buffer object is bound.The symbolic constant must be GL_ARRAY_BUFFER, GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_PIXEL_PACK_BUFFER, GL_PIXEL_UNPACK_BUFFER, GL_TEXTURE_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER, or GL_UNIFORM_BUFFER.

            buffer
            Specifies the name of a buffer object.*/
        glBindBuffer(target, operator[](i));
    }

    void BindIBO(unsigned int i)
    { BindVBO(i, GL_ELEMENT_ARRAY_BUFFER); }


    void SetNextBufferData(std::size_t num_bytes, unsigned char *data)
    {
        SetBufferData(mDirty++, num_bytes, data);
    }

    void SetBufferData(unsigned int i, std::size_t num_bytes, unsigned char *data = NULL)
    {
        BindVBO(i);
        if (mBytes[i] < num_bytes)
        {
            //�������ʼ��֮�������ʹ��glBufferData()�����ݿ������������

                /*void glBufferData(GLenum target��GLsizeiptr size, const GLvoid * data, GLenum usage);
                ��һ������target����ΪGL_ARRAY_BUFFER��GL_ELEMENT_ARRAY��sizeΪ�����������ֽ�����������������ΪԴ��������ָ�룬
                ��dataΪNULL����VBO����Ԥ���������ݴ�С���ڴ�ռ䡣���һ������usage��־λVBO����һ��������ʾ�����ṩ����������ʹ�ã�static��dynamic��stream����read��copy��draw��*/
            mBytes[i] = static_cast<unsigned int>(num_bytes);
            glBufferData(GL_ARRAY_BUFFER, num_bytes, data, GL_STATIC_DRAW); // Static, because all changes happen in the edit mesh
        }
        else
            glBufferSubData(GL_ARRAY_BUFFER, 0, num_bytes, data);
    }
    void SetAttributeData(tAttribInfo &info)
    { SetAttributeData(info.mAttribNumber, info.mNumComp, info.mDataOffset, info.mDataStride); }

    void SetAttributeData(unsigned int mAttribNumber, unsigned int mNumComp, unsigned int mDataOffset, unsigned int mDataStride)
    {
        //void glEnableVertexAttribArray(GLuint index);
        //glEnableVertexAttribArray and glEnableVertexArrayAttrib enable the generic vertex attribute array specified by index.
        /*We assigned the attribute index of the position attribute to 0 in the vertex shader, 
            so the call to glEnableVertexAttribArray(0) enables the attribute index for the position attribute. ... 
            If the attribute is not enabled, it will not be used during rendering.*/

        //��һ������ָ������Ҫ���õĶ������ԡ����ǵ������ڶ�����ɫ����ʹ��layout(location = 0)������position�������Ե�λ��ֵ(Location)�������԰Ѷ������Ե�λ��ֵ����Ϊ0����Ϊ����ϣ�������ݴ��ݵ���һ�����������У������������Ǵ���0��
        //    �ڶ�������ָ���������ԵĴ�С������������һ��vec3������3��ֵ��ɣ����Դ�С��3��
        //    ����������ָ�����ݵ����ͣ�������GL_FLOAT(GLSL��vec * �����ɸ�����ֵ��ɵ�)��
        //    �¸��������������Ƿ�ϣ�����ݱ���׼��(Normalize)�������������ΪGL_TRUE���������ݶ��ᱻӳ�䵽0�������з�����signed������ - 1����1֮�䡣���ǰ�������ΪGL_FALSE��
        //    �����������������(Stride)�������������������Ķ���������֮��ļ���������¸���λ��������3��float֮�����ǰѲ�������Ϊ3 * sizeof(float)��Ҫע�������������֪����������ǽ������еģ���������������֮��û�п�϶������Ҳ��������Ϊ0����OpenGL�������岽���Ƕ��٣�ֻ�е���ֵ�ǽ�������ʱ�ſ��ã���һ�������и���Ķ������ԣ����Ǿͱ����С�ĵض���ÿ����������֮��ļ���������ں���ῴ����������ӣ���ע: �����������˼��˵���Ǵ�������Եڶ��γ��ֵĵط�����������0λ��֮���ж����ֽڣ���
        //    ���һ��������������void * ������ָ�룬 ���ֵ�ܵ�VBO��Ӱ�죻1���ڲ�ʹ��VBO������£�����һ��ָ�룬ָ�������Ҫ�ϴ�����������ָ�룬��Ŀ��ͨ���ڲ�ʹ��VBO������£�����֮ǰ��ִ��glBindBuffer(GL_ARRAY_BUFFER, 0)������ᵼ�����鶥����Ч�������޷���ʾ��2��ʹ��VBO������£���Ҫִ��glBindBuffer(GL_ARRAY_BUFFER, 1)�����һ�����Ʒ���Ļ�����󱻰���GL_ARRAY_BUFFERĿ�꣨��glBindBuffer���Ҵ�ʱһ�������������鱻ָ���ˣ���ôpointer�������û���������ݴ洢�����ֽ�ƫ���������ң��������󶨣�GL_ARRAY_BUFFER_BINDING���ᱻ��Ϊ����Ϊindex�Ķ�����������ͻ���״̬����ʱָ��ָ��ľͲ��Ǿ���������ˡ���Ϊ�����Ѿ������ڻ������ˡ������ָ���ʾλ�������ڻ�������ʼλ�õ�ƫ����(Offset)
        //    ��������������������������������
        //    ��Ȩ����������ΪCSDN������soft_logic����ԭ�����£���ѭCC 4.0 BY - SA��ȨЭ�飬ת���븽��ԭ�ĳ������Ӽ���������
        //    ԭ�����ӣ�https ://blog.csdn.net/weixin_37459951/article/details/96433508

        glEnableVertexAttribArray(mAttribNumber);
        glVertexAttribPointer(mAttribNumber, mNumComp, GL_FLOAT, GL_FALSE,
                              mDataStride, (GLvoid *) mDataOffset);
    }
    /*void glGenBuffers(GLsizei  	n,
            GLuint * buffers);
        Parameters
            n
            Specifies the number of buffer object names to be generated.

            buffers
            Specifies an array in which the generated buffer object names are stored.*/
    GLuint operator[](unsigned int i)
    {
        if (i >= mVboIDs.size())
        {
            mVboIDs.resize(i+1, 0);
            mBytes.resize(i+1, 0);
        }
        
        if (mVboIDs[i] == 0)
            glGenBuffers(1, &mVboIDs[i]);
        return mVboIDs[i];
    }

    void MakeClean()
    { mDirty = 0; }

private:
    int mDirty; // used for loading to copy into appropriate buffers

    GLuint mVaoID;
    std::vector<GLuint> mVboIDs;
	std::vector<GLuint> mBytes; // used to avoid needless resizing of buffers
};