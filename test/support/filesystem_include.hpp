#ifndef TEST_SUPPORT_FILESYSTEM_INCLUDE_HPP
#define TEST_SUPPORT_FILESYSTEM_INCLUDE_HPP

#ifdef TEST_INCLUDE_EXPERIMENTAL_FILESYSTEM
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

#endif
