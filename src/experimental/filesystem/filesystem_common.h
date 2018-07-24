//===----------------------------------------------------------------------===////
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===////

#ifndef FILESYSTEM_COMMON_H
#define FILESYSTEM_COMMON_H

#include "experimental/__config"
#include "array"
#include "chrono"
#include "cstdlib"
#include "climits"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <fcntl.h> /* values for fchmodat */

#include <experimental/filesystem>

#if (__APPLE__)
#if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 101300
#define _LIBCXX_USE_UTIMENSAT
#endif
#elif defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
#if __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 110000
#define _LIBCXX_USE_UTIMENSAT
#endif
#elif defined(__ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__)
#if __ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__ >= 110000
#define _LIBCXX_USE_UTIMENSAT
#endif
#elif defined(__ENVIRONMENT_WATCH_OS_VERSION_MIN_REQUIRED__)
#if __ENVIRONMENT_WATCH_OS_VERSION_MIN_REQUIRED__ >= 40000
#define _LIBCXX_USE_UTIMENSAT
#endif
#endif // __ENVIRONMENT_.*_VERSION_MIN_REQUIRED__
#else
// We can use the presence of UTIME_OMIT to detect platforms that provide
// utimensat.
#if defined(UTIME_OMIT)
#define _LIBCXX_USE_UTIMENSAT
#endif
#endif // __APPLE__

#if !defined(_LIBCXX_USE_UTIMENSAT)
#include <sys/time.h> // for ::utimes as used in __last_write_time
#endif

#if !defined(UTIME_OMIT)
#include <sys/time.h> // for ::utimes as used in __last_write_time
#endif

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

_LIBCPP_BEGIN_NAMESPACE_EXPERIMENTAL_FILESYSTEM

namespace detail {
namespace {

static string format_string_imp(const char* msg, ...) {
  // we might need a second shot at this, so pre-emptivly make a copy
  struct GuardVAList {
    va_list& target;
    bool active = true;
    GuardVAList(va_list &target) : target(target), active(true) {}
    void clear() {
      if (active)
        va_end(target);
      active = false;
    }
    ~GuardVAList() {
      if (active)
        va_end(target);
    }
  };
  va_list args;
  va_start(args, msg);
  GuardVAList args_guard(args);

  va_list args_cp;
  va_copy(args_cp, args);
  GuardVAList args_copy_guard(args_cp);

  array<char, 256> local_buff;
  size_t size = local_buff.size();
  auto ret = ::vsnprintf(local_buff.data(), size, msg, args_cp);

  args_copy_guard.clear();

  // handle empty expansion
  if (ret == 0)
    return string{};
  if (static_cast<size_t>(ret) < size)
    return string(local_buff.data());

  // we did not provide a long enough buffer on our first attempt.
  // add 1 to size to account for null-byte in size cast to prevent overflow
  size = static_cast<size_t>(ret) + 1;
  auto buff_ptr = unique_ptr<char[]>(new char[size]);
  ret = ::vsnprintf(buff_ptr.get(), size, msg, args);
  return string(buff_ptr.get());
}

const char* unwrap(string const& s) { return s.c_str(); }
const char* unwrap(path const& p) { return p.native().c_str(); }
template <class Arg>
Arg const& unwrap(Arg const& a) {
  static_assert(!is_class<Arg>::value, "cannot pass class here");
  return a;
}

template <class... Args>
string format_string(const char* fmt, Args const&... args) {
  return format_string_imp(fmt, unwrap(args)...);
}

error_code capture_errno() {
  _LIBCPP_ASSERT(errno, "Expected errno to be non-zero");
  return error_code(errno, generic_category());
}

template <class T>
T error_value();
template <>
_LIBCPP_CONSTEXPR_AFTER_CXX11 void error_value<void>() {}
template <>
constexpr bool error_value<bool>() {
  return false;
}
template <>
constexpr uintmax_t error_value<uintmax_t>() {
  return uintmax_t(-1);
}
template <>
_LIBCPP_CONSTEXPR_AFTER_CXX11 file_time_type error_value<file_time_type>() {
  return file_time_type::min();
}
template <>
path error_value<path>() {
  return {};
}

template <class T>
struct ErrorHandler {
  const char* func_name;
  error_code* ec = nullptr;
  const path* p1 = nullptr;
  const path* p2 = nullptr;

