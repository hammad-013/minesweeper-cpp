#!/bin/bash
# Check this guide https://thescienceofcode.com/raylib-appimage/

wget -O appimage-builder-x86_64.AppImage https://github.com/AppImageCrafters/appimage-builder/releases/download/v1.1.0/appimage-builder-1.1.0-x86_64.AppImage
chmod +x appimage-builder-x86_64.AppImage

# install
sudo mv appimage-builder-x86_64.AppImage /usr/local/bin/appimage-builder

cd build/linux

# Create directory structure
mkdir -p AppDir/lib
# Copy executable
cp main AppDir/
# Run generator
appimage-builder --generate 
# Copy missing raylib shared libraries
cp /usr/local/lib64/libraylib.so.5.5.0 AppDir/lib/
mv AppDir/lib/libraylib.so.5.5.0 AppDir/lib/libraylib.so.550
# Build AppImage
appimage-builder --recipe AppImageBuilder.yml