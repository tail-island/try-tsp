#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <vector>

#include "io.hpp"
#include "solver.hpp"

int main(int argc, char **argv) {
  const auto cities = [&] {
    auto stream = std::ifstream{argv[1]};

    return two_opt::read_cities(stream);
  }();

  const auto distances = [&] {
    auto result = std::vector<std::vector<float>>{};

    std::ranges::copy(cities | std::views::transform([&](const auto &city_1) {
                        auto result = std::vector<float>{};

                        std::ranges::copy(cities | std::views::transform([&](const auto &city_2) {
                                            const auto &[x_1, y_1] = city_1;
                                            const auto &[x_2, y_2] = city_2;

                                            return std::sqrt(std::pow(x_1 - x_2, 2.0f) + std::pow(y_1 - y_2, 2.0f));
                                          }),
                                          std::back_inserter(result));

                        return result;
                      }),
                      std::back_inserter(result));

    return result;
  }();

  const auto starting_time = std::chrono::system_clock::now();
  const auto route = two_opt::two_opt(distances);
  std::cerr << "esapsed time:\t" << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - starting_time) << std::endl;

  const auto distance = [&] {
    auto indices = std::views::iota(0, static_cast<int>(std::size(cities))); // C++23がリリースされたら、std::views::adjacentする。

    return std::accumulate(std::begin(indices), std::end(indices), 0.0f, // で、adjacentする際は、初期値を最後～最初の距離にする。
                           [&](const auto &acc, const auto &i) {
                             return acc + distances[route[i]][route[(i + 1) % static_cast<int>(std::size(cities))]];
                           });
  }();

  std::cerr << "distance:\t" << distance << std::endl;

  for (const auto &index : route) {
    std::cout << index << std::endl;
  }

  return 0;
}
