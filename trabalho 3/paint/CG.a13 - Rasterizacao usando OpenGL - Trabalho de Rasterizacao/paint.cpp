/*
 * Computacao Grafica
 * Codigo Exemplo: Rasterizacao de Segmentos de Reta e Circunferencias com GLUT/OpenGL
 * Autor: Prof. Laurindo de Sousa Britto Neto
 */

// Bibliotecas utilizadas pelo OpenGL
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

#include <cmath>        
#include <cstdio>       
#include <cstdlib>
#include <forward_list> 
#include <vector>       
#include <algorithm>    
#include <string>       
#include <iostream>     
#include <stack>        

#include "glut_text.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

// Variaveis Globais
#define ESC 27


enum tipo_forma{LIN = 1, TRI = 2, RET = 3, POL = 4, CIR = 5, FILL = 6, // as formas basicas
                ACTION_FINISH_POLYGON = 100, ACTION_CANCEL_POLYGON = 101, // acoes de controle do desenho do poligono
                ACTION_RESET_TRIANGLE = 102, // acao de reset do triangulo
                
                TRANS_TRANSLATE_UP = 200, TRANS_TRANSLATE_DOWN, TRANS_TRANSLATE_LEFT, TRANS_TRANSLATE_RIGHT,
                TRANS_SCALE_UP, TRANS_SCALE_DOWN,
                TRANS_ROTATE_CW, TRANS_ROTATE_CCW,
                TRANS_SHEAR_X_POS, TRANS_SHEAR_X_NEG,
                TRANS_SHEAR_Y_POS, TRANS_SHEAR_Y_NEG,
                TRANS_REFLECT_X, TRANS_REFLECT_Y, TRANS_REFLECT_ORIGIN,
                SELECT_LAST_FORM = 300 // Ação para selecionar a última forma adicionada
                };

//controle do estado do mouse e desenho 
//verdadeiro se o primeiro clique do mouse foi registrado para formas de 2 cliques
bool click1 = false;
// Controla o número de cliques para o triângulo
int triangleClicks = 0;
//Verifica se está desenhando um polígono
bool drawingPolygon = false;
// Conta os vértices do polígono atual
int polygonVertexCount = 0;

//Coordenadas da posicao atual do mouse
int m_x, m_y;

//Coordenadas dos cliques do mouse (para linha, retângulo, triângulo, círculo)
// Para círculo, x_coords[0], y_coords[0] será o centro
// e x_coords[1], y_coords[1] será um ponto na circunferência para calcular o raio.
int x_coords[3], y_coords[3]; // Usado para armazenar até 3 cliques 

// Indice da forma selecionada para transformações. -1 = nenhuma.
int selectedFormIndex = -1;

//Indica o tipo de forma geometrica ativa para desenhar
int modo = LIN;


int width = 512, height = 512;

// parte da estrutura de dados
// definicao de vertice 
struct vertice{
    int x;
    int y;

    vertice() : x(0), y(0) {}
    vertice(int _x, int _y) : x(_x), y(_y) {}
};

// Definicao das formas geometricas
struct forma{
    int tipo;
    forward_list<vertice> v; //lista encadeada de vertices
};

// Lista encadeada de formas geometricas
forward_list<forma> formas;

// Definicao de uma matriz 3x3 para transformações 2D homogêneas
struct Matrix3x3 {
    double m[3][3];

    
    Matrix3x3() {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                m[i][j] = (i == j) ? 1.0 : 0.0;
            }
        }
    }

 
    static Matrix3x3 multiply(const Matrix3x3& A, const Matrix3x3& B) {
        Matrix3x3 C; // Resultante
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                C.m[i][j] = 0.0;
                for (int k = 0; k < 3; ++k) {
                    C.m[i][j] += A.m[i][k] * B.m[k][j];
                }
            }
        }
        return C;
    }

    
    vertice transform(const vertice& p) const {
        vertice result; 
        double x_hom = p.x * m[0][0] + p.y * m[0][1] + 1.0 * m[0][2];
        double y_hom = p.x * m[1][0] + p.y * m[1][1] + 1.0 * m[1][2];

        result.x = static_cast<int>(round(x_hom));
        result.y = static_cast<int>(round(y_hom));
        return result;
    }
};

// Funcoes para Criar Matrizes de Transformacao
Matrix3x3 createTranslationMatrix(double tx, double ty) {
    Matrix3x3 T;
    T.m[0][2] = tx;
    T.m[1][2] = ty;
    return T;
}

Matrix3x3 createScaleMatrix(double sx, double sy) {
    Matrix3x3 S;
    S.m[0][0] = sx;
    S.m[1][1] = sy;
    return S;
}

// Matriz de Rotação (ângulo em graus)
Matrix3x3 createRotationMatrix(double angleDegrees) {
    double angleRadians = angleDegrees * M_PI / 180.0; // Converter para radianos
    double cosTheta = cos(angleRadians);
    double sinTheta = sin(angleRadians);

    Matrix3x3 R;
    R.m[0][0] = cosTheta;
    R.m[0][1] = -sinTheta;
    R.m[1][0] = sinTheta;
    R.m[1][1] = cosTheta;
    return R;
}

