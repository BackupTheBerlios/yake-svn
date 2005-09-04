@echo off
echo Building YAKE API Reference...
echo NOTE: You need to have doxygen.exe in your path!
cd ..\..
doxygen documentation\api\api.cfg
