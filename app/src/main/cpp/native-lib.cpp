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

#define LOG_TAG "pareto"
extern "C" long raw_syscall();

#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define FUNC_LOG() ALOGD("%s" , __FUNCTION__ );

#include <hook_jni.h>
#include <map>
#include "dobby.h"
#include <jni.h>

JNIEnv  * oEnv;
using namespace std;
bool HasException = false;

map<string * , string> gClassNameMap;
map<string * , string> gMethodIDMap;
JavaVM * oJvm;
hook_JNIEnv fake_env ;
hook_JavaVM  fake_jvm;


jint hook_GetEnv(JavaVM *  , void ** env,  jint){
    ALOGD("%s" , __FUNCTION__ );
    *env = &fake_env;
    return 0;
}

jint hook_DestoryJavaVM(JavaVM *){
    ALOGD("%s" , __FUNCTION__ );
    return 0;
}

jint hook_AttachCurrentThread(JavaVM *  , JNIEnv ** ,  void*){
    ALOGD("%s" , __FUNCTION__ );
    return 0;
}

jint hook_DetachCurrentThread(JavaVM * ){
    ALOGD("%s" , __FUNCTION__ );
    return 0;
}

jint hook_AttachCurrentThreadAsDaemon(JavaVM *  , JNIEnv ** ,  void *){
    ALOGD("%s" , __FUNCTION__ );
    return 0;
}

jclass hook_FindClass(hook_JNIEnv*, const char* className){
//    ALOGD("%s %s" , __FUNCTION__  , className);
    for(auto & iter : gClassNameMap){
        if(iter.second == className){
            return reinterpret_cast<jclass>(iter.first);
        }
    }
    string * name = new string(className);
    // 保证FindClass不为空
    // todo: 维护一个classlist 后续使用 对应的GetStaticMethodID  ， CallStaticVoidMethod 进行对应
//    ALOGD("hook_FindClass %p" , name);
    gClassNameMap[name] = className;
    return reinterpret_cast<jclass>(name);
}

jint hook_RegisterNatives(hook_JNIEnv*, jclass cls, const JNINativeMethod* jniNativeMethod,
                                   jint n){
    //dump jniNativeMethod
    string clsName = gClassNameMap[reinterpret_cast<string *>(cls)];
    for(int i = 0 ; i <  n ; i++){
        ALOGD("hook_RegisterNatives %s.%s %s %p" ,clsName.c_str() ,jniNativeMethod[i].name , jniNativeMethod[i].signature ,jniNativeMethod[i].fnPtr);
    }
    return JNI_TRUE;

}

jmethodID hook_FromReflectedMethod (hook_JNIEnv*, jobject){
    ALOGD("%s" , __FUNCTION__ );
    return reinterpret_cast<jmethodID>(1);
}

jfieldID hook_FromReflectedField(hook_JNIEnv*, jobject){
    ALOGD("%s" , __FUNCTION__ );
    return reinterpret_cast<jfieldID>(1);
}

jobject     hook_ToReflectedMethod(hook_JNIEnv*, jclass, jmethodID, jboolean){
    ALOGD("%s" , __FUNCTION__ );
    return reinterpret_cast<jobject>(1);
}

