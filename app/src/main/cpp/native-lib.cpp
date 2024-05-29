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

#define LOG_TAG "pareto"
extern "C" long raw_syscall();

#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

struct hook_JNIEnv;
struct hook_JavaVM;
typedef hook_JNIEnv hook_JNIEnv;
typedef hook_JavaVM hook_JavaVM;

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


struct hook_JNINativeInterface {
    void*       reserved0;
    void*       reserved1;
    void*       reserved2;
    void*       reserved3;

    jint        (*GetVersion)(hook_JNIEnv *);

    jclass      (*DefineClass)(hook_JNIEnv*, const char*, jobject, const jbyte*,
                               jsize);
    jclass      (*FindClass)(hook_JNIEnv*, const char*);

    jmethodID   (*FromReflectedMethod)(hook_JNIEnv*, jobject);
    jfieldID    (*FromReflectedField)(hook_JNIEnv*, jobject);
    /* spec doesn't show jboolean parameter */
    jobject     (*ToReflectedMethod)(hook_JNIEnv*, jclass, jmethodID, jboolean);

    jclass      (*GetSuperclass)(hook_JNIEnv*, jclass);
    jboolean    (*IsAssignableFrom)(hook_JNIEnv*, jclass, jclass);

    /* spec doesn't show jboolean parameter */
    jobject     (*ToReflectedField)(hook_JNIEnv*, jclass, jfieldID, jboolean);

    jint        (*Throw)(hook_JNIEnv*, jthrowable);
    jint        (*ThrowNew)(hook_JNIEnv *, jclass, const char *);
    jthrowable  (*ExceptionOccurred)(hook_JNIEnv*);
    void        (*ExceptionDescribe)(hook_JNIEnv*);
    void        (*ExceptionClear)(hook_JNIEnv*);
    void        (*FatalError)(hook_JNIEnv*, const char*);

    jint        (*PushLocalFrame)(hook_JNIEnv*, jint);
    jobject     (*PopLocalFrame)(hook_JNIEnv*, jobject);

    jobject     (*NewGlobalRef)(hook_JNIEnv*, jobject);
    void        (*DeleteGlobalRef)(hook_JNIEnv*, jobject);
    void        (*DeleteLocalRef)(hook_JNIEnv*, jobject);
    jboolean    (*IsSameObject)(hook_JNIEnv*, jobject, jobject);

    jobject     (*NewLocalRef)(hook_JNIEnv*, jobject);
    jint        (*EnsureLocalCapacity)(hook_JNIEnv*, jint);

    jobject     (*AllocObject)(hook_JNIEnv*, jclass);
    jobject     (*NewObject)(hook_JNIEnv*, jclass, jmethodID, ...);
    jobject     (*NewObjectV)(hook_JNIEnv*, jclass, jmethodID, va_list);
    jobject     (*NewObjectA)(hook_JNIEnv*, jclass, jmethodID, const jvalue*);

    jclass      (*GetObjectClass)(hook_JNIEnv*, jobject);
    jboolean    (*IsInstanceOf)(hook_JNIEnv*, jobject, jclass);
    jmethodID   (*GetMethodID)(hook_JNIEnv*, jclass, const char*, const char*);

    jobject     (*CallObjectMethod)(hook_JNIEnv*, jobject, jmethodID, ...);
    jobject     (*CallObjectMethodV)(hook_JNIEnv*, jobject, jmethodID, va_list);
    jobject     (*CallObjectMethodA)(hook_JNIEnv*, jobject, jmethodID, const jvalue*);
    jboolean    (*CallBooleanMethod)(hook_JNIEnv*, jobject, jmethodID, ...);
    jboolean    (*CallBooleanMethodV)(hook_JNIEnv*, jobject, jmethodID, va_list);
    jboolean    (*CallBooleanMethodA)(hook_JNIEnv*, jobject, jmethodID, const jvalue*);
    jbyte       (*CallByteMethod)(hook_JNIEnv*, jobject, jmethodID, ...);
    jbyte       (*CallByteMethodV)(hook_JNIEnv*, jobject, jmethodID, va_list);
    jbyte       (*CallByteMethodA)(hook_JNIEnv*, jobject, jmethodID, const jvalue*);
    jchar       (*CallCharMethod)(hook_JNIEnv*, jobject, jmethodID, ...);
    jchar       (*CallCharMethodV)(hook_JNIEnv*, jobject, jmethodID, va_list);
    jchar       (*CallCharMethodA)(hook_JNIEnv*, jobject, jmethodID, const jvalue*);
    jshort      (*CallShortMethod)(hook_JNIEnv*, jobject, jmethodID, ...);
    jshort      (*CallShortMethodV)(hook_JNIEnv*, jobject, jmethodID, va_list);
    jshort      (*CallShortMethodA)(hook_JNIEnv*, jobject, jmethodID, const jvalue*);
    jint        (*CallIntMethod)(hook_JNIEnv*, jobject, jmethodID, ...);
    jint        (*CallIntMethodV)(hook_JNIEnv*, jobject, jmethodID, va_list);
    jint        (*CallIntMethodA)(hook_JNIEnv*, jobject, jmethodID, const jvalue*);
    jlong       (*CallLongMethod)(hook_JNIEnv*, jobject, jmethodID, ...);
    jlong       (*CallLongMethodV)(hook_JNIEnv*, jobject, jmethodID, va_list);
    jlong       (*CallLongMethodA)(hook_JNIEnv*, jobject, jmethodID, const jvalue*);
    jfloat      (*CallFloatMethod)(hook_JNIEnv*, jobject, jmethodID, ...);
    jfloat      (*CallFloatMethodV)(hook_JNIEnv*, jobject, jmethodID, va_list);
    jfloat      (*CallFloatMethodA)(hook_JNIEnv*, jobject, jmethodID, const jvalue*);
    jdouble     (*CallDoubleMethod)(hook_JNIEnv*, jobject, jmethodID, ...);
    jdouble     (*CallDoubleMethodV)(hook_JNIEnv*, jobject, jmethodID, va_list);
    jdouble     (*CallDoubleMethodA)(hook_JNIEnv*, jobject, jmethodID, const jvalue*);
    void        (*CallVoidMethod)(hook_JNIEnv*, jobject, jmethodID, ...);
    void        (*CallVoidMethodV)(hook_JNIEnv*, jobject, jmethodID, va_list);
    void        (*CallVoidMethodA)(hook_JNIEnv*, jobject, jmethodID, const jvalue*);

