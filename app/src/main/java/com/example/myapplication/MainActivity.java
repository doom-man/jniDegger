package com.example.myapplication;

import static java.lang.Class.forName;

import androidx.appcompat.app.AppCompatActivity;

import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.provider.Settings;
import android.util.Log;
import android.widget.TextView;

import com.example.myapplication.databinding.ActivityMainBinding;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.net.NetworkInterface;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;
import java.util.Enumeration;
import java.lang.reflect.Proxy;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'myapplication' library on application startup.
    static {
        System.loadLibrary("myapplication");
//        System.loadLibrary("");
    }

    private ActivityMainBinding binding;
    private HandlerThread mHandlerThread;
    private Handler mHandler;

    public final static String MD5 = "MD5";
    public final static String SHA1 = "SHA1";
    public final static String SHA256 = "SHA256";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

//        mHandlerThread = new HandlerThread("MyHandlerThread");
//        mHandlerThread.start();

//        ApkVerifier.Builder builder = new ApkVerifier.Builder()
        // Example of a call to a native method
        TextView tv = binding.sampleText;

//        initHandler(mHandlerThread);

//        Message msg = new Message();
//        msg.what = 1;
//        msg.obj = "heeeeeeee";

//        mHandler.sendMessage(msg);
//        getSig(this.getApplicationInfo().sourceDir);
        tv.setText(stringFromJNI2());
//        tv.setText("hello");

        StringBuilder sb =  new StringBuilder();
        PackageManager pm = this.getApplicationContext().getPackageManager();
        String packageName = this.getPackageName();
        PackageInfo packageInfo;
        try {
//             packageInfo = pm.getPackageInfo(packageName, PackageManager.GET_SIGNING_CERTIFICATES);
            packageInfo = pm.getPackageInfo(packageName, PackageManager.GET_SIGNATURES);

        } catch (PackageManager.NameNotFoundException e) {
            throw new RuntimeException(e);
        }

//            Throwable.class.newInstance().fillInStackTrace();
        //        SigningInfo signingInfo;
//        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
//            signingInfo = packageInfo.signingInfo;
//            Signature [] signatures = signingInfo.getSigningCertificateHistory();
//            for(Signature signature : signatures){
//                sb.append(signature.toCharsString());
//            }
//        }

        // call packagemanager parseApkLit to verify apk
//        try {
//            forName("android.content.pm.PackageManagerService").getMethod("parseBaseApk", String.class).invoke(pm, "/sdcard/1.apk");
//        } catch (IllegalAccessException e) {
//            throw new RuntimeException(e);
//        } catch (InvocationTargetException e) {
//            throw new RuntimeException(e);
//        } catch (NoSuchMethodException e) {
//            throw new RuntimeException(e);
//        } catch (ClassNotFoundException e) {
//            throw new RuntimeException(e);
//        }
//
//
        for(Signature signature : packageInfo.signatures) {
            sb.append(signature.toCharsString());
        }
