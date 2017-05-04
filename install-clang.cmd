@echo on
if "%CLANG_VERSION%"=="ToT" (
    if NOT EXIST install-llvm (
      appveyor DownloadFile http://efcs.ca/downloads/llvm-tot-win32.zip -FileName llvm-package.zip
      7z x llvm-package.zip -oC:\projects\deps
    )
    dir .
    dir install-llvm
    move "C:\Program Files\LLVM" "C:\Program Files\LLVM_BAK"
    move "C:\projects\deps\install-llvm" "C:\Program Files\LLVM"
    dir "C:\Program Files\LLVM"
)
