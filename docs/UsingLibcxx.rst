============
Using libc++
============

.. contents::
  :local:

Getting Started
===============

If you already have libc++ installed you can use it with clang.

.. code-block:: bash

    $ clang++ -stdlib=libc++ test.cpp
    $ clang++ -std=c++11 -stdlib=libc++ test.cpp

On OS X and FreeBSD libc++ is the default standard library
and the ``-stdlib=libc++`` is not required.

.. _alternate libcxx:

If you want to select an alternate installation of libc++ you
can use the following options.

.. code-block:: bash

  $ clang++ -std=c++11 -stdlib=libc++ -nostdinc++ \
            -I<libcxx-install-prefix>/include/c++/v1 \
            -L<libcxx-install-prefix>/lib \
            -Wl,-rpath,<libcxx-install-prefix>/lib \
            test.cpp

The option ``-Wl,-rpath,<libcxx-install-prefix>/lib`` adds a runtime library
search path. Meaning that the systems dynamic linker will look for libc++ in
``<libcxx-install-prefix>/lib`` whenever the program is run. Alternatively the
environment variable ``LD_LIBRARY_PATH`` (``DYLD_LIBRARY_PATH`` on OS X) can
be used to change the dynamic linkers search paths after a program is compiled.

An example of using ``LD_LIBRARY_PATH``:

.. code-block:: bash

  $ clang++ -stdlib=libc++ -nostdinc++ \
            -I<libcxx-install-prefix>/include/c++/v1
            -L<libcxx-install-prefix>/lib \
            test.cpp -o
  $ ./a.out # Searches for libc++ in the systems library paths.
  $ export LD_LIBRARY_PATH=<libcxx-install-prefix>/lib
  $ ./a.out # Searches for libc++ along LD_LIBRARY_PATH

Using libc++experimental and ``<experimental/...>``
=====================================================

Libc++ provides implementations of experimental technical specifications
in a separate library, ``libc++experimental.a``. Users of ``<experimental/...>``
headers may be required to link ``-lc++experimental``.

.. code-block:: bash

  $ clang++ -std=c++14 -stdlib=libc++ test.cpp -lc++experimental

Libc++experimental.a may not always be available, even when libc++ is already
installed. For information on building libc++experimental from source see
:ref:`Building Libc++ <build instructions>` and
:ref:`libc++experimental CMake Options <libc++experimental options>`.

Also see the `Experimental Library Implementation Status <http://libcxx.llvm.org/ts1z_status.html>`__
page.

.. warning::
  Experimental libraries are Experimental.
    * The contents of the ``<experimental/...>`` headers and ``libc++experimental.a``
      library will not remain compatible between versions.
    * No guarantees of API or ABI stability are provided.

Using libc++ on Linux
=====================

On Linux libc++ can typically be used with only '-stdlib=libc++'. However
some libc++ installations require the user manually link libc++abi themselves.
If you are running into linker errors when using libc++ try adding '-lc++abi'
to the link line.  For example:

.. code-block:: bash

  $ clang++ -stdlib=libc++ test.cpp -lc++ -lc++abi -lm -lc -lgcc_s -lgcc

Alternately, you could just add libc++abi to your libraries list, which in
most situations will give the same result:

.. code-block:: bash

  $ clang++ -stdlib=libc++ test.cpp -lc++abi


Using libc++ with GCC
---------------------

GCC does not provide a way to switch from libstdc++ to libc++. You must manually
configure the compile and link commands.

In particular you must tell GCC to remove the libstdc++ include directories
using ``-nostdinc++`` and to not link libstdc++.so using ``-nodefaultlibs``.

Note that ``-nodefaultlibs`` removes all of the standard system libraries and
not just libstdc++ so they must be manually linked. For example:

.. code-block:: bash

  $ g++ -nostdinc++ -I<libcxx-install-prefix>/include/c++/v1 \
         test.cpp -nodefaultlibs -lc++ -lc++abi -lm -lc -lgcc_s -lgcc


GDB Pretty printers for libc++
------------------------------

GDB does not support pretty-printing of libc++ symbols by default. Unfortunately
libc++ does not provide pretty-printers itself. However there are 3rd
party implementations available and although they are not officially
supported by libc++ they may be useful to users.

Known 3rd Party Implementations Include:

* `Koutheir's libc++ pretty-printers <https://github.com/koutheir/libcxx-pretty-printers>`_.


Libc++ Configuration Macros
===========================

Libc++ provides a number of configuration macros which can be used to enable
or disable extended libc++ behavior, including enabling "debug mode" or
thread safety annotations.

**_LIBCPP_DEBUG**:
  See :ref:`using-debug-mode` for more information.

**_LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS**:
  This macro is used to enable -Wthread-safety annotations on libc++'s
  ``std::mutex`` and ``std::lock_guard``. By default these annotations are
  disabled and must be manually enabled by the user.

**_LIBCPP_DISABLE_VISIBILITY_ANNOTATIONS**:
  This macro is used to disable all visibility annotations inside libc++.
  Defining this macro and then building libc++ with hidden visibility gives a
  build of libc++ which does not export any symbols, which can be useful when
  building statically for inclusion into another library.

**_LIBCPP_DISABLE_EXTERN_TEMPLATE**:
  This macro is used to disable extern template declarations in the libc++
  headers. The intended use case is for clients who wish to use the libc++
  headers without taking a dependency on the libc++ library itself.