// Matriz de Cisalhamento 
Matrix3x3 createShearMatrix(double shx, double shy) {
    Matrix3x3 Sh;
    Sh.m[0][1] = shx; // Cisalhamento em X baseado em Y
    Sh.m[1][0] = shy; // Cisalhamento em Y baseado em X
    return Sh;
}

// Matriz de Reflexão em X 
Matrix3x3 createReflectionXMatrix() {
    Matrix3x3 Rx;
    Rx.m[0][0] = -1.0;
    Rx.m[1][1] = 1.0;
    return Rx;
}

// Matriz de Reflexão em Y 
Matrix3x3 createReflectionYMatrix() {
    Matrix3x3 Ry;
    Ry.m[0][0] = 1.0;
    Ry.m[1][1] = -1.0;
    return Ry;
}

// Matriz de Reflexão sobre a origem 
Matrix3x3 createReflectionOriginMatrix() {
    Matrix3x3 Ro;
    Ro.m[0][0] = -1.0;
    Ro.m[1][1] = -1.0;
    return Ro;
}

// Funcao para armazenar uma forma geometrica na lista de formas
// Armazena sempre no inicio da lista
void pushForma(int tipo){
    forma f;
    f.tipo = tipo;
    formas.push_front(f);
}

// Funcao para armazenar um vertice na forma do inicio da lista de formas geometricas
// Armazena sempre no inicio da lista
void pushVertice(int x, int y){
    
    formas.front().v.push_front(vertice(x, y));
}

//Funcao para armazenar uma Linha na lista de formas geometricas
void pushLinha(int x1, int y1, int x2, int y2){
    pushForma(LIN);
    pushVertice(x1, y1);
    pushVertice(x2, y2);
}

//Funcao para armazenar um retangulo na lista de formas geometricas
void pushRetangulo(int x1, int y1, int x2, int y2){
    pushForma(RET);
    pushVertice(x1, y1);
    pushVertice(x2, y2);
}

//Funcao para armazenar um triangulo na lista de formas geometricas
void pushTriangulo(int x1, int y1, int x2, int y2, int x3, int y3){
    pushForma(TRI);
    pushVertice(x1, y1);
    pushVertice(x2, y2);
    pushVertice(x3, y3);
}

//Função para armazenar um polígono na lista de formas geométricas
void pushPoligono(){
    pushForma(POL);
}

//Funcao para armazenar um circulo na lista de formas geometricas
void pushCirculo(int xc, int yc, int R){
    pushForma(CIR);
    // Para o círculo, armazenamos o raio como um vértice e o centro como outro.
    pushVertice(R, 0);      
    pushVertice(xc, yc);   
}

// Funcao para aplicar transformacao a uma forma selecionada
void applyTransformationToSelectedForm(const Matrix3x3& transformationMatrix) {
    if (selectedFormIndex == -1) {
        printf("Nenhuma forma selecionada para transformar. Use 'Selecionar Ultima Forma'.\n");
        return;
    }

    int current_index = 0;
    for (forward_list<forma>::iterator f_it = formas.begin(); f_it != formas.end(); ++f_it, ++current_index) {
        if (current_index == selectedFormIndex) {
            printf("Aplicando transformacao a forma %d (tipo: %d).\n", selectedFormIndex, f_it->tipo);

            // Para circulos, a transformacao e aplicada apenas ao centro e ao raio.
            if (f_it->tipo == CIR) {
                // A forward_list para CIR armazena: {centro}, {raio}
                forward_list<vertice>::iterator it_center = f_it->v.begin();
                vertice center_v = *it_center; // O centro é o primeiro vertice

                forward_list<vertice>::iterator it_radius = it_center;
                ++it_radius; // Avança para o segundo elemento que contém o raio
                int radius_val = it_radius->x; 

                vertice transformed_center = transformationMatrix.transform(center_v);

                double scale_x = transformationMatrix.m[0][0];
                double scale_y = transformationMatrix.m[1][1];
                int transformed_radius = radius_val;

             
                // Se houver qualquer cisalhamento ou translação, não consideramos uma escala "toal" para o raio.
                bool is_pure_scale_matrix = (fabs(transformationMatrix.m[0][1]) < 1e-9 &&
                                             fabs(transformationMatrix.m[1][0]) < 1e-9 &&
                                             fabs(transformationMatrix.m[0][2]) < 1e-9 &&
                                             fabs(transformationMatrix.m[1][2]) < 1e-9 &&
                                             (fabs(scale_x - 1.0) > 1e-9 || fabs(scale_y - 1.0) > 1e-9));

                if (is_pure_scale_matrix) {
                     
                    transformed_radius = static_cast<int>(round(radius_val * max(fabs(scale_x), fabs(scale_y))));
                }
             
              

                // Atualiza os vertices da forma (centro e raio)
                f_it->v.clear();
                // Adiciona de volta na ordem correta para manter a estrutura {centro}, {raio}
                f_it->v.push_front(vertice(transformed_radius, 0)); 
                f_it->v.push_front(vertice(transformed_center.x, transformed_center.y)); 

            } else { // Para linha, triangulo, retangulo,poligonos,transformamos todos os vértices normalmente
                forward_list<vertice> transformed_vertices_temp;

                vector<vertice> temp_vec_original_order;
                for (forward_list<vertice>::iterator v_it = f_it->v.begin(); v_it != f_it->v.end(); ++v_it) {
                    temp_vec_original_order.push_back(*v_it);
                }
                reverse(temp_vec_original_order.begin(), temp_vec_original_order.end());

                // Itera sobre o vector e transforma cada vértice
                for (vector<vertice>::iterator original_v_it = temp_vec_original_order.begin(); original_v_it != temp_vec_original_order.end(); ++original_v_it) {
                    transformed_vertices_temp.push_front(transformationMatrix.transform(*original_v_it));
                }

                vector<vertice> final_transformed_order_vec;
                for(forward_list<vertice>::iterator v_it = transformed_vertices_temp.begin(); v_it != transformed_vertices_temp.end(); ++v_it) {
                    final_transformed_order_vec.push_back(*v_it);
                }
                reverse(final_transformed_order_vec.begin(), final_transformed_order_vec.end());

                f_it->v.clear();
                // Agora, adiciona os vertices transformados de volta à forward_list da forma
                for (vector<vertice>::reverse_iterator rit = final_transformed_order_vec.rbegin(); rit != final_transformed_order_vec.rend(); ++rit) {
                    //Passa um objeto vertice para push_front
                    f_it->v.push_front(vertice(rit->x, rit->y));
                }
            }

            glutPostRedisplay();
            return;
        }
    }
    printf("Erro: Forma selecionada (indice %d) nao encontrada na lista. Pode ter sido removida ou o indice esta incorreto.\n", selectedFormIndex);
}



  //Declaracoes das funcoes
 
