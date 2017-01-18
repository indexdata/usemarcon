@echo off

cd pcre
devenv pcre-7.10.vcproj /clean "Release"
if errorlevel 1 goto error
cd ..

cd library
devenv usemarcon_library-7.10.vcproj /clean "Release"
if errorlevel 1 goto error
cd ..

cd program
devenv usemarcon-7.10.vcproj /clean "Release"
if errorlevel 1 goto error
cd ..

goto end

:error
echo Clean failed!

:end