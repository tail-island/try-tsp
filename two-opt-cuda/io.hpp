#include <istream>
#include <tuple>
#include <vector>

namespace two_opt {

template <typename T>
inline auto read_data(std::istream &stream) noexcept {
  auto result = T{};

  stream >> result;

  return result;
}

inline auto read_cities(std::istream &stream) noexcept {
  auto result = std::vector<std::tuple<float, float>>{};

  for (;;) {
    const auto x = read_data<float>(stream);

    if (stream.fail()) {
      break;
    }

    const auto y = read_data<float>(stream);

    result.emplace_back(x, y);
  }

  return result;
}

} // namespace two_opt
