@echo off

cd pcre
devenv pcre.vcproj /clean "Release"
if errorlevel 1 goto error
cd ..

cd library
devenv usemarcon_library.vcproj /clean "Release"
if errorlevel 1 goto error
cd ..

cd program
devenv usemarcon.vcproj /clean "Release"
if errorlevel 1 goto error
cd ..

goto end

:error
echo Clean failed!

:end