#include <jni.h>
#include <string>
#include <sys/stat.h>
#include <android/log.h>
#include <ThreadPool.h>
#include <unistd.h>
#include <fstream>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>
#include <sys/types.h>
#include <ifaddrs.h>

#define LOG_TAG "LitEye.Native"
extern "C" long raw_syscall();

#if !defined(NDEBUG)
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)



#else
#define ALOGV(...)
    #define ALOGD(...)
    #define ALOGI(...)
    #define ALOGW(...)
    #define ALOGE(...)
#endif

struct hook_JavaVM {
    struct JNIInvokeInterface* functions;

#if defined(__cplusplus)
    jint DestroyJavaVM()
    { return functions->DestroyJavaVM(reinterpret_cast<JavaVM *>(this)); }
    jint AttachCurrentThread(JNIEnv** p_env, void* thr_args)
    { return functions->AttachCurrentThread(reinterpret_cast<JavaVM *>(this), p_env, thr_args); }
    jint DetachCurrentThread()
    { return functions->DetachCurrentThread(reinterpret_cast<JavaVM *>(this)); }
    jint GetEnv(void** env, jint version)
    { return functions->GetEnv(reinterpret_cast<JavaVM *>(this), env, version); }
    jint AttachCurrentThreadAsDaemon(JNIEnv** p_env, void* thr_args)
    { return functions->AttachCurrentThreadAsDaemon(reinterpret_cast<JavaVM *>(this), p_env, thr_args); }
#endif /*__cplusplus*/
};

JNIEnv  * gEnv;


jint hook_GetEnv(JavaVM *  , void ** env,  jint){
    ALOGD("GetEnv");
    *env = gEnv;
    return 0;
}

jint hook_DestoryJavaVM(JavaVM *){
    ALOGD("DestoryJavaVM");
    return 0;
}

jint hook_AttachCurrentThread(JavaVM *  , JNIEnv ** ,  void*){
    ALOGD("AttachCurrentThread");
    return 0;
}

jint hook_DetachCurrentThread(JavaVM * ){
    ALOGD("DetachCurrentThread");
    return 0;
}

jint hook_AttachCurrentThreadAsDaemon(JavaVM *  , JNIEnv ** ,  void *){
    ALOGD("AttachCurrentThreadAsDaemon");
    return 0;
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_stringFromJNI2(JNIEnv *env, jobject thiz) {
    // TODO: implement stringFromJNI2()
    const char * hello = "Hello from C++";
    //新建jvm接口 , 并且伪造JavaVM提供的函数
//    JavaVM * jvm;
    hook_JavaVM  jvm;
    jvm.functions->GetEnv = hook_GetEnv;
    jvm.functions->DestroyJavaVM = hook_DestoryJavaVM;
    jvm.functions->DetachCurrentThread = hook_DetachCurrentThread;
    jvm.functions->AttachCurrentThread = hook_AttachCurrentThread;
    jvm.functions->AttachCurrentThreadAsDaemon = hook_AttachCurrentThreadAsDaemon;
//     赋值全局jenv
    gEnv = env;

    JNIEnv * fade;
    jvm.GetEnv(reinterpret_cast<void **>(&fade), 0x10004);
    fade->NewStringUTF("hello World");

    void * handle  =  dlopen("lib.so" , RTLD_NOW);
//    void * handle  =  dlopen("libliteye.so" , RTLD_NOW);

    jint (*func)(JavaVM *, void *pVoid);
    func = (jint (*)(JavaVM* , void *))dlsym(handle, "JNI_OnLoad");

//    int ret = func(jvm , NULL);
    // 伪造为hook_JavaVM时用
    int ret = func((JavaVM *)&jvm , NULL);

    ALOGD("JNI_OnLoad %d " , ret);

    return env->NewStringUTF(hello);

}

