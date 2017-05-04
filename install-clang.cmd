@echo on
if "%CLANG_VERSION%"=="ToT" (
    if NOT EXIST install-llvm (
      appveyor DownloadFile http://efcs.ca/downloads/llvm-tot-win32.zip -FileName llvm-package.zip
      7z x llvm-package.zip -oC:\projects\deps
    )
    move "C:\Program Files\LLVM" "C:\Program Files\LLVM_BAK"
    mklink /D "C:\Program Files\LLVM" "C:\projects\deps\install-llvm"
    set LLVM_PATH="C:\projects\deps\install-llvm"
    dir "C:\Program Files\LLVM"
)