  ErrorHandler(const char* fname, error_code* ec, const path* p1 = nullptr,
               const path* p2 = nullptr)
      : func_name(fname), ec(ec), p1(p1), p2(p2) {
    if (ec)
      ec->clear();
  }

  T report(const error_code& m_ec) const {
    if (ec) {
      *ec = m_ec;
      return error_value<T>();
    }
    string what = string("in ") + func_name;
    switch (bool(p1) + bool(p2)) {
    case 0:
      __throw_filesystem_error(what, m_ec);
    case 1:
      __throw_filesystem_error(what, *p1, m_ec);
    case 2:
      __throw_filesystem_error(what, *p1, *p2, m_ec);
    }
    _LIBCPP_UNREACHABLE();
  }

  template <class... Args>
  T report(const error_code& m_ec, const char* msg, Args const&... args) const {
    if (ec) {
      *ec = m_ec;
      return error_value<T>();
    }
    string what =
        string("in ") + func_name + ": " + format_string(msg, args...);
    switch (bool(p1) + bool(p2)) {
    case 0:
      __throw_filesystem_error(what, m_ec);
    case 1:
      __throw_filesystem_error(what, *p1, m_ec);
    case 2:
      __throw_filesystem_error(what, *p1, *p2, m_ec);
    }
    _LIBCPP_UNREACHABLE();
  }

  T report(errc const& err) const { return report(make_error_code(err)); }

  template <class... Args>
  T report(errc const& err, const char* msg, Args const&... args) const {
    return report(make_error_code(err), msg, args...);
  }

private:
  ErrorHandler(ErrorHandler const&) = delete;
  ErrorHandler& operator=(ErrorHandler const&) = delete;
};

namespace time_util {

using chrono::duration;
using chrono::duration_cast;

template <class FileTimeT, class TimeT,
          bool IsFloat = is_floating_point<typename FileTimeT::rep>::value>
struct fs_time_util_base {
  using rep = typename FileTimeT::rep;
  using fs_duration = typename FileTimeT::duration;
  using fs_seconds = duration<rep>;
  using fs_nanoseconds = duration<rep, nano>;
  using fs_microseconds = duration<rep, micro>;

  static constexpr rep max_seconds =
      duration_cast<fs_seconds>(FileTimeT::duration::max()).count();

  static constexpr rep max_nsec =
      duration_cast<fs_nanoseconds>(FileTimeT::duration::max() -
                                    fs_seconds(max_seconds))
          .count();

  static constexpr rep min_seconds =
      duration_cast<fs_seconds>(FileTimeT::duration::min()).count();

  static constexpr rep min_nsec_timespec =
      duration_cast<fs_nanoseconds>(
          (FileTimeT::duration::min() - fs_seconds(min_seconds)) +
          fs_seconds(1))
          .count();

private:
  static constexpr fs_duration get_min_nsecs() {
    return duration_cast<fs_duration>(
        fs_nanoseconds(min_nsec_timespec) -
        duration_cast<fs_nanoseconds>(fs_seconds(1)));
  }

  typedef duration<long long, ratio<3600 * 24 * 365> > Years;

  static_assert(max_seconds >= numeric_limits<TimeT>::max() ||
                    duration_cast<Years>(fs_seconds(max_seconds)) > Years(500),
                "");
  static_assert(min_seconds <= numeric_limits<TimeT>::min() ||
                    duration_cast<Years>(fs_seconds(min_seconds)) < Years(-500),
                "");

