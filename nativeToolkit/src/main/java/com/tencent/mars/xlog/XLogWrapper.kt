package com.tencent.mars.xlog

import android.content.Context
import android.os.SystemClock


class XLogWrapper(context: Context, logPath: String) {
    companion object {
        private const val TAG = "XLogWrapper"
        fun defaultLogDir(context: Context, logDir: String = "xlog"): String {
            val file = context.getExternalFilesDir(logDir)
             return file?.run {
                try {
                    if (!exists()) {
                        if (mkdirs()) absolutePath else context.filesDir.absolutePath + "/" + logDir
                    } else {
                        absolutePath
                    }
                } catch (e: SecurityException) {
                    //ctx.getFilesDir().toString() + "/tombstones";
                    Log.e(TAG, "setLogDir failed, use default dir", e)
                    context.filesDir.absolutePath + "/" + logDir
                }
            } ?: (context.filesDir.absolutePath + "/" + logDir)
        }
    }

    init {
        val cachePath = context.filesDir.absolutePath + "/xlog" //todo
        System.loadLibrary("c++_shared")
        System.loadLibrary("marsxlog")
        Xlog.open(false, Xlog.LEVEL_DEBUG, Xlog.AppednerModeAsync, cachePath, logPath, "XLog", "");
        //Log.setConsoleLogOpen(true);
        val xlog = Xlog()
        //下面需要放到appenderOpen方法后面，即Xlog.open后面。
        xlog.setConsoleLogOpen(0, false) //由于采用hook的方式，已经启用了控制台日志输出，这里关闭。
        var logFileSaveDays = 1
        //机器日期重启
        val maxAliveTime = logFileSaveDays.toLong() * 24 * 60 * 60
        xlog.setMaxAliveTime(0, maxAliveTime)
        xlog.setMaxFileSize(0, 10*1024*1024) // 单位为Byte
        Log.setLogImp(xlog)
    }

    fun sync() {
        //Log.appenderFlush()
        val start = SystemClock.uptimeMillis()
        Log.appenderFlushSync(true)
        Log.i("XLogWrapper", "sync cost: ${SystemClock.uptimeMillis() - start}ms")
    }

    fun unInit() {
        Log.appenderClose()
    }
}