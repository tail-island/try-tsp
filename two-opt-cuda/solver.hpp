#include <algorithm>
#include <limits>
#include <ranges>
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/reverse.h>
#include <thrust/transform_reduce.h>
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

class get_delta {
  const int n_;
  const float *distances_;
  const int *route_;

public:
  get_delta(int n, thrust::device_vector<float> &distances, thrust::device_vector<int> &route) noexcept : n_(n), distances_(distances.data().get()), route_(route.data().get()) {
    ;
  }

  __device__ thrust::tuple<float, int, int> operator()(const thrust::tuple<int, int> &index) const noexcept {
    const auto &i = thrust::get<0>(index);
    const auto &j = thrust::get<1>(index);

    const auto delta = 0.0f - distances_[route_[i] * n_ + route_[(i + 1) % n_]] - distances_[route_[j] * n_ + route_[(j + 1) % n_]] + distances_[route_[i] * n_ + route_[j]] + distances_[route_[(i + 1) % n_] * n_ + route_[(j + 1) % n_]];

    return thrust::make_tuple(delta, i, j);
  }
};

class get_min_delta {
public:
  __device__ thrust::tuple<float, int, int> operator()(const thrust::tuple<float, int, int> &acc, const thrust::tuple<float, int, int> &delta) const noexcept {
    return thrust::get<0>(delta) < thrust::get<0>(acc) ? delta : acc;
  }
};

inline auto two_opt(const std::vector<std::vector<float>> &distances) noexcept {
  const auto n = static_cast<int>(std::size(distances));

  auto result_device = thrust::device_vector<int>{greedy(distances)};

  auto indices_device = [&] {
    auto result_host = thrust::host_vector<thrust::tuple<int, int>>{};

    for (const auto &i : std::views::iota(0, n - 1)) {
      for (const auto &j : std::views::iota(i + 1, n)) {
        result_host.push_back(thrust::make_tuple(i, j));
      }
    }

    return thrust::device_vector<thrust::tuple<int, int>>(result_host);
  }();

  auto distances_device = [&] {
    auto result_host = thrust::host_vector<float>{};

    for (const auto &xs : distances) {
      for (const auto &x : xs) {
        result_host.push_back(x);
      }
    }

    return thrust::device_vector<float>{result_host};
  }();

  const auto get_delta = two_opt::get_delta{n, distances_device, result_device};
  const auto get_min_delta = two_opt::get_min_delta{};

  for (;;) {
    const auto delta = thrust::transform_reduce(std::begin(indices_device), std::end(indices_device),
                                                get_delta,
                                                thrust::make_tuple(std::numeric_limits<float>::max(), 0, 0),
                                                get_min_delta);

    if (thrust::get<0>(delta) >= 0.0f - std::numeric_limits<float>::epsilon()) {
      break;
    }

    thrust::reverse(std::begin(result_device) + thrust::get<1>(delta) + 1, std::begin(result_device) + thrust::get<2>(delta) + 1);
  }

  return thrust::host_vector<int>{result_device};
}

} // namespace two_opt
