#include <iostream>

#include "DeepMimicCore.h"

#include "util/FileUtil.h"

#include "render/DrawUtil.h"
#include "render/TextureDesc.h"

#include "math.h"
using namespace std;
//GLEW：windows开发环境，默认只支持opengl1.1的环境，但 OpenGL现在都发展到4.4了，要使用这些OpenGL的高级特性，就必须下载最新的扩展，
//另外，不同的显卡公司，也会发布一些只有自家显卡才支 持的扩展函数，你要想用这数涵数，不得不去寻找最新的glext.h, 有了GLEW扩展库，你就再也不用为找不到函数的接口而烦恼，
//因为GLEW能自动识 别你的平台所支持的全部OpenGL高级扩展函数。也就是说，只要包含一个glew.h头文件，你就能使用gl, glu, glext, wgl, glx的全 部函数。

//1. OpenGL中窗口的坐标是笛卡尔坐标系，原点在窗口的中心点，左右和上下边界坐标分别为 - 1、1、1、 - 1。水平方向上是x轴，垂直方向上是y轴。
//2. 图像窗口每次改变，都会引发重绘。
//3. 绘制这些简单图形的核心是顶点坐标的确定，依序分别绘制，顺序很重要。

GLuint VBO;//首选定义一个自己的3D指标结构，方便信息表示

static void RenderSceneCB()
{
	glClear(GL_COLOR_BUFFER_BIT);

	//设置缓存块，绑定缓存块，填充缓存块
	glEnableVertexAttribArray(0);

	//在渲染前需要设置开启渲染管道属性，在渲染结束后需要关闭
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0 ,3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_POINT, 0, 1);//p1:绘制类型，p2:绘制Vertex起始位置，p3:绘制Vertex数目


	glDisableVertexAttribArray(0);

	glutSwapBuffers();
}

static void InitializeGlutCallbacks()
{
	glutDisplayFunc(RenderSceneCB);
}

static void CreateVertexBuffer()
{
	Eigen::Vector3f v[1];
	v[0] = Eigen::Vector3f(0.0f, 0.0f ,0.0f);

	glGenBuffers(1, &VBO);//用于存储定点缓存数据
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
}


int n = 3600;  //圆绘制次数
float PI = 3.1415926f;
float R = 0.8f;  //半径

void myDisplay(void)
{
	//画圆
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);
	glColor4f(0, 0, 1, 0);
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(R * cos(2 * PI * i / n), R * sin(2 * PI * i / n));   //定义顶点
	}
	glEnd();
	glFlush();

	cout << "画圆成功，任意键继续…" << endl;
	system("pause");

	//画五角星
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);
	glColor4f(0, 0, 1, 0);
	glBegin(GL_LINE_LOOP);

	//以下ABCDE分别是五角星的5个顶点
	GLfloat xA = R * cos(90 * 2 * PI / 360);
	GLfloat yA = R * sin(90 * 2 * PI / 360);

	GLfloat xB = R * cos(306 * 2 * PI / 360);
	GLfloat yB = R * sin(306 * 2 * PI / 360);

	GLfloat xC = R * cos(162 * 2 * PI / 360);
	GLfloat yC = R * sin(162 * 2 * PI / 360);

	GLfloat xD = R * cos(18 * 2 * PI / 360);
	GLfloat yD = R * sin(18 * 2 * PI / 360);

	GLfloat xE = R * cos(234 * 2 * PI / 360);
	GLfloat yE = R * sin(234 * 2 * PI / 360);

	glVertex2f(xA, yA);
	glVertex2f(xB, yB);
	glVertex2f(xC, yC);
	glVertex2f(xD, yD);
	glVertex2f(xE, yE);
	glEnd();
	glFlush();

	cout << "画五角星成功，任意键继续…" << endl;
	system("pause");

	//画正弦曲线，画5个，在x方向上压缩
	GLfloat x = -1.0;
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_LINES);

	glVertex2f(-1.0f, 0.0f);  //过中心的十字
	glVertex2f(1.0f, 0.0f);
	glVertex2f(0.0f, -1.0f);
	glVertex2f(0.0f, 1.0f);

	glEnd();
	glBegin(GL_LINE_STRIP);
	for (float x = -5 * PI; x < 5 * PI; x += 0.1f)
	{
		glVertex2f(x / (5 * PI), sin(x));
	}
	glEnd();
	glFlush();
	cout << "正弦线完成" << endl;
}


//int main(int argc, char* argv[])
//{
//	glutInit(&argc, argv);   //初始化GLUT
//	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
//	glutInitWindowPosition(100, 100);
//	glutInitWindowSize(400, 400);
//	glutCreateWindow("My first OpenGL program");
//	glutDisplayFunc(&myDisplay);   //回调函数 
//	glutMainLoop();    //持续显示，当窗口改变会重新绘制图形
//	return 0;
//}

//————————————————
//版权声明：本文为CSDN博主「 - 牧野 - 」的原创文章，遵循CC 4.0 BY - SA版权协议，转载请附上原文出处链接及本声明。
//原文链接：https ://blog.csdn.net/dcrmg/article/details/53057394

//int main(int argc, char** argv)
//{
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
//	glutInitWindowSize(600, 400);
//	glutInitWindowPosition(100, 100);
//	glutCreateWindow("Tutorial 02");
//
//	InitializeGlutCallbacks();
//
//	//Must be done after glut is initialized!
//	GLenum res = glewInit();
//	if (res != GLEW_OK) {
//		fprintf(stderr, "ERRor: '%s' \n", glewGetErrorString(res));
//		return 1;
//	}
//
//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//
//	CreateVertexBuffer();
//	glutMainLoop();
//
//	return 0;
//}