  // Static assert that these values properly round trip.
  static_assert(fs_seconds(min_seconds) + get_min_nsecs() ==
                    FileTimeT::duration::min(),
                "");
};

template <class FileTimeT, class TimeT>
struct fs_time_util_base<FileTimeT, TimeT, true> {
  using rep = typename FileTimeT::rep;
  using fs_duration = typename FileTimeT::duration;
  using fs_seconds = duration<rep>;
  using fs_nanoseconds = duration<rep, nano>;
  using fs_microseconds = duration<rep, micro>;

  static const rep max_seconds;
  static const rep max_nsec;
  static const rep min_seconds;
  static const rep min_nsec_timespec;
};

template <class FileTimeT, class TimeT>
const typename FileTimeT::rep
    fs_time_util_base<FileTimeT, TimeT, true>::max_seconds =
        duration_cast<fs_seconds>(FileTimeT::duration::max()).count();

template <class FileTimeT, class TimeT>
const typename FileTimeT::rep
    fs_time_util_base<FileTimeT, TimeT, true>::max_nsec =
        duration_cast<fs_nanoseconds>(FileTimeT::duration::max() -
                                      fs_seconds(max_seconds))
            .count();

template <class FileTimeT, class TimeT>
const typename FileTimeT::rep
    fs_time_util_base<FileTimeT, TimeT, true>::min_seconds =
        duration_cast<fs_seconds>(FileTimeT::duration::min()).count();

template <class FileTimeT, class TimeT>
const typename FileTimeT::rep
    fs_time_util_base<FileTimeT, TimeT, true>::min_nsec_timespec =
        duration_cast<fs_nanoseconds>((FileTimeT::duration::min() -
                                       fs_seconds(min_seconds)) +
                                      fs_seconds(1))
            .count();

template <class FileTimeT, class TimeT, class TimeSpecT>
struct fs_time_util : fs_time_util_base<FileTimeT, TimeT> {
  using Base = fs_time_util_base<FileTimeT, TimeT>;
  using Base::max_nsec;
  using Base::max_seconds;
  using Base::min_nsec_timespec;
  using Base::min_seconds;

  using typename Base::fs_duration;
  using typename Base::fs_microseconds;
  using typename Base::fs_nanoseconds;
  using typename Base::fs_seconds;

public:
  template <class CType, class ChronoType>
  static _LIBCPP_CONSTEXPR_AFTER_CXX11 bool checked_set(CType* out,
                                                        ChronoType time) {
    using Lim = numeric_limits<CType>;
    if (time > Lim::max() || time < Lim::min())
      return false;
    *out = static_cast<CType>(time);
    return true;
  }

  static _LIBCPP_CONSTEXPR_AFTER_CXX11 bool is_representable(TimeSpecT tm) {
    if (tm.tv_sec >= 0) {
      return (tm.tv_sec < max_seconds) ||
             (tm.tv_sec == max_seconds && tm.tv_nsec <= max_nsec);
    } else if (tm.tv_sec == (min_seconds - 1)) {
      return tm.tv_nsec >= min_nsec_timespec;
    } else {
      return (tm.tv_sec >= min_seconds);
    }
  }

  static _LIBCPP_CONSTEXPR_AFTER_CXX11 bool is_representable(FileTimeT tm) {
    auto secs = duration_cast<fs_seconds>(tm.time_since_epoch());
    auto nsecs = duration_cast<fs_nanoseconds>(tm.time_since_epoch() - secs);
    if (nsecs.count() < 0) {
      secs = secs + fs_seconds(1);
      nsecs = nsecs + fs_seconds(1);
    }
    using TLim = numeric_limits<TimeT>;
    if (secs.count() >= 0)
      return secs.count() <= TLim::max();
    return secs.count() >= TLim::min();
  }

  static _LIBCPP_CONSTEXPR_AFTER_CXX11 FileTimeT
  convert_from_timespec(TimeSpecT tm) {
    if (tm.tv_sec >= 0 || tm.tv_nsec == 0) {
      return FileTimeT(fs_seconds(tm.tv_sec) +
                       duration_cast<fs_duration>(fs_nanoseconds(tm.tv_nsec)));
    } else if (tm.tv_nsec == 0) {
      return FileTimeT(fs_seconds(tm.tv_sec));
    } else { // tm.tv_sec < 0
      auto adj_subsec = duration_cast<fs_duration>(fs_seconds(1) -
                                                   fs_nanoseconds(tm.tv_nsec));
      auto Dur = fs_seconds(tm.tv_sec + 1) - adj_subsec;
      return FileTimeT(Dur);
    }
  }

