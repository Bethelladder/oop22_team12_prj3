/* Simple geometry viewer:  Left mouse: rotate;  Right mouse:   translate;  ESC to quit. */
#include <assert.h>
#include <math.h>
#include <stdlib.h>
//�߰��ؾ��� #include <iostream>
#include <GL/glut.h>
#include <iostream>
// you may try "#include <GL/glut.h>" if "#include <GLUT/glut.h>" wouldn't work
//#include <GL/glut.h>

//GLdouble rotMatrix[4][16];
//�߰��ؾ��� const int NO_SPHERE = 17;
const int NO_SPHERE = 3;
const int WALL_ID = 1000;
/* �߰��ؾ���
const int WALL_WIDTH=16;
const int WALL_HIGHT=16;
*/
/*�߰��ؾ���
int rotate_x=180, rotate_y=80;
int choice=2;*/
int rotate_x = 0, rotate_y = 0;
int choice = 1;

GLfloat BoxVerts[][3] = { // �ٴ��� ������ ��ǥ, ������ �������ͷ� �ٴ�ũ�⸦ �ٲٷ��� CWALL�� �����ڸ� �ٲ��
	{-1.0,-1.0,-1.0},
	{-1.0,-1.0,1.0},
	{-1.0,1.0,-1.0},
	{-1.0,1.0,1.0},
	{1.0,-1.0,-1.0},
	{1.0,-1.0,1.0},
	{1.0,1.0,-1.0},
	{1.0,1.0,1.0}
};

GLfloat bNorms[][3] = {
	{-1.0,0.0,0.0},
	{1.0,0.0,0.0},
	{0.0,1.0,0.0},
	{0.0,-1.0,0.0},
	{0.0,0.0,-1.0},
	{0.0,0.0,1.0}
};

int cubeIndices[][4] = {
	{0,1,3,2},
	{4,6,7,5},
	{2,3,7,6},
	{0,4,5,1},
	{0,2,6,4},
	{1,5,7,3}
};

/* Viewer state */
float sdepth = 20; //�ָ��������� �����̼� ������
float zNear = 1.0, zFar = 100.0;
float aspect = 5.0 / 4.0;
float xcam = 0, ycam = 0;
long xsize, ysize;
int downX, downY;
bool leftButton = false, middleButton = false, rightButton = false;
int i, j;
GLfloat light0Position[] = { 0, 1, 0, 1.0 };
int displayMenu, mainMenu;

void MyIdleFunc(void) { glutPostRedisplay(); } /* things to do while idle */
void RunIdleFunc(void) { glutIdleFunc(MyIdleFunc); }
void PauseIdleFunc(void) { glutIdleFunc(NULL); }
void renderScene();
/* �߰��ؾ� ��
void InitObjects();
void auto_restart_game();
void restart_game();
float get_distance(float x1, float x2, float z1, float z2) {
	float deltax=x2-x1;
	float deltaz=z2-z1;
	float ret;

	ret=sqrt(deltax*deltax+deltaz*deltaz);
	return (ret);
}
*/

class CSphere
{
public:
	float center_x, center_y, center_z;
	float color_r, color_g, color_b;
	float dir_x, dir_y, dir_z;
	float speed;

public:
	GLdouble m_mRotate[16];
	CSphere()
	{
		center_x = center_y = center_z = 0.0;
	}