    jobject     (*CallNonvirtualObjectMethod)(hook_JNIEnv*, jobject, jclass,
                                              jmethodID, ...);
    jobject     (*CallNonvirtualObjectMethodV)(hook_JNIEnv*, jobject, jclass,
                                               jmethodID, va_list);
    jobject     (*CallNonvirtualObjectMethodA)(hook_JNIEnv*, jobject, jclass,
                                               jmethodID, const jvalue*);
    jboolean    (*CallNonvirtualBooleanMethod)(hook_JNIEnv*, jobject, jclass,
                                               jmethodID, ...);
    jboolean    (*CallNonvirtualBooleanMethodV)(hook_JNIEnv*, jobject, jclass,
                                                jmethodID, va_list);
    jboolean    (*CallNonvirtualBooleanMethodA)(hook_JNIEnv*, jobject, jclass,
                                                jmethodID, const jvalue*);
    jbyte       (*CallNonvirtualByteMethod)(hook_JNIEnv*, jobject, jclass,
                                            jmethodID, ...);
    jbyte       (*CallNonvirtualByteMethodV)(hook_JNIEnv*, jobject, jclass,
                                             jmethodID, va_list);
    jbyte       (*CallNonvirtualByteMethodA)(hook_JNIEnv*, jobject, jclass,
                                             jmethodID, const jvalue*);
    jchar       (*CallNonvirtualCharMethod)(hook_JNIEnv*, jobject, jclass,
                                            jmethodID, ...);
    jchar       (*CallNonvirtualCharMethodV)(hook_JNIEnv*, jobject, jclass,
                                             jmethodID, va_list);
    jchar       (*CallNonvirtualCharMethodA)(hook_JNIEnv*, jobject, jclass,
                                             jmethodID, const jvalue*);
    jshort      (*CallNonvirtualShortMethod)(hook_JNIEnv*, jobject, jclass,
                                             jmethodID, ...);
    jshort      (*CallNonvirtualShortMethodV)(hook_JNIEnv*, jobject, jclass,
                                              jmethodID, va_list);
    jshort      (*CallNonvirtualShortMethodA)(hook_JNIEnv*, jobject, jclass,
                                              jmethodID, const jvalue*);
    jint        (*CallNonvirtualIntMethod)(hook_JNIEnv*, jobject, jclass,
                                           jmethodID, ...);
    jint        (*CallNonvirtualIntMethodV)(hook_JNIEnv*, jobject, jclass,
                                            jmethodID, va_list);
    jint        (*CallNonvirtualIntMethodA)(hook_JNIEnv*, jobject, jclass,
                                            jmethodID, const jvalue*);
    jlong       (*CallNonvirtualLongMethod)(hook_JNIEnv*, jobject, jclass,
                                            jmethodID, ...);
    jlong       (*CallNonvirtualLongMethodV)(hook_JNIEnv*, jobject, jclass,
                                             jmethodID, va_list);
    jlong       (*CallNonvirtualLongMethodA)(hook_JNIEnv*, jobject, jclass,
                                             jmethodID, const jvalue*);
    jfloat      (*CallNonvirtualFloatMethod)(hook_JNIEnv*, jobject, jclass,
                                             jmethodID, ...);
    jfloat      (*CallNonvirtualFloatMethodV)(hook_JNIEnv*, jobject, jclass,
                                              jmethodID, va_list);
    jfloat      (*CallNonvirtualFloatMethodA)(hook_JNIEnv*, jobject, jclass,
                                              jmethodID, const jvalue*);
    jdouble     (*CallNonvirtualDoubleMethod)(hook_JNIEnv*, jobject, jclass,
                                              jmethodID, ...);
    jdouble     (*CallNonvirtualDoubleMethodV)(hook_JNIEnv*, jobject, jclass,
                                               jmethodID, va_list);
    jdouble     (*CallNonvirtualDoubleMethodA)(hook_JNIEnv*, jobject, jclass,
                                               jmethodID, const jvalue*);
    void        (*CallNonvirtualVoidMethod)(hook_JNIEnv*, jobject, jclass,
                                            jmethodID, ...);
    void        (*CallNonvirtualVoidMethodV)(hook_JNIEnv*, jobject, jclass,
                                             jmethodID, va_list);
    void        (*CallNonvirtualVoidMethodA)(hook_JNIEnv*, jobject, jclass,
                                             jmethodID, const jvalue*);

    jfieldID    (*GetFieldID)(hook_JNIEnv*, jclass, const char*, const char*);

    jobject     (*GetObjectField)(hook_JNIEnv*, jobject, jfieldID);
    jboolean    (*GetBooleanField)(hook_JNIEnv*, jobject, jfieldID);
    jbyte       (*GetByteField)(hook_JNIEnv*, jobject, jfieldID);
    jchar       (*GetCharField)(hook_JNIEnv*, jobject, jfieldID);
    jshort      (*GetShortField)(hook_JNIEnv*, jobject, jfieldID);
    jint        (*GetIntField)(hook_JNIEnv*, jobject, jfieldID);
    jlong       (*GetLongField)(hook_JNIEnv*, jobject, jfieldID);
    jfloat      (*GetFloatField)(hook_JNIEnv*, jobject, jfieldID);
    jdouble     (*GetDoubleField)(hook_JNIEnv*, jobject, jfieldID);

    void        (*SetObjectField)(hook_JNIEnv*, jobject, jfieldID, jobject);
    void        (*SetBooleanField)(hook_JNIEnv*, jobject, jfieldID, jboolean);
    void        (*SetByteField)(hook_JNIEnv*, jobject, jfieldID, jbyte);
    void        (*SetCharField)(hook_JNIEnv*, jobject, jfieldID, jchar);
    void        (*SetShortField)(hook_JNIEnv*, jobject, jfieldID, jshort);
    void        (*SetIntField)(hook_JNIEnv*, jobject, jfieldID, jint);
    void        (*SetLongField)(hook_JNIEnv*, jobject, jfieldID, jlong);
    void        (*SetFloatField)(hook_JNIEnv*, jobject, jfieldID, jfloat);
    void        (*SetDoubleField)(hook_JNIEnv*, jobject, jfieldID, jdouble);

    jmethodID   (*GetStaticMethodID)(hook_JNIEnv*, jclass, const char*, const char*);

