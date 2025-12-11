
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
#include <vector>
#include <cstdio>
#include <string>
#include <cctype>
#include <cstdlib>
#ifdef _WIN32
    #include <windows.h>
    #include <mmsystem.h>
    #pragma comment(lib, "winmm.lib")
#endif

#include "glm.h"
#include "glut_text.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif


// Estrutura que representa vetores e posiçoes em 3d
struct Vec3 { float x, y, z; };

struct GameObject {
    GLMmodel* model;
    Vec3 position;
    Vec3 scale;
    bool active;
    Vec3 initial_position;
    Vec3 direction;
    bool is_out_of_house;
    int release_timer;

    GameObject() : model(NULL), active(true) {
        position.x = 0; position.y = 0; position.z = 0;
        initial_position.x = 0; initial_position.y = 0; initial_position.z = 0;
        scale.x = 1; scale.y = 1; scale.z = 1;
        direction.x = 0; direction.y = 0; direction.z = -1;
        is_out_of_house = false;
        release_timer = 0;
    }
};

struct Player {
    Vec3 position;
    Vec3 initial_position;
    int score;
    int lives;
};

// Definicoes das variaveis globais
GLuint width = 800, height = 600;
Player pacman;
bool key_states[256];
std::vector<GameObject> pac_dots;
std::vector<GameObject> power;
std::vector<GameObject> ghosts;

// Estado atual do jogo
bool game_over = false;
bool ghosts_M = false;
int ghost_M_timer = 0;
const int M_DURATION = 500;

// Os modelso .obj
GLMmodel* ghost_model = NULL;
GLMmodel* pac_dot_model = NULL;

// Definiçao do mapa
const int MAZE_WIDTH = 28;
const int MAZE_HEIGHT = 31;
const float TILE_SIZE = 1.0f;
// Construçao do mapa utilizando matrizes 
//0 representa espaço vazio, onde o player pecorre
//1 representa parede
//2 moeda
//3 power pellet
//4 spawn do pacman
//5 spawn dos fantasma
const int maze_layout[MAZE_HEIGHT][MAZE_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,3,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,3,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,2,1,1,1,1,1,0,1,1,0,1,1,1,1,1,2,1,1,1,1,1,1},
    {0,0,0,0,0,1,2,1,1,1,1,1,0,1,1,0,1,1,1,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,0,0,0,0,0,0,0,0,0,0,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,0,1,1,1,0,0,1,1,1,0,1,1,2,1,0,0,0,0,0},
    {1,1,1,1,1,1,2,1,1,0,1,0,0,5,5,0,0,1,0,1,1,2,1,1,1,1,1,1},
    {0,0,0,0,0,0,2,0,0,0,1,0,5,5,5,5,0,1,0,0,0,2,0,0,0,0,0,0},
    {1,1,1,1,1,1,2,1,1,0,1,0,0,0,0,0,0,1,0,1,1,2,1,1,1,1,1,1},
    {0,0,0,0,0,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,0,0,0,0,0,0,0,0,0,0,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,0,0,0,0,0},
    {1,1,1,1,1,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,3,2,2,1,1,2,2,2,2,2,2,2,0,0,2,2,2,2,2,2,2,1,1,2,2,3,1},
    {1,1,1,2,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,2,1,1,1},
    {1,1,1,2,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,2,1,1,1},
    {1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,1,1,2,1},
    {1,2,1,1,1,1,1,1,1,1,1,1,2,1,1,4,1,1,1,1,1,1,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// inicializacao de funcoes 
void init();
void display(); 
void reshape(int, int); 
void draw_maze();
void keyboard_down(unsigned char, int, int); 
void keyboard_up(unsigned char, int, int);
void process_input(); 
bool check_wall_collision(Vec3&); 
void update(int);
void build_scene(); 
void load_models(); 
void reset_game();
void check_collisions(); 
void update_ghosts_ai(); 
void draw_hud();
// inicializar a parte grafica do glut
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Pac-Man 3D");

    #ifdef _WIN32
    PlaySound(TEXT("sons/music.wav"), NULL, SND_ASYNC);
    #endif

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard_down);
    glutKeyboardUpFunc(keyboard_up);
    glutIgnoreKeyRepeat(1);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
