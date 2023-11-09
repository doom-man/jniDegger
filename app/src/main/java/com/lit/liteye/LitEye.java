package com.lit.liteye;

import android.content.Context;
import android.util.Log;

import java.io.File;

public class LitEye {

    private static final String TAG = "LitEye.SDK";

    public static volatile Context sApplicationContext = null;
    public static volatile boolean initSuccess = false;
    public static volatile String filesDir = null;

    static{
        System.loadLibrary("liteye");
    }

    public static void init(Context context){
        try{
            filesDir = context.getFilesDir().getAbsolutePath();
            Log.i(TAG, "init: filesdir = " + filesDir);
            sApplicationContext = context.getApplicationContext();
            File crashDump = new File(filesDir, "crashDump");
            if (!crashDump.exists()){
                crashDump.mkdir();
            }
            initBreakpad(crashDump.getAbsolutePath());
            initSuccess = true;
        } catch (Exception e){
            e.printStackTrace();
        }
    }

//    public static void runtimeCheckl(ResultCallback callback){
//        if (sApplicationContext == null){
//            callback.onError(-1 , "init failed");
//            return;
//        }
//
//        String result = runtimeCheck(sApplicationContext);
//        if(result == null){
//            result = "runtimeCheck failed";
//            callback.onError(-2 , result);
//        }
//        else {
//            callback.onSuccess(result);
//        }
//    }

    static native void initBreakpad(String storagePath);

    public static native String detect(Context cxt);

    public static native String siua(Context cxt);

    public static native void runtimeCheck(Context cxt ,ResultCallback callback);

    public static native String dfpToken(Context cxt);

    public static native String litSign(String input);



}