    jobject     (*CallStaticObjectMethod)(hook_JNIEnv*, jclass, jmethodID, ...);
    jobject     (*CallStaticObjectMethodV)(hook_JNIEnv*, jclass, jmethodID, va_list);
    jobject     (*CallStaticObjectMethodA)(hook_JNIEnv*, jclass, jmethodID, const jvalue*);
    jboolean    (*CallStaticBooleanMethod)(hook_JNIEnv*, jclass, jmethodID, ...);
    jboolean    (*CallStaticBooleanMethodV)(hook_JNIEnv*, jclass, jmethodID,
                                            va_list);
    jboolean    (*CallStaticBooleanMethodA)(hook_JNIEnv*, jclass, jmethodID, const jvalue*);
    jbyte       (*CallStaticByteMethod)(hook_JNIEnv*, jclass, jmethodID, ...);
    jbyte       (*CallStaticByteMethodV)(hook_JNIEnv*, jclass, jmethodID, va_list);
    jbyte       (*CallStaticByteMethodA)(hook_JNIEnv*, jclass, jmethodID, const jvalue*);
    jchar       (*CallStaticCharMethod)(hook_JNIEnv*, jclass, jmethodID, ...);
    jchar       (*CallStaticCharMethodV)(hook_JNIEnv*, jclass, jmethodID, va_list);
    jchar       (*CallStaticCharMethodA)(hook_JNIEnv*, jclass, jmethodID, const jvalue*);
    jshort      (*CallStaticShortMethod)(hook_JNIEnv*, jclass, jmethodID, ...);
    jshort      (*CallStaticShortMethodV)(hook_JNIEnv*, jclass, jmethodID, va_list);
    jshort      (*CallStaticShortMethodA)(hook_JNIEnv*, jclass, jmethodID, const jvalue*);
    jint        (*CallStaticIntMethod)(hook_JNIEnv*, jclass, jmethodID, ...);
    jint        (*CallStaticIntMethodV)(hook_JNIEnv*, jclass, jmethodID, va_list);
    jint        (*CallStaticIntMethodA)(hook_JNIEnv*, jclass, jmethodID, const jvalue*);
    jlong       (*CallStaticLongMethod)(hook_JNIEnv*, jclass, jmethodID, ...);
    jlong       (*CallStaticLongMethodV)(hook_JNIEnv*, jclass, jmethodID, va_list);
    jlong       (*CallStaticLongMethodA)(hook_JNIEnv*, jclass, jmethodID, const jvalue*);
    jfloat      (*CallStaticFloatMethod)(hook_JNIEnv*, jclass, jmethodID, ...);
    jfloat      (*CallStaticFloatMethodV)(hook_JNIEnv*, jclass, jmethodID, va_list);
    jfloat      (*CallStaticFloatMethodA)(hook_JNIEnv*, jclass, jmethodID, const jvalue*);
    jdouble     (*CallStaticDoubleMethod)(hook_JNIEnv*, jclass, jmethodID, ...);
    jdouble     (*CallStaticDoubleMethodV)(hook_JNIEnv*, jclass, jmethodID, va_list);
    jdouble     (*CallStaticDoubleMethodA)(hook_JNIEnv*, jclass, jmethodID, const jvalue*);
    void        (*CallStaticVoidMethod)(hook_JNIEnv*, jclass, jmethodID, ...);
    void        (*CallStaticVoidMethodV)(hook_JNIEnv*, jclass, jmethodID, va_list);
    void        (*CallStaticVoidMethodA)(hook_JNIEnv*, jclass, jmethodID, const jvalue*);

    jfieldID    (*GetStaticFieldID)(hook_JNIEnv*, jclass, const char*,
                                    const char*);

    jobject     (*GetStaticObjectField)(hook_JNIEnv*, jclass, jfieldID);
    jboolean    (*GetStaticBooleanField)(hook_JNIEnv*, jclass, jfieldID);
    jbyte       (*GetStaticByteField)(hook_JNIEnv*, jclass, jfieldID);
    jchar       (*GetStaticCharField)(hook_JNIEnv*, jclass, jfieldID);
    jshort      (*GetStaticShortField)(hook_JNIEnv*, jclass, jfieldID);
    jint        (*GetStaticIntField)(hook_JNIEnv*, jclass, jfieldID);
    jlong       (*GetStaticLongField)(hook_JNIEnv*, jclass, jfieldID);
    jfloat      (*GetStaticFloatField)(hook_JNIEnv*, jclass, jfieldID);
    jdouble     (*GetStaticDoubleField)(hook_JNIEnv*, jclass, jfieldID);

    void        (*SetStaticObjectField)(hook_JNIEnv*, jclass, jfieldID, jobject);
    void        (*SetStaticBooleanField)(hook_JNIEnv*, jclass, jfieldID, jboolean);
    void        (*SetStaticByteField)(hook_JNIEnv*, jclass, jfieldID, jbyte);
    void        (*SetStaticCharField)(hook_JNIEnv*, jclass, jfieldID, jchar);
    void        (*SetStaticShortField)(hook_JNIEnv*, jclass, jfieldID, jshort);
    void        (*SetStaticIntField)(hook_JNIEnv*, jclass, jfieldID, jint);
    void        (*SetStaticLongField)(hook_JNIEnv*, jclass, jfieldID, jlong);
    void        (*SetStaticFloatField)(hook_JNIEnv*, jclass, jfieldID, jfloat);
    void        (*SetStaticDoubleField)(hook_JNIEnv*, jclass, jfieldID, jdouble);

    jstring     (*NewString)(hook_JNIEnv*, const jchar*, jsize);
    jsize       (*GetStringLength)(hook_JNIEnv*, jstring);
    const jchar* (*GetStringChars)(hook_JNIEnv*, jstring, jboolean*);
    void        (*ReleaseStringChars)(hook_JNIEnv*, jstring, const jchar*);
    jstring     (*NewStringUTF)(hook_JNIEnv*, const char*);
    jsize       (*GetStringUTFLength)(hook_JNIEnv*, jstring);
    /* JNI spec says this returns const jbyte*, but that's inconsistent */
    const char* (*GetStringUTFChars)(hook_JNIEnv*, jstring, jboolean*);
    void        (*ReleaseStringUTFChars)(hook_JNIEnv*, jstring, const char*);
    jsize       (*GetArrayLength)(hook_JNIEnv*, jarray);
    jobjectArray (*NewObjectArray)(hook_JNIEnv*, jsize, jclass, jobject);
    jobject     (*GetObjectArrayElement)(hook_JNIEnv*, jobjectArray, jsize);
    void        (*SetObjectArrayElement)(hook_JNIEnv*, jobjectArray, jsize, jobject);

    jbooleanArray (*NewBooleanArray)(hook_JNIEnv*, jsize);
    jbyteArray    (*NewByteArray)(hook_JNIEnv*, jsize);
    jcharArray    (*NewCharArray)(hook_JNIEnv*, jsize);
    jshortArray   (*NewShortArray)(hook_JNIEnv*, jsize);
    jintArray     (*NewIntArray)(hook_JNIEnv*, jsize);
    jlongArray    (*NewLongArray)(hook_JNIEnv*, jsize);
    jfloatArray   (*NewFloatArray)(hook_JNIEnv*, jsize);
    jdoubleArray  (*NewDoubleArray)(hook_JNIEnv*, jsize);

    jboolean*   (*GetBooleanArrayElements)(hook_JNIEnv*, jbooleanArray, jboolean*);
    jbyte*      (*GetByteArrayElements)(hook_JNIEnv*, jbyteArray, jboolean*);
    jchar*      (*GetCharArrayElements)(hook_JNIEnv*, jcharArray, jboolean*);
    jshort*     (*GetShortArrayElements)(hook_JNIEnv*, jshortArray, jboolean*);
    jint*       (*GetIntArrayElements)(hook_JNIEnv*, jintArray, jboolean*);
    jlong*      (*GetLongArrayElements)(hook_JNIEnv*, jlongArray, jboolean*);
    jfloat*     (*GetFloatArrayElements)(hook_JNIEnv*, jfloatArray, jboolean*);
    jdouble*    (*GetDoubleArrayElements)(hook_JNIEnv*, jdoubleArray, jboolean*);