void init(void);
void reshape(int w, int h);
void display(void);
void menu_popup(int value);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void mousePassiveMotion(int x, int y);
void drawPixel(int x, int y);
void plotCirclePoints(int xc, int yc, int x, int y);
// Função que implementa o Algoritmo de Bresenham para rasterização de circunferências
void circuloBresenham(int xc, int yc, int R);
// Funcao que percorre a lista de formas geometricas, desenhando na tela
void drawFormas();

// Funções para o Flood Fill

void floodFill4(int x, int y, GLubyte newColorR, GLubyte newColorG, GLubyte newColorB);
void getPixelColorInMemory(GLubyte* pixels, int imgWidth, int imgHeight, int x, int y, GLubyte color[3]);
void setPixelColorInMemory(GLubyte* pixels, int imgWidth, int imgHeight, int x, int y, GLubyte r, GLubyte g, GLubyte b);
void floodFill4(int x, int y, GLubyte newColorR, GLubyte newColorG, GLubyte newColorB);


//Funcao que implementa o Algoritmo de Bresenham para rasterizacao de segmentos de retas
 
void reduceToFirstOctant(int &x1, int &y1, int &x2, int &y2, int &originalOctant);
void transformFromFirstOctant(int &x, int &y, int originalOctant);
void retaBresenham(int x1, int y1, int x2, int y2);
void drawQuadrilateral(int x1, int y1, int x2, int y2);
void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
void drawPolygon(forward_list<vertice> vertices);



  //main
 
int main(int argc, char** argv){
    glutInit(&argc, argv); 
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); 
    glutInitWindowSize (width, height);  
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("Computacao Grafica: Paint"); 
    init(); 
    glutReshapeFunc(reshape); 
    glutKeyboardFunc(keyboard); 
    glutMouseFunc(mouse); 
    glutPassiveMotionFunc(mousePassiveMotion); 
    glutDisplayFunc(display); 

    // menu pop-up
    glutCreateMenu(menu_popup);
    glutAddMenuEntry("Linha", LIN);
    glutAddMenuEntry("Retangulo", RET);
    glutAddMenuEntry("Triangulo", TRI);
    glutAddMenuEntry("Poligono", POL);
    glutAddMenuEntry("Circulo", CIR); 
    glutAddMenuEntry("Preencher (Flood Fill)", FILL); 
    glutAddMenuEntry("----------------", -1); 
    glutAddMenuEntry("Finalizar Poligono", ACTION_FINISH_POLYGON);
    glutAddMenuEntry("Cancelar Poligono", ACTION_CANCEL_POLYGON);
    glutAddMenuEntry("Resetar Triangulo", ACTION_RESET_TRIANGLE);
    glutAddMenuEntry("----------------", -1); 
    glutAddMenuEntry("Selecionar Ultima Forma", SELECT_LAST_FORM);
    glutAddMenuEntry("----------------", -1); 
    glutAddMenuEntry("Transladar Cima (Y+)", TRANS_TRANSLATE_UP);
    glutAddMenuEntry("Transladar Baixo (Y-)", TRANS_TRANSLATE_DOWN);
    glutAddMenuEntry("Transladar Esquerda (X-)", TRANS_TRANSLATE_LEFT);
    glutAddMenuEntry("Transladar Direita (X+)", TRANS_TRANSLATE_RIGHT);
    glutAddMenuEntry("Aumentar Escala", TRANS_SCALE_UP);
    glutAddMenuEntry("Diminuir Escala", TRANS_SCALE_DOWN);
    glutAddMenuEntry("Rotacionar Horario (CW)", TRANS_ROTATE_CW);
    glutAddMenuEntry("Rotacionar Anti-Horario (CCW)", TRANS_ROTATE_CCW);
    glutAddMenuEntry("Cisalhar X Positivo", TRANS_SHEAR_X_POS);
    glutAddMenuEntry("Cisalhar X Negativo", TRANS_SHEAR_X_NEG);
    glutAddMenuEntry("Cisalhar Y Positivo", TRANS_SHEAR_Y_POS);
    glutAddMenuEntry("Cisalhar Y Negativo", TRANS_SHEAR_Y_NEG);
    glutAddMenuEntry("Refletir X", TRANS_REFLECT_X);
    glutAddMenuEntry("Refletir Y", TRANS_REFLECT_Y);
    glutAddMenuEntry("Refletir Origem", TRANS_REFLECT_ORIGIN);
    glutAddMenuEntry("----------------", -1); 
    glutAddMenuEntry("Sair", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
GLubyte fillR = 0, fillG = 255, fillB = 255;

    glutMainLoop();
    return EXIT_SUCCESS; 
}