	void init()
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix(); // ���� �𵨺� ����� ����, �׸��� �� ������ �׷����� �͵鿡 ���� ���� �𵨺� ����� ������ �ٽ� �������Ѱ��鼭 ǥ��
		glLoadIdentity(); // ����� ������ķ� �ʱ�ȭ�Ѵ�.
		glGetDoublev(GL_MODELVIEW_MATRIX, m_mRotate); // ��Ʈ���� ����, ���� ���� ��Ʈ����
		glPopMatrix(); // �׵��� �����ߴ� ����� ���ְ� push�� ����� �ٽ� �ҷ��´�.
	}

	void setCenter(float x, float y, float z)
	{
		center_x = x;    center_y = y;    center_z = z;
	}

	void setColor(float r, float g, float b)
	{
		color_r = r; color_g = g; color_b = b;
	}
	/*�߰��ؾ��� 
	bool hasIntersected(float x, float z)
	{
		float deltaX;
		float deltaZ;

		deltaX = this->center_x - x;
		deltaZ = this->center_z - z;
		if (sqrt(deltaX * deltaX + deltaZ * deltaZ) <= 0.85)
			return (true);
		return (false);
	}
	int currentTime, previousTime = -1;

	void hitBy(CSphere hitSphere)
	{
		float deltaX;
		float deltaZ;
		float distance;
		float hit_angle;
		float temp;

		deltaX = hitSphere.center_x - this->center_x;
		deltaZ = hitSphere.center_z - this->center_z;

		distance = sqrt(deltaX * deltaX + deltaZ * deltaZ);
		hit_angle = acosf(deltaX / distance);
		temp = cos(hit_angle) * dir_x - sin(hit_angle) * dir_z;
		dir_z = sin(hit_angle) * dir_x + cos(hit_angle) * dir_z;
		dir_x = temp;

		//        if (deltaZ > 0)
		//            hit_angle = M_PI + M_PI - hit_angle;
				// �� ���� ���� ���̴� ���� ����! ���̸� ��¦ �� ������ �����̴� ���� �Ű��ش�.
		while (get_distance(this->center_x + 0.03 * 3 * dir_x, hitSphere.center_x, this->center_z + 0.03 * 3 * dir_z, hitSphere.center_z) < 0.85) {

			temp = cos(hit_angle) * dir_x - sin(hit_angle) * dir_z;
			dir_z = sin(hit_angle) * dir_x + cos(hit_angle) * dir_z;
			dir_x = temp;
			if (get_distance(this->center_x + 0.03 + 0.03 * 3 * dir_x, hitSphere.center_x, this->center_z + 0.03 + 0.03 * 3 * dir_z, hitSphere.center_z) > get_distance(this->center_x - 0.03 + 0.03 * 3 * dir_x, hitSphere.center_x, this->center_z - 0.03 + 0.03 * 3 * dir_z, hitSphere.center_z))
			{
				this->center_x += 0.03;
				this->center_z += 0.03;
			}
			else
			{
				this->center_x -= 0.03;
				this->center_z -= 0.03;
			}
		}
		//std::cout << timeDelta << std::endl;
	}
	*/
	void draw()
	{
		glLoadIdentity(); //������ķ� �ʱ�ȭ
		glTranslatef(0.0, 0.0, -sdepth); //+Z������ ȭ�鿡�� �츮�� ���¹����̹Ƿ� -���ϸ� ��ҵǴ� ȿ��, wall�� ������ ���߱����� ����ѵ�
		glMultMatrixd(m_mRotate); // ���콺 �̵������� motion�ݹ��Լ����� m_mrotate����� �����Ǵµ� �� mult�Լ��� ������Ŀ� ���Ͽ� m_mrotate��� ȸ���Ѵ�.
		glTranslated(center_x, center_y, center_z); // �߾����� �̵��Ѵ�.
		glColor3f(color_r, color_g, color_b); //�� ����
		//�߰��ؾ��� glutSoilidSphere(0.4,20,16);
		glutSolidSphere(0.5, 20, 16); //0.5�� ������ ������ �� ���ڴ� ���� ��Ÿ���� �漱�� ����
	}
};

class CWall // �߰��ؾ��� CSphere���� ���
{

public:
	float width, height, depth;
	float center_x, center_y, center_z;
	float color_r, color_g, color_b;
	/*float up_wall=8;
	float left_wall=8;
	float right_wall=-8;
	
	*/

	GLfloat Verts[8][3];

public:
	GLdouble m_mRotate[16];
	CWall(float w = 0.0, float h = 0.0, float d = 0.0)
	{
		width = w; height = h; depth = d;
		color_r = 0.0; color_g = 1.0; color_b = 0.0;

		int i, j;
		float coef;
		for (i = 0; i < 8; i++) {
			for (j = 0; j < 3; j++) {
				if (j == 0) coef = w / 2.0;
				if (j == 1) coef = h / 2.0;
				if (j == 2) coef = d / 2.0;
				Verts[i][j] = coef * BoxVerts[i][j]; // BoxVerts�� ������ ��������, coef�� ������ �������Ϳ� �������� ũ��
			}
		}
	}

	void init() //initGL���� initrotate�� ȣ���ϰ� �ű⼭ ȣ���
	{ // openGL���� ����� mode�� ������ ��, transelate, rotate���� ��ȯ�� ��ġ�µ�, �Ϲ������� ��ȯ�� �� �� LoadIdentity�� �ʱ�ȭ�� ����� �Ѵ�. �ȱ׷��� �����Ǵϱ�.. �ٵ� �� ��� ����
		//������ �̿��ؼ� ���� ��ġ�� push�� ������ �� ������ �� �Ŀ� �׳� pop���� �������� �ִ�.
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glGetDoublev(GL_MODELVIEW_MATRIX, m_mRotate); //��Ʈ���� ����, ���� ���� ��Ʈ����, m_mRotate�� ��������� ��
		glPopMatrix();
	}