    void        (*ReleaseBooleanArrayElements)(hook_JNIEnv*, jbooleanArray,
                                               jboolean*, jint);
    void        (*ReleaseByteArrayElements)(hook_JNIEnv*, jbyteArray,
                                            jbyte*, jint);
    void        (*ReleaseCharArrayElements)(hook_JNIEnv*, jcharArray,
                                            jchar*, jint);
    void        (*ReleaseShortArrayElements)(hook_JNIEnv*, jshortArray,
                                             jshort*, jint);
    void        (*ReleaseIntArrayElements)(hook_JNIEnv*, jintArray,
                                           jint*, jint);
    void        (*ReleaseLongArrayElements)(hook_JNIEnv*, jlongArray,
                                            jlong*, jint);
    void        (*ReleaseFloatArrayElements)(hook_JNIEnv*, jfloatArray,
                                             jfloat*, jint);
    void        (*ReleaseDoubleArrayElements)(hook_JNIEnv*, jdoubleArray,
                                              jdouble*, jint);

    void        (*GetBooleanArrayRegion)(hook_JNIEnv*, jbooleanArray,
                                         jsize, jsize, jboolean*);
    void        (*GetByteArrayRegion)(hook_JNIEnv*, jbyteArray,
                                      jsize, jsize, jbyte*);
    void        (*GetCharArrayRegion)(hook_JNIEnv*, jcharArray,
                                      jsize, jsize, jchar*);
    void        (*GetShortArrayRegion)(hook_JNIEnv*, jshortArray,
                                       jsize, jsize, jshort*);
    void        (*GetIntArrayRegion)(hook_JNIEnv*, jintArray,
                                     jsize, jsize, jint*);
    void        (*GetLongArrayRegion)(hook_JNIEnv*, jlongArray,
                                      jsize, jsize, jlong*);
    void        (*GetFloatArrayRegion)(hook_JNIEnv*, jfloatArray,
                                       jsize, jsize, jfloat*);
    void        (*GetDoubleArrayRegion)(hook_JNIEnv*, jdoubleArray,
                                        jsize, jsize, jdouble*);

    /* spec shows these without const; some jni.h do, some don't */
    void        (*SetBooleanArrayRegion)(hook_JNIEnv*, jbooleanArray,
                                         jsize, jsize, const jboolean*);
    void        (*SetByteArrayRegion)(hook_JNIEnv*, jbyteArray,
                                      jsize, jsize, const jbyte*);
    void        (*SetCharArrayRegion)(hook_JNIEnv*, jcharArray,
                                      jsize, jsize, const jchar*);
    void        (*SetShortArrayRegion)(hook_JNIEnv*, jshortArray,
                                       jsize, jsize, const jshort*);
    void        (*SetIntArrayRegion)(hook_JNIEnv*, jintArray,
                                     jsize, jsize, const jint*);
    void        (*SetLongArrayRegion)(hook_JNIEnv*, jlongArray,
                                      jsize, jsize, const jlong*);
    void        (*SetFloatArrayRegion)(hook_JNIEnv*, jfloatArray,
                                       jsize, jsize, const jfloat*);
    void        (*SetDoubleArrayRegion)(hook_JNIEnv*, jdoubleArray,
                                        jsize, jsize, const jdouble*);

    jint        (*RegisterNatives)(hook_JNIEnv*, jclass, const JNINativeMethod*,
                                   jint);
    jint        (*UnregisterNatives)(hook_JNIEnv*, jclass);
    jint        (*MonitorEnter)(hook_JNIEnv*, jobject);
    jint        (*MonitorExit)(hook_JNIEnv*, jobject);
    jint        (*GetJavaVM)(hook_JNIEnv*, JavaVM**);

    void        (*GetStringRegion)(hook_JNIEnv*, jstring, jsize, jsize, jchar*);
    void        (*GetStringUTFRegion)(hook_JNIEnv*, jstring, jsize, jsize, char*);

    void*       (*GetPrimitiveArrayCritical)(hook_JNIEnv*, jarray, jboolean*);
    void        (*ReleasePrimitiveArrayCritical)(hook_JNIEnv*, jarray, void*, jint);

    const jchar* (*GetStringCritical)(hook_JNIEnv*, jstring, jboolean*);
    void        (*ReleaseStringCritical)(hook_JNIEnv*, jstring, const jchar*);

    jweak       (*NewWeakGlobalRef)(hook_JNIEnv*, jobject);
    void        (*DeleteWeakGlobalRef)(hook_JNIEnv*, jweak);

    jboolean    (*ExceptionCheck)(hook_JNIEnv*);

    jobject     (*NewDirectByteBuffer)(hook_JNIEnv*, void*, jlong);
    void*       (*GetDirectBufferAddress)(hook_JNIEnv*, jobject);
    jlong       (*GetDirectBufferCapacity)(hook_JNIEnv*, jobject);

    /* added in JNI 1.6 */
    jobjectRefType (*GetObjectRefType)(hook_JNIEnv*, jobject);
};


//伪造JNIEnv
//jni 对应的定义在jni.h:490
struct hook_JNIEnv {
    /* do not rename this; it does not seem to be entirely opaque */
    struct hook_JNINativeInterface* functions;

#if defined(__cplusplus)

    jint GetVersion()
    { return functions->GetVersion(this); }

    jclass DefineClass(const char *name, jobject loader, const jbyte* buf,
                       jsize bufLen)
    { return functions->DefineClass(this, name, loader, buf, bufLen); }

    jclass FindClass(const char* name)
    { return functions->FindClass(this, name); }

    jmethodID FromReflectedMethod(jobject method)
    { return functions->FromReflectedMethod(this, method); }

    jfieldID FromReflectedField(jobject field)
    { return functions->FromReflectedField(this, field); }

    jobject ToReflectedMethod(jclass cls, jmethodID methodID, jboolean isStatic)
    { return functions->ToReflectedMethod(this, cls, methodID, isStatic); }

    jclass GetSuperclass(jclass clazz)
    { return functions->GetSuperclass(this, clazz); }

    jboolean IsAssignableFrom(jclass clazz1, jclass clazz2)
    { return functions->IsAssignableFrom(this, clazz1, clazz2); }

    jobject ToReflectedField(jclass cls, jfieldID fieldID, jboolean isStatic)
    { return functions->ToReflectedField(this, cls, fieldID, isStatic); }

    jint Throw(jthrowable obj)
    { return functions->Throw(this, obj); }

    jint ThrowNew(jclass clazz, const char* message)
    { return functions->ThrowNew(this, clazz, message); }

    jthrowable ExceptionOccurred()
    { return functions->ExceptionOccurred(this); }

    void ExceptionDescribe()
    { functions->ExceptionDescribe(this); }

    void ExceptionClear()
    { functions->ExceptionClear(this); }

    void FatalError(const char* msg)
    { functions->FatalError(this, msg); }

    jint PushLocalFrame(jint capacity)
    { return functions->PushLocalFrame(this, capacity); }

    jobject PopLocalFrame(jobject result)
    { return functions->PopLocalFrame(this, result); }

    jobject NewGlobalRef(jobject obj)
    { return functions->NewGlobalRef(this, obj); }

