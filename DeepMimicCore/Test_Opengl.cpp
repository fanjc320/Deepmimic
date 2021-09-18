#include <iostream>

#include "DeepMimicCore.h"

#include "util/FileUtil.h"

#include "render/DrawUtil.h"
#include "render/TextureDesc.h"

#include "math.h"
using namespace std;
//GLEW��windows����������Ĭ��ֻ֧��opengl1.1�Ļ������� OpenGL���ڶ���չ��4.4�ˣ�Ҫʹ����ЩOpenGL�ĸ߼����ԣ��ͱ����������µ���չ��
//���⣬��ͬ���Կ���˾��Ҳ�ᷢ��һЩֻ���Լ��Կ���֧ �ֵ���չ��������Ҫ�����������������ò�ȥѰ�����µ�glext.h, ����GLEW��չ�⣬�����Ҳ����Ϊ�Ҳ��������Ľӿڶ����գ�
//��ΪGLEW���Զ�ʶ �����ƽ̨��֧�ֵ�ȫ��OpenGL�߼���չ������Ҳ����˵��ֻҪ����һ��glew.hͷ�ļ��������ʹ��gl, glu, glext, wgl, glx��ȫ ��������

//1. OpenGL�д��ڵ������ǵѿ�������ϵ��ԭ���ڴ��ڵ����ĵ㣬���Һ����±߽�����ֱ�Ϊ - 1��1��1�� - 1��ˮƽ��������x�ᣬ��ֱ��������y�ᡣ
//2. ͼ�񴰿�ÿ�θı䣬���������ػ档
//3. ������Щ��ͼ�εĺ����Ƕ��������ȷ��������ֱ���ƣ�˳�����Ҫ��

GLuint VBO;//��ѡ����һ���Լ���3Dָ��ṹ��������Ϣ��ʾ

static void RenderSceneCB()
{
	glClear(GL_COLOR_BUFFER_BIT);

	//���û���飬�󶨻���飬��仺���
	glEnableVertexAttribArray(0);

	//����Ⱦǰ��Ҫ���ÿ�����Ⱦ�ܵ����ԣ�����Ⱦ��������Ҫ�ر�
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0 ,3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_POINT, 0, 1);//p1:�������ͣ�p2:����Vertex��ʼλ�ã�p3:����Vertex��Ŀ


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

	glGenBuffers(1, &VBO);//���ڴ洢���㻺������
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
}


int n = 3600;  //Բ���ƴ���
float PI = 3.1415926f;
float R = 0.8f;  //�뾶

void myDisplay(void)
{
	//��Բ
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);
	glColor4f(0, 0, 1, 0);
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(R * cos(2 * PI * i / n), R * sin(2 * PI * i / n));   //���嶥��
	}
	glEnd();
	glFlush();

	cout << "��Բ�ɹ��������������" << endl;
	system("pause");

	//�������
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);
	glColor4f(0, 0, 1, 0);
	glBegin(GL_LINE_LOOP);

	//����ABCDE�ֱ�������ǵ�5������
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

	cout << "������ǳɹ��������������" << endl;
	system("pause");

	//���������ߣ���5������x������ѹ��
	GLfloat x = -1.0;
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_LINES);

	glVertex2f(-1.0f, 0.0f);  //�����ĵ�ʮ��
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
	cout << "���������" << endl;
}


//int main(int argc, char* argv[])
//{
//	glutInit(&argc, argv);   //��ʼ��GLUT
//	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
//	glutInitWindowPosition(100, 100);
//	glutInitWindowSize(400, 400);
//	glutCreateWindow("My first OpenGL program");
//	glutDisplayFunc(&myDisplay);   //�ص����� 
//	glutMainLoop();    //������ʾ�������ڸı�����»���ͼ��
//	return 0;
//}

//��������������������������������
//��Ȩ����������ΪCSDN������ - ��Ұ - ����ԭ�����£���ѭCC 4.0 BY - SA��ȨЭ�飬ת���븽��ԭ�ĳ������Ӽ���������
//ԭ�����ӣ�https ://blog.csdn.net/dcrmg/article/details/53057394

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