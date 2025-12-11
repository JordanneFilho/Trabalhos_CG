# Trabalhos de Computação Gráfica

Este repositório contém 4 trabalhos desenvolvidos para a disciplina de Computação Gráfica utilizando OpenGL/GLUT.

---

## 📚 Índice
1. [Trabalho 1 - Gato 2D](#trabalho-1---gato-2d)
2. [Trabalho 2 - Braço Robótico 3D](#trabalho-2---braço-robótico-3d)
3. [Trabalho 3 - Paint (Rasterização)](#trabalho-3---paint-rasterização)
4. [Projeto Pacman - Jogo 3D](#projeto-pacman---jogo-3d)

---

## 🐱 Trabalho 1 - Gato 2D

**Pasta:** `trabalho1/gato/`

### Descrição
Implementação de um desenho 2D de um gato utilizando primitivas geométricas do OpenGL. O projeto demonstra o uso de formas básicas para criar figuras complexas.

### Características
- Desenho de um rosto de gato utilizando círculos e elipses
- Orelhas triangulares com detalhes internos
- Olhos com íris, pupilas e brilho
- Nariz estilizado com formato geométrico
- Uso de `GL_TRIANGLE_FAN` para círculos e elipses
- Sistema de coordenadas 2D com `gluOrtho2D`

### Tecnologias
- OpenGL
- GLUT
- C++

### Como Compilar
```bash
cd trabalho1/gato/
g++ main.cpp -o gato -lGL -lGLU -lglut
./gato
```

---

## 🦾 Trabalho 2 - Braço Robótico 3D

**Pasta:** `trabalho2/braço/`

### Descrição
Simulação de um braço robótico articulado em 3D com múltiplas juntas e dedos móveis. Demonstra conceitos de transformações hierárquicas e modelagem 3D.

### Características
- Braço articulado com ombro, cotovelo e punho
- Mão com dedos articulados (indicador, polegar e anelar)
- Rotação completa do braço no eixo Y
- Controle interativo via teclado
- Uso de transformações hierárquicas (matriz de transformação)
- Visualização em perspectiva 3D com `gluPerspective`

### Controles do Teclado
- **o/O**: Rotação do ombro
- **c/C**: Rotação do cotovelo
- **m/M**: Rotação do punho
- **i/I**: Movimento do dedo indicador
- **p/P**: Movimento do dedo polegar
- **a/A**: Movimento do dedo anelar
- **y/Y**: Rotação completa do braço
- **ESC**: Sair

### Tecnologias
- OpenGL
- GLUT
- C++
- Depth Buffer (Z-Buffer)

### Como Compilar
```bash
cd trabalho2/braço/
g++ braco.cpp -o braco -lGL -lGLU -lglut
./braco
```

---

## 🎨 Trabalho 3 - Paint (Rasterização)

**Pasta:** `trabalho 3/paint/CG.a13 - Rasterizacao usando OpenGL - Trabalho de Rasterizacao/`

### Descrição
Aplicação de desenho vetorial que implementa algoritmos de rasterização para desenhar formas geométricas 2D. Inclui ferramentas de transformação geométrica e preenchimento de áreas.

### Características
- **Formas Geométricas:**
  - Linhas
  - Triângulos
  - Retângulos
  - Polígonos (com número variável de vértices)
  - Círculos
  - Preenchimento de áreas (Flood Fill)

- **Transformações 2D:**
  - Translação (↑ ↓ ← →)
  - Escala (aumentar/diminuir)
  - Rotação (horária/anti-horária)
  - Cisalhamento (shear) em X e Y
  - Reflexão (X, Y, origem)

- **Funcionalidades:**
  - Sistema de seleção de formas
  - Lista encadeada para armazenar formas desenhadas
  - Estrutura de dados com vértices
  - Transformações usando matrizes 3x3 homogêneas
  - Interface interativa com mouse

### Estrutura de Dados
```cpp
struct vertice {
    int x, y;
};

struct forma {
    int tipo;
    forward_list<vertice> v;
};
```

### Tecnologias
- OpenGL
- GLUT
- C++ (STL: forward_list, vector, stack)
- Algoritmos de rasterização

### Como Compilar
```bash
cd "trabalho 3/paint/CG.a13 - Rasterizacao usando OpenGL - Trabalho de Rasterizacao/"
g++ paint.cpp -o paint -lGL -lGLU -lglut
./paint
```

---

## 👻 Projeto Pacman - Jogo 3D

**Pasta:** `Pacman.Jordanne/`

### Descrição
Implementação de um jogo inspirado no clássico Pac-Man em ambiente 3D, com modelos carregados de arquivos .obj, iluminação, texturas e mecânicas de jogo completas.

### Características
- **Modelos 3D:**
  - Pac-Man (personagem principal)
  - Fantasmas (inimigos)
  - Moedas (pac dots)
  - Power-ups

- **Recursos Gráficos:**
  - Carregamento de modelos .obj com a biblioteca GLM
  - Texturas em formato PPM
  - Sistema de iluminação com modelo de Phong
  - Materiais configuráveis (Ka, Kd, Ks)
  - Depth Test (Z-Buffer)
  - Aplicação de texturas (GL_ADD)

- **Mecânicas de Jogo:**
  - Sistema de pontuação
  - Sistema de vidas
  - Fantasmas com IA básica
  - Power-ups que permitem comer fantasmas
  - Detecção de colisões
  - Game Over

- **Sons:**
  - Pasta `sons/` para efeitos sonoros do jogo (Windows)

### Estrutura de Dados
```cpp
struct GameObject {
    GLMmodel* model;
    Vec3 position;
    Vec3 scale;
    bool active;
    Vec3 direction;
};

struct Player {
    Vec3 position;
    int score;
    int lives;
};
```

### Arquivos Principais
- `modelo3D.cpp`: Motor principal do jogo
- `luz_textura.cpp`: Exemplos de iluminação e texturas
- `glm.h/glm.cpp`: Biblioteca para carregar modelos .obj
- `textureRGB.h`: Sistema de texturas
- `data/`: Texturas e materiais
- `models/`: Modelos 3D dos objetos do jogo

### Tecnologias
- OpenGL
- GLUT
- C++
- GLM (OpenGL Model loader)
- Windows Multimedia API (sons)

### Como Compilar
```bash
cd Pacman.Jordanne/
# Windows:
g++ modelo3D.cpp glm.cpp -o pacman -lGL -lGLU -lglut -lwinmm
./pacman
```

---

## 🛠️ Requisitos Gerais

### Dependências
- OpenGL
- GLUT (FreeGLUT)
- GLU
- Compilador C++ (g++, MinGW, MSVC)

### Instalação das Dependências

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get install freeglut3-dev
sudo apt-get install mesa-common-dev
```

**Windows:**
- Instalar MinGW ou MSVC
- Baixar FreeGLUT e configurar no ambiente

**macOS:**
```bash
brew install freeglut
```

---

## 👨‍💻 Autor

**Jordanne**

Trabalhos desenvolvidos como parte do curso de Computação Gráfica.

---

## 📝 Observações

- Todos os projetos foram desenvolvidos em C++ com OpenGL/GLUT
- Os códigos incluem comentários e são baseados em exemplos do Prof. Laurindo de Sousa Britto Neto
- Cada projeto demonstra diferentes aspectos da programação gráfica:
  - **Trabalho 1**: Primitivas 2D
  - **Trabalho 2**: Transformações 3D hierárquicas
  - **Trabalho 3**: Rasterização e transformações 2D
  - **Projeto Pacman**: Jogo completo com modelos 3D, texturas e iluminação

---