	void setSize(float w, float h, float d)
	{
		width = w;
		height = h;
		depth = d;
	}

	void setCenter(float x, float y, float z)
	{
		center_x = x;    center_y = y;    center_z = z;
	}

	void setColor(float r, float g, float b)
	{
		color_r = r; color_g = g; color_b = b;
	}

	void draw() //display �ݹ��Լ����� ȣ���
	{
		glLoadIdentity();
		glTranslatef(0.0, 0.0, -sdepth); //  �� ����� ������ �𵨺���Ŀ� �������� �����̵��� �Ѵ�.
														//�𵨺���Ŀ� 1 0 0 0.0
														//			   0 1 0 0.0
														//			   0 0 1 -sdepth
								//									   0 0 0   1  �� �������� �ȴ�.
						//ȭ�鿡�� �츮���� z���Ͱ� +�� �Ǵ����̹Ƿ� sdepth�� Ŀ������ wall�� �۾����� ȿ���� ���� �� �ִ�.
		glMultMatrixd(m_mRotate); //���� ��Ŀ� rotate��������Ѵ�. m_mRotate�� motion�ݹ��Լ����� ���콺 �����ӿ����� �����ȴ�.
		glTranslatef(center_x, center_y, center_z); //wall�� center_x,y,z��ŭ�� ��ġ�� �����̵�

		glColor3f(color_r, color_g, color_b); //�� ����

		int i;
		int v1, v2, v3, v4;

		for (i = 0; i < 6; i++) {
			v1 = cubeIndices[i][0];
			v2 = cubeIndices[i][1];
			v3 = cubeIndices[i][2];
			v4 = cubeIndices[i][3];

			glBegin(GL_QUADS); //glbegin�� ������ �׸��� �����ߴٴ� ��
			glNormal3f(bNorms[i][0], bNorms[i][1], bNorms[i][2]);
			glVertex3f(Verts[v1][0], Verts[v1][1], Verts[v1][2]);
			glNormal3f(bNorms[i][0], bNorms[i][1], bNorms[i][2]);
			glVertex3f(Verts[v2][0], Verts[v2][1], Verts[v2][2]);
			glNormal3f(bNorms[i][0], bNorms[i][1], bNorms[i][2]);
			glVertex3f(Verts[v3][0], Verts[v3][1], Verts[v3][2]);
			glNormal3f(bNorms[i][0], bNorms[i][1], bNorms[i][2]);
			glVertex3f(Verts[v4][0], Verts[v4][1], Verts[v4][2]);
			glEnd(); //glbegin�� ���������� �����׸��� ��
		}
	}
	/*�߰��ؾ��� 
	bool hasUpIntersected(CSphere* sphere)
	{
		if (sphere->center_z + 0.425 >= WALL_HIGHT / 2)
			return (true);
		return (false);
	}

	bool hasRightLeftIntersected(CSphere* sphere)
	{
		if (sphere->center_x + 0.425 >= WALL_WIDTH / 2 || sphere->center_x - 0.425 <= -1 * WALL_WIDTH / 2)
			return (true);
		return (false);
	}

	void hitBy(CSphere* sphere)
	{
		if (hasUpIntersected(sphere))
		{
			sphere->dir_z = -(sphere->dir_z);
		}
		else if (hasRightLeftIntersected(sphere))
		{
			sphere->dir_x = -(sphere->dir_x);
		}
	}*/


};


/*�߰��ؾ��� 
CSphere g_sphere[NO_SPHERE];
CWall g_wall(WALL_WIDTH, 0.2, WALL_HIGHT);
*/





CSphere g_sphere[3];
CWall g_wall(11, 0.2, 11);

