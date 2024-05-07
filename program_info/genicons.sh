#/bin/bash

# ICO

inkscape -w 16 -h 16 -o enderforgemc_16.png org.enderforgemc.EnderForgeMC.svg
inkscape -w 24 -h 24 -o enderforgemc_24.png org.enderforgemc.EnderForgeMC.svg
inkscape -w 32 -h 32 -o enderforgemc_32.png org.enderforgemc.EnderForgeMC.svg
inkscape -w 48 -h 48 -o enderforgemc_48.png org.enderforgemc.EnderForgeMC.svg
inkscape -w 64 -h 64 -o enderforgemc_64.png org.enderforgemc.EnderForgeMC.svg
inkscape -w 128 -h 128 -o enderforgemc_128.png org.enderforgemc.EnderForgeMC.svg

convert enderforgemc_128.png enderforgemc_64.png enderforgemc_48.png enderforgemc_32.png enderforgemc_24.png enderforgemc_16.png enderforgemc.ico

rm -f enderforgemc_*.png

inkscape -w 1024 -h 1024 -o enderforgemc_1024.png org.enderforgemc.EnderForgeMC.bigsur.svg

mkdir enderforgemc.iconset

sips -z 16 16     enderforgemc_1024.png --out enderforgemc.iconset/icon_16x16.png
sips -z 32 32     enderforgemc_1024.png --out enderforgemc.iconset/icon_16x16@2x.png
sips -z 32 32     enderforgemc_1024.png --out enderforgemc.iconset/icon_32x32.png
sips -z 64 64     enderforgemc_1024.png --out enderforgemc.iconset/icon_32x32@2x.png
sips -z 128 128   enderforgemc_1024.png --out enderforgemc.iconset/icon_128x128.png
sips -z 256 256   enderforgemc_1024.png --out enderforgemc.iconset/icon_128x128@2x.png
sips -z 256 256   enderforgemc_1024.png --out enderforgemc.iconset/icon_256x256.png
sips -z 512 512   enderforgemc_1024.png --out enderforgemc.iconset/icon_256x256@2x.png
sips -z 512 512   enderforgemc_1024.png --out enderforgemc.iconset/icon_512x512.png
cp enderforgemc_1024.png enderforgemc.iconset/icon_512x512@2x.png

iconutil -c icns enderforgemc.iconset

rm -f enderforgemc_*.png
rm -rf enderforgemc.iconset

for dir in ../launcher/resources/*/scalable
do
    cp -v org.enderforgemc.EnderForgeMC.svg $dir/launcher.svg
done
