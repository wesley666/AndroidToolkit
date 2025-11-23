package com.wesley.android.c.toolkit

import android.util.Log
import com.bytedance.android.bytehook.ByteHook

class NativeHelper {

    /**
     * A native method that is implemented by the 'nativelib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'nativelib' library on application startup.
        init {
            val ret = ByteHook.init()
            Log.e("ByteHook", "init ret: $ret")
            System.loadLibrary("toolkit")
        }

        external fun stringFromJNI(): String
        external fun hookLogcat(consoleLogOpen: Boolean = true, saveToFile: Boolean = true): Boolean
        external fun unhookLogcat()

    }
}