void ReshapeCallback(int width, int height)
{

	xsize = width;
	ysize = height;
	aspect = (float)xsize / (float)ysize;

	glViewport(0, 0, xsize, ysize); //ó�� �����Ǵ� ������� InitGL�� glutInitWindowsize���� ���������� ����Ǵ� ���α׷� ��ü�� ���� �� �Լ��� �ػ󵵿� ���� �����ȴ�. ���Ҹ��� ������
									// ���� 1920,1080���� ������ ���� �˱⽬���
	glMatrixMode(GL_PROJECTION); //openGL�� �𵨸��� GL_MODELVIEW, GL_PROJECTION, GL_TEXTURE 3���� ��ķ� ȭ�鿡ǥ���Ѵ�. ���� (1,1,1)�� �� �׸��ٸ� �ش� ��ǥ�� GL_MODELVIEW ��Ʈ������
								// ���ؼ� �������� ��ġ�� �����Ѵ�. ���� �������� ��ġ������ �ϳ����� GL_MODELVIEW ��Ʈ������ ����ȴٸ� ���� �ٸ� ��ġ�� �׷�����. �̷��� �׷��� ����
									//GL_PROJECTION ��İ� ������ ȭ��� �����ȴ�. �� GL_MODELVIEW�� �𵨸��� ���� ���� ��ġ��� GL_PROJECTION�� �ش� �𵨸��� ���������� ��� ǥ������ �����Ѵ�.
	
	/*
	GL_MODELVIEW�� ����: Ư����ǥ�� ������ �׸��ٸ� GL_MODELVIEW�� ���� �������� ��ġ�� ����, ���� �̰� ����ȴٸ� �����ٸ� ��ǥ�� �׷���
	glTranslatef, glScalef, glRotatef���� ���� ��ü�� ��ġ�� ��ȯ �Ǵ� ������ �̸� �𵨸� ��ȯ �̶�� ��,�𵨺�����̶� ��ǥ�� ��ȯ�� ���� ��ü�� ��ġ�� ������ �����ϱ� ����
	4x4��İ� ���ؽ����� �ϳ��� ���� ������ ��ķ� ��������� �𵨺���İ� ������

	GL_PROJECTION�� ����: GL_MODELVIEW�� �׷��� ������ ��ġ��� ������ GL_PROJECTION�� ��İ� ������ ���������� ��� ȭ�鿡 �Ѹ����ΰ��� ����Ѵ�.
	�������� glOrtho �������� gluPerspectivef
	
	
	*/
	
	
	glLoadIdentity(); // ������ķ� �ʱ�ȭ�Ѵ�.
	gluPerspective(64.0, aspect, zNear, zFar); //��������
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // ������ķ� �ʱ�ȭ�Ѵ�.	������ �����ߴ� ȸ������ �����ϰ� �ٽ� �����ϱ����� ������ �ؾ��Ѵ�. �� �ڵ尡 ������ ȸ���� ��� ��������ȴ�.
						//�̿Ͱ��� glMatrixMode(GL_MODELVIEW)���Ŀ� glLoadIdentity()�� �����°� �̷��� �����̴�.

	glutPostRedisplay();
}



void DisplayCallback(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //glclear�� ���۵��� �̸������� ������ �ٲ۴�.	
														//GL_COLOR_BUFFER_BIT�� initgl�� glclearcolor���� ������ ���̴�. 
														//���� 3������ 2������ �׸������� depth��� ������ �ִ�. ���� x,y�� depth���̿� ���� �׷����� ���� ��������.
														//�� �� ��ü�� �ϳ��� ���뱸 ����ϰ� ���̴µ�, 3�����׸� ��ü�� ����� ������ �Ѵ�.
	glMatrixMode(GL_MODELVIEW); 



	for (i = 0; i < NO_SPHERE; i++) g_sphere[i].draw(); //�� �׸���
	g_wall.draw(); // �� �׸���

	
	
	glutSwapBuffers(); // front���ۿ� back���۸� swapping �ϱ� ���Ѱ�, ����Ʈ���۳����� ȭ�鿡 �ѷ����� ���� ���ο� ������ ����ۿ� ���̰� ����ۿ� ����� �� �Ǹ� ����Ʈ�� ���� �ٲ��.
						//����ۿ� �׸��� �� �׷����� ������ۿ� ��°�� ��ü�Ѵ�. ����� �ĸ��� �Ͻÿ� ��ü�ǹǷ� ����ۿ� �̸� �غ��ص� �׸��� ��Ÿ����.
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); ���� ������µ�?
}

int space_flag = 0;