    void DeleteGlobalRef(jobject globalRef)
    { functions->DeleteGlobalRef(this, globalRef); }

    void DeleteLocalRef(jobject localRef)
    { functions->DeleteLocalRef(this, localRef); }

    jboolean IsSameObject(jobject ref1, jobject ref2)
    { return functions->IsSameObject(this, ref1, ref2); }

    jobject NewLocalRef(jobject ref)
    { return functions->NewLocalRef(this, ref); }

    jint EnsureLocalCapacity(jint capacity)
    { return functions->EnsureLocalCapacity(this, capacity); }

    jobject AllocObject(jclass clazz)
    { return functions->AllocObject(this, clazz); }

    jobject NewObject(jclass clazz, jmethodID methodID, ...)
    {
        va_list args;
        va_start(args, methodID);
        jobject result = functions->NewObjectV(this, clazz, methodID, args);
        va_end(args);
        return result;
    }

    jobject NewObjectV(jclass clazz, jmethodID methodID, va_list args)
    { return functions->NewObjectV(this, clazz, methodID, args); }

    jobject NewObjectA(jclass clazz, jmethodID methodID, const jvalue* args)
    { return functions->NewObjectA(this, clazz, methodID, args); }

    jclass GetObjectClass(jobject obj)
    { return functions->GetObjectClass(this, obj); }

    jboolean IsInstanceOf(jobject obj, jclass clazz)
    { return functions->IsInstanceOf(this, obj, clazz); }

    jmethodID GetMethodID(jclass clazz, const char* name, const char* sig)
    { return functions->GetMethodID(this, clazz, name, sig); }

#define CALL_TYPE_METHOD(_jtype, _jname)                                    \
    _jtype Call##_jname##Method(jobject obj, jmethodID methodID, ...)       \
    {                                                                       \
        _jtype result;                                                      \
        va_list args;                                                       \
        va_start(args, methodID);                                           \
        result = functions->Call##_jname##MethodV(this, obj, methodID,      \
                    args);                                                  \
        va_end(args);                                                       \
        return result;                                                      \
    }
#define CALL_TYPE_METHODV(_jtype, _jname)                                   \
    _jtype Call##_jname##MethodV(jobject obj, jmethodID methodID,           \
        va_list args)                                                       \
    { return functions->Call##_jname##MethodV(this, obj, methodID, args); }
#define CALL_TYPE_METHODA(_jtype, _jname)                                   \
    _jtype Call##_jname##MethodA(jobject obj, jmethodID methodID,           \
                                 const jvalue* args)                        \
    { return functions->Call##_jname##MethodA(this, obj, methodID, args); }

#define CALL_TYPE(_jtype, _jname)                                           \
    CALL_TYPE_METHOD(_jtype, _jname)                                        \
    CALL_TYPE_METHODV(_jtype, _jname)                                       \
    CALL_TYPE_METHODA(_jtype, _jname)

    CALL_TYPE(jobject, Object)
    CALL_TYPE(jboolean, Boolean)
    CALL_TYPE(jbyte, Byte)
    CALL_TYPE(jchar, Char)
    CALL_TYPE(jshort, Short)
    CALL_TYPE(jint, Int)
    CALL_TYPE(jlong, Long)
    CALL_TYPE(jfloat, Float)
    CALL_TYPE(jdouble, Double)

    void CallVoidMethod(jobject obj, jmethodID methodID, ...)
    {
        va_list args;
        va_start(args, methodID);
        functions->CallVoidMethodV(this, obj, methodID, args);
        va_end(args);
    }
    void CallVoidMethodV(jobject obj, jmethodID methodID, va_list args)
    { functions->CallVoidMethodV(this, obj, methodID, args); }
    void CallVoidMethodA(jobject obj, jmethodID methodID, const jvalue* args)
    { functions->CallVoidMethodA(this, obj, methodID, args); }

#define CALL_NONVIRT_TYPE_METHOD(_jtype, _jname)                            \
    _jtype CallNonvirtual##_jname##Method(jobject obj, jclass clazz,        \
        jmethodID methodID, ...)                                            \
    {                                                                       \
        _jtype result;                                                      \
        va_list args;                                                       \
        va_start(args, methodID);                                           \
        result = functions->CallNonvirtual##_jname##MethodV(this, obj,      \
                    clazz, methodID, args);                                 \
        va_end(args);                                                       \
        return result;                                                      \
    }
#define CALL_NONVIRT_TYPE_METHODV(_jtype, _jname)                           \
    _jtype CallNonvirtual##_jname##MethodV(jobject obj, jclass clazz,       \
        jmethodID methodID, va_list args)                                   \
    { return functions->CallNonvirtual##_jname##MethodV(this, obj, clazz,   \
        methodID, args); }
#define CALL_NONVIRT_TYPE_METHODA(_jtype, _jname)                           \
    _jtype CallNonvirtual##_jname##MethodA(jobject obj, jclass clazz,       \
        jmethodID methodID, const jvalue* args)                             \
    { return functions->CallNonvirtual##_jname##MethodA(this, obj, clazz,   \
        methodID, args); }

