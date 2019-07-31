
#include <deque>
#include <iostream>
#include <cassert>

#include "min_allocator.h"
#define DECL_FRIEND(N) template <class ...Args> auto std::__libcpp_test_friend<N>

using Friend = std::__libcpp_test_friend<0>;

template <>
struct std::__libcpp_test_friend<0> {
  template <class Deque>
  static void print(const Deque& d) {
    std::cout << d.size()
                      << " : __front_spare() == " << d.__front_spare()
                      << " : __back_spare() == " << d.__back_spare()
                      << " : __capacity() == " << d.__capacity()
                      << " : bytes allocated == " << malloc_allocator_base::bytes_allocated << '\n';
  }

  static void test_basic();
};

void Friend::test_basic() {
   std::deque<char, malloc_allocator<char> > d(32*1024, 'a');
    bool take_from_front = true;
    while (d.size() > 0)
    {
        if (take_from_front)
        {
            d.pop_front();
            take_from_front = false;
        }
        else
        {
            d.pop_back();
            take_from_front = true;
        }
        if (d.size() % 1000 == 0 || d.size() < 50)
        {
          print(d);
        }
    }
}

int main() {
  Friend::test_basic();
}
