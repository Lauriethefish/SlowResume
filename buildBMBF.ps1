# Builds a .zip file for loading with BMBF
& $PSScriptRoot/build.ps1

if ($?) {
    Compress-Archive -Path "./libs/arm64-v8a/libslow-resume.so", "./libs/arm64-v8a/libquestui.so", "./libs/arm64-v8a/libcustom-types.so", "./libs/arm64-v8a/libbs-utils.so", "./bmbfmod.json" -DestinationPath "./slow-resume_v0.1.0.zip" -Update
}
