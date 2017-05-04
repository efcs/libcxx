@echo on
if NOT EXIST C:\projects\deps (
  mkdir C:\projects\deps
)
cd C:\projects\deps

::###########################################################################
:: Setup the path to Clang-cl
::###########################################################################
if NOT EXIST install-llvm (
    appveyor DownloadFile http://efcs.ca/downloads/llvm-tot-win32.zip -FileName llvm-package.zip
    7z x llvm-package.zip -oC:\projects\deps
)
if "%CLANG_VERSION%"=="ToT" (
    move "C:\Program Files\LLVM" "C:\Program Files\LLVM_BAK"
    mklink /D "C:\Program Files\LLVM" "C:\projects\deps\install-llvm"
)
set PATH="C:\Program Files\LLVM\bin";%PATH%
clang-cl -v

::###########################################################################
:: Install a recent CMake
::###########################################################################
if NOT EXIST cmake (
  appveyor DownloadFile https://cmake.org/files/v3.7/cmake-3.7.2-win64-x64.zip -FileName cmake.zip
  7z x cmake.zip -oC:\projects\deps > nul
  move C:\projects\deps\cmake-* C:\projects\deps\cmake
)
set PATH=C:\projects\deps\cmake\bin;%PATH%
cmake --version

::###########################################################################
:: Install Ninja
::###########################################################################
if NOT EXIST ninja (
  appveyor DownloadFile https://github.com/ninja-build/ninja/releases/download/v1.6.0/ninja-win.zip -FileName ninja.zip
  7z x ninja.zip -oC:\projects\deps\ninja > nul
)
set PATH=C:\projects\deps\ninja;%PATH%
ninja --version

::###########################################################################
:: Setup the cached copy of LLVM
::###########################################################################
if NOT EXIST llvm (
  git clone --depth=1 http://llvm.org/git/llvm.git
) ELSE (
  git -C llvm pull --ff-only --rebase=true
)