**_LIBCPP_ENABLE_TUPLE_IMPLICIT_REDUCED_ARITY_EXTENSION**:
  This macro is used to re-enable an extension in `std::tuple` which allowed
  it to be implicitly constructed from fewer initializers than contained
  elements. Elements without an initializer are default constructed. For example:

  .. code-block:: cpp

    std::tuple<std::string, int, std::error_code> foo() {
      return {"hello world", 42}; // default constructs error_code
    }


  Since libc++ 4.0 this extension has been disabled by default. This macro
  may be defined to re-enable it in order to support existing code that depends
  on the extension. New use of this extension should be discouraged.
  See `PR 27374 <http://llvm.org/PR27374>`_ for more information.

  Note: The "reduced-arity-initialization" extension is still offered but only
  for explicit conversions. Example:

  .. code-block:: cpp

    auto foo() {
      using Tup = std::tuple<std::string, int, std::error_code>;
      return Tup{"hello world", 42}; // explicit constructor called. OK.
    }

**_LIBCPP_DISABLE_ADDITIONAL_DIAGNOSTICS**:
  This macro disables the additional diagnostics generated by libc++ using the
  `diagnose_if` attribute. These additional diagnostics include checks for:

    * Giving `set`, `map`, `multiset`, `multimap` a comparator which is not
      const callable.

**_LIBCPP_NO_VCRUNTIME**:
  Microsoft's C and C++ headers are fairly entangled, and some of their C++
  headers are fairly hard to avoid. In particular, `vcruntime_new.h` gets pulled
  in from a lot of other headers and provides definitions which clash with
  libc++ headers, such as `nothrow_t` (note that `nothrow_t` is a struct, so
  there's no way for libc++ to provide a compatible definition, since you can't
  have multiple definitions).

  By default, libc++ solves this problem by deferring to Microsoft's vcruntime
  headers where needed. However, it may be undesirable to depend on vcruntime
  headers, since they may not always be available in cross-compilation setups,
  or they may clash with other headers. The `_LIBCPP_NO_VCRUNTIME` macro
  prevents libc++ from depending on vcruntime headers. Consequently, it also
  prevents libc++ headers from being interoperable with vcruntime headers (from
  the aforementioned clashes), so users of this macro are promising to not
  attempt to combine libc++ headers with the problematic vcruntime headers. This
  macro also currently prevents certain `operator new`/`operator delete`
  replacement scenarios from working, e.g. replacing `operator new` and
  expecting a non-replaced `operator new[]` to call the replaced `operator new`.

**_LIBCPP_ENABLE_FILESYSTEM_DIRECTORY_ENTRY_STRICT_ERROR_REPORTING**:
  The current specification for ``filesystem::directory_entry`` requires the
  constructors, ``assign``, ``replace_filename``, and ``refresh`` to throw
  an exception when the entity the directory entry refers to doesn't exist,
  and when no ``error_code`` is provided. This causes the functions to quite
  eagerly throw exceptions in arguably common use cases, which in turn
  may surprise users and cause programs to terminate unexpectedly with uncaught
  exceptions. In the example below, each function is incorrect, and will result
  in an uncaught exception:

  .. code-block:: cpp

    void print_filesystem_entry_info(path p) {
      directory_entry ent(p); // THROWS!
      if (!ent.exists()) {
        cout << "Not Found: " << ent << "\n";
      } else {
        if (ent.is_directory()) {
          cout << "Directory: " << ent << "\n";
        } if (ent.is_regular_file()) {
          cout << "Regular File: " << ent << "\n";
          cout << "  Size: " << ent.file_size() << "\n";
        } else {
          cout << "Other File: " << ent << "\n";
        }
      }
    }

    void canonicalize_extensions_in_dir(path dir) {
      for (auto ent : directory_iterator(dir)) {
        if (ent.is_directory())
          continue;
        const path old_name = ent;
        path new_name = old_name.filename();
        new_name.replace_extension(ToUpperCase(new_name.extension()));
        ent.replace_filename(new_name); // BOOM
        if (ent.exists())
          return report_error("cannot canonicalize extension");
        copy_file(old_name, ent);
      }
    }

    void copy_files_in_directory(path dir, path new_dir) {
      for (auto ent : directory_iterator(dir)) {
        if (ent.is_directory())
          continue;
        const path old_name = ent;
        ent.assign(new_dir / old_name.filename()); // BOOM
        if (!ent.exists())
          copy_file(old_name, ent);
      }
    }

  By default, libc++ does not treat cases when the entry doesn't exist as
  a "hard" error, and instead allows users to determine this by calling the
  ``directory_entry::exists()`` function, which was seemingly provided for
  this purpose. The goal is to make `directory_entry` safer to use, especially
  in contexts where exceptions are disabled.

  Users wanting strictly conforming error reporting may enable it by defining
  the ``_LIBCPP_ENABLE_FILESYSTEM_DIRECTORY_ENTRY_STRICT_ERROR_REPORTING``
  macro.

C++17 Specific Configuration Macros
-----------------------------------
**_LIBCPP_ENABLE_CXX17_REMOVED_FEATURES**:
  This macro is used to re-enable all the features removed in C++17. The effect
  is equivalent to manually defining each macro listed below.

**_LIBCPP_ENABLE_CXX17_REMOVED_UNEXPECTED_FUNCTIONS**:
  This macro is used to re-enable the `set_unexpected`, `get_unexpected`, and
  `unexpected` functions, which were removed in C++17.

**_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR**:
  This macro is used to re-enable `std::auto_ptr` in C++17.