#define CALL_NONVIRT_TYPE(_jtype, _jname)                                   \
    CALL_NONVIRT_TYPE_METHOD(_jtype, _jname)                                \
    CALL_NONVIRT_TYPE_METHODV(_jtype, _jname)                               \
    CALL_NONVIRT_TYPE_METHODA(_jtype, _jname)

    CALL_NONVIRT_TYPE(jobject, Object)
    CALL_NONVIRT_TYPE(jboolean, Boolean)
    CALL_NONVIRT_TYPE(jbyte, Byte)
    CALL_NONVIRT_TYPE(jchar, Char)
    CALL_NONVIRT_TYPE(jshort, Short)
    CALL_NONVIRT_TYPE(jint, Int)
    CALL_NONVIRT_TYPE(jlong, Long)
    CALL_NONVIRT_TYPE(jfloat, Float)
    CALL_NONVIRT_TYPE(jdouble, Double)

    void CallNonvirtualVoidMethod(jobject obj, jclass clazz,
                                  jmethodID methodID, ...)
    {
        va_list args;
        va_start(args, methodID);
        functions->CallNonvirtualVoidMethodV(this, obj, clazz, methodID, args);
        va_end(args);
    }
    void CallNonvirtualVoidMethodV(jobject obj, jclass clazz,
                                   jmethodID methodID, va_list args)
    { functions->CallNonvirtualVoidMethodV(this, obj, clazz, methodID, args); }
    void CallNonvirtualVoidMethodA(jobject obj, jclass clazz,
                                   jmethodID methodID, const jvalue* args)
    { functions->CallNonvirtualVoidMethodA(this, obj, clazz, methodID, args); }

    jfieldID GetFieldID(jclass clazz, const char* name, const char* sig)
    { return functions->GetFieldID(this, clazz, name, sig); }

    jobject GetObjectField(jobject obj, jfieldID fieldID)
    { return functions->GetObjectField(this, obj, fieldID); }
    jboolean GetBooleanField(jobject obj, jfieldID fieldID)
    { return functions->GetBooleanField(this, obj, fieldID); }
    jbyte GetByteField(jobject obj, jfieldID fieldID)
    { return functions->GetByteField(this, obj, fieldID); }
    jchar GetCharField(jobject obj, jfieldID fieldID)
    { return functions->GetCharField(this, obj, fieldID); }
    jshort GetShortField(jobject obj, jfieldID fieldID)
    { return functions->GetShortField(this, obj, fieldID); }
    jint GetIntField(jobject obj, jfieldID fieldID)
    { return functions->GetIntField(this, obj, fieldID); }
    jlong GetLongField(jobject obj, jfieldID fieldID)
    { return functions->GetLongField(this, obj, fieldID); }
    jfloat GetFloatField(jobject obj, jfieldID fieldID)
    { return functions->GetFloatField(this, obj, fieldID); }
    jdouble GetDoubleField(jobject obj, jfieldID fieldID)
    { return functions->GetDoubleField(this, obj, fieldID); }

    void SetObjectField(jobject obj, jfieldID fieldID, jobject value)
    { functions->SetObjectField(this, obj, fieldID, value); }
    void SetBooleanField(jobject obj, jfieldID fieldID, jboolean value)
    { functions->SetBooleanField(this, obj, fieldID, value); }
    void SetByteField(jobject obj, jfieldID fieldID, jbyte value)
    { functions->SetByteField(this, obj, fieldID, value); }
    void SetCharField(jobject obj, jfieldID fieldID, jchar value)
    { functions->SetCharField(this, obj, fieldID, value); }
    void SetShortField(jobject obj, jfieldID fieldID, jshort value)
    { functions->SetShortField(this, obj, fieldID, value); }
    void SetIntField(jobject obj, jfieldID fieldID, jint value)
    { functions->SetIntField(this, obj, fieldID, value); }
    void SetLongField(jobject obj, jfieldID fieldID, jlong value)
    { functions->SetLongField(this, obj, fieldID, value); }
    void SetFloatField(jobject obj, jfieldID fieldID, jfloat value)
    { functions->SetFloatField(this, obj, fieldID, value); }
    void SetDoubleField(jobject obj, jfieldID fieldID, jdouble value)
    { functions->SetDoubleField(this, obj, fieldID, value); }

    jmethodID GetStaticMethodID(jclass clazz, const char* name, const char* sig)
    { return functions->GetStaticMethodID(this, clazz, name, sig); }

#define CALL_STATIC_TYPE_METHOD(_jtype, _jname)                             \
    _jtype CallStatic##_jname##Method(jclass clazz, jmethodID methodID,     \
        ...)                                                                \
    {                                                                       \
        _jtype result;                                                      \
        va_list args;                                                       \
        va_start(args, methodID);                                           \
        result = functions->CallStatic##_jname##MethodV(this, clazz,        \
                    methodID, args);                                        \
        va_end(args);                                                       \
        return result;                                                      \
    }
#define CALL_STATIC_TYPE_METHODV(_jtype, _jname)                            \
    _jtype CallStatic##_jname##MethodV(jclass clazz, jmethodID methodID,    \
        va_list args)                                                       \
    { return functions->CallStatic##_jname##MethodV(this, clazz, methodID,  \
        args); }
#define CALL_STATIC_TYPE_METHODA(_jtype, _jname)                            \
    _jtype CallStatic##_jname##MethodA(jclass clazz, jmethodID methodID,    \
                                       const jvalue* args)                  \
    { return functions->CallStatic##_jname##MethodA(this, clazz, methodID,  \
        args); }