// inicializa iluminacao,materiais
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < 256; i++) key_states[i] = false;
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat light_pos[] = {0.0f, 20.0f, 0.0f, 1.0f};
    GLfloat white_light[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat ambient_light[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);
    glEnable(GL_COLOR_MATERIAL);

    load_models();
    reset_game();
}
//carrega os modelos
void load_models() {
    printf("Carregando modelos...\n");
    ghost_model = glmReadOBJ((char*)"models/ghost.obj");
    if(ghost_model) glmUnitize(ghost_model);
    else printf("FALHA AO CARREGAR: models/ghost.obj. Usando fallback.\n");

    pac_dot_model = glmReadOBJ((char*)"models/pac_dot.obj");
    if(pac_dot_model) glmScale(pac_dot_model, 0.35f);
    else printf("FALHA AO CARREGAR: models/pac_dot.obj. Usando fallback.\n");
    printf("Modelos carregados.\n");
}
// posiciona os elementos na tela
void build_scene() {
    pac_dots.clear();
    power.clear();
    ghosts.clear();
    
    float x1 = (14.0f - MAZE_WIDTH / 2.0f) * TILE_SIZE;
    float z2 = (14.0f - MAZE_HEIGHT / 2.0f) * TILE_SIZE;

    for (int i = 0; i < MAZE_HEIGHT; ++i) {
        for (int j = 0; j < MAZE_WIDTH; ++j) {
            float x = (j - MAZE_WIDTH / 2.0f) * TILE_SIZE;
            float z = (i - MAZE_HEIGHT / 2.0f) * TILE_SIZE;
            
            if (maze_layout[i][j] == 2) {
                GameObject dot;
                dot.model = pac_dot_model;
                dot.position.x = x; dot.position.y = 0.0f; dot.position.z = z;
                pac_dots.push_back(dot);
            } else if (maze_layout[i][j] == 3) {
                GameObject pellet;
                pellet.model = pac_dot_model;
                pellet.position.x = x; pellet.position.y = 0.0f; pellet.position.z = z;
                pellet.scale.x = 3.8f; pellet.scale.y = 3.8f; pellet.scale.z = 3.8f;
                power.push_back(pellet);
            } else if (maze_layout[i][j] == 4) {
                pacman.initial_position.x = x; pacman.initial_position.y = 0.0f; pacman.initial_position.z = z;
            }
        }
    }
    
    for (int i = 0; i < 4; i++) {
        GameObject ghost;
        ghost.model = ghost_model;
        ghost.position.x = x1;
        ghost.position.z = z2;
        ghost.initial_position = ghost.position;
        ghost.scale.x = ghost.scale.y = ghost.scale.z = 0.8f;
        ghost.release_timer = i * 120;
        ghosts.push_back(ghost);
    }
    
    pacman.position = pacman.initial_position;
}
//funcao parar resetar tudo
void reset_game() {
    game_over = false;
    ghosts_M = false;
    ghost_M_timer = 0;
    pacman.score = 0;
    pacman.lives = 3;
    build_scene();
}
// desenha a cena do jogo a cada frame
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    //posicionamento da camera
    gluLookAt(0.0f, 18.0f, 18.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    draw_maze();

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	// le as posicoes das moedas e coloca a cor do material nelas
    for(size_t i = 0; i < pac_dots.size(); ++i) {
        if(pac_dots[i].active) {
            glPushMatrix();
            glTranslatef(pac_dots[i].position.x, pac_dots[i].position.y, pac_dots[i].position.z);
            glColor3f(1.0f, 1.0f, 0.0f); 
            if (pac_dots[i].model) {
                glmDraw(pac_dots[i].model, GLM_SMOOTH); 
            } else {
                glutSolidSphere(0.14f, 10, 10);
            }
            glPopMatrix();
        }
    }
	// faz o mesmo para as pilulas
    for(size_t i = 0; i < power.size(); ++i) {
        if(power[i].active) {
            glPushMatrix();
            glTranslatef(power[i].position.x, power[i].position.y, power[i].position.z);
            glScalef(power[i].scale.x, power[i].scale.y, power[i].scale.z);
            glColor3f(1.0f, 0.5f, 0.0f);
            if (power[i].model) {
                glmDraw(power[i].model, GLM_SMOOTH);
            } else {
                glutSolidSphere(0.3f, 16, 16);
            }
            glPopMatrix();
        }
    }
	// faz os mesmos para os fantasma colocando cada um de uma cor
    float ghost_colors[4][3] = {{1.0f, 0.0f, 0.0f}, {1.0f, 0.7f, 0.8f}, {0.4f, 1.0f, 1.0f}, {1.0f, 0.7f, 0.3f}};
    for(size_t i = 0; i < ghosts.size(); ++i) {
        if(ghosts[i].active) {
            glPushMatrix();
            glTranslatef(ghosts[i].position.x, ghosts[i].position.y, ghosts[i].position.z);
            glScalef(ghosts[i].scale.x, ghosts[i].scale.y, ghosts[i].scale.z);

            
            if (ghosts_M) {
                // Define o tempo que o fanstama comeca a piscar 
                int pisca_pisca = M_DURATION * 0.3; 

                if (ghost_M_timer < pisca_pisca) {
                    // alterna a cor a cada 10 frames
                    if ((ghost_M_timer / 10) % 2 == 0) {
                        glColor3f(1.0f, 1.0f, 1.0f); 
                    } else {
                        glColor3f(0.5f, 0.5f, 0.8f); 
                    }
                } else {
                    
                    glColor3f(0.5f, 0.5f, 0.8f); 
                }
            } else {
                
                glColor3fv(ghost_colors[i]);
            }

	            if (ghosts[i].model) glmDraw(ghosts[i].model, GLM_SMOOTH);
	            else glutSolidCube(0.8f);

            
            glPushMatrix();
            glColor3f(1.0f, 1.0f, 1.0f);
            glTranslatef(0.2f, 0.1f, 0.4f);
            glutSolidSphere(0.15f, 10, 10);
            glColor3f(0.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, 0.1f);
            glutSolidSphere(0.07f, 10, 10);
            glPopMatrix();

            glPushMatrix();
            glColor3f(1.0f, 1.0f, 1.0f);
            glTranslatef(-0.2f, 0.1f, 0.4f);
            glutSolidSphere(0.15f, 10, 10);
            glColor3f(0.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, 0.1f);
            glutSolidSphere(0.07f, 10, 10);
            glPopMatrix();
            
            glPopMatrix();
        }
    }
    
	//desenha pacman
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();
    glTranslatef(pacman.position.x, pacman.position.y, pacman.position.z);
    glutSolidSphere(0.48f, 20, 20);
    glPopMatrix();
    
    draw_hud();
    glutSwapBuffers();
}
//ajusta camera para o tamanho da janela quando for redimensionada
void reshape(int w, int h) {
    width = w; height = h;
    if (h == 0) h = 1;
    float ratio = (float)w / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    float view_size = 18.0f;
    glOrtho(-view_size * ratio, view_size * ratio, -view_size, view_size, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}
//desenha o labirinto do jogo
void draw_maze() {
    glColor3f(0.1f, 0.2f, 1.0f);
    for (int i = 0; i < MAZE_HEIGHT; ++i) {
        for (int j = 0; j < MAZE_WIDTH; ++j) {
            if (maze_layout[i][j] == 1) {
                glPushMatrix();
                float x = (j - MAZE_WIDTH / 2.0f) * TILE_SIZE;
                float z = (i - MAZE_HEIGHT / 2.0f) * TILE_SIZE;
                glTranslatef(x, 0.0f, z);
                glutSolidCube(TILE_SIZE);
                glPopMatrix();
            }
        }
    }
}
//estado das teclas 
void keyboard_down(unsigned char key, int x, int y) {
    if (key == 'r' || key == 'R') {
        if (game_over) reset_game();
    }
    key_states[tolower(key)] = true;
    if (key == 27) exit(0);
}

void keyboard_up(unsigned char key, int x, int y) {
    key_states[tolower(key)] = false;
}
// funcao que verifica se existe paredes baseado na matriz
bool check_wall_collision(Vec3& pos) {
    int grid_j = round(pos.x / TILE_SIZE + MAZE_WIDTH / 2.0f);
    int grid_i = round(pos.z / TILE_SIZE + MAZE_HEIGHT / 2.0f);

    if (grid_j <= 0 && grid_i == 14) { 
        pos.x = (MAZE_WIDTH - 1.5f - MAZE_WIDTH / 2.0f) * TILE_SIZE; 
        return false; 
    }
    if (grid_j >= MAZE_WIDTH - 1 && grid_i == 14) { 
        pos.x = (0.5f - MAZE_WIDTH / 2.0f) * TILE_SIZE; 
        return false; 
    }
    if (grid_i < 0 || grid_i >= MAZE_HEIGHT || grid_j < 0 || grid_j >= MAZE_WIDTH) 
	return true;

    if (maze_layout[grid_i][grid_j] == 1) return true;
    return false;
}
// movimentacao
void process_input() {
    if (game_over) return;
    float move_speed = 0.11f;
    Vec3 nextPos = pacman.position;
    
    if (key_states['w']) nextPos.z -= move_speed;
    else if (key_states['s']) nextPos.z += move_speed;
    else if (key_states['a']) nextPos.x -= move_speed;
    else if (key_states['d']) nextPos.x += move_speed;
    
    if (!check_wall_collision(nextPos)) pacman.position = nextPos;
}
// atualiza o jogo a cada frame
void update(int value) {
    process_input();
    
    if (!game_over) {
        if (ghosts_M) {
            ghost_M_timer--;
            if (ghost_M_timer <= 0) {
                ghosts_M = false;
                for(size_t i = 0; i < ghosts.size(); ++i) {
                     if(!ghosts[i].active) {
                        ghosts[i].active = true;
                        ghosts[i].position = ghosts[i].initial_position;
                        ghosts[i].is_out_of_house = false;
                        ghosts[i].release_timer = i * 120;
                     }
                }
            }
        }
        check_collisions();
        update_ghosts_ai();
    }
    
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}
// verificaçao da colisao do pacman com as moedas
void check_collisions() {
    if (game_over) return;
    for (size_t i = 0; i < pac_dots.size(); ++i) {
        if (pac_dots[i].active) {
            float dx = pacman.position.x - pac_dots[i].position.x;
            float dz = pacman.position.z - pac_dots[i].position.z;
            if (dx*dx + dz*dz < 0.5f * 0.5f) {
                pac_dots[i].active = false;
                pacman.score += 10;
                #ifdef _WIN32
                PlaySound(TEXT("sons/eat.wav"), NULL, SND_ASYNC);
                #endif
            }
        }
    }
	// verificacao de colisao do pacman com as pilulas
    for (size_t i = 0; i < power.size(); ++i) {
        if (power[i].active) {
            float dx = pacman.position.x - power[i].position.x;
            float dz = pacman.position.z - power[i].position.z;
            if (dx*dx + dz*dz < 0.7f * 0.7f) {
                power[i].active = false;
                pacman.score += 50;
                ghosts_M = true;
                ghost_M_timer = M_DURATION;
                #ifdef _WIN32
                PlaySound(TEXT("sons/music base.wav"), NULL, SND_ASYNC);
                #endif
            }
        }
    }
	// verificao de colisao do pacman com os fantasmas em todos os modos
    for (size_t i = 0; i < ghosts.size(); ++i) {
        if (ghosts[i].active) {
            float dx = pacman.position.x - ghosts[i].position.x;
            float dz = pacman.position.z - ghosts[i].position.z;
            if (dx*dx + dz*dz < 0.6f * 0.6f) {
                if (ghosts_M) {
                    ghosts[i].active = false;
                    pacman.score += 200;
                } else {
                    pacman.lives--;
                    #ifdef _WIN32
                    PlaySound(NULL, 0, 0); 
                    PlaySound(TEXT("sons/die.wav"), NULL, SND_SYNC);
                    #endif

                    if (pacman.lives <= 0) {
                        game_over = true;
                    } else {
                        pacman.position = pacman.initial_position;
                        for(size_t g=0; g<ghosts.size(); ++g) {
                            ghosts[g].position = ghosts[g].initial_position;
                            ghosts[g].is_out_of_house = false;
                            ghosts[g].release_timer = g * 120;
                        }
                    }
                }
            }
        }
    }
}

void update_ghosts_ai() {
    if (game_over) return;
    float ghost_speed = 0.1f; 

    Vec3 house_exit_pos;
    house_exit_pos.x = (13.5f - MAZE_WIDTH / 2.0f) * TILE_SIZE;
    house_exit_pos.y = 0.0f;
    house_exit_pos.z = (11.0f - MAZE_HEIGHT / 2.0f) * TILE_SIZE;

    for (size_t i = 0; i < ghosts.size(); ++i) {
        if (!ghosts[i].active) continue;

        if (ghosts[i].release_timer > 0) {
            ghosts[i].release_timer--;
            continue;
        }

        Vec3 pos = ghosts[i].position;
        Vec3 target_pos;

        if (!ghosts[i].is_out_of_house) {
            target_pos = house_exit_pos;
            float dx = pos.x - house_exit_pos.x;
            float dz = pos.z - house_exit_pos.z;
            if (dx*dx + dz*dz < 0.5f) {
                ghosts[i].is_out_of_house = true;
            }
        } else {
            target_pos = pacman.position;
        }
        
        int grid_j = round(pos.x / TILE_SIZE + MAZE_WIDTH / 2.0f);
        int grid_i = round(pos.z / TILE_SIZE + MAZE_HEIGHT / 2.0f);
        float tile_center_x = (grid_j - MAZE_WIDTH / 2.0f) * TILE_SIZE;
        float tile_center_z = (grid_i - MAZE_HEIGHT / 2.0f) * TILE_SIZE;

        if (fabs(pos.x - tile_center_x) < ghost_speed / 2.0f && fabs(pos.z - tile_center_z) < ghost_speed / 2.0f) {
            
            Vec3 best_direction = {0, 0, 0};
            float best_dist_sq = -1.0f;
            int valid_moves_count = 0;
            Vec3 directions_to_check[4] = {{0,0,-1}, {0,0,1}, {-1,0,0}, {1,0,0}};
            
           
            	
            for(int k=0; k<4; ++k) {
                int next_i = grid_i + (int)directions_to_check[k].z;
                int next_j = grid_j + (int)directions_to_check[k].x;
                if (next_i >= 0 && next_i < MAZE_HEIGHT && next_j >= 0 && next_j < MAZE_WIDTH && maze_layout[next_i][next_j] != 1) {
                    valid_moves_count++;
                }
            }
            
            for(int k=0; k<4; ++k) {
                Vec3 current_dir = directions_to_check[k];
                if (valid_moves_count > 1 && (current_dir.x == -ghosts[i].direction.x && current_dir.z == -ghosts[i].direction.z)) {
                    continue;
                }

                int next_grid_i = grid_i + (int)current_dir.z;
                int next_grid_j = grid_j + (int)current_dir.x;
				// determina a distancia da proxima posiçao do fantasma e o pacman
                if (next_grid_i >= 0 && next_grid_i < MAZE_HEIGHT && next_grid_j >= 0 && next_grid_j < MAZE_WIDTH && maze_layout[next_grid_i][next_grid_j] != 1) {
                    float dx = ((next_grid_j - MAZE_WIDTH/2.0f) * TILE_SIZE) - target_pos.x;
                    float dz = ((next_grid_i - MAZE_HEIGHT/2.0f) * TILE_SIZE) - target_pos.z;
                    float dist_sq = dx*dx + dz*dz;
                    // verifica estado do fanstama e escolhe menor ou maior distancia
                    bool is_better_move = false;
                    if(best_dist_sq < 0) is_better_move = true;
                    else if (ghosts_M && ghosts[i].is_out_of_house) {
                        if(dist_sq > best_dist_sq) is_better_move = true; //foge
                    } else {
                        if(dist_sq < best_dist_sq) is_better_move = true; //vai atras
                    }

                    if(is_better_move) {
                        best_dist_sq = dist_sq;
                        best_direction = current_dir;
                    }
                }
            }
            if (best_direction.x != 0 || best_direction.z != 0) {
                 ghosts[i].direction = best_direction;
            }
        }
        
        Vec3 next_pos = ghosts[i].position;
        next_pos.x += ghosts[i].direction.x * ghost_speed;
        next_pos.z += ghosts[i].direction.z * ghost_speed;
        
        if (!check_wall_collision(next_pos)) {
            ghosts[i].position = next_pos;
        }
    }
}

void draw_hud() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glColor3f(1.0f, 1.0f, 1.0f);
    std::string score_text = "Score: " + my::to_string(pacman.score);
    draw_text_bitmap(10, height - 25, score_text, font_bitmap);
    std::string lives_text = "Lives: " + my::to_string(pacman.lives);
    draw_text_bitmap(width - 100, height - 25, lives_text, font_bitmap);

    int active_dots = 0;
    for (size_t i = 0; i < pac_dots.size(); ++i) {
        if (pac_dots[i].active) active_dots++;
    }
    
    if (active_dots == 0 && !game_over) {
        glColor3f(1.0f, 1.0f, 0.0f);
        std::string win_text = "VOCE VENCEU!";
        float text_width = 0;
        for (size_t i = 0; i < win_text.length(); i++) text_width += glutBitmapWidth(font_bitmap, win_text[i]);
        draw_text_bitmap((width - text_width) / 2.0f, height / 2.0f, win_text, font_bitmap);
        game_over = true;
    }

    if (game_over && pacman.lives <= 0) {
        glColor3f(1.0f, 0.0f, 0.0f);
        std::string over_text = "GAME OVER";
        float text_width = 0;
        for (size_t i = 0; i < over_text.length(); i++) text_width += glutBitmapWidth(font_bitmap, over_text[i]);
        draw_text_bitmap((width - text_width) / 2.0f, height / 2.0f, over_text, font_bitmap);
        
        glColor3f(1.0f, 1.0f, 1.0f);
        std::string restart_text = "Pressione 'R' para reiniciar";
        text_width = 0;
        for (size_t i = 0; i < restart_text.length(); i++) text_width += glutBitmapWidth(font_bitmap, restart_text[i]);
        draw_text_bitmap((width - text_width) / 2.0f, height / 2.0f - 25, restart_text, font_bitmap);
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}