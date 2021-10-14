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
            //当缓存初始化之后，你可以使用glBufferData()将数据拷贝到缓存对象。

                /*void glBufferData(GLenum target，GLsizeiptr size, const GLvoid * data, GLenum usage);
                第一个参数target可以为GL_ARRAY_BUFFER或GL_ELEMENT_ARRAY。size为待传递数据字节数量。第三个参数为源数据数组指针，
                如data为NULL，则VBO仅仅预留给定数据大小的内存空间。最后一个参数usage标志位VBO的另一个性能提示，它提供缓存对象将如何使用：static、dynamic或stream、与read、copy或draw。*/
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

        //第一个参数指定我们要配置的顶点属性。还记得我们在顶点着色器中使用layout(location = 0)定义了position顶点属性的位置值(Location)吗？它可以把顶点属性的位置值设置为0。因为我们希望把数据传递到这一个顶点属性中，所以这里我们传入0。
        //    第二个参数指定顶点属性的大小。顶点属性是一个vec3，它由3个值组成，所以大小是3。
        //    第三个参数指定数据的类型，这里是GL_FLOAT(GLSL中vec * 都是由浮点数值组成的)。
        //    下个参数定义我们是否希望数据被标准化(Normalize)。如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是 - 1）到1之间。我们把它设置为GL_FALSE。
        //    第五个参数叫做步长(Stride)，它告诉我们在连续的顶点属性组之间的间隔。由于下个组位置数据在3个float之后，我们把步长设置为3 * sizeof(float)。要注意的是由于我们知道这个数组是紧密排列的（在两个顶点属性之间没有空隙）我们也可以设置为0来让OpenGL决定具体步长是多少（只有当数值是紧密排列时才可用）。一旦我们有更多的顶点属性，我们就必须更小心地定义每个顶点属性之间的间隔，我们在后面会看到更多的例子（译注: 这个参数的意思简单说就是从这个属性第二次出现的地方到整个数组0位置之间有多少字节）。
        //    最后一个参数的类型是void * ，数据指针， 这个值受到VBO的影响；1：在不使用VBO的情况下，就是一个指针，指向的是需要上传到顶点数据指针，项目中通常在不使用VBO的情况下，绘制之前，执行glBindBuffer(GL_ARRAY_BUFFER, 0)，否则会导致数组顶点无效，界面无法显示；2：使用VBO的情况下，先要执行glBindBuffer(GL_ARRAY_BUFFER, 1)，如果一个名称非零的缓冲对象被绑定至GL_ARRAY_BUFFER目标（见glBindBuffer）且此时一个定点属性数组被指定了，那么pointer被当做该缓冲对象数据存储区的字节偏移量。并且，缓冲对象绑定（GL_ARRAY_BUFFER_BINDING）会被存为索引为index的顶点属性数组客户端状态；此时指针指向的就不是具体的数据了。因为数据已经缓存在缓冲区了。这里的指针表示位置数据在缓冲中起始位置的偏移量(Offset)
        //    ————————————————
        //    版权声明：本文为CSDN博主「soft_logic」的原创文章，遵循CC 4.0 BY - SA版权协议，转载请附上原文出处链接及本声明。
        //    原文链接：https ://blog.csdn.net/weixin_37459951/article/details/96433508

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