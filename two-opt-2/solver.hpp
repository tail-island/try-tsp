#include <algorithm>
#include <limits>
#include <numeric>
#include <ranges>
#include <tuple>
#include <vector>

namespace two_opt {

inline auto greedy(const std::vector<std::vector<float>> &distances) noexcept {
  const auto n = static_cast<int>(std::size(distances));

  auto result = std::vector<int>{};

  std::ranges::copy(std::views::iota(0, n),
                    std::back_inserter(result));

  for (const auto &i : std::views::iota(1, n - 1)) {
    std::iter_swap(std::begin(result) + i,
                   std::min_element(std::begin(result) + i, std::end(result),
                                    [&](const auto &index_1, const auto &index_2) {
                                      return distances[result[i - 1]][index_1] < distances[result[i - 1]][index_2];
                                    }));
  }

  return result;
}

inline auto two_opt(const std::vector<std::vector<float>> &distances) noexcept {
  const auto n = static_cast<int>(std::size(distances));

  auto result = greedy(distances);

  const auto indices = [&] {
    auto result = std::vector<std::tuple<int, int>>{};

    for (const auto &i : std::views::iota(0, n - 1)) {
      for (const auto &j : std::views::iota(i + 1, n)) {
        result.emplace_back(i, j);
      }
    }

    return result;
  }();

  for (;;) {
    const auto delta = std::transform_reduce(
        std::begin(indices), std::end(indices),
        std::make_tuple(std::numeric_limits<float>::max(), 0, 0),
        [](const auto &acc, const auto &delta) {
          return std::get<0>(delta) < std::get<0>(acc) ? delta : acc;
        },
        [&](const auto &index) {
          const auto &[i, j] = index;

          return std::make_tuple(0.0f - distances[result[i]][result[(i + 1) % n]] - distances[result[j]][result[(j + 1) % n]] + distances[result[i]][result[j]] + distances[result[(i + 1) % n]][result[(j + 1) % n]], i, j);
        });

    if (std::get<0>(delta) >= 0.0f - std::numeric_limits<float>::epsilon()) {
      break;
    }

    std::reverse(std::begin(result) + std::get<1>(delta) + 1, std::begin(result) + std::get<2>(delta) + 1);
  }

  return result;
}

} // namespace two_opt
