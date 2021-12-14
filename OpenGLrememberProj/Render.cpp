#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include <cmath>

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

double* vector(double* Tochka1, double* Tochka2, double vector_[])
{
	vector_[0] = Tochka1[0] - Tochka2[0];
	vector_[1] = Tochka1[1] - Tochka2[1];
	vector_[2] = Tochka1[2] - Tochka2[2];
	return vector_;
}

double* normal (double* Tochka1, double* Tochka2, double* Tochka3)
{
	double Vector1[3];
	double Vector2[3];
	vector(Tochka1, Tochka2, Vector1);
	vector(Tochka2, Tochka3, Vector2);

	double normal_[] = { 0,0,0 };
	double dlina_vectora;
	normal_[0] = Vector1[1] * Vector2[2] - Vector2[1] * Vector1[2];
	normal_[1] = -Vector1[0] * Vector2[2] + Vector2[0] * Vector1[2];
	normal_[2] = Vector1[0] * Vector2[1] - Vector2[0] * Vector1[1];
	dlina_vectora = sqrt(normal_[0] * normal_[0] + normal_[1] * normal_[1]+ normal_[2] * normal_[2]);
	normal_[0] /= dlina_vectora;
	normal_[1] /= dlina_vectora;
	normal_[2] /= dlina_vectora;
	return normal_;
}

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


	//������ ��������� ���������� ��������

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	double A1[] = { 8,4,0 };
	double A2[] = { 3,5,0 };
	double A3[] = { 0,1,0 };
	double A4[] = { -6,2,0 };
	double A5[] = { -8,-5,0 };
	double A6[] = { -4,-8,0 };
	double A7[] = { 0,-1,0 };
	double A8[] = { 5,-5,0 };

	double A1_[] = { 8,4,2 };
	double A2_[] = { 3,5,2 };
	double A3_[] = { 0,1,2 };
	double A4_[] = { -6,2,2 };
	double A5_[] = { -8,-5,2 };
	double A6_[] = { -4,-8,2 };
	double A7_[] = { 0,-1,2 };
	double A8_[] = { 5,-5,2 };

	glBegin(GL_TRIANGLES);
	//1
	glColor3d(1, 0, 1);
	glNormal3dv(normal(A7,A1,A8));
	glVertex3dv(A8);
	glVertex3dv(A1);
	glVertex3dv(A7);
	//2
	glVertex3dv(A1);
	glVertex3dv(A7);
	glVertex3dv(A3);
	//3
	glVertex3dv(A1);
	glVertex3dv(A2);
	glVertex3dv(A3);
	//4
	glColor3d(0, 1, 0);
	glVertex3dv(A7);
	glVertex3dv(A6);
	glVertex3dv(A5);
	//5
	glColor3d(0, 1, 0);
	glVertex3dv(A7);
	glVertex3dv(A5);
	glVertex3dv(A3);
	//6
	glColor3d(0, 1, 0);
	glVertex3dv(A3);
	glVertex3dv(A4);
	glVertex3dv(A5);

	//1_
	glColor3d(1, 0, 1);
	glNormal3dv(normal(A8_, A1_, A7_));
	glVertex3dv(A8_);
	glVertex3dv(A1_);
	glVertex3dv(A7_);
	//2_
	glVertex3dv(A1_);
	glVertex3dv(A7_);
	glVertex3dv(A3_);
	//3_
	glVertex3dv(A1_);
	glVertex3dv(A2_);
	glVertex3dv(A3_);
	//4_
	glColor3d(0, 1, 0);
	glVertex3dv(A7_);
	glVertex3dv(A6_);
	glVertex3dv(A5_);
	//5_
	glColor3d(0, 1, 0);
	glVertex3dv(A7_);
	glVertex3dv(A5_);
	glVertex3dv(A3_);
	//6_
	glColor3d(0, 1, 0);
	glVertex3dv(A3_);
	glVertex3dv(A4_);
	glVertex3dv(A5_);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3dv(normal(A8, A1, A1_));
	glColor3d(1, 0, 1);
	glVertex3dv(A8_);
	glVertex3dv(A8);
	glVertex3dv(A1);
	glVertex3dv(A1_);

	glNormal3dv(normal(A1_, A1, A2_));
	glVertex3dv(A1_);
	glVertex3dv(A1);
	glVertex3dv(A2);
	glVertex3dv(A2_);

	glNormal3dv(normal(A1_, A1, A2_));
	glVertex3dv(A2_);
	glVertex3dv(A2);
	glVertex3dv(A3);
	glVertex3dv(A3_);

	glColor3d(0, 1, 0);
	glNormal3dv(normal(A3_, A3, A4));
	glVertex3dv(A3_);
	glVertex3dv(A3);
	glVertex3dv(A4);
	glVertex3dv(A4_);

	glNormal3dv(normal(A4_, A4, A5));
	glVertex3dv(A4_);
	glVertex3dv(A4);
	glVertex3dv(A5);
	glVertex3dv(A5_);

	glNormal3dv(normal(A5_, A5, A6));
	glVertex3dv(A5_);
	glVertex3dv(A5);
	glVertex3dv(A6);
	glVertex3dv(A6_);

	glNormal3dv(normal(A6_, A6, A7));
	glVertex3dv(A6_);
	glVertex3dv(A6);
	glVertex3dv(A7);
	glVertex3dv(A7_);

	glColor3d(1, 0, 1);
	glNormal3dv(normal(A7_, A7, A8));
	glVertex3dv(A7_);
	glVertex3dv(A7);
	glVertex3dv(A8);
	glVertex3dv(A8_);

	glEnd();
	//����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}