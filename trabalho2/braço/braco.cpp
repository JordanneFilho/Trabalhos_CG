
#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
    #include <GLUT/glut.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/glut.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#define ESC 27

static int ombro = 0, cotovelo = 0;
static int punho = 0, indicador = 0, polegar = 0, anelar = 0;
static int rotacaoBraco = 0;

void init(void);
void keyboard (unsigned char key, int x, int y);
void display(void);
void reshape (int w, int h);


int main(int argc, char** argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800,600);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Braco Robotico");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return EXIT_SUCCESS;
}

void init(void){
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
}

void reshape(int w, int h){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    gluPerspective(60, (float)w/(float)h , 1.0, 20.0);
    gluLookAt(0.0,0.0,10.0, 0.0,1.0,0.0, 0.0,1.0,0.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y){
    switch (key) {
        case 'o': ombro = (ombro - 5) % 360; break;
        case 'O': ombro = (ombro + 5) % 360; break;
        case 'c': cotovelo = (cotovelo - 5) % 360; break;
        case 'C': cotovelo = (cotovelo + 5) % 360; break;
        case 'm': punho = (punho - 5) % 360; break;
        case 'M': punho = (punho + 5) % 360; break;
        case 'i': indicador = (indicador - 5) % 90; break;
        case 'I': indicador = (indicador + 5) % 90; break;
        case 'p': polegar = (polegar - 5) % 90; break;
        case 'P': polegar = (polegar + 5) % 90; break;
        case 'a': anelar = (anelar - 5) % 90; break;
        case 'A': anelar = (anelar + 5) % 90; break;
        case 'y': rotacaoBraco = (rotacaoBraco - 5) % 360; break;
        case 'Y': rotacaoBraco = (rotacaoBraco + 5) % 360; break;
        case ESC: exit(EXIT_SUCCESS); break;
    }
    glutPostRedisplay();
}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glRotatef((GLfloat) rotacaoBraco, 0.0, 1.0, 0.0);

    // Ombro
    glPushMatrix();
        glRotatef((GLfloat) ombro, 0.0, 0.0, 1.0);
        glTranslatef(1.0, 0.0, 0.0);
        glPushMatrix();
            glScalef(2.0, 0.5, 0.5);
            glColor3f(1.0, 1.0, 0.0); 
            glutSolidCube(1.0);
        glPopMatrix();

        // Cotovelo
        glTranslatef(1.0, 0.0, 0.0);
        glRotatef((GLfloat) cotovelo, 0.0, 0.0, 1.0);
        glTranslatef(1.0, 0.0, 0.0);
        glPushMatrix();
            glScalef(2.0, 0.4, 0.4);
            glColor3f(0.5, 0.5, 0.5); 
            glutSolidCube(1.0);
        glPopMatrix();

        // Mão
        glTranslatef(1.0, 0.0, 0.0);
        glRotatef((GLfloat) punho, 1.0, 0.0, 0.0);
        glPushMatrix();
            glScalef(0.6, 0.4, 0.8);
            glColor3f(1.0, 1.0, 0.0); 
            glutSolidCube(1.0);
        glPopMatrix();

        // Dedo Indicador
        glPushMatrix();
            glTranslatef(0.4, 0.1, -0.3);
            glRotatef((GLfloat) indicador, 0.0, 1.0, 0.0);
            glTranslatef(0.15, 0.0, 0.0);
            glPushMatrix();
                glScalef(0.3, 0.1, 0.1);
                glColor3f(0.5, 0.5, 0.5); 
                glutSolidCube(1.0);
            glPopMatrix();
        glPopMatrix();

        // Dedo Anelar
        glPushMatrix();
            glTranslatef(0.4, -0.1, -0.3);
            glRotatef((GLfloat) anelar, 0.0, 1.0, 0.0);
            glTranslatef(0.15, 0.0, 0.0);
            glPushMatrix();
                glScalef(0.3, 0.1, 0.1);
                glColor3f(0.5, 0.5, 0.5); 
                glutSolidCube(1.0);
            glPopMatrix();
        glPopMatrix();

        // Polegar 
        glPushMatrix();
            glTranslatef(0.4, 0.0, 0.3);
            glRotatef((GLfloat) polegar, 0.0, 1.0, 0.0);
            glTranslatef(0.15, 0.0, 0.0);
            glPushMatrix();
                glScalef(0.3, 0.1, 0.1);
                glColor3f(0.5, 0.5, 0.5); 
                glutSolidCube(1.0);
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();

    glutSwapBuffers();
}
	