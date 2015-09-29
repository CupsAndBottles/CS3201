#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>

#include "Database.h"
#include "ASTView.h"

using namespace std;

vector<Tnode*> getAllChildren(Tnode* node);
string drawNode(Tnode* node, int x, int y);
void display(void);
void drawLine(int x1, int y1, int x2, int y2);
int getParentIndex(Tnode* curNode, int i);
string enumToPrintable(int enumVal);
vector<vector<Tnode*>> astVector;
vector<vector<int>> window;
vector<vector<int>> starting;
int spacing = 35;

void init(void)
{
	glClearColor(0.7, 0.9, 1.0, 0.0);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_COLOR_MATERIAL);
}

void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

void resize(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, h, 0);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void display(void)
{
	int horizontal = 0;
	int vertical = 0;
	string parentTxt, childTxt;
	GetDesktopResolution(horizontal, vertical);
	vector<Tnode*> children = vector<Tnode*>();
	Tnode *curNode, *childNode;

	glClear(GL_COLOR_BUFFER_BIT);
	for (unsigned int i = 0; i < astVector.size(); i++) {
		for (unsigned int j = 0; j < astVector.at(i).size(); j++) {
			curNode = astVector.at(i).at(j);
			children = getAllChildren(curNode);
			if (i < astVector.size() - 1) {
				for (unsigned int k = 0; k < children.size(); k++) {
					window.at(i + 1).at(find(astVector.at(i + 1).begin(), astVector.at(i + 1).end(), children.at(k)) - astVector.at(i + 1).begin()) = window.at(i).at(j) / children.size();
					starting.at(i + 1).at(find(astVector.at(i + 1).begin(), astVector.at(i + 1).end(), children.at(k)) - astVector.at(i + 1).begin()) = starting.at(i).at(j) + k*(window.at(i + 1).at(find(astVector.at(i + 1).begin(), astVector.at(i + 1).end(), children.at(k)) - astVector.at(i + 1).begin()));

				}
			}
			parentTxt = drawNode(curNode, starting.at(i).at(j) + window.at(i).at(j) / 2, 580 - (i)*spacing);

			for (unsigned int k = 0; k < children.size(); k++) {
				childNode = children.at(k);
				int childIndex = find(astVector.at(i + 1).begin(), astVector.at(i + 1).end(), childNode) - astVector.at(i + 1).begin();
				childTxt = drawNode(childNode, starting.at(i + 1).at(childIndex) + window.at(i + 1).at(childIndex) / 2, 580 - (i + 1)* spacing);

				drawLine(starting.at(i).at(j) + window.at(i).at(j) / 2, (580 - (i)* spacing - 2),
					starting.at(i + 1).at(childIndex) + window.at(i + 1).at(childIndex) / 2, (580 - (i + 1) * spacing) + 9);
			}
		}
	}
	glutSwapBuffers();
}

void drawLine(int x1, int y1, int x2, int y2) {
	glMatrixMode(GL_PROJECTION);
	double *matrix = new double[16];
	glGetDoublev(GL_PROJECTION_MATRIX, matrix);
	glLoadIdentity();
	glOrtho(0, 800, 0, 600, -5, 5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(x1, y1);

	glLineWidth(0.5);
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matrix);
	glMatrixMode(GL_MODELVIEW);
}

void drawText(const char *text, int length, int x, int y) {
	glMatrixMode(GL_PROJECTION);
	double *matrix = new double[16];
	glGetDoublev(GL_PROJECTION_MATRIX, matrix);
	glLoadIdentity();
	glOrtho(0, 800, 0, 600, -5, 5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(x, y);
	for (int i = 0; i < length; i++) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)text[i]);
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matrix);
	glMatrixMode(GL_MODELVIEW);
}

string drawNode(Tnode* node, int x, int y) {
	glLoadIdentity();

	string txt = "";
	txt = txt + enumToPrintable(node->getType());
	if (node->getName() != "") {
		txt = node->getName() + txt;
	}
	else {
		if (node->getType() == Tnode::CONSTANT) {
			txt = to_string(node->getValue()) + txt;
		}
	}
	glColor3f(0, 0, 0);
	drawText(txt.data(), txt.size(), x - 4 * txt.length() / 2, y);

	return txt;
}

vector<Tnode*> getAllChildren(Tnode* node) {
	vector<Tnode*> childList = vector<Tnode*>();
	Tnode* curNode = node;
	if (node->getFirstChild() == NULL) {
		return childList;
	}
	else {
		curNode = curNode->getFirstChild();
		childList.push_back(curNode);
		while (curNode->getRightSibling() != NULL) {
			curNode = curNode->getRightSibling();
			childList.push_back(curNode);
		}
	}
	return childList;
}

int getParentIndex(Tnode* curNode, int i) {
	while (curNode->getLeftSibling() != NULL) {
		curNode = curNode->getLeftSibling();
	}
	return find(astVector.at(i - 1).begin(), astVector.at(i - 1).end(), curNode->getParent()) - astVector.at(i - 1).begin();
}

string enumToPrintable(int enumVal)
{
	switch (enumVal)
	{
	case Tnode::PROGRAM:
		return ":program";
	case Tnode::PROCEDURE:
		return ":procedure";
	case Tnode::STMTLST:
		return ":stmtLst";
	case Tnode::STMT_CALL:
		return ":call";
	case Tnode::STMT_WHILE:
		return ":while";
	case Tnode::STMT_IF:
		return ":if";
	case Tnode::STMT_ASSIGN:
		return ":assign";
	case Tnode::VARIABLE:
		return "";
	case Tnode::CONSTANT:
		return "";
	case Tnode::EXPR_PLUS:
		return ":+";
	case Tnode::EXPR_MINUS:
		return ":-";
	case Tnode::EXPR_TIMES:
		return ":*";
	default:
		return "Unknown";
	}
}

int viewAST(vector<vector<Tnode*>> inputVector, string fileName)
{
	char fakeParam[] = "fake";
	char *fakeargv[] = { fakeParam, NULL };
	int fakeargc = 1;
	glutInit(&fakeargc, fakeargv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	int horizontal = 0;
	int vertical = 0;
	GetDesktopResolution(horizontal, vertical);
	glutInitWindowSize(horizontal, vertical);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(fileName.c_str());

	astVector = inputVector;

	for (unsigned int i = 0; i < astVector.size(); i++) {
		window.push_back(vector<int>());
		starting.push_back(vector<int>());
		for (unsigned int j = 0; j < astVector.at(i).size(); j++) {
			window.at(i).push_back(0);
			starting.at(i).push_back(0);
			if (i == 0) {
				window.at(i).at(j) = 800;
			}
		}
	}

	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	init();
	glutMainLoop();

	return 0;
}