  template <class SubSecDurT, class SubSecT>
  static _LIBCPP_CONSTEXPR_AFTER_CXX11 bool
  set_times_checked(TimeT* sec_out, SubSecT* subsec_out, FileTimeT tp) {
    auto dur = tp.time_since_epoch();
    auto sec_dur = duration_cast<fs_seconds>(dur);
    auto subsec_dur = duration_cast<SubSecDurT>(dur - sec_dur);
    // The tv_nsec and tv_usec fields must not be negative so adjust accordingly
    if (subsec_dur.count() < 0) {
      if (sec_dur.count() > min_seconds) {
        sec_dur -= fs_seconds(1);
        subsec_dur += fs_seconds(1);
      } else {
        subsec_dur = SubSecDurT::zero();
      }
    }
    return checked_set(sec_out, sec_dur.count()) &&
           checked_set(subsec_out, subsec_dur.count());
  }
  static _LIBCPP_CONSTEXPR_AFTER_CXX11 bool convert_to_timespec(TimeSpecT& dest,
                                                                FileTimeT tp) {
    if (!is_representable(tp))
      return false;
    return set_times_checked<fs_nanoseconds>(&dest.tv_sec, &dest.tv_nsec, tp);
  }
};

} // namespace time_util


using TimeSpec = struct ::timespec;
using StatT = struct ::stat;

using FSTime = time_util::fs_time_util<file_time_type, time_t, struct timespec>;

#if defined(__APPLE__)
TimeSpec extract_mtime(StatT const& st) { return st.st_mtimespec; }
TimeSpec extract_atime(StatT const& st) { return st.st_atimespec; }
#else
TimeSpec extract_mtime(StatT const& st) { return st.st_mtim; }
TimeSpec extract_atime(StatT const& st) { return st.st_atim; }
#endif

#if !defined(_LIBCXX_USE_UTIMENSAT)
using TimeStruct = struct ::timeval;
using TimeStructArray = TimeStruct[2];
#else
using TimeStruct = TimeSpec;
using TimeStructArray = TimeStruct[2];
#endif

bool SetFileTimes(const path& p, TimeStructArray const& TS,
                  error_code& ec) {
#if !defined(_LIBCXX_USE_UTIMENSAT)
  if (::utimes(p.c_str(), TS) == -1)
#else
  if (::utimensat(AT_FDCWD, p.c_str(), TS, 0) == -1)
#endif
  {
    ec = capture_errno();
    return true;
  }
  return false;
}

void SetTimeStructTo(TimeStruct& TS, TimeSpec ToTS) {
  using namespace chrono;
  using namespace time_util;
  TS.tv_sec = ToTS.tv_sec;
#if !defined(_LIBCXX_USE_UTIMENSAT)
  TS.tv_usec = duration_cast<typename FSTime::fs_microseconds>(
                   typename FSTime::fs_nanoseconds(ToTS.tv_nsec))
                   .count();
#else
  TS.tv_nsec = ToTS.tv_nsec;
#endif
}

bool SetTimeStructTo(TimeStruct& TS, file_time_type NewTime) {
  using namespace chrono;
  using namespace time_util;
#if !defined(_LIBCXX_USE_UTIMENSAT)
  return !FSTime::set_times_checked<typename FSTime::fs_microseconds>(
      &TS.tv_sec, &TS.tv_usec, NewTime);
#else
  return !FSTime::set_times_checked<typename FSTime::fs_nanoseconds>(
      &TS.tv_sec, &TS.tv_nsec, NewTime);
#endif
}

} // namespace
} // end namespace detail

_LIBCPP_END_NAMESPACE_EXPERIMENTAL_FILESYSTEM



#endif // FILESYSTEM_COMMON_H
