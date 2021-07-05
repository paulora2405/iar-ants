#include <GL/freeglut.h>
#include <GL/gl.h>

#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include "class_boid.hpp"

const int width = 1600, height = 900;
const auto bg_rgba = std::make_tuple(0.0, 0.6, 1.0, 1.0);
const int QNT_BOIDS = 150;
std::vector<Boid*> boids;

void init() {
  for(int i = 0; i < QNT_BOIDS; i++) {
    boids.push_back(new Boid(width, height));
  }
}

void show_boids() {
  for(auto b = boids.begin(); b != boids.end(); b++) {
    double red, green, blue;
    std::tie(red, green, blue) = (*b)->rgb;
    glColor3f(red, green, blue);
    glLineWidth(8.0);
    glBegin(GL_LINES);
    glVertex3f((*b)->x, (*b)->y, 0);
    double off_x = (*b)->vel_x * 2;
    double off_y = (*b)->vel_y * 2;
    glVertex3f((*b)->x + off_x, (*b)->y + off_y, 0);
    glEnd();
  }
}

void boid_interaction() {
#pragma omp parallel for
  for(auto boid = boids.begin(); boid != boids.end(); boid++) {
    std::vector<Boid*> close_boids = (*boid)->get_close_boids(boids);
    (*boid)->cohesion(close_boids);
    (*boid)->alignment(close_boids);
    (*boid)->separation(close_boids);
    (*boid)->move(width, height);
  }
}

void draw_scene() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  double r, g, b, a;
  std::tie(r, g, b, a) = bg_rgba;
  glClearColor(r, g, b, a);
  glDisable(GL_LINE_SMOOTH);
  glMatrixMode(GL_MODELVIEW);
  show_boids();
  glutSwapBuffers();
}

void reshape(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, width, 0.0, height, 0.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void idle() {
  boid_interaction();
  glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
  if(key == ' ')
    boids.clear();

  else if(key == 'm')
    for(int i = 0; i < 10; i++) {
      boids.push_back(new Boid(width, height));
    }

  else if(key == 'n')
    for(int i = 0; i < 10; i++) {
      if(!boids.empty()) boids.pop_back();
    }

  else if(key == 's')
    glutDestroyWindow(glutGetWindow());
}

void mouse(int button, int state, int x, int y) {
  if(button == GLUT_LEFT_BUTTON)
    if(state == GLUT_DOWN) {
      boids.push_back(new Boid(std::make_tuple(x, height - y)));
    }

  if(button == GLUT_RIGHT_BUTTON)
    if(state == GLUT_DOWN)
      if(!boids.empty()) {
        boids.pop_back();
      }
}

void info_update(int) {
  std::string s1 = "Boids = ";
  const unsigned short int leading_zeros = 4;
  std::string s2 = std::to_string(boids.size());
  if(s2.length() < leading_zeros) {
    s2 = std::string(leading_zeros - s2.length(), '0') + s2;
  }
  std::cout << s1 + s2 << " \t\r" << std::flush;
  glutTimerFunc(16, info_update, 0);
}

int main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize(width, height);
  glutCreateWindow("TC4 - Paulo Albuquerque");
  init();
  glutDisplayFunc(draw_scene);
  glutReshapeFunc(reshape);
  glutIdleFunc(idle);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);

  info_update(0);
  glutMainLoop();

  return 0;
}
