# jniDegger


1. 打印RegisterNatives中JNINativeMethod
```agsl


// 日志
hook_GetEnv
hook_FindClass com/vmos/core/utils/NativeUtil
hook_RegisterNatives
hook_RegisterNatives killuid (I)I 0xdd537391
hook_RegisterNatives getCrashDump (Ljava/lang/String;)[B 0xdd536ff9
hook_RegisterNatives getLogcatDump (Ljava/lang/String;)V 0xdd537211
hook_RegisterNatives startzygote (Z)I 0xdd5356e9
hook_RegisterNatives startzygote71_32 (Z)I 0xdd5357d1
hook_RegisterNatives startzygote_32_64 (Z)I 0xdd5358b9
hook_RegisterNatives startzygote_d (I)I 0xdd5367c9
hook_RegisterNatives isProcessAlive (I)I 0xdd537739

```

## todo 
1. 未补充的jnienv函数会直接报错，需要补充。
