#include <tuple>
#include <type_traits>

// This class is implicitly convertible to anything and explicitly convertible
// to nothing.
//
//   static_assert(std::is_convertible<ErrorFactory<B>, int>(), "");
//   static_assert(!std::is_constructible<int, ErrorFactory<B>>(), "");
//
// The second condition is relevant when converting to optional<T>. Without it
// the implicit conversion would be ambiguous.
class ErrorFactory {
 public:
  explicit ErrorFactory(decltype(nullptr));

  template <class R>
  operator R() && {
    return std::declval<R>();
  }

  // This overload makes explicit conversion to R ambiguous, which in turn makes
  // std::is_constructible<R, ErrorFactory>() false.
  template <class R, class = void>
  explicit operator R() && {
    return *this;
  }

 private:
  // These constructors are private to avoid returning ErrorFactory accidentally
  // from functions with `auto` result type.
  //
  //   auto F = [] {
  //     using gerror::GErrorDomain;
  //     // Compile error: calling a private constructor of class ErrorFactory.
  //     GVERIFY(2 + 2 == 4);
  //     ...
  //   };
  ErrorFactory(const ErrorFactory&) = default;
  ErrorFactory(ErrorFactory&&) = default;


};

void Function2() {
  using ReturnType = std::tuple<int, int>;
  static_assert(std::is_convertible<ErrorFactory, ReturnType>(), "");
  static_assert(!std::is_constructible<ReturnType, ErrorFactory>(), "");  // fails libc++
}

int main() {

}