void KeyboardCallback(unsigned char ch, int x, int y)
{
	switch (ch)
	{
	case '1': choice = 1; break;
	case '2': choice = 2; break;
	case '3': choice = 3; break;

	case 32: //�����̽���
		if (space_flag) space_flag = 0;
		else {
			space_flag = 1;
			g_sphere[0].dir_x = g_sphere[2].center_x - g_sphere[0].center_x; //sphere[0]�� �����̽��� ������ �����̴� ���� ��
			g_sphere[0].dir_y = g_sphere[2].center_y - g_sphere[0].center_y;
			g_sphere[0].dir_z = g_sphere[2].center_z - g_sphere[0].center_z;
		}
		break; // SPACE_KEY

	case 27: //ESCŰ
		exit(0);
		break;
	}
	glutPostRedisplay();
}

void MouseCallback(int button, int state, int x, int y)
{ // GLUT_DOWN=���콺�� ������ �ִ� ����, LEFT,RIGHT,MIDDLE ���� ���� ������ �� �̷���
	downX = x; downY = y;
	leftButton = ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN));
	middleButton = ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN));
	rightButton = ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN));
	glutPostRedisplay();
}

void rotate(int id)
{
	glMatrixMode(GL_MODELVIEW); //�𵨺� ���� ����ϰڴ�.
	glPushMatrix(); // ���� ��� ����
	glLoadIdentity(); // ������ķ� �ʱ�ȭ (���� ������İ� �ٸ�)

	glRotated(((double)rotate_y), 1.0, 0.0, 0.0); //������ķ� �ʱ�ȭ�Ȱ� rotate����
	glRotated(((double)rotate_x), 0.0, 1.0, 0.0); // ��������

	if (id < NO_SPHERE) {
		glGetDoublev(GL_MODELVIEW_MATRIX, g_sphere[id].m_mRotate); //��Ʈ���� ����, ���� ���� ��Ʈ����, �� modelview_matrix������ g_sphere...�� �ٷ� ������ rotate�� ����� �޾ƿ��ڴ�. 
	}

	if (id == WALL_ID) {
		glGetDoublev(GL_MODELVIEW_MATRIX, g_wall.m_mRotate);
	}
	glPopMatrix();
}

//�߰��ؾ��� int k=0;

void MotionCallback(int x, int y) { // ������ �ٸ�
	int tdx = x - downX, tdy = 0, tdz = y - downY, id = choice - 1;
	if (leftButton) { //���ʸ��콺�� ������ ���� ���� ȸ��, ���� ���ָ� ���콺 ������ ȭ����ȯ ���Ҽ� ����
		rotate_x += x - downX;
		rotate_y += y - downY;
		for (i = 0; i < NO_SPHERE; i++) rotate(i);
		rotate(WALL_ID);
	}
	else if (rightButton) { // �������� ��ġ����, �̺κ��� �� ������ arkanoid���� ����� ��ġ ���� ����
		if (id < NO_SPHERE) g_sphere[id].setCenter(g_sphere[id].center_x + tdx / 100.0, g_sphere[id].center_y + tdy / 100.0, g_sphere[id].center_z + tdz / 100.0);
	}
	downX = x;   downY = y;
	glutPostRedisplay();
}

void initRotate() { // ������ ��¦ �ٸ� initGL���� ȣ��
	g_sphere[0].init();
	g_sphere[1].init();
	g_sphere[2].init();
	g_wall.init();
}

void InitGL() {
	//opengl�� �⺻ ������
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // ���̹��۰� �ְ� rgb������۰� �ִ� ���߹���â�� ��带 ����
	glutInitWindowSize(1024, 768);
	glutCreateWindow("OpenGL Applications"); // Displaymode���� ������ ���۴�� â�� �����
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.7, 0.7, 0.7, 0.0);
	glPolygonOffset(1.0, 1.0);
	glDisable(GL_CULL_FACE);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
	glEnable(GL_LIGHT0);
	initRotate(); //���� wall�� initȣ��

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);

	

	//���� opengl�� �̺�Ʈ ó���κ�
	//�ݹ��Լ���: �̺�Ʈ ó������ �Ѵ�.w�� ������ ������ ����, esc�� ������ �����°ſ� ���� � ���������� ����� ��Ÿ���� �Լ��ε�, ���÷����ݹ��Լ��� ��� �����Ӹ��� ȣ��ɰ��̴�.

	glutIdleFunc(renderScene); // ���ø����̼��� �޸�(idle)�ð��� ȣ��Ǵ� �Լ�(renderscene)�� ȣ���ϵ����Ѵ�. �� �ִϸ��̼��� �ȴ�.
	glutReshapeFunc(ReshapeCallback); // GLUT�� ó�� �����츦 ����, ������ ��ġ�� �ű涧, ������ ũ�⸦ �����Ҷ� reshapeevent�� �߻��Ѱɷ� ����. �Ű��������� �Լ��� ���ϴ� �Լ�ȿ���� �Ҽ��ִ�.
									  // reshapeevent �� �߻��ϸ� ��ȣ���� �Ķ���͸����� ����� �������� ���� ���̸� �ݹ��Լ��� �����Ѵ�. �׷��� width�� height�� �׳� �޾Ƽ� ���µ�.
	glutDisplayFunc(DisplayCallback); // �� �Լ��� DisplayCallback�̶�� �Լ��� ���÷����̺�Ʈ�� ���� �ݹ��Լ��� ����ϴ� �Լ�, �Ű������� ������ �Լ��� ���÷����̺�Ʈ���� ȣ��ȴ�.
	glutKeyboardFunc(KeyboardCallback); // Ű���尡 ��������� �۵��ϴ� �ݹ��Լ�
	glutMouseFunc(MouseCallback); // ���콺�� ��������� �۵��ϴ� �ݹ��Լ�
	glutMotionFunc(MotionCallback); // ��ư�� �������¿��� ���콺�� �����϶� �۵��ϴ� �ݹ��Լ�, �ƹ��� ��ư�� ������ ���� ���¿��� ���콺�� �����̸� glutPassivemotionfunc�̴�.
}

