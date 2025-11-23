package com.wesley.android.toolkit

import android.app.Application
import com.tencent.mars.xlog.XLogWrapper
import com.wesley.android.c.toolkit.NativeHelper
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.delay
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch

class APP: Application() {
    override fun onCreate() {
        super.onCreate()
        val xlog = XLogWrapper(this, XLogWrapper.defaultLogDir(this))
        NativeHelper.hookLogcat(consoleLogOpen = true, saveToFile = true)

        GlobalScope.launch(Dispatchers.IO) {
            delay(30_000)
            while (isActive) {
                delay(30_000)
                xlog.sync()
            }
        }
    }
}