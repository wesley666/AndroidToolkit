package com.wesley.android.c.toolkit

class NativeLib {

    /**
     * A native method that is implemented by the 'toolkit' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'toolkit' library on application startup.
        init {
            System.loadLibrary("toolkit")
        }
    }
}