/*
 * Inicializa alguns parametros do GLUT
 */
void init(void){
    glClearColor(1.0, 1.0, 1.0, 1.0); //Limpa a tela com a cor branca;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
}

/*
 * Ajusta a projecao para o redesenho da janela
 */
void reshape(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, w, h);
    width = w;
    height = h;
    glOrtho (0, w, 0, h, -1 ,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT); 
    glColor3f (0.0, 0.0, 0.0); 
    drawFormas(); 

    char coords_str[50]; 
    sprintf(coords_str, "(%d, %d)", m_x, m_y);
    draw_text_stroke(0, 0, coords_str, 0.2); 

    if (selectedFormIndex != -1) {
        char selected_str[50];
        sprintf(selected_str, "Forma Selecionada: %d", selectedFormIndex);
        draw_text_stroke(width - 250, 0, selected_str, 0.2); 
    }
    glutSwapBuffers(); 

}

/*
 * Controla o menu pop-up
 */
void menu_popup(int value){
    // Ações de finalização/cancelamento do polígono ou reset do triângulo
    if (value == ACTION_FINISH_POLYGON) {
        if (modo == POL && drawingPolygon) {
            if (polygonVertexCount >= 3) { // Confere se há pelo menos 3 vértices para um polígono (triângulo)
                printf("Menu: Tentando finalizar poligono. Vertices atuais: %d\n", polygonVertexCount);
                printf("Menu: Poligono finalizado com sucesso!\n");
                drawingPolygon = false;
                click1 = false;
                polygonVertexCount = 0;
                selectedFormIndex = 0; 
            } else {
                printf("Menu: ERRO: Poligono nao finalizado. Requer 3 ou mais vertices. Vertices: %d\n", polygonVertexCount);
                if (!formas.empty() && formas.front().tipo == POL) {
                    formas.pop_front(); // Remove o polígono incompleto/vazio
                }
                drawingPolygon = false;
                click1 = false;
                polygonVertexCount = 0;
            }
        } else {
            printf("Menu: Nao esta desenhando um poligono no modo POL para finalizar.\n");
        }
        glutPostRedisplay();
        return; 
    } else if (value == ACTION_CANCEL_POLYGON) {
        if (modo == POL && drawingPolygon) {
            printf("Menu: Poligono cancelado.\n");
            if (!formas.empty() && formas.front().tipo == POL) {
                formas.pop_front(); // Remove o polígono atual
            }
            drawingPolygon = false;
            click1 = false;
            polygonVertexCount = 0;
        } else {
            printf("Menu: Nao esta desenhando um poligono para cancelar.\n");
        }
        glutPostRedisplay();
        return; 
    } else if (value == ACTION_RESET_TRIANGLE) {
        if (modo == TRI) {
            printf("Menu: Triangulo resetado.\n");
            triangleClicks = 0;
        } else {
            printf("Menu: Nao esta desenhando um triangulo para resetar.\n");
        }
        glutPostRedisplay();
        return; 
    } else if (value == SELECT_LAST_FORM) {
        if (!formas.empty()) {
            selectedFormIndex = 0; // A última forma adicionada está sempre na frente (índice 0)
            printf("Ultima forma selecionada (indice: %d, tipo: %d).\n", selectedFormIndex, formas.front().tipo);
        } else {
            selectedFormIndex = -1;
            printf("Nenhuma forma para selecionar.\n");
        }
        glutPostRedisplay();
        return;
    }
    // --- Lógica de Transformação ---
    else if (value >= TRANS_TRANSLATE_UP && value <= TRANS_REFLECT_ORIGIN) {
        Matrix3x3 transformMatrix;
        double trans_step = 10.0; // Passos de translação
        double scale_factor = 1.1; // Fator de escala
        double rotate_angle = 15.0; // Ângulo de rotação em graus
        double shear_factor = 0.1; // Fator de cisalhamento

        switch (value) {
            case TRANS_TRANSLATE_UP:    transformMatrix = createTranslationMatrix(0, trans_step); break;
            case TRANS_TRANSLATE_DOWN:  transformMatrix = createTranslationMatrix(0, -trans_step); break;
            case TRANS_TRANSLATE_LEFT:  transformMatrix = createTranslationMatrix(-trans_step, 0); break;
            case TRANS_TRANSLATE_RIGHT: transformMatrix = createTranslationMatrix(trans_step, 0); break;
            case TRANS_SCALE_UP:        transformMatrix = createScaleMatrix(scale_factor, scale_factor); break;
            case TRANS_SCALE_DOWN:      transformMatrix = createScaleMatrix(1.0 / scale_factor, 1.0 / scale_factor); break;
            case TRANS_ROTATE_CW:       transformMatrix = createRotationMatrix(-rotate_angle); break; 
            case TRANS_ROTATE_CCW:      transformMatrix = createRotationMatrix(rotate_angle); break;  
            case TRANS_SHEAR_X_POS:     transformMatrix = createShearMatrix(shear_factor, 0); break;
            case TRANS_SHEAR_X_NEG:     transformMatrix = createShearMatrix(-shear_factor, 0); break;
            case TRANS_SHEAR_Y_POS:     transformMatrix = createShearMatrix(0, shear_factor); break;
            case TRANS_SHEAR_Y_NEG:     transformMatrix = createShearMatrix(0, -shear_factor); break;
            case TRANS_REFLECT_X:       transformMatrix = createReflectionXMatrix(); break;
            case TRANS_REFLECT_Y:       transformMatrix = createReflectionYMatrix(); break;
            case TRANS_REFLECT_ORIGIN:  transformMatrix = createReflectionOriginMatrix(); break;
        }
        applyTransformationToSelectedForm(transformMatrix);
        return;
    }


    // Se o valor não for uma das ações acima, então é uma mudança de modo
    if (value == 0) exit(EXIT_SUCCESS); // Sair

    if (modo != value) {
        // Se estava desenhando um polígono e mudou de modo, cancela o polígono incompleto
        if (drawingPolygon && modo == POL) {
            printf("Menu: Modo mudou, poligono incompleto cancelado.\n");
            if (!formas.empty() && formas.front().tipo == POL) {
                formas.pop_front(); // Remove o polígono que estava sendo desenhado
            }
        }
        drawingPolygon = false; // Garante que a flag de desenho de polígono esteja desativada
        click1 = false; // Reseta o clique para novas formas 
        triangleClicks = 0; // Reseta os cliques do triângulo
        polygonVertexCount = 0; // Reseta a contagem de vértices do polígono
        selectedFormIndex = -1; // Desseleciona a forma ao mudar de modo
    }
    modo = value; 
    glutPostRedisplay();
}