/* �߰��ؾ���
void restart_game()
{
	space_flag = 0;
	InitObjects();
}

void auto_restart_game()
{
	int finish_idx;
	finish_idx = 3;
	while (finish_idx < NO_SPHERE)
	{
		if (g_sphere[finish_idx].center_z == 100)
			finish_idx++;
		else
			break;
		if (finish_idx == NO_SPHERE)
		{
			space_flag = 0;
			InitObjects();
		}
	}
}

void lose_game()
{
	if (g_sphere[0].center_z < -10)
	{
		space_flag = 0;
	}
}

int currentTime, previousTime = -1;

*/

int currentTime, previousTime = -1; // �ű⿣ ����
void renderScene() // ���� �ٸ�, ���ø����̼��� �޸�ð��� ȣ��Ǵ� �Լ�, glutidleFunc()���� ȣ��ȴ�.
{
	
	int timeDelta;
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	if (previousTime == -1) timeDelta = 0;
	else timeDelta = currentTime - previousTime;
	
	float x = g_sphere[0].center_x;
	float y = g_sphere[0].center_y;
	float z = g_sphere[0].center_z;

	if (space_flag) g_sphere[0].setCenter( 
		x + timeDelta * 0.002 * g_sphere[0].dir_x, // �ش籸ü�� �ӵ�, 0.002�� �⺻��, 
		y + timeDelta * 0.002 * g_sphere[0].dir_y,
		z + timeDelta * 0.002 * g_sphere[0].dir_z);
	glutPostRedisplay(); // �����츦 �ٽñ׸����� ��û, �ٷ� ���÷����ݹ��Լ�(renderscene)�� ȣ����� �ʰ� ���η���(�Ƹ� glutMainloop?)���� ȣ������� �����Ѵ�. �̰� ������ ������ ���ڿ�������
	previousTime = currentTime;

}

void InitObjects()
{
	// specify initial colors and center positions of each spheres
	g_sphere[0].setColor(0.8, 0.2, 0.2); g_sphere[0].setCenter(0.0, 0.0, 0.0);
	g_sphere[1].setColor(0.2, 0.8, 0.2); g_sphere[1].setCenter(1.0, 0.0, 0.0);
	g_sphere[2].setColor(0.2, 0.2, 0.8); g_sphere[2].setCenter(0.0, 0.0, 1.0);

	// specify initial colors and center positions of a wall
	g_wall.setColor(0.0, 1.0, 0.0); g_wall.setCenter(0.0, -0.6, 0.0);
}

// �߰��ؾ��� using namespace std; 
int main(int argc, char** argv)
{
	glutInit(&argc, argv); // glut ���� �ʱ�ȭ, ���� �׷��ȸ����µ��� �Ѵٰų� �ϸ� ���� ������
	InitObjects(); // �ʱ�ȭ��, �� ���� ��ġ�� ����
	InitGL(); // opengl ���� �ʱ�ȭ
	glutMainLoop(); // �̺�Ʈ ������ �����°�, �̺�Ʈ���� �ݹ��Լ� ����� �������� �̺�Ʈ ������ �����϶�.
	return 0;
}