// todo: 假如传入的是一个真实的obj 能否解析这个obj对应类名
jclass hook_GetObjectClass(hook_JNIEnv*, jobject obj){
//    ALOGD("%s %p" , __FUNCTION__  , obj);

    // 伪造的obj
    if(gClassNameMap[reinterpret_cast<string *>(obj)] != ""){
        ALOGD("hook_GetObjectClass %s" , gClassNameMap[reinterpret_cast<string *>(obj)].c_str());
        return reinterpret_cast<jclass>(obj);
    }
    // 真实的obj
    jclass cls = oEnv->GetObjectClass(obj);

    jmethodID mid = oEnv->GetMethodID(cls, "getClass", "()Ljava/lang/Class;");
    jobject clsObj = oEnv->CallObjectMethod(obj, mid);

    cls = oEnv->GetObjectClass(clsObj);

    mid = oEnv->GetMethodID(cls, "getName", "()Ljava/lang/String;");

    jstring strObj = (jstring)oEnv->CallObjectMethod(clsObj, mid);

    const char* str = oEnv->GetStringUTFChars(strObj, NULL);
    ALOGD("%s %p" , __FUNCTION__  , str);

    oEnv->ReleaseStringUTFChars(strObj, str);

    return reinterpret_cast<jclass>(1);
}
jmethodID hook_GetMethodID(hook_JNIEnv*, jclass cls, const char* name, const char* sig){
//    ALOGD("hook_GetMethodID %p" , cls);
    string clsName = gClassNameMap[reinterpret_cast<string*>(cls)];

    string nameSig = name;
    nameSig.append(sig);
    for(auto & iter : gMethodIDMap){
        if(iter.second == nameSig){
            return reinterpret_cast<jmethodID>(iter.first);
        }
    }
    string * value = new string(nameSig);

    gMethodIDMap[value] = nameSig;
    ALOGD("%s %s.%s" , __FUNCTION__ ,clsName.c_str(), nameSig.c_str() );
    return reinterpret_cast<jmethodID>(value);
}
jmethodID hook_GetStaticMethodID(hook_JNIEnv*, jclass cls, const char* name, const char* sig){
//    ALOGD("%s %p" , __FUNCTION__ , cls);
    string clsName = gClassNameMap[reinterpret_cast<string*>(cls)];
    string nameSig = "static ";
    nameSig.append(name);
    nameSig.append(sig);
    for(auto & iter : gMethodIDMap){
        if(iter.second == nameSig){
            return reinterpret_cast<jmethodID>(iter.first);
        }
    }
    string * value = new string(nameSig);

    gMethodIDMap[value] = nameSig;
    ALOGD("%s %s.%s" , __FUNCTION__ ,clsName.c_str(), nameSig.c_str() );
    return reinterpret_cast<jmethodID>(value);
}

jobject hook_CallStaticObjectMethod(hook_JNIEnv*, jclass cls, jmethodID mid, ...){
//    ALOGD("%s" , __FUNCTION__ );
    string clsName = gClassNameMap[reinterpret_cast<string*>(cls)];
    string nameSig = gMethodIDMap[reinterpret_cast<string*>(mid)];
    ALOGD("%s %s %s" , __FUNCTION__ , clsName.c_str() , nameSig.c_str());
    return reinterpret_cast<jobject>(cls);
}

jobject hook_CallStaticObjectMethodA(hook_JNIEnv*, jclass cls, jmethodID mid, const jvalue*){
//    ALOGD("%s" , __FUNCTION__ );
    string clsName = gClassNameMap[reinterpret_cast<string*>(cls)];
    string nameSig = gMethodIDMap[reinterpret_cast<string*>(mid)];
    ALOGD("%s %s %s" , __FUNCTION__ , clsName.c_str() , nameSig.c_str());
    return reinterpret_cast<jobject>(cls);
}