#define CALL_STATIC_TYPE(_jtype, _jname)                                    \
    CALL_STATIC_TYPE_METHOD(_jtype, _jname)                                 \
    CALL_STATIC_TYPE_METHODV(_jtype, _jname)                                \
    CALL_STATIC_TYPE_METHODA(_jtype, _jname)

    CALL_STATIC_TYPE(jobject, Object)
    CALL_STATIC_TYPE(jboolean, Boolean)
    CALL_STATIC_TYPE(jbyte, Byte)
    CALL_STATIC_TYPE(jchar, Char)
    CALL_STATIC_TYPE(jshort, Short)
    CALL_STATIC_TYPE(jint, Int)
    CALL_STATIC_TYPE(jlong, Long)
    CALL_STATIC_TYPE(jfloat, Float)
    CALL_STATIC_TYPE(jdouble, Double)

    void CallStaticVoidMethod(jclass clazz, jmethodID methodID, ...)
    {
        va_list args;
        va_start(args, methodID);
        functions->CallStaticVoidMethodV(this, clazz, methodID, args);
        va_end(args);
    }
    void CallStaticVoidMethodV(jclass clazz, jmethodID methodID, va_list args)
    { functions->CallStaticVoidMethodV(this, clazz, methodID, args); }
    void CallStaticVoidMethodA(jclass clazz, jmethodID methodID, const jvalue* args)
    { functions->CallStaticVoidMethodA(this, clazz, methodID, args); }

    jfieldID GetStaticFieldID(jclass clazz, const char* name, const char* sig)
    { return functions->GetStaticFieldID(this, clazz, name, sig); }

    jobject GetStaticObjectField(jclass clazz, jfieldID fieldID)
    { return functions->GetStaticObjectField(this, clazz, fieldID); }
    jboolean GetStaticBooleanField(jclass clazz, jfieldID fieldID)
    { return functions->GetStaticBooleanField(this, clazz, fieldID); }
    jbyte GetStaticByteField(jclass clazz, jfieldID fieldID)
    { return functions->GetStaticByteField(this, clazz, fieldID); }
    jchar GetStaticCharField(jclass clazz, jfieldID fieldID)
    { return functions->GetStaticCharField(this, clazz, fieldID); }
    jshort GetStaticShortField(jclass clazz, jfieldID fieldID)
    { return functions->GetStaticShortField(this, clazz, fieldID); }
    jint GetStaticIntField(jclass clazz, jfieldID fieldID)
    { return functions->GetStaticIntField(this, clazz, fieldID); }
    jlong GetStaticLongField(jclass clazz, jfieldID fieldID)
    { return functions->GetStaticLongField(this, clazz, fieldID); }
    jfloat GetStaticFloatField(jclass clazz, jfieldID fieldID)
    { return functions->GetStaticFloatField(this, clazz, fieldID); }
    jdouble GetStaticDoubleField(jclass clazz, jfieldID fieldID)
    { return functions->GetStaticDoubleField(this, clazz, fieldID); }

    void SetStaticObjectField(jclass clazz, jfieldID fieldID, jobject value)
    { functions->SetStaticObjectField(this, clazz, fieldID, value); }
    void SetStaticBooleanField(jclass clazz, jfieldID fieldID, jboolean value)
    { functions->SetStaticBooleanField(this, clazz, fieldID, value); }
    void SetStaticByteField(jclass clazz, jfieldID fieldID, jbyte value)
    { functions->SetStaticByteField(this, clazz, fieldID, value); }
    void SetStaticCharField(jclass clazz, jfieldID fieldID, jchar value)
    { functions->SetStaticCharField(this, clazz, fieldID, value); }
    void SetStaticShortField(jclass clazz, jfieldID fieldID, jshort value)
    { functions->SetStaticShortField(this, clazz, fieldID, value); }
    void SetStaticIntField(jclass clazz, jfieldID fieldID, jint value)
    { functions->SetStaticIntField(this, clazz, fieldID, value); }
    void SetStaticLongField(jclass clazz, jfieldID fieldID, jlong value)
    { functions->SetStaticLongField(this, clazz, fieldID, value); }
    void SetStaticFloatField(jclass clazz, jfieldID fieldID, jfloat value)
    { functions->SetStaticFloatField(this, clazz, fieldID, value); }
    void SetStaticDoubleField(jclass clazz, jfieldID fieldID, jdouble value)
    { functions->SetStaticDoubleField(this, clazz, fieldID, value); }

    jstring NewString(const jchar* unicodeChars, jsize len)
    { return functions->NewString(this, unicodeChars, len); }

    jsize GetStringLength(jstring string)
    { return functions->GetStringLength(this, string); }

    const jchar* GetStringChars(jstring string, jboolean* isCopy)
    { return functions->GetStringChars(this, string, isCopy); }

    void ReleaseStringChars(jstring string, const jchar* chars)
    { functions->ReleaseStringChars(this, string, chars); }

    jstring NewStringUTF(const char* bytes)
    { return functions->NewStringUTF(this, bytes); }

    jsize GetStringUTFLength(jstring string)
    { return functions->GetStringUTFLength(this, string); }

    const char* GetStringUTFChars(jstring string, jboolean* isCopy)
    { return functions->GetStringUTFChars(this, string, isCopy); }

    void ReleaseStringUTFChars(jstring string, const char* utf)
    { functions->ReleaseStringUTFChars(this, string, utf); }

    jsize GetArrayLength(jarray array)
    { return functions->GetArrayLength(this, array); }

    jobjectArray NewObjectArray(jsize length, jclass elementClass,
                                jobject initialElement)
    { return functions->NewObjectArray(this, length, elementClass,
                                       initialElement); }

    jobject GetObjectArrayElement(jobjectArray array, jsize index)
    { return functions->GetObjectArrayElement(this, array, index); }

    void SetObjectArrayElement(jobjectArray array, jsize index, jobject value)
    { functions->SetObjectArrayElement(this, array, index, value); }

    jbooleanArray NewBooleanArray(jsize length)
    { return functions->NewBooleanArray(this, length); }
    jbyteArray NewByteArray(jsize length)
    { return functions->NewByteArray(this, length); }
    jcharArray NewCharArray(jsize length)
    { return functions->NewCharArray(this, length); }
    jshortArray NewShortArray(jsize length)
    { return functions->NewShortArray(this, length); }
    jintArray NewIntArray(jsize length)
    { return functions->NewIntArray(this, length); }
    jlongArray NewLongArray(jsize length)
    { return functions->NewLongArray(this, length); }
    jfloatArray NewFloatArray(jsize length)
    { return functions->NewFloatArray(this, length); }
    jdoubleArray NewDoubleArray(jsize length)
    { return functions->NewDoubleArray(this, length); }

    jboolean* GetBooleanArrayElements(jbooleanArray array, jboolean* isCopy)
    { return functions->GetBooleanArrayElements(this, array, isCopy); }
    jbyte* GetByteArrayElements(jbyteArray array, jboolean* isCopy)
    { return functions->GetByteArrayElements(this, array, isCopy); }
    jchar* GetCharArrayElements(jcharArray array, jboolean* isCopy)
    { return functions->GetCharArrayElements(this, array, isCopy); }
    jshort* GetShortArrayElements(jshortArray array, jboolean* isCopy)
    { return functions->GetShortArrayElements(this, array, isCopy); }
    jint* GetIntArrayElements(jintArray array, jboolean* isCopy)
    { return functions->GetIntArrayElements(this, array, isCopy); }
    jlong* GetLongArrayElements(jlongArray array, jboolean* isCopy)
    { return functions->GetLongArrayElements(this, array, isCopy); }
    jfloat* GetFloatArrayElements(jfloatArray array, jboolean* isCopy)
    { return functions->GetFloatArrayElements(this, array, isCopy); }
    jdouble* GetDoubleArrayElements(jdoubleArray array, jboolean* isCopy)
    { return functions->GetDoubleArrayElements(this, array, isCopy); }

    void ReleaseBooleanArrayElements(jbooleanArray array, jboolean* elems,
                                     jint mode)
    { functions->ReleaseBooleanArrayElements(this, array, elems, mode); }
    void ReleaseByteArrayElements(jbyteArray array, jbyte* elems,
                                  jint mode)
    { functions->ReleaseByteArrayElements(this, array, elems, mode); }
    void ReleaseCharArrayElements(jcharArray array, jchar* elems,
                                  jint mode)
    { functions->ReleaseCharArrayElements(this, array, elems, mode); }
    void ReleaseShortArrayElements(jshortArray array, jshort* elems,
                                   jint mode)
    { functions->ReleaseShortArrayElements(this, array, elems, mode); }
    void ReleaseIntArrayElements(jintArray array, jint* elems,
                                 jint mode)
    { functions->ReleaseIntArrayElements(this, array, elems, mode); }
    void ReleaseLongArrayElements(jlongArray array, jlong* elems,
                                  jint mode)
    { functions->ReleaseLongArrayElements(this, array, elems, mode); }
    void ReleaseFloatArrayElements(jfloatArray array, jfloat* elems,
                                   jint mode)
    { functions->ReleaseFloatArrayElements(this, array, elems, mode); }
    void ReleaseDoubleArrayElements(jdoubleArray array, jdouble* elems,
                                    jint mode)
    { functions->ReleaseDoubleArrayElements(this, array, elems, mode); }

    void GetBooleanArrayRegion(jbooleanArray array, jsize start, jsize len,
                               jboolean* buf)
    { functions->GetBooleanArrayRegion(this, array, start, len, buf); }
    void GetByteArrayRegion(jbyteArray array, jsize start, jsize len,
                            jbyte* buf)
    { functions->GetByteArrayRegion(this, array, start, len, buf); }
    void GetCharArrayRegion(jcharArray array, jsize start, jsize len,
                            jchar* buf)
    { functions->GetCharArrayRegion(this, array, start, len, buf); }
    void GetShortArrayRegion(jshortArray array, jsize start, jsize len,
                             jshort* buf)
    { functions->GetShortArrayRegion(this, array, start, len, buf); }
    void GetIntArrayRegion(jintArray array, jsize start, jsize len,
                           jint* buf)
    { functions->GetIntArrayRegion(this, array, start, len, buf); }
    void GetLongArrayRegion(jlongArray array, jsize start, jsize len,
                            jlong* buf)
    { functions->GetLongArrayRegion(this, array, start, len, buf); }
    void GetFloatArrayRegion(jfloatArray array, jsize start, jsize len,
                             jfloat* buf)
    { functions->GetFloatArrayRegion(this, array, start, len, buf); }
    void GetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len,
                              jdouble* buf)
    { functions->GetDoubleArrayRegion(this, array, start, len, buf); }

    void SetBooleanArrayRegion(jbooleanArray array, jsize start, jsize len,
                               const jboolean* buf)
    { functions->SetBooleanArrayRegion(this, array, start, len, buf); }
    void SetByteArrayRegion(jbyteArray array, jsize start, jsize len,
                            const jbyte* buf)
    { functions->SetByteArrayRegion(this, array, start, len, buf); }
    void SetCharArrayRegion(jcharArray array, jsize start, jsize len,
                            const jchar* buf)
    { functions->SetCharArrayRegion(this, array, start, len, buf); }
    void SetShortArrayRegion(jshortArray array, jsize start, jsize len,
                             const jshort* buf)
    { functions->SetShortArrayRegion(this, array, start, len, buf); }
    void SetIntArrayRegion(jintArray array, jsize start, jsize len,
                           const jint* buf)
    { functions->SetIntArrayRegion(this, array, start, len, buf); }
    void SetLongArrayRegion(jlongArray array, jsize start, jsize len,
                            const jlong* buf)
    { functions->SetLongArrayRegion(this, array, start, len, buf); }
    void SetFloatArrayRegion(jfloatArray array, jsize start, jsize len,
                             const jfloat* buf)
    { functions->SetFloatArrayRegion(this, array, start, len, buf); }
    void SetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len,
                              const jdouble* buf)
    { functions->SetDoubleArrayRegion(this, array, start, len, buf); }

    jint RegisterNatives(jclass clazz, const JNINativeMethod* methods,
                         jint nMethods)
    { return functions->RegisterNatives(this, clazz, methods, nMethods); }

    jint UnregisterNatives(jclass clazz)
    { return functions->UnregisterNatives(this, clazz); }

    jint MonitorEnter(jobject obj)
    { return functions->MonitorEnter(this, obj); }

    jint MonitorExit(jobject obj)
    { return functions->MonitorExit(this, obj); }

    jint GetJavaVM(JavaVM** vm)
    { return functions->GetJavaVM(this, vm); }

    void GetStringRegion(jstring str, jsize start, jsize len, jchar* buf)
    { functions->GetStringRegion(this, str, start, len, buf); }

    void GetStringUTFRegion(jstring str, jsize start, jsize len, char* buf)
    { return functions->GetStringUTFRegion(this, str, start, len, buf); }

    void* GetPrimitiveArrayCritical(jarray array, jboolean* isCopy)
    { return functions->GetPrimitiveArrayCritical(this, array, isCopy); }

    void ReleasePrimitiveArrayCritical(jarray array, void* carray, jint mode)
    { functions->ReleasePrimitiveArrayCritical(this, array, carray, mode); }

    const jchar* GetStringCritical(jstring string, jboolean* isCopy)
    { return functions->GetStringCritical(this, string, isCopy); }

    void ReleaseStringCritical(jstring string, const jchar* carray)
    { functions->ReleaseStringCritical(this, string, carray); }

    jweak NewWeakGlobalRef(jobject obj)
    { return functions->NewWeakGlobalRef(this, obj); }

    void DeleteWeakGlobalRef(jweak obj)
    { functions->DeleteWeakGlobalRef(this, obj); }

    jboolean ExceptionCheck()
    { return functions->ExceptionCheck(this); }

    jobject NewDirectByteBuffer(void* address, jlong capacity)
    { return functions->NewDirectByteBuffer(this, address, capacity); }

    void* GetDirectBufferAddress(jobject buf)
    { return functions->GetDirectBufferAddress(this, buf); }

    jlong GetDirectBufferCapacity(jobject buf)
    { return functions->GetDirectBufferCapacity(this, buf); }

    /* added in JNI 1.6 */
    jobjectRefType GetObjectRefType(jobject obj)
    { return functions->GetObjectRefType(this, obj); }
