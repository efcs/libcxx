@echo on
if "%CLANG_VERSION%"=="ToT" (
    if NOT EXISTS llvm-package.zip appveyor DownloadFile http://efcs.ca/downloads/llvm-tot-win32.zip -FileName llvm-package.zip
    move "C:\\Program Files\\LLVM" "C:\\Program Files\\LLVM_BAK"
    7z x llvm-package.zip -o"C:\\Program Files\\LLVM"
    dir "C:\\Program Files\\LLVM"
)
