#ifndef CLASS_BOID_DOD_H
#define CLASS_BOID_DOD_H

#include <algorithm>
#include <random>
#include <tuple>
#include <vector>

std::default_random_engine generator;
std::uniform_real_distribution<double> uniform_real(0.3, 1.0);
std::uniform_int_distribution<int> uniform_int_vel(4, 8 + 1);
std::uniform_int_distribution<int> uniform_int_1to10(1, 10 + 1);
std::uniform_int_distribution<int> choice(0, 2);

struct class_boid_dod {
  // static fields
  const unsigned short int max_velocity = 10;
  const unsigned short int min_distance = 20;
  const unsigned short int close_distance = 150;
  const unsigned short int border = 25;
  unsigned short num_boids;
  unsigned short width;
  unsigned short height;

  // non-static fields represented as vectors
  // an instance of an object is a index in the vector
  std::vector<double> x;
  std::vector<double> y;
  std::vector<double> vel_x;
  std::vector<double> vel_y;
  std::vector<std::tuple<double, double, double>> rgb;

  // struct constructor
  class_boid_dod(const int n, int _width, int _height) {
    num_boids = n;

    x.resize(n);
    y.resize(n);
    vel_x.resize(n);
    vel_y.resize(n);
    rgb.resize(n);

    if(_width % 2 != 0) _width += 1;
    if(_height % 2 != 0) _height += 1;
    width = _width;
    height = _height;
    std::uniform_int_distribution<int> uniform_int_x(0, width / 2 + 1);
    std::uniform_int_distribution<int> uniform_int_y(0, height / 2 + 1);

    for(auto &this_x : x) this_x = uniform_int_x(generator) + width / 4;

    for(auto &this_y : y) this_y = uniform_int_y(generator) + height / 4;

    for(auto &this_vel_x : vel_x)
      this_vel_x = uniform_int_vel(generator) * (choice(generator) ? 1 : -1);

    for(auto &this_vel_y : vel_y)
      this_vel_y = uniform_int_vel(generator) * (choice(generator) ? 1 : -1);

    for(auto &this_rgb : rgb)
      this_rgb = std::make_tuple(uniform_real(generator), uniform_real(generator),
                                 uniform_real(generator) / 2.5);
  }

  unsigned int get_size() { return num_boids; }

  void clear_vectors() {
    num_boids = 0;
    x.clear();
    y.clear();
    vel_x.clear();
    vel_y.clear();
    rgb.clear();
  }

  void from_pos(const int _x, const int _y) {
    num_boids++;
    x.push_back(_x);
    y.push_back(_y);

    vel_x.push_back(uniform_int_vel(generator) * (choice(generator) ? 1 : -1));

    vel_y.push_back(uniform_int_vel(generator) * (choice(generator) ? 1 : -1));

    rgb.push_back(std::make_tuple(uniform_real(generator), uniform_real(generator),
                                  uniform_real(generator) / 2.5));
  }

  void resize_vectors(short int n) {
    if(num_boids < 1) return;

    num_boids += n >= 0 ? n - num_boids : 0;

    n = n >= 0 ? n - x.size() : -num_boids;
    if(n > 0) {
      std::uniform_int_distribution<int> uniform_int_x(0, width / 2 + 1);
      std::uniform_int_distribution<int> uniform_int_y(0, height / 2 + 1);
      for(auto i = 0; i < n; i++) {
        x.push_back(uniform_int_x(generator) + width / 4);
        y.push_back(uniform_int_y(generator) + height / 4);
        vel_x.push_back(uniform_int_vel(generator) * (choice(generator) ? 1 : -1));
        vel_y.push_back(uniform_int_vel(generator) * (choice(generator) ? 1 : -1));
        rgb.push_back(std::make_tuple(uniform_real(generator), uniform_real(generator),
                                      uniform_real(generator) / 2.5));
      }
    }

    else if(n < 0)
      for(auto i = 0; i < abs(n); i++) {
        x.pop_back();
        y.pop_back();
        vel_x.pop_back();
        vel_y.pop_back();
        rgb.pop_back();
      }
    // else n == 0 { do nothing }
  }

  void separation(const unsigned short this_i, const std::vector<int> indexes) {
    if(indexes.empty()) return;

    double dis_x = 0, dis_y = 0;
    unsigned short num_close = 0;

    for(auto &i : indexes) {
      if(this_i == i) continue;

      double distance = distance_to(this_i, i);

      if(distance < min_distance) {
        num_close++;
        double x_diff = x[this_i] - x[i];
        double y_diff = y[this_i] - y[i];

        if(x_diff >= 0)
          x_diff = sqrt(min_distance) - x_diff;
        else
          x_diff = -sqrt(min_distance) - x_diff;

        if(y_diff >= 0)
          y_diff = sqrt(min_distance) - y_diff;
        else
          y_diff = -sqrt(min_distance) - y_diff;

        dis_x += x_diff;
        dis_y += y_diff;
      }
    }

    if(num_close == 0) return;

    vel_x[this_i] -= dis_x / 5.0;
    vel_y[this_i] -= dis_y / 5.0;
  }

