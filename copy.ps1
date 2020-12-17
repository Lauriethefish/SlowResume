& $PSScriptRoot/build.ps1
if ($?) {
    adb push libs/arm64-v8a/libslow-resume.so /sdcard/Android/data/com.beatgames.beatsaber/files/mods/libslow-resume.so
    if ($?) {
        adb shell am force-stop com.beatgames.beatsaber
        adb shell am start com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity
        if ($args[0] -eq "--log") {
            $timestamp = Get-Date -Format "MM-dd HH:mm:ss.fff"
            adb logcat -T "$timestamp" main-modloader:W QuestHook[slow-resume`|v0.1.0]:* AndroidRuntime:E *:S
        }
    }
}