jobject hook_CallStaticObjectMethodV(hook_JNIEnv*, jclass cls, jmethodID mid, va_list){
//        ALOGD("%s" , __FUNCTION__ );
        string clsName = gClassNameMap[reinterpret_cast<string*>(cls)];
        string nameSig = gMethodIDMap[reinterpret_cast<string*>(mid)];
        ALOGD("%s %s %s" , __FUNCTION__ , clsName.c_str() , nameSig.c_str());
        return reinterpret_cast<jobject>(cls);
}
jobject hook_CallObjectMethod(hook_JNIEnv*, jobject cls, jmethodID mid, ...){
//    ALOGD("%s" , __FUNCTION__ );
    string clsName = gClassNameMap[reinterpret_cast<string*>(cls)];
    string nameSig = gMethodIDMap[reinterpret_cast<string*>(mid)];
    ALOGD("%s %s %s" , __FUNCTION__ , clsName.c_str() , nameSig.c_str());
    return reinterpret_cast<jobject>(cls);
}
jobject hook_CallObjectMethodA(hook_JNIEnv*, jobject jobj, jmethodID mid, const jvalue*){
    ALOGD("%s" , __FUNCTION__ );
    string nameSig = gMethodIDMap[reinterpret_cast<string*>(mid)];
    ALOGD("%s %s" , __FUNCTION__ ,  nameSig.c_str());
    return reinterpret_cast<jobject>(jobj);
}
jlong hook_CallStaticLongMethodV(hook_JNIEnv*, jclass cls, jmethodID mid, va_list ){
    string clsName = gClassNameMap[reinterpret_cast<string*>(cls)];
    string nameSig = gMethodIDMap[reinterpret_cast<string*>(mid)];
    ALOGD("%s %s %s" , __FUNCTION__ , clsName.c_str() , nameSig.c_str());
    return reinterpret_cast<jlong>(cls);
}
jobject hook_CallObjectMethodV(hook_JNIEnv*, jobject jobj, jmethodID mid, va_list){
//    ALOGD("%s" , __FUNCTION__ );
    string clsName = gClassNameMap[reinterpret_cast<string*>(jobj)];
    string nameSig = gMethodIDMap[reinterpret_cast<string*>(mid)];
    ALOGD("%s %s %s" , __FUNCTION__ ,  clsName.c_str() ,nameSig.c_str());
    return reinterpret_cast<jobject>(jobj);
}

jobject hook_NewGlobalRef(hook_JNIEnv*, jobject jobj){
//    ALOGD("%s" , __FUNCTION__ );
    string clsName = gClassNameMap[reinterpret_cast<string*>(jobj)];
    ALOGD("%s %s" , __FUNCTION__ ,  clsName.c_str() );
    return reinterpret_cast<jobject>(jobj);
}

jobject hook_NewObject(hook_JNIEnv*, jclass cls, jmethodID mid, ...){
//    ALOGD("%s" , __FUNCTION__ );
    string clsName = gClassNameMap[reinterpret_cast<string*>(cls)];
    string nameSig = gMethodIDMap[reinterpret_cast<string*>(mid)];
    ALOGD("%s %s %s" , __FUNCTION__ ,  clsName.c_str() ,nameSig.c_str());
    return reinterpret_cast<jobject>(cls);
}
jobject hook_NewObjectV(hook_JNIEnv*, jclass cls, jmethodID mid, va_list){
//    ALOGD("%s" , __FUNCTION__ );
    string clsName = gClassNameMap[reinterpret_cast<string*>(cls)];
    string nameSig = gMethodIDMap[reinterpret_cast<string*>(mid)];
    ALOGD("%s %s %s" , __FUNCTION__ ,  clsName.c_str() ,nameSig.c_str());
    return reinterpret_cast<jobject>(cls);
}
jobject hook_NewObjectA(hook_JNIEnv*, jclass cls, jmethodID mid, const jvalue*){
//    ALOGD("%s" , __FUNCTION__ );
    string clsName = gClassNameMap[reinterpret_cast<string*>(cls)];
    string nameSig = gMethodIDMap[reinterpret_cast<string*>(mid)];
    ALOGD("%s %s %s" , __FUNCTION__ ,  clsName.c_str() ,nameSig.c_str());
    return reinterpret_cast<jobject>(cls);
}

void hook_DeleteGlobalRef(hook_JNIEnv*, jobject jobj){
    string clsName = gClassNameMap[reinterpret_cast<string*>(jobj)];
    ALOGD("%s %s" , __FUNCTION__ ,  clsName.c_str() );
    return ;
}

void hook_ExceptionClear(hook_JNIEnv *env){
    ALOGD("%s" , __FUNCTION__ );
    HasException = false;
    return ;
}

void hook_ExceptionDescribe(hook_JNIEnv *env){
    ALOGD("%s" , __FUNCTION__ );
    return ;
}

jthrowable  hook_ExceptionOccurred(hook_JNIEnv*){
    FUNC_LOG();
    return reinterpret_cast<jthrowable>(HasException);
}

