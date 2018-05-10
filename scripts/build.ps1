$Root = "$PSScriptRoot/.."

$ErrorActionPreference = "Stop"

Push-Location $Root
if (-Not (Test-Path vcpkg)) {
    git clone https://github.com/Microsoft/vcpkg
    Push-Location vcpkg
    git checkout 90ecc3c44d83be034105c58dd7a6aa3e0cb8d09a
    Pop-Location
    .\vcpkg\bootstrap-vcpkg.bat
}
#.\vcpkg\vcpkg.exe install cairo:x64-windows-static fontconfig:x64-windows-static freetype:x64-windows-static pango:x64-windows-static
.\vcpkg\vcpkg.exe install cairo:x64-windows fontconfig:x64-windows freetype:x64-windows pango:x64-windows

if (-Not (Test-Path build)) {
    New-Item -ItemType Directory build
}
Push-Location build

& cmake.exe -DENABLE_DYNDRIVERS=OFF "-DCMAKE_INSTALL_PREFIX=${Root}/target" "-DCMAKE_TOOLCHAIN_FILE=${Root}\vcpkg\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows -G "Visual Studio 15 2017 Win64" "$Root"

cmake.exe --build . --config Release
cmake.exe --build . --config Release --target INSTALL

Copy-Item -Force "$Root/vcpkg/installed/x64-windows/bin/*.dll" "$Root/target/bin"

Pop-Location
Pop-Location

