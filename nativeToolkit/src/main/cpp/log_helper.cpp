#include <jni.h>
#include <string>
//#include <android/log.h>
#include <bytehook.h>
#include "xlogger/android_xlog.h"
#define TAG "wesley-jni"
//#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG,  __VA_ARGS__);
//#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG,  __VA_ARGS__);
//#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG,  __VA_ARGS__);

static bool g_consoleLogOpen = false;
static bool g_saveToFile = false;

int android_log_buf_write_proxy(int bufID, int prio, const char *tag, const char *text) {
    BYTEHOOK_STACK_SCOPE();
    //LOGE("hook  log")
    int ret = 0;
    char tag_buffer[50];
    if (g_consoleLogOpen) {
        snprintf(tag_buffer, sizeof(tag_buffer), "wesley-%s", tag);
        ret = BYTEHOOK_CALL_PREV(android_log_buf_write_proxy, bufID, prio, tag_buffer, text);
    }
    if (g_saveToFile) {
        if (prio >= ANDROID_LOG_ERROR) {
            XLOGE(tag, "%s", text);
        } else {
            XLOGI(tag, "%s", text);
        }
    }
    return ret;
}

// 过滤器函数。返回 true 表示需要 hook 这个 so；返回 false 表示不需要 hook 这个 so。
bool hook_log_allow_filter(const char *caller_path_name, void *arg)
{
    //if(nullptr != strstr(caller_path_name, "liblog.so")) return true;

    //android_util_Log_println_native 所在so
    if(nullptr != strstr(caller_path_name, "libandroid_runtime.so")) return true;

    return false;
}

bytehook_stub_t stub_of_android_log_buf_write = nullptr; // hook 的存根，用于后续 unhook
int hook_logcat() {
    //__android_log_write
    //bytehook_hook_all(nullptr, "android_log_print_proxy", (void *) (my_log_print), nullptr,
     //                 nullptr);
//    stub_of_android_log_buf_write = bytehook_hook_all(nullptr,
//                                                      "__android_log_buf_write",
//                                                      (void *) (android_log_buf_write_proxy),
//                                                      nullptr,
//                                                      nullptr);
    stub_of_android_log_buf_write = bytehook_hook_partial(
            hook_log_allow_filter,
            nullptr,
            nullptr,
            "__android_log_buf_write",
            reinterpret_cast<void *>(android_log_buf_write_proxy),
            nullptr,
            nullptr);
    if (stub_of_android_log_buf_write == nullptr) {
        return -1;
    }
    return 0;
}

void unhook_logcat() {
    if (stub_of_android_log_buf_write != nullptr) {
        bytehook_unhook(stub_of_android_log_buf_write);
        stub_of_android_log_buf_write = nullptr;
    }
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_wesley_android_c_toolkit_NativeHelper_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_wesley_android_c_toolkit_NativeHelper_00024Companion_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}



extern "C"
JNIEXPORT void JNICALL
Java_com_wesley_android_c_toolkit_NativeHelper_00024Companion_unhookLogcat(JNIEnv *env,
                                                                         jobject thiz) {
    XLOGE(TAG, "unhook_logcat");
    unhook_logcat();
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_wesley_android_c_toolkit_NativeHelper_00024Companion_hookLogcat(JNIEnv *env, jobject thiz,
                                                                       jboolean console_log_open, jboolean save_to_file) {
    XLOGE(TAG,"hook_logcat");
    g_consoleLogOpen = (console_log_open == JNI_TRUE);
    g_saveToFile = (save_to_file == JNI_TRUE);
    if (hook_logcat() == 0) {
        return JNI_TRUE;
    }
    return JNI_FALSE;
}