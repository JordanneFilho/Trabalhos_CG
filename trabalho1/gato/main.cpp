	
	#include <GL/glut.h>
	#include <cmath>
	
	const float PI = 3.14;
	
	void drawCircle(float x, float y, float radius, int segments = 100) {
	    glBegin(GL_TRIANGLE_FAN);
	    glVertex2f(x, y);
	    for (int i = 0; i <= segments; i++) {
	        float angle = 2.0f * PI * float(i) / float(segments);
	        float dx = radius * cos(angle);
	        float dy = radius * sin(angle);
	        glVertex2f(x + dx, y + dy);
	    }
	    glEnd();
	}
	void drawEllipse(float x, float y, float radiusX, float radiusY, int segments = 100) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * float(i) / float(segments);
        float dx = radiusX * cos(angle);
        float dy = radiusY * sin(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

	
	void drawEar(float x, float y, bool invertida = false) {
    float mult = invertida ? -1.0f : 1.0f;

    glColor3f(0.0f, 0.0f, 0.0f); 
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + 0.3f);                        
    glVertex2f(x + mult * -0.1f, y - 0.15f);        
    glVertex2f(x + mult * 0.1f, y);                
    glEnd();

    glColor3f(0.3f, 0.1f, 0.1f); 
	glBegin(GL_TRIANGLES);
	glVertex2f(x, y + 0.24f);                          
	glVertex2f(x + mult * -0.06f, y - 0.10f);          
	glVertex2f(x + mult * 0.06f, y);                  
	glEnd();

}


	
void drawEyes(float offsetX, float pupilOffsetX) {
    // iris
    glColor3f(0.8f, 0.6f, 0.0f);
    drawEllipse(offsetX, 0.0f, 0.08f, 0.12f);

    // pupila 
    glColor3f(0.0f, 0.0f, 0.0f);
    drawEllipse(offsetX + pupilOffsetX, 0.0f, 0.05f, 0.08f);

    // brilho
    glColor3f(1.0f, 1.0f, 1.0f);
    drawEllipse(offsetX + pupilOffsetX + 0.015f, 0.025f, 0.01f, 0.015f);
}


void drawNose() {
    glColor3f(0.3f, 0.1f, 0.1f); 

    
    glBegin(GL_QUADS);
    glVertex2f(-0.045f, -0.035f);  
    glVertex2f(0.045f, -0.035f);
    glVertex2f(0.045f, -0.065f);   
    glVertex2f(-0.045f, -0.065f);
    glEnd();

    
    glBegin(GL_QUADS);
    glVertex2f(-0.015f, -0.065f);
    glVertex2f(0.015f, -0.065f);
    glVertex2f(0.015f, -0.11f);
    glVertex2f(-0.015f, -0.11f);
    glEnd();

   
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(-0.018f, -0.0675f, 0.009f);
    drawCircle(0.018f, -0.0675f, 0.009f);

}


	
	void display() {
	    glClear(GL_COLOR_BUFFER_BIT);
	
	    glPushMatrix();
	    glScalef(0.8f, 0.8f, 1.0f);  
	

	    drawEar(-0.259f, 0.29f, false);  
		drawEar(0.259f, 0.29f, true);    
	    
	    glColor3f(0.1f, 0.1f, 0.1f);
	    drawCircle(0.0f, 0.0f, 0.38f);
	

	    drawEyes(-0.18f, 0.030f);
		drawEyes(0.18f, -0.030f); 

	
	    drawNose();
	
	    glPopMatrix();
	    glFlush();
	}
	
	void init() {
	    glClearColor(1.0, 1.0, 1.0, 1.0); 
	    glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();
	    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
	}
	
	int main(int argc, char** argv) {
	    glutInit(&argc, argv);
	    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	    glutInitWindowSize(500, 500);
	    glutInitWindowPosition(100, 100);
	    glutCreateWindow("Gato");
	    init();
	    glutDisplayFunc(display);
	    glutMainLoop();
	    return 0;
	}
