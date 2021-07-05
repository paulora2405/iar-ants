#include <GL/freeglut.h>
#include <GL/gl.h>

#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include "class_boid_dod.hpp"

const int width = 1600, height = 900;
const auto bg_rgba = std::make_tuple(0.0, 0.6, 1.0, 1.0);
const int QNT_BOIDS = 150;
struct class_boid_dod boids(QNT_BOIDS, width, height);

void show_boids() {
  for(unsigned int i = 0; i < boids.get_size(); i++) {
    float red, green, blue;
    std::tie(red, green, blue) = boids.rgb[i];
    glColor3f(red, green, blue);
    glLineWidth(8.0);
    glBegin(GL_LINES);
    glVertex3f(boids.x[i], boids.y[i], 0);
    float off_x = boids.vel_x[i] * 2;
    float off_y = boids.vel_y[i] * 2;
    glVertex3f(boids.x[i] + off_x, boids.y[i] + off_y, 0);
    glEnd();
  }
}

void boid_interaction() {
  for(unsigned int i = 0; i < boids.get_size(); i++) {
    std::vector<int> close_boids_indexes = boids.get_close_boids(i);
    boids.cohesion(i, close_boids_indexes);
    boids.alignment(i, close_boids_indexes);
    boids.separation(i, close_boids_indexes);
    boids.move(i);
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
    boids.clear_vectors();

  else if(key == 'm')
    boids.resize_vectors(boids.get_size() + 10);

  else if(key == 'n')
    boids.resize_vectors(boids.get_size() - 10);

  else if(key == 's')
    glutDestroyWindow(glutGetWindow());
}

void mouse(int button, int state, int x, int y) {
  if(button == GLUT_LEFT_BUTTON)
    if(state == GLUT_DOWN) {
      boids.from_pos(x, height - y);
    }

  if(button == GLUT_RIGHT_BUTTON)
    if(state == GLUT_DOWN)
      if(boids.get_size() > 0) {
        boids.resize_vectors(boids.get_size() - 1);
      }
}

void info_update(int) {
  std::string s1 = "Boids = ";
  const unsigned short int leading_zeros = 4;
  std::string s2 = std::to_string(boids.get_size());
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
  // init();
  glutDisplayFunc(draw_scene);
  glutReshapeFunc(reshape);
  glutIdleFunc(idle);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);

  info_update(0);
  glutMainLoop();

  return 0;
}