/*
 * Controle das teclas comuns do teclado
 */
void keyboard(unsigned char key, int x, int y){
    switch (key) { 
        case ESC: exit(EXIT_SUCCESS); break;
    }
}

/*
 * Controle dos botoes do mouse
 */
void mouse(int button, int state, int x, int y){
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN) {
                selectedFormIndex = -1; // Desseleciona ao clicar para desenhar nova forma 
                switch(modo){
                    case LIN:
                        if(click1){
                            x_coords[1] = x;
                            y_coords[1] = height - y - 1;
                            printf("Linha: Clique 2(%d, %d)\n",x_coords[1],y_coords[1]);
                            pushLinha(x_coords[0], y_coords[0], x_coords[1], y_coords[1]);
                            click1 = false;
                            selectedFormIndex = 0; // Seleciona a forma recém-criada
                            glutPostRedisplay();
                        }else{
                            click1 = true;
                            x_coords[0] = x;
                            y_coords[0] = height - y - 1;
                            printf("Linha: Clique 1(%d, %d)\n",x_coords[0],y_coords[0]);
                        }
                        break;
                    case RET:
                        if(click1){
                            x_coords[1] = x;
                            y_coords[1] = height - y - 1;
                            printf("Retangulo: Clique 2(%d, %d)\n",x_coords[1],y_coords[1]);
                            pushRetangulo(x_coords[0], y_coords[0], x_coords[1], y_coords[1]);
                            click1 = false;
                            selectedFormIndex = 0; // Seleciona a forma recém-criada
                            glutPostRedisplay();
                        }else{
                            click1 = true;
                            x_coords[0] = x;
                            y_coords[0] = height - y - 1;
                            printf("Retangulo: Clique 1(%d, %d)\n",x_coords[0],y_coords[0]);
                        }
                        break;
                    case TRI:
                        x_coords[triangleClicks] = x;
                        y_coords[triangleClicks] = height - y - 1;
                        printf("Triangulo: Clique %d(%d, %d)\n", triangleClicks + 1, x_coords[triangleClicks], y_coords[triangleClicks]);
                        triangleClicks++;

                        if (triangleClicks == 3) {
                            pushTriangulo(x_coords[0], y_coords[0], x_coords[1], y_coords[1], x_coords[2], y_coords[2]);
                            triangleClicks = 0; // Reseta para o próximo triângulo
                            selectedFormIndex = 0; // Seleciona a forma recém-criada
                            glutPostRedisplay();
                        }
                        break;
                    case POL:
                        if (!drawingPolygon) {
                            pushPoligono(); // Apenas inicia a forma na lista no PRIMEIRO clique
                            drawingPolygon = true; // Ativa o modo de desenho de polígono
                            polygonVertexCount = 0; // Garante que a contagem comece do zero para o NOVO polígono
                        }
                        pushVertice(x, height - y - 1); // Adiciona o vértice à forma atual
                        polygonVertexCount++; // Incrementa a contagem de vértices
                        printf("Poligono: Vertice adicionado. Total de vertices: %d\n", polygonVertexCount);
                        glutPostRedisplay();
                        break;
                    case CIR: // NOVO: Lógica para desenhar um círculo
                        if(click1){
                            x_coords[1] = x; // Ponto na circunferência
                            y_coords[1] = height - y - 1;
                            printf("Circulo: Clique 2(%d, %d)\n",x_coords[1],y_coords[1]);

                            int R = static_cast<int>(round(sqrt(pow(x_coords[1] - x_coords[0], 2) + pow(y_coords[1] - y_coords[0], 2))));

                            pushCirculo(x_coords[0], y_coords[0], R);
                            click1 = false;
                            selectedFormIndex = 0; // Seleciona o círculo recém-criado
                            glutPostRedisplay();
                        }else{
                            click1 = true;
                            x_coords[0] = x; // Centro do círculo
                            y_coords[0] = height - y - 1;
                            printf("Circulo: Clique 1 (Centro)(%d, %d)\n",x_coords[0],y_coords[0]);
                        }
                        break;
                    case FILL: 
                        
                        GLubyte fillR = 0, fillG = 255, fillB = 255;
                        // Chama o Flood Fill com o ponto clicado
                        floodFill4(x, height - y - 1, fillR, fillG, fillB);
                        glutPostRedisplay(); // Redesenha para mostrar o preenchimento
                        break;
                }
            }
            break;

        case GLUT_RIGHT_BUTTON:
            
            if (modo == POL && drawingPolygon) {
                
            }
            break;

    }
}

