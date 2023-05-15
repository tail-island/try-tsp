#include <algorithm>
#include <limits>
#include <ranges>
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

  for (;;) {
    auto improved = false;

    for (const auto &i : std::views::iota(0, n - 1)) {
      for (const auto &j : std::views::iota(i + 1, n)) {
        if (0.0f - distances[result[i]][result[(i + 1) % n]] - distances[result[j]][result[(j + 1) % n]] + distances[result[i]][result[j]] + distances[result[(i + 1) % n]][result[(j + 1) % n]] >= 0.0f - std::numeric_limits<float>::epsilon()) {
          continue;
        }

        std::reverse(std::begin(result) + i + 1, std::begin(result) + j + 1);
        improved = true;
      }
    }

    if (!improved) {
      break;
    }
  }

  return result;
}

} // namespace two_opt
