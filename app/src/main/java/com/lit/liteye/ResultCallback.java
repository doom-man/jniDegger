package com.lit.liteye;
import androidx.annotation.Keep;

@Keep
public interface ResultCallback {
    public void onSuccess(String result);
    public void onError(int code , String errorString);
}
