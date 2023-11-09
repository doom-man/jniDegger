package com.example.myapplication;

import android.util.Log;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;

public class VehicalInvacationHandler implements InvocationHandler{

    private  final IVehical vehical;

    public VehicalInvacationHandler(IVehical vehical) {
        this.vehical = vehical;
    }

    @Override
    public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
        Log.d("Proxy","Before invoke method: " + method.getName());
        Object invoke = method.invoke(vehical, args);
        Log.d("Proxy","After invoke method: " + method.getName());
        return invoke;
    }
}