//
//        Log.d("TAG" , "signature:\t" + sb.toString());
//
//
//        Enumeration<NetworkInterface>  networkInterfaces;
//
//
//        System.getProperty("http.proxyHost");
//
//        IVehical car = new Car();
//        IVehical proxyCar = (IVehical) Proxy.newProxyInstance(car.getClass().getClassLoader(), car.getClass().getInterfaces(), new VehicalInvacationHandler(car));
//        proxyCar.run();
//
//        if(proxyCar instanceof Proxy){
//           Log.d("Proxy" , "car is proxy");
//        }
//
//        Object currentActivityThread = Reflect.onClass("android.app.ActivityThread").call("currentActivityThread").get();
//        Object sPackageManager = Reflect.on(currentActivityThread).call("getPackageManager").get();
//
//        Class<?> iPackageManagerClass = Reflect.onClass("android.content.pm.IPackageManager").type();
//
//        Object proxy = Proxy.newProxyInstance(iPackageManagerClass.getClassLoader(), new Class<?>[]{iPackageManagerClass}, new PmsHookBinderInvocationHandler(sPackageManager , packageName , 0));
//        Reflect.on(currentActivityThread).set("sPackageManager" , proxy);
//        PackageManager pm2 = getApplicationContext().getPackageManager();
//        Reflect.on(pm2).set("mPM",proxy);
//
//
//        Object psPackageManager = Reflect.on(currentActivityThread).call("getPackageManager").get();
//        if(psPackageManager instanceof Proxy){
//            Log.d("Proxy" , "psPackageManager is proxy");
//        }
//
//        String path = getApplicationInfo().dataDir;
//        Log.d("TAG" , "dataDir" + path);
//
//        String sid = Settings.Secure.getString(getContentResolver(), Settings.Secure.ANDROID_ID);
//        Log.d("TAG" , "secure_id" + sid);
//
//
//        try {
//            PackageInfo pi = pm.getPackageInfo(packageName , 0);
//            long time = pi.firstInstallTime;
//            Log.d("timestamp" , "time"+String.valueOf(time));
//        } catch (PackageManager.NameNotFoundException e) {
//            throw new RuntimeException(e);
//        }
//        try {
//            KeyStore Instance = KeyStore.getInstance("AndroidCAStore");
//            Instance.load(null,null);
//            Enumeration<String> obj_aliases = Instance.aliases();
//            while (obj_aliases.hasMoreElements()){
//                String alias = obj_aliases.nextElement();
////                Instance.getCertificate(alias).getIssuerDN().getName();
//                X509Certificate cer = (X509Certificate) Instance.getCertificate(alias);
//
//                Log.d("TAG" , "alias" + alias + "\t" + cer.getIssuerDN().getName());
//            }
//
//
//        } catch (KeyStoreException e) {
//            throw new RuntimeException(e);
//        } catch (CertificateException e) {
//            throw new RuntimeException(e);
//        } catch (IOException e) {
//            throw new RuntimeException(e);
//        } catch (NoSuchAlgorithmException e) {
//            throw new RuntimeException(e);
//        }
//        ArrayList<NetworkInterface> list  = (ArrayList<NetworkInterface>) networkInterfaces;
//        list.size();
//        for (NetworkInterface networkInterface : list) {
//            sb.append(networkInterface.toString());
//        }
//
//        Log.d("TAG" , "networkface:\t" + sb.toString());

//        Signature[]  signatures  = packageInfo.signatures;

//        for (Signature sig : signatures) {
//            String tmp = "error!";
//            if (MD5.equals("MD5")) {
//                tmp = getSignatureByteString(sig, MD5);
//            } else if (SHA1.equals(type)) {
//                tmp = getSignatureByteString(sig, SHA1);
//            } else if (SHA256.equals(type)) {
//                tmp = getSignatureByteString(sig, SHA256);
//            }
//        }



        // signature compare
//        int retval = pm.checkSignatures(uid, uid) ;

//        try {
//            int flags = getApplicationContext().getPackageManager().getApplicationInfo(packageName , GET_META_DATA).flags;
//            String rS = getPackageResourcePath();
////            String pN = pm.getInstallerPackageName(packageName);
////            Log.d("getInstallerPackageName", pN);
//            if( (flags & ApplicationInfo.FLAG_EXTRACT_NATIVE_LIBS) != 0){
//                Log.d("extrace_native_libs", String.valueOf(flags));
//            }
//        } catch (PackageManager.NameNotFoundException e) {
//            throw new RuntimeException(e);
//        }
//        // if you have access to the external storage, do whatever you need
//        if (build.version.sdk_int >= build.version_codes.r) {
//            if (environment.isexternalstoragemanager()){
//                try {
//                    parseapk("/sdcard/1.apk");
//                } catch (minsdkversionexception e) {
//                    throw new runtimeexception(e);
//                }
//            }else{
//                intent intent = new intent();
//                intent.setaction(settings.action_manage_app_all_files_access_permission);
//                uri uri = uri.fromparts("package", this.getpackagename(), null);
//                intent.setdata(uri);
//                startactivity(intent);
//            }
//        }


//        String wifi_on = Settings.Global.getString(getContentResolver(), "wifi_on");
//        Log.d("Settings" , wifi_on );



    }



    @Override
    protected void onDestroy() {
        super.onDestroy();
        mHandlerThread.quitSafely();
    }

    private void initHandler(final HandlerThread handlerThread){
        mHandler = new Handler(handlerThread.getLooper()){
            @Override
            public void handleMessage(android.os.Message msg) {
                super.handleMessage(msg);
//                binding.sampleText.setText("handleMessage");
                switch (msg.what){
                    case 1:
                        String content = (String) msg.obj;
                        binding.sampleText.setText(content);
                        break;
                    case 2:
                        break;
                    default:
                        break;
                }
            }
        };

    }

    /**
     * A native method that is implemented by the 'myapplication' native library,
     * which is packaged with this application.
     */

    public native String stringFromJNI2();
//    public native String getSig(String path);
}