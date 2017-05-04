@echo on
if "%CLANG_VERSION%"=="ToT" (
    set CLANG_URL="http://llvm.org/pre-releases/win-snapshots/LLVM-5.0.0-r301646-win32.exe"
    appveyor DownloadFile http://efcs.ca/downloads/llvm-tot-win32.zip -FileName llvm-package.zip
    move "C:\\Program Files\\LLVM" "C:\\Program Files\\LLVM_BAK"
    7z x llvm-package.zip -o"C:\\Program Files\\LLVM"
)
