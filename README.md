# jniDegger

## intro
通过dlopen打开目标elf文件、调用JNI_OnLoad 模拟System.loadLibrary加载so文件. 
构造fake_jvm 和 fake_jnienv，输出JNI_Onload阶段执行的jni函数.

和unidbg的差异？
* 思想是一致的，unidbg通过unicorn 执行指令，也是传入构造函数指针. 差异在一个真实android执行环境、一个是模拟CPU环境。换句话说，假设目标elf调用jni函数，我们可能获取到真实的返回结果,当然这个结果可能不是正确的.

所以如果不需要获取jni函数的返回结果，可以使用unidbg，如果需要获取jni函数的返回结果，可以使用jniDegger.

1. 打印RegisterNatives中JNINativeMethod和jni调用java
```agsl
// 日志
D  path : libmyapplication.so
D  hook_GetEnv
D  hook_RegisterNatives com/example/myapplication/MainActivity.stringFromJNI ()Ljava/lang/String; 0xd44ba839
D  hook_GetMethodID android/content/Context.getPackageName()Ljava/lang/String;
D  hook_GetStaticMethodID android/app/ActivityThread.static currentActivityThread()Landroid/app/ActivityThread;
D  hook_CallStaticObjectMethodV android/app/ActivityThread static currentActivityThread()Landroid/app/ActivityThread;
D  hook_GetMethodID android/app/ActivityThread.getApplication()Landroid/app/Application;
D  hook_CallObjectMethodV android/app/ActivityThread getApplication()Landroid/app/Application;
D  hook_CallObjectMethodV android/app/ActivityThread getPackageName()Ljava/lang/String;

```

## todo 
1. 未补充的jnienv函数会直接报错，需要补充。
2. 如果需要打印系统调用可以参考 https://github.com/besnardjb/ptperf/blob/master/tracee.c#L193
3. 提供ptrace方式的svc trace.
4. 当findclass查找的类和后续调用该类的方法都是android环境存在的类时 ，返回执行的结果。
