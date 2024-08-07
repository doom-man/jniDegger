package com.ano.gshell;

import android.app.Application;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.os.Build;
import android.text.TextUtils;
import java.io.File;
import java.lang.reflect.Field;

/* loaded from: classes.dex */
public class AnoApplication{
    /* JADX WARN: Failed to parse debug info
    java.lang.IllegalArgumentException: newPosition > limit: (28505960 > 6565264)
    	at java.base/java.nio.Buffer.createPositionException(Unknown Source)
    	at java.base/java.nio.Buffer.position(Unknown Source)
    	at java.base/java.nio.ByteBuffer.position(Unknown Source)
    	at jadx.plugins.input.dex.sections.SectionReader.absPos(SectionReader.java:82)
    	at jadx.plugins.input.dex.sections.debuginfo.DebugInfoParser.process(DebugInfoParser.java:84)
    	at jadx.plugins.input.dex.sections.DexCodeReader.getDebugInfo(DexCodeReader.java:118)
    	at jadx.core.dex.nodes.MethodNode.getDebugInfo(MethodNode.java:626)
    	at jadx.core.dex.visitors.debuginfo.DebugInfoAttachVisitor.visit(DebugInfoAttachVisitor.java:39)
     */


    /* JADX WARN: Failed to parse debug info
    java.lang.IllegalArgumentException: newPosition > limit: (28506070 > 6565264)
    	at java.base/java.nio.Buffer.createPositionException(Unknown Source)
    	at java.base/java.nio.Buffer.position(Unknown Source)
    	at java.base/java.nio.ByteBuffer.position(Unknown Source)
    	at jadx.plugins.input.dex.sections.SectionReader.absPos(SectionReader.java:82)
    	at jadx.plugins.input.dex.sections.debuginfo.DebugInfoParser.process(DebugInfoParser.java:84)
    	at jadx.plugins.input.dex.sections.DexCodeReader.getDebugInfo(DexCodeReader.java:118)
    	at jadx.core.dex.nodes.MethodNode.getDebugInfo(MethodNode.java:626)
    	at jadx.core.dex.visitors.debuginfo.DebugInfoAttachVisitor.visit(DebugInfoAttachVisitor.java:39)
     */
    public static String dec(String str) {
        if (TextUtils.isEmpty(str)) {
            return "";
        }
        char[] charArray = str.toCharArray();
        for (int i10 = 0; i10 < charArray.length; i10++) {
            charArray[i10] = (char) (charArray[i10] ^ '\n');
        }
        return String.valueOf(charArray);
    }

    private final native int initialize(String str, String str2, String str3, String str4, String[] strArr, String str5);

    /* JADX INFO: Access modifiers changed from: protected */
    /* JADX WARN: Failed to parse debug info
    java.lang.IllegalArgumentException: newPosition > limit: (28506200 > 6565264)
    	at java.base/java.nio.Buffer.createPositionException(Unknown Source)
    	at java.base/java.nio.Buffer.position(Unknown Source)
    	at java.base/java.nio.ByteBuffer.position(Unknown Source)
    	at jadx.plugins.input.dex.sections.SectionReader.absPos(SectionReader.java:82)
    	at jadx.plugins.input.dex.sections.debuginfo.DebugInfoParser.process(DebugInfoParser.java:84)
    	at jadx.plugins.input.dex.sections.DexCodeReader.getDebugInfo(DexCodeReader.java:118)
    	at jadx.core.dex.nodes.MethodNode.getDebugInfo(MethodNode.java:626)
    	at jadx.core.dex.visitors.debuginfo.DebugInfoAttachVisitor.visit(DebugInfoAttachVisitor.java:39)
     */
    public void attachBaseContext(Context context) {
   }
}