#endif /*__cplusplus*/
};

JNIEnv  * gEnv;


jint hook_GetEnv(JavaVM *  , void ** env,  jint){
    ALOGD("hook_GetEnv");
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

jclass hook_FindClass(hook_JNIEnv*, const char* className){
    ALOGD("hook_FindClass %s" , className);
    // 保证FindClass不为空
    return reinterpret_cast<jclass>(1);
}

jint hook_RegisterNatives(hook_JNIEnv*, jclass, const JNINativeMethod* jniNativeMethod,
                                   jint n){
    //dump jniNativeMethod

    ALOGD("hook_RegisterNatives");
    for(int i = 0 ; i <  n ; i++){
        ALOGD("hook_RegisterNatives %s %s %p" , jniNativeMethod[i].name , jniNativeMethod[i].signature , jniNativeMethod[i].fnPtr);
    }
    return JNI_TRUE;

}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_stringFromJNI2(JNIEnv *env, jobject thiz) {
    // TODO: implement stringFromJNI2()
    const char * hello = "Hello from C++";
    //新建jvm接口 , 并且伪造JavaVM提供的函数
    // 通过JNIEnv获取 获取javaVM
    JavaVM * origin_jvm;
    env->GetJavaVM(&origin_jvm);

    hook_JNIEnv fake_env ;
    fake_env.functions = new hook_JNINativeInterface();
    fake_env.functions->FindClass = hook_FindClass;
    fake_env.functions->RegisterNatives = hook_RegisterNatives;

    hook_JavaVM  fake_jvm;
    fake_jvm.functions = new JNIInvokeInterface();
    fake_jvm.functions->GetEnv = hook_GetEnv;
    fake_jvm.functions->DestroyJavaVM = hook_DestoryJavaVM;
    fake_jvm.functions->DetachCurrentThread = hook_DetachCurrentThread;
    fake_jvm.functions->AttachCurrentThread = hook_AttachCurrentThread;
    fake_jvm.functions->AttachCurrentThreadAsDaemon = hook_AttachCurrentThreadAsDaemon;

//     将伪造的fake_env赋值全局gEnv ，通过hook_JavaVM的GetEnv函数交给目标调用
    gEnv = (JNIEnv*)&fake_env;

    // 测试伪造的JavaVM
//    JNIEnv * fade;
//    fake_jvm.GetEnv(reinterpret_cast<void **>(&fade), 0x10004);
//    fade->NewStringUTF("hello World");

//    void * handle  =  dlopen("lib.so" , RTLD_NOW);
    void * handle  =  dlopen("libtarget.so" , RTLD_NOW);

    jint (*func)(JavaVM *, void *pVoid);
    func = (jint (*)(JavaVM* , void *))dlsym(handle, "JNI_OnLoad");

//    int ret = func(origin_jvm , NULL);
    // 伪造为hook_JavaVM时用
    int ret = func((JavaVM *)&fake_jvm , NULL);

    ALOGD("JNI_OnLoad %d " , ret);
    // Fatal signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr 0x0 通常表示目标进程没有jniEnv 环境没有补充完整 ,调用到空指针引起

    return env->NewStringUTF(hello);

}