/*
 * Controle da posicao do cursor do mouse
 */
void mousePassiveMotion(int x, int y){
    m_x = x; m_y = height - y - 1;
    glutPostRedisplay();
}

/*
 * Funcao para desenhar apenas um pixel na tela
 */
void drawPixel(int x, int y){
    glBegin(GL_POINTS); // Seleciona a primitiva GL_POINTS para desenhar
        glVertex2i(x, y);
    glEnd();  // indica o fim do ponto
}

// Funcao auxiliar para desenhar os 8 pontos simétricos do círculo
void plotCirclePoints(int xc, int yc, int x, int y) {
    drawPixel(xc + x, yc + y);
    drawPixel(xc - x, yc + y);
    drawPixel(xc + x, yc - y);
    drawPixel(xc - x, yc - y);
    drawPixel(xc + y, yc + x);
    drawPixel(xc - y, yc + x);
    drawPixel(xc + y, yc - x);
    drawPixel(xc - y, yc - x);
}

// Função que implementa o Algoritmo de Bresenham para rasterização de circunferências
void circuloBresenham(int xc, int yc, int R) {
    int x = 0;
    int y = R;
    int p = 1 - R; 

    plotCirclePoints(xc, yc, x, y);

    while (x < y) {
        x++;
        if (p < 0) {
            p = p + 2 * x + 1; 
        } else {
            y--;
            p = p + 2 * (x - y) + 1; 
        }
        plotCirclePoints(xc, yc, x, y);
    }
}

/*
 *Funcao que desenha a lista de formas geometricas
 */
