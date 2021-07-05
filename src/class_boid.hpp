#ifndef CLASS_BOID_H
#define CLASS_BOID_H

#include <algorithm>
#include <random>
#include <tuple>
#include <vector>

std::default_random_engine generator;
std::uniform_real_distribution<double> uniform_real(0.3, 1.0);
std::uniform_int_distribution<int> uniform_int_vel(4, 8 + 1);
std::uniform_int_distribution<int> uniform_int_1to10(1, 10 + 1);
std::uniform_int_distribution<int> choice(0, 2);

// ####### BEGIN-CLASS #######
class Boid {
public:
  const static int max_velocity = 10;
  const static int min_distance = 20;
  const static int close_distance = 150;
  const static int border = 25;

  double x, y, vel_x, vel_y;

  std::tuple<double, double, double> rgb;

public:
  friend bool operator==(Boid &lhs, Boid &rhs);
  Boid(int, int);
  Boid(std::tuple<int, int> xy);
  ~Boid();

  void cohesion(std::vector<Boid *>);
  void alignment(std::vector<Boid *>);
  void separation(std::vector<Boid *>);

  void move(int, int);

  std::vector<Boid *> get_close_boids(std::vector<Boid *>);

  double distance_to(Boid);
};
// ####### END-CLASS #######

Boid::Boid(int width, int height) {
  if(width % 2 != 0) width += 1;
  if(height % 2 != 0) height += 1;
  std::uniform_int_distribution<int> uniform_int_x(0, width / 2 + 1);
  std::uniform_int_distribution<int> uniform_int_y(0, height / 2 + 1);

  this->x = uniform_int_x(generator) + width / 4;
  this->y = uniform_int_y(generator) + height / 4;

  this->vel_x = uniform_int_vel(generator) * (choice(generator) ? 1 : -1);
  this->vel_y = uniform_int_vel(generator) * (choice(generator) ? 1 : -1);

  this->rgb = std::make_tuple(uniform_real(generator), uniform_real(generator),
                              uniform_real(generator) / 2.5);
}

Boid::Boid(std::tuple<int, int> xy) {
  double x, y;
  std::tie(x, y) = xy;

  this->x = x;
  this->y = y;

  this->vel_x = uniform_int_vel(generator) * (choice(generator) ? 1 : -1);
  this->vel_y = uniform_int_vel(generator) * (choice(generator) ? 1 : -1);

  this->rgb = std::make_tuple(uniform_real(generator), uniform_real(generator),
                              uniform_real(generator) / 2.5);
}

Boid::~Boid() {}

void Boid::separation(std::vector<Boid *> boids) {
  if(boids.size() < 1) return;
  double dis_x = 0, dis_y = 0;
  int num_close = 0;

  for(auto boid = boids.begin(); boid != boids.end(); boid++) {
    if(this == *boid) continue;

    double distance = this->distance_to(*(*boid));

    if(distance < this->min_distance) {
      num_close++;
      double x_diff = this->x - (*boid)->x;
      double y_diff = this->y - (*boid)->y;

      if(x_diff >= 0)
        x_diff = sqrt(this->min_distance) - x_diff;
      else
        x_diff = -sqrt(this->min_distance) - x_diff;

      if(y_diff >= 0)
        y_diff = sqrt(this->min_distance) - y_diff;
      else
        y_diff = -sqrt(this->min_distance) - y_diff;

      dis_x += x_diff;
      dis_y += y_diff;
    }
  }

  if(num_close == 0) return;

  this->vel_x -= dis_x / 5.0;
  this->vel_y -= dis_y / 5.0;
}

void Boid::cohesion(std::vector<Boid *> boids) {
  if(boids.size() < 1) return;

  double avg_x = 0, avg_y = 0;

  for(auto boid = boids.begin(); boid != boids.end(); boid++) {
    if(this == *boid) continue;

    avg_x += this->x - (*boid)->x;
    avg_y += this->y - (*boid)->y;
  }

  avg_x /= boids.size();
  avg_y /= boids.size();

  this->vel_x -= avg_x / 100;
  this->vel_y -= avg_y / 100;
}

void Boid::alignment(std::vector<Boid *> boids) {
  if(boids.size() < 1) return;

  double avg_x = 0, avg_y = 0;

  for(auto boid = boids.begin(); boid != boids.end(); boid++) {
    if(this == *boid) continue;

    avg_x += (*boid)->vel_x;
    avg_y += (*boid)->vel_y;
  }

  avg_x /= boids.size();
  avg_y /= boids.size();

  this->vel_x += avg_x / 40;
  this->vel_y += avg_y / 40;
}

void Boid::move(int width, int height) {
  if(this->x < this->border && this->vel_x < 0) {
    this->vel_x *= -uniform_int_1to10(generator) / 5.0;
    this->vel_y *= 1 + uniform_int_1to10(generator) / 5.0;
  }
  if(this->x > width - this->border && this->vel_x > 0) {
    this->vel_x *= -uniform_int_1to10(generator) / 5.0;
    this->vel_y *= 1 + uniform_int_1to10(generator) / 5.0;
  }

  if(this->y < this->border && this->vel_y < 0) {
    this->vel_y *= -uniform_int_1to10(generator) / 5.0;
    this->vel_x *= 1 + uniform_int_1to10(generator) / 5.0;
  }
  if(this->y > height - this->border && this->vel_y > 0) {
    this->vel_y *= -uniform_int_1to10(generator) / 5.0;
    this->vel_x *= 1 + uniform_int_1to10(generator) / 5.0;
  }

  if(abs(this->vel_x) > this->max_velocity || abs(this->vel_y) > this->max_velocity) {
    double scale_factor = this->max_velocity / std::max(abs(this->vel_x), abs(this->vel_y));
    this->vel_x *= scale_factor;
    this->vel_y *= scale_factor;
  }

  this->x += this->vel_x;
  this->y += this->vel_y;
}

std::vector<Boid *> Boid::get_close_boids(std::vector<Boid *> boids) {
  std::vector<Boid *> close_boids;
  for(auto other = boids.begin(); other != boids.end(); other++) {
    if(this == *other) continue;

    double distance = this->distance_to(*(*other));
    if(distance < this->close_distance) {
      close_boids.push_back(*other);
    }
  }
  return close_boids;
}

double Boid::distance_to(Boid other) {
  double dist_x = this->x - other.x;
  double dist_y = this->y - other.y;
  dist_x = pow(dist_x, 2);
  dist_y = pow(dist_y, 2);
  return sqrt(dist_x + dist_y);
}

bool operator==(Boid &b1, Boid &b2) {
  if(b1.rgb == b2.rgb)
    if(b1.x == b2.x && b1.y == b2.y) {
      return true;
    }

  return false;
}

#endif