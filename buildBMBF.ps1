# Builds a .zip file for loading with BMBF
& $PSScriptRoot/build.ps1

if ($?) {
    Compress-Archive -Path "./libs/arm64-v8a/libslow-resume.so", "cover.png", "./bmbfmod.json" -DestinationPath "./slow-resume_v0.1.1.zip" -Update
}