void drawFormas(){
  
    if(click1 && modo == LIN) retaBresenham(x_coords[0], y_coords[0], m_x, m_y);
    
    if(click1 && modo == RET) drawQuadrilateral(x_coords[0], y_coords[0], m_x, m_y);
    
    if(modo == TRI && triangleClicks == 1) retaBresenham(x_coords[0], y_coords[0], m_x, m_y);
    
    if(modo == TRI && triangleClicks == 2) {
        retaBresenham(x_coords[0], y_coords[0], x_coords[1], y_coords[1]);
        retaBresenham(x_coords[1], y_coords[1], m_x, m_y);
        retaBresenham(m_x, m_y, x_coords[0], y_coords[0]);
    }
   
    if(click1 && modo == CIR) {
        int R_temp = static_cast<int>(round(sqrt(pow(m_x - x_coords[0], 2) + pow(m_y - y_coords[0], 2))));
        circuloBresenham(x_coords[0], y_coords[0], R_temp);
    }
 
    if (drawingPolygon && !formas.empty() && formas.front().tipo == POL) {
        
        if (polygonVertexCount > 0) {
            vector<vertice> current_polygon_vertices;
            // Copia e inverte os vértices para a ordem de clique
            for (forward_list<vertice>::iterator it = formas.front().v.begin(); it != formas.front().v.end(); ++it) {
                current_polygon_vertices.push_back(*it);
            }
            reverse(current_polygon_vertices.begin(), current_polygon_vertices.end()); // Reverte para a ordem de clique

            // Desenha as linhas entre os vértices já confirmados 
            if (current_polygon_vertices.size() > 1) { // Verifica o tamanho do vector temporário
                for (size_t i = 0; i < current_polygon_vertices.size() - 1; ++i) {
                    retaBresenham(current_polygon_vertices[i].x, current_polygon_vertices[i].y,
                                  current_polygon_vertices[i+1].x, current_polygon_vertices[i+1].y);
                }
            }
            // Desenha a linha do último vértice adicionado ao mouse
            retaBresenham(current_polygon_vertices.back().x, current_polygon_vertices.back().y, m_x, m_y);
            if (polygonVertexCount >= 3) { // Alterado para 3 vértices
                retaBresenham(m_x, m_y, current_polygon_vertices.front().x, current_polygon_vertices.front().y);
            }
        }
    }

    
    int current_form_draw_index = 0;
    for(forward_list<forma>::iterator f = formas.begin(); f != formas.end(); f++, current_form_draw_index++){
        // Desenha a forma selecionada em outra cor
        if (current_form_draw_index == selectedFormIndex) {
            glColor3f(1.0, 0.0, 0.0); // Vermelho
            glLineWidth(2.0); 
        } else {
            glColor3f(0.0, 0.0, 0.0); // Preto
            glLineWidth(1.0); 
        }

        switch (f->tipo) {
            case LIN: {
                auto it_v = f->v.begin();
                int x2 = it_v->x; int y2 = it_v->y;
                it_v++;
                int x1 = it_v->x; int y1 = it_v->y;
                retaBresenham(x1, y1, x2, y2);
                break;
            }
            case RET: {
                auto it_v = f->v.begin();
                int x2 = it_v->x; int y2 = it_v->y;
                it_v++;
                int x1 = it_v->x; int y1 = it_v->y;
                drawQuadrilateral(x1, y1, x2, y2);
                break;
            }
            case TRI: {
                auto it_v = f->v.begin();
                int x3 = it_v->x; int y3 = it_v->y;
                it_v++;
                int x2 = it_v->x; int y2 = it_v->y;
                it_v++;
                int x1 = it_v->x; int y1 = it_v->y;

                drawTriangle(x1, y1, x2, y2, x3, y3);
                break;
            }
            case POL: {
                // A forward_list está invertida devido ao push_front.
                vector<vertice> temp_vertices;
                for (forward_list<vertice>::iterator it_v = f->v.begin(); it_v != f->v.end(); ++it_v) {
                    temp_vertices.push_back(*it_v);
                }
                reverse(temp_vertices.begin(), temp_vertices.end()); // Reverte a ordem

                forward_list<vertice> ordered_vertices;
                // Adicionar do final do vector para o início da forward_list para manter a ordem
                for (vector<vertice>::reverse_iterator rit = temp_vertices.rbegin(); rit != temp_vertices.rend(); ++rit) {
                    ordered_vertices.push_front(vertice(rit->x, rit->y)); // Cria um objeto vertice
                }

                // Apenas desenha o polígono finalizado se ele tem 3 ou mais vértices.
                if (temp_vertices.size() >= 3) { // Alterado para 3 vértices
                    drawPolygon(ordered_vertices);
                }
                break;
            }
            case CIR: { // Desenha círculo finalizado
                // Os vértices armazenados são {Centro, Raio}. 
                // A forward_list para CIR contém: {centro_x, centro_y}, {raio_x, raio_y(ignorado)}.
                // Devido ao push_front, o centro é o primeiro elemento e o raio é o segundo.
                forward_list<vertice>::iterator it_center = f->v.begin();
                int xc_stored = it_center->x;
                int yc_stored = it_center->y;

                forward_list<vertice>::iterator it_radius = it_center;
                ++it_radius; // Avança para o próximo elemento
                int R_stored = it_radius->x; // O raio é armazenado no componente X

                circuloBresenham(xc_stored, yc_stored, R_stored);
                break;
            }
        }
    }
    
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(1.0);
}

/*
 * Funcao que implementa o Algoritmo de Bresenham para rasterizacao de segmentos de retas
 */
void reduceToFirstOctant(int &x1, int &y1, int &x2, int &y2, int &originalOctant) {
    int dx = x2 - x1;
    int dy = y2 - y1;

    originalOctant = 0;

    if (abs(dy) > abs(dx)) { 
        swap(x1, y1);
        swap(x2, y2);
        originalOctant |= 1;
    }
    if (x1 > x2) { // Garante desenho da esquerda para a direita
        x1 = -x1;
        x2 = -x2;
        originalOctant |= 2;
    }
    if (y1 > y2) { // Garante desenho de baixo para cima
        y1 = -y1;
        y2 = -y2;
        originalOctant |= 4;
    }
}

void transformFromFirstOctant(int &x, int &y, int originalOctant) {
    if (originalOctant & 4) { 
        y = -y;
    }
    if (originalOctant & 2) { 
        x = -x;
    }
    if (originalOctant & 1) { 
        swap(x, y);
    }
}

void retaBresenham(int x1, int y1, int x2, int y2) {
    int originalOctant;
    int current_x1 = x1, current_y1 = y1;
    int current_x2 = x2, current_y2 = y2;

    reduceToFirstOctant(current_x1, current_y1, current_x2, current_y2, originalOctant);

    int dx = abs(current_x2 - current_x1);
    int dy = abs(current_y2 - current_y1);
    int sx = (current_x1 < current_x2) ? 1 : -1;
    int sy = (current_y1 < current_y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        int tx = current_x1, ty = current_y1;
        transformFromFirstOctant(tx, ty, originalOctant);
        drawPixel(tx, ty); 

        if (current_x1 == current_x2 && current_y1 == current_y2) break; // Termina quando chega ao ponto final

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            current_x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            current_y1 += sy;
        }
    }
}