jboolean  hook_ExceptionCheck(hook_JNIEnv* env){
    ALOGD("%s" , __FUNCTION__ );
    return  0;
}
void hook_FatalError(hook_JNIEnv *env , const char * str){
    ALOGD("%s %s" , __FUNCTION__  , str);
    return ;
}
jobject  hook_PopLocalFrame(hook_JNIEnv*, jobject){
    FUNC_LOG();
    return 0;
}
void tryError(){
    ALOGD("%s" , __FUNCTION__  );
    return ;
}

jint  hook_PushLocalFrame(hook_JNIEnv* env , jint num){
    ALOGD("%s" , __FUNCTION__  );
    return 0;
}

const char* hook_GetStringUTFChars(hook_JNIEnv* env, jstring pjstr, jboolean* bvalue){
    FUNC_LOG();
    const char * nlstr = "null";
    const char * retVal = "parse urself";
    if(pjstr == NULL){
        ALOGD("%s","null");
        return nlstr;
    }else{
        ALOGD("%s",retVal);
        return retVal;
    }
}

void hook_ReleaseStringUTFChars(hook_JNIEnv*, jstring pjstr, const char* cchr){
    FUNC_LOG();
    ALOGD("%s",cchr);
    return;
}

jint hook_Throw(hook_JNIEnv*, jthrowable){
    FUNC_LOG();
    HasException = true;
    return  0;
}
static void (*o_open)(const char  * , int );

static void
my_open(const char  * path, int flags) {
    ALOGD("path : %s", path);
    return o_open(path , flags);
}

static void doHook() {
    void *handle = DobbySymbolResolver("libc.so", "open");
    if (handle == nullptr) {
        ALOGD("Couldn't find 'open' handle.");
        return;
    }
    ALOGD("Found 'open' handle at %p", handle);
    DobbyHook(handle, (void *) my_open,
              (void **) &o_open);
}

extern int g_acf_array[] = {0 , 1};
extern int g_aco_array[] = {1,1};

extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_loadELF(JNIEnv *env, jobject thiz , jstring name) {

    const char * path = env->GetStringUTFChars(name, nullptr);
    void * handle  =  dlopen(path , RTLD_NOW);

    if(handle == NULL){
        ALOGD("dlopen error %s" , dlerror());
    }
    else {
        ALOGD("dlopen successful");
    }
//
    jint (*func)(JavaVM *, void *pVoid);
    func = (jint (*)(JavaVM* , void *))dlsym(handle, "JNI_OnLoad");

    if(func ==NULL){
        ALOGD("JNI_Onload not found");
    }

//    int ret = func(origin_jvm , NULL);
    // 伪造为hook_JavaVM时用
    int ret = func((JavaVM *)&fake_jvm , NULL);

    // the ret should be
    //#define JNI_VERSION_1_1 0x00010001
    //#define JNI_VERSION_1_2 0x00010002
    //#define JNI_VERSION_1_4 0x00010004
    //#define JNI_VERSION_1_6 0x00010006

    ALOGD("JNI_OnLoad %d " , ret);
    // Fatal signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr 0xXX 通常表示目标进程没有jniEnv 环境没有补充完整 ,调用到空指针引起

}

// Register native methods
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    return JNI_VERSION_1_6; // Success
}



extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_initial(JNIEnv *env, jobject thiz) {
    // TODO: implement initial()
    env->GetJavaVM(&oJvm);
    fake_env.functions = new hook_JNINativeInterface();

    // 将函数指针赋值为对应的函数索引，在未正确赋值的情况会导致SIGSEGV，通过fault addr 的值，可以明确那个函数需要完善。
    // 例如:fault addr 0x22 则说明第34个函数指针需要完善 , 到functions.txt 定位到对应的函数
    for(int i = 0 ;i < sizeof(hook_JNINativeInterface) / sizeof(size_t) ; i++){
        ((size_t *)fake_env.functions)[i] = i + 5;
    }

    fake_env.functions->FindClass = hook_FindClass;
    fake_env.functions->RegisterNatives = hook_RegisterNatives;
    fake_env.functions->FromReflectedMethod = hook_FromReflectedMethod;
    fake_env.functions->FromReflectedField = hook_FromReflectedField;
    fake_env.functions->ToReflectedMethod = hook_ToReflectedMethod;
    fake_env.functions->GetObjectClass = hook_GetObjectClass;
    fake_env.functions->GetMethodID = hook_GetMethodID;
    fake_env.functions->GetStaticMethodID = hook_GetStaticMethodID;
    fake_env.functions->CallStaticObjectMethod = hook_CallStaticObjectMethod;
    fake_env.functions->CallStaticObjectMethodA = hook_CallStaticObjectMethodA;
    fake_env.functions->CallStaticObjectMethodV = hook_CallStaticObjectMethodV;
    fake_env.functions->CallObjectMethod = hook_CallObjectMethod;
    fake_env.functions->CallObjectMethodV = hook_CallObjectMethodV;
    fake_env.functions->CallObjectMethodA = hook_CallObjectMethodA;
    fake_env.functions->CallIntMethod = reinterpret_cast<jint (*)(hook_JNIEnv *, jobject, jmethodID,
                                                                  ...)>(hook_CallObjectMethod);
    fake_env.functions->CallIntMethodV = reinterpret_cast<jint (*)(hook_JNIEnv *, jobject,
                                                                   jmethodID,
                                                                   va_list)>(hook_CallObjectMethodV);
    fake_env.functions->CallIntMethodA = reinterpret_cast<jint (*)(hook_JNIEnv *, jobject,
                                                                   jmethodID,
                                                                   const jvalue *)>(hook_CallObjectMethodA);
    fake_env.functions->CallShortMethod = reinterpret_cast<jshort (*)(hook_JNIEnv *, jobject,
                                                                      jmethodID,
                                                                      ...)>(hook_CallObjectMethod);
    fake_env.functions->CallShortMethodV = reinterpret_cast<jshort (*)(hook_JNIEnv *, jobject,
                                                                       jmethodID,
                                                                       va_list )>(hook_CallObjectMethodV);
    fake_env.functions->CallShortMethodA = reinterpret_cast<jshort (*)(hook_JNIEnv *, jobject,
                                                                       jmethodID,
                                                                       const jvalue *)>(hook_CallObjectMethodA);
    fake_env.functions->CallStaticLongMethodV = hook_CallStaticLongMethodV;
    fake_env.functions->NewObject = hook_NewObject;
    fake_env.functions->NewObjectV = hook_NewObjectV;
    fake_env.functions->NewObjectA = hook_NewObjectA;
    fake_env.functions->DeleteLocalRef = hook_DeleteGlobalRef;
    fake_env.functions->DeleteGlobalRef = hook_DeleteGlobalRef;
    fake_env.functions->NewGlobalRef = hook_NewGlobalRef;
    fake_env.functions->ExceptionClear = hook_ExceptionClear;
    fake_env.functions->ExceptionDescribe = hook_ExceptionDescribe;
    fake_env.functions->FatalError = hook_FatalError;
    fake_env.functions->PopLocalFrame = reinterpret_cast<jobject (*)(hook_JNIEnv *,
                                                                     jobject)>(tryError);

    fake_env.functions->PushLocalFrame = hook_PushLocalFrame;
    fake_env.functions->ExceptionCheck = hook_ExceptionCheck;
    fake_env.functions->ExceptionOccurred = hook_ExceptionOccurred;
    fake_env.functions->GetStringUTFChars = hook_GetStringUTFChars;
    fake_env.functions->ReleaseStringUTFChars = hook_ReleaseStringUTFChars;
    fake_env.functions->Throw = hook_Throw;

    fake_jvm.functions = new JNIInvokeInterface();
    fake_jvm.functions->GetEnv = hook_GetEnv;
    fake_jvm.functions->DestroyJavaVM = hook_DestoryJavaVM;
    fake_jvm.functions->DetachCurrentThread = hook_DetachCurrentThread;
    fake_jvm.functions->AttachCurrentThread = hook_AttachCurrentThread;
    fake_jvm.functions->AttachCurrentThreadAsDaemon = hook_AttachCurrentThreadAsDaemon;

    // dobby inlinehook 用例
//    doHook();

}