  void cohesion(const unsigned short this_i, const std::vector<int> indexes) {
    if(indexes.empty()) return;

    double avg_x = 0, avg_y = 0;

    for(auto &i : indexes) {
      if(this_i == i) continue;

      avg_x += x[this_i] - x[i];
      avg_y += y[this_i] - y[i];
    }

    avg_x /= indexes.size();
    avg_y /= indexes.size();

    vel_x[this_i] -= avg_x / 100;
    vel_y[this_i] -= avg_y / 100;
  }

  void alignment(const unsigned short this_i, const std::vector<int> indexes) {
    if(indexes.empty()) return;

    double avg_x = 0, avg_y = 0;

    for(auto &i : indexes) {
      if(this_i == i) continue;

      avg_x += vel_x[i];
      avg_y += vel_y[i];
    }

    avg_x /= indexes.size();
    avg_y /= indexes.size();

    vel_x[this_i] += avg_x / 40;
    vel_y[this_i] += avg_y / 40;
  }

  void move(const unsigned short i) {
    if(x[i] < border && vel_x[i] < 0) {
      vel_x[i] *= -uniform_int_1to10(generator) / 5.0;
      vel_y[i] *= 1 + uniform_int_1to10(generator) / 5.0;
    }
    if(x[i] > width - border && vel_x[i] > 0) {
      vel_x[i] *= -uniform_int_1to10(generator) / 5.0;
      vel_y[i] *= 1 + uniform_int_1to10(generator) / 5.0;
    }
    if(y[i] < border && vel_y[i] < 0) {
      vel_y[i] *= -uniform_int_1to10(generator) / 5.0;
      vel_x[i] *= 1 + uniform_int_1to10(generator) / 5.0;
    }
    if(y[i] > height - border && vel_y[i] > 0) {
      vel_y[i] *= -uniform_int_1to10(generator) / 5.0;
      vel_x[i] *= 1 + uniform_int_1to10(generator) / 5.0;
    }

    if(abs(vel_x[i]) > max_velocity || abs(vel_y[i]) > max_velocity) {
      const double scale_factor = max_velocity / std::max(abs(vel_x[i]), abs(vel_y[i]));
      vel_x[i] *= scale_factor;
      vel_y[i] *= scale_factor;
    }

    x[i] += vel_x[i];
    y[i] += vel_y[i];
  }

  void move() {
    for(auto i = 0; i < num_boids; i++) {
      if(x[i] < border && vel_x[i] < 0) {
        vel_x[i] *= -uniform_int_1to10(generator) / 5.0;
        vel_y[i] *= 1 + uniform_int_1to10(generator) / 5.0;
      }
      if(x[i] > width - border && vel_x[i] > 0) {
        vel_x[i] *= -uniform_int_1to10(generator) / 5.0;
        vel_y[i] *= 1 + uniform_int_1to10(generator) / 5.0;
      }
      if(y[i] < border && vel_y[i] < 0) {
        vel_y[i] *= -uniform_int_1to10(generator) / 5.0;
        vel_x[i] *= 1 + uniform_int_1to10(generator) / 5.0;
      }
      if(y[i] > height - border && vel_y[i] > 0) {
        vel_y[i] *= -uniform_int_1to10(generator) / 5.0;
        vel_x[i] *= 1 + uniform_int_1to10(generator) / 5.0;
      }

      if(abs(vel_x[i]) > max_velocity || abs(vel_y[i]) > max_velocity) {
        const double scale_factor = max_velocity / std::max(abs(vel_x[i]), abs(vel_y[i]));
        vel_x[i] *= scale_factor;
        vel_y[i] *= scale_factor;
      }

      x[i] += vel_x[i];
      y[i] += vel_y[i];
    }
  }

  std::vector<int> get_close_boids(const unsigned short index) {
    std::vector<int> close_boids_indexes;
    for(auto i = 0; i < num_boids; i++) {
      if(i == index) continue;

      double distance = distance_to(index, i);
      if(distance < close_distance) {
        close_boids_indexes.push_back(i);
      }
    }
    return close_boids_indexes;
  }

  double distance_to(const unsigned short index_a, const unsigned short index_b) {
    double dist_x = x[index_a] - x[index_b];
    double dist_y = y[index_a] - y[index_b];
    dist_x = pow(dist_x, 2);
    dist_y = pow(dist_y, 2);
    return sqrt(dist_x + dist_y);
  }
};

#endif