void drawQuadrilateral(int x1, int y1, int x2, int y2) {
    // Desenha as quatro arestas do quadrilátero
    retaBresenham(x1, y1, x2, y1); // Linha superior
    retaBresenham(x2, y1, x2, y2); // Linha direita
    retaBresenham(x2, y2, x1, y2); // Linha inferior
    retaBresenham(x1, y2, x1, y1); // Linha esquerda
}
void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    // Desenha as três arestas do triângulo
    retaBresenham(x1, y1, x2, y2); // Primeira aresta
    retaBresenham(x2, y2, x3, y3); // Segunda aresta
    retaBresenham(x3, y3, x1, y1); // Terceira aresta
}
void drawPolygon(forward_list<vertice> vertices) {
    if (vertices.empty()) return;

    // Precisa de um iterador válido aqui
    forward_list<vertice>::iterator it = vertices.begin();
    int x_start = it->x, y_start = it->y; // Guarda o primeiro vértice para fechar o polígono
    int x_prev = x_start, y_prev = y_start;

    // Percorre a lista para desenhar as arestas entre vértices consecutivos
    forward_list<vertice>::iterator next_it = it; 
    ++next_it; 

    while (next_it != vertices.end()) {
        int x_curr = next_it->x, y_curr = next_it->y;
        retaBresenham(x_prev, y_prev, x_curr, y_curr);
        x_prev = x_curr;
        y_prev = y_curr;
        it = next_it; 
        ++next_it; 
    }

    // Fecha o polígono conectando o último vértice ao primeiro
    retaBresenham(x_prev, y_prev, x_start, y_start);
}



void getPixelColorInMemory(GLubyte* pixels, int imgWidth, int imgHeight, int x, int y, GLubyte color[3]) {
    if (x < 0 || x >= imgWidth || y < 0 || y >= imgHeight) {
        color[0] = 255; color[1] = 255; color[2] = 255; 
        return;
    }
    int index = (y * imgWidth + x) * 3;
    color[0] = pixels[index];
    color[1] = pixels[index + 1];
    color[2] = pixels[index + 2];
}

void setPixelColorInMemory(GLubyte* pixels, int imgWidth, int imgHeight, int x, int y, GLubyte r, GLubyte g, GLubyte b) {
    if (x >= 0 && x < imgWidth && y >= 0 && y < imgHeight) {
        int index = (y * imgWidth + x) * 3;
        pixels[index] = r;
        pixels[index + 1] = g;
        pixels[index + 2] = b;
    }
}
// --- FUNÇÕES DE FLOOD FILL ---

// Função Flood Fill iterativa com vizinhança 4
void floodFill4(int startX, int startY, GLubyte newColorR, GLubyte newColorG, GLubyte newColorB) {
    GLubyte* pixels = new GLubyte[width * height * 3];
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    GLubyte targetColor[3];
    getPixelColorInMemory(pixels, width, height, startX, startY, targetColor);

    printf("DEBUG: Clicado em (%d, %d). Cor lida (targetColor): R=%d, G=%d, B=%d\n", startX, startY, targetColor[0], targetColor[1], targetColor[2]);
    printf("DEBUG: Nova cor de preenchimento (newColor): R=%d, G=%d, B=%d\n", newColorR, newColorG, newColorB);

    
    if ((targetColor[0] == newColorR && targetColor[1] == newColorG && targetColor[2] == newColorB) ||
        (targetColor[0] == 0 && targetColor[1] == 0 && targetColor[2] == 0)) { 
        printf("DEBUG: Flood Fill interrompido: Ponto inicial já tem a cor de preenchimento ou é uma borda preta.\n");
        delete[] pixels;
        return;
    }

    printf("Flood Fill: Iniciando em (%d, %d). Cor alvo: (%d, %d, %d). Nova cor: (%d, %d, %d)\n",
           startX, startY, targetColor[0], targetColor[1], targetColor[2], newColorR, newColorG, newColorB);

    std::stack<vertice> pixelStack;
    pixelStack.push(vertice(startX, startY));

    while (!pixelStack.empty()) {
        vertice currentPixel = pixelStack.top();
        pixelStack.pop();

        int x = currentPixel.x;
        int y = currentPixel.y;

        GLubyte currentColor[3];
        // Obtém a cor do pixel no array em memória usando a função auxiliar externa
        getPixelColorInMemory(pixels, width, height, x, y, currentColor);

        // Se a cor do pixel atual for a cor alvo, preenche e adiciona vizinhos
        if (currentColor[0] == targetColor[0] &&
            currentColor[1] == targetColor[1] &&
            currentColor[2] == targetColor[2]) {

            // Define a nova cor no array em memória usando a função auxiliar externa
            setPixelColorInMemory(pixels, width, height, x, y, newColorR, newColorG, newColorB);

            // Adiciona vizinhos (vizinhança 4) à pilha
            pixelStack.push(vertice(x + 1, y));
            pixelStack.push(vertice(x - 1, y));
            pixelStack.push(vertice(x, y + 1));
            pixelStack.push(vertice(x, y - 1));
        }
    }

    
    glClear(GL_COLOR_BUFFER_BIT); 
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    delete[] pixels;
}