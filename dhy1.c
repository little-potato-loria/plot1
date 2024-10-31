#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
const double c2 = 3.053753378638252798e+02,
c5 = 2.367852246387234239e+00,
c6 = 3.274658677400876881e-03,
y00 = 1.000001,
h0 = 0.000001,
t0 = 1,
t_end = -2.5;
int booldone = 0;

double f(double x, double y) {
    return -c5 * y / pow(c6 * (y - 1) + 1, 1.0 / 3.0) / pow(10, x);
}
double df(double x, double y) {
    return -c5 * (3.0 + c6 * (-3 + 2 * y)) / pow(10, x) / 3 / pow(1 + c6 * (y - 1), 4.0 / 3.0);
}
double newton(double (*f)(double, double), double (*df)(double, double), double t, double y, double h, double y_prev, double tol) {
    double y_next = y;
    double y_next_old;
    int max_iter = 100;
    int iter = 0;
    do {
        y_next_old = y_next;
        double g = y_next - (4.0 / 3.0) * y + (1.0 / 3.0) * y_prev + (2.0 / 3.0) * h * f(t, y_next);
        double g_prime = 1 + (2.0 / 3.0) * h * df(t, y_next);
        y_next = y_next_old - g / g_prime;
        iter++;
        if (iter > (max_iter - 3)) {
            printf("error newton of t=%.17e,y=%.17e,h=%.17e", t, y, h);
            return  NAN;
        }
    } while (fabs(y_next - y_next_old) > tol);
    //printf("error of newton is %.17e,tol is%.17e\n", y_next - y_next_old,tol);
    return y_next;
}
void BDF2(double (*f)(double, double), double (*df)(double, double)) {
    double h = h0;
    double t = t0 - h;
    double y = y00 - h * f(t, y00);
    double y_prev = y;
    double min_h = 1e-15;
    while (t > t_end) {
        glVertex2f((GLfloat)t, log10(pow(((GLfloat)y - 1) / c2 + 1,0.33333333333333333)-1));
        double tol = fmax(1e-7 * fabs(y - 1), 1e-9);
        double y_half = newton(f, df, t - 0.5 * h, y, 0.5 * h, y_prev, tol);
        double y_full = newton(f, df, t - h, y, h, y_prev, tol);
        double y_half_full = newton(f, df, t - h, y_half, 0.5 * h, y_prev, tol);
        double error = fabs(y_half_full - y_full) * 3.0 / 2.0;
        //printf("h = %.17e, t = %.17e, y = %.17e, error= %.17e, y_half_full= %.17e, y_full= %.17e\n", h, t, y,error,y_half_full,y_full);
        while (error > fmax(1e-5 * fabs(y - 1), 1e-6)) {
            if (h < min_h) {
                printf("Step size too small, stopping iteration.  error is %.17e,at t=%.17e\n", error, t);
                booldone = 1;
                return;
            }
            h *= 0.5;
            y_half = newton(f, df, t - 0.5 * h, y, 0.5 * h, y_prev, tol);
            y_full = newton(f, df, t - h, y, h, y_prev, tol);
            y_half_full = newton(f, df, t - h, y_half, 0.5 * h, y_prev, tol);
            printf("Reducing step size: h = %.17e,error is %.17e,y_full is %.17e y_half_full is %.17e\n", h, error, y_full, y_half_full);
            error = fabs(y_half_full - y_full) * 3.0 / 2.0;
        }
        while (error < 1e-7 * fabs(y - 1)) {
            h *= 2.2;
            printf("Increasing step size: h = %.17e。  error is %.17e\n", h, error);
            y_half = newton(f, df, t - 0.5 * h, y, 0.5 * h, y_prev, tol);
            y_full = newton(f, df, t - h, y, h, y_prev, tol);
            y_half_full = newton(f, df, t - h, y_half, 0.5 * h, y_prev, tol);
            error = fabs(y_half_full - y_full) * 3.0 / 2.0;
        }
        y_prev = y;
        y = y_full;
        t -= h;
    }
    booldone = 1;
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-2.75, 1, -4.5, 2);
}
void drawString(float x, float y, const char* string, void* font) {
    glRasterPos2f(x, y);
    while (*string) {
        glutBitmapCharacter(font, *string++);
    }
}
void display() {
    if (!booldone) {
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(0.0, 0.0, 0.0);
        glBegin(GL_LINES);
        glVertex2f(-3, -4);
        glVertex2f(1, -4);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f(-2.5, 2);
        glVertex2f(-2.5, -4.5);
        glEnd();
        for (int j = -3; j < 10; j++) {
            glBegin(GL_LINES);
            glVertex2f(j, -4);
            glVertex2f(j, -3.5);
            glEnd();
            char buffer[3], ui[3] = { "10" };
            snprintf(buffer, sizeof(buffer), "%d", j);
            drawString(j + 0.04, -4.2, buffer, GLUT_BITMAP_HELVETICA_12);
            drawString(j - 0.08, -4.3, ui, GLUT_BITMAP_HELVETICA_18);
            for (int i = 2; i < 10; i++) {
                glBegin(GL_LINES);
                glVertex2f(j + log10(i), -4);
                glVertex2f(j + log10(i), -3.8);
                glEnd();
            }
        }
        for (int j = -5; j < 2; j++) {
            glBegin(GL_LINES);
            glVertex2f(-2.5, j);
            glVertex2f(-2.4, j);
            glEnd();
            char buffer[3], ui[3] = { "10" };
            snprintf(buffer, sizeof(buffer), "%d", j);
            drawString(-2.57, j, buffer, GLUT_BITMAP_HELVETICA_12);
            drawString(-2.72, j - 0.15, ui, GLUT_BITMAP_HELVETICA_18);
            for (int i = 2; i < 10; i++) {
                glBegin(GL_LINES);
                glVertex2f(-2.5, j + log10(i));
                glVertex2f(-2.45, j + log10(i));
                glEnd();
            }
        }
        glColor3f(0.0, 0.0, 0.0);
        glBegin(GL_LINE_STRIP);
        BDF2(f, df);
        glEnd();
        glFlush();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("made by dhy");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
