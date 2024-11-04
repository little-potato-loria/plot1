/*需要glut环境，右键项目，选择管理NuGet程序包，下载名为glut的两个包（下载任何一个会自动下载两个）即可*/
#include <GL/freeglut.h>
#include <math.h>
#include <stdlib.h>
#include<stdio.h>

float n = 0.1f;
float a = 3.0f;
double dt = 0.01;
double yrem[2048] = { 0 };

inline double func(double t, double y, double v) {
    if (t == 0 && v == 0) {
        return -pow(y, n) / 3;
    }
    else { return -pow(y, n) - 2 / t * v; }
}

void rk4(double t, double* y, double* v) {
    double k1, k2, k3, k4, l1, l2, l3, l4;
    k1 = dt * (*v);
    l1 = dt * func(t, *y, *v);
    k2 = dt * (*v + 0.5 * l1);
    l2 = dt * func(t + dt / 2, *y + 0.5 * k1, *v + 0.5 * l1);
    k3 = dt * (*v + 0.5 * l2);
    l3 = dt * func(t + dt / 2, *y + 0.5 * k2, *v + 0.5 * l2);
    k4 = dt * (*v + l3);
    l4 = dt * func(t + dt, *y + k3, *v + l3);
    *y = *y + (k1 + 2 * k2 + 2 * k3 + k4) / 6;
    *v = *v + (l1 + 2 * l2 + 2 * l3 + l4) / 6;
}

void drawAxes() {
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    glVertex2f(0, 0);
    glVertex2f(a, 0);
    glVertex2f(0, 0);
    glVertex2f(0, 1);
    glEnd();
}

void drawTicks() {
    glColor3f(1.0, 1.0, 1.0); 
    for (float x = 0.0f; x <= a; x += 0.5f) {
        glBegin(GL_LINES);
        glVertex2f(x, 0);
        glVertex2f(x, 0.02);
        glEnd();
        glRasterPos2f(x, -0.05);
        char buf[10];
        sprintf_s(buf, sizeof(buf), "%.1f", x);
        for (char* p = buf; *p; p++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *p);
        }
    }
    for (float y = 0.0f; y <= 1.0f; y += 0.1f) {
        glBegin(GL_LINES);
        glVertex2f(0, y);
        glVertex2f(0.03, y);
        glEnd();
        glRasterPos2f(-0.1, y);
        char buf[10];
        sprintf_s(buf, sizeof(buf), "%.1f", y);
        for (char* p = buf; *p; p++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *p);
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    gluOrtho2D(-0.2, a, -0.07, 1);
    drawAxes();
    drawTicks();
    glBegin(GL_LINE_STRIP);
    int i = 0;
    for (float x = 0.0f; x < a; x += 0.01f) {
        glVertex2f(x, pow(yrem[i], n));
        i++;
    }
    glEnd();
    glutSwapBuffers();
}

float caclulate(void) {
    double y = 1.0, v = 0, t = 0;
    int i = 0;
    while (y > 0) {
        yrem[i] = y;
        rk4(t, &y, &v);
        t += dt;
        i++;
    }
    return i * dt;
}

void timer(int extra) {
    n += 0.01f;
    a = caclulate();
    if (a > 20) {
        n = 0;
        for (int i = 0; i < 2048; i++) {
            yrem[i] = 0;
        }

    }
    glutPostRedisplay();
    glutTimerFunc(30, timer, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(700, 700); 
    glutCreateWindow("made by dhy");
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
