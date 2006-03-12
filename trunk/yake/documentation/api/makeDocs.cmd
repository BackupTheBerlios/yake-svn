@echo off
echo Building YAKE API Reference...
REM echo NOTE: You need to have doxygen.exe in your path!
echo NOTE: You need to have doxys[.exe] in your path!
pushd ..\..
doxys documentation\api\DoxySfile
popd
