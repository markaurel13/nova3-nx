#include "jni.h"
#include "util.h"
#include <string.h>

static jvalue jni_empty_string(JObj *self, const jvalue *args, const JMethod *m) {
    jvalue r; r.l = jni_str(""); return r;
}

static jvalue jni_return_1(JObj *self, const jvalue *args, const JMethod *m) {
    jvalue r; r.i = 1; return r;
}

static jvalue jni_return_0(JObj *self, const jvalue *args, const JMethod *m) {
    jvalue r; r.i = 0; return r;
}

static jvalue jni_return_1280(JObj *self, const jvalue *args, const JMethod *m) {
    jvalue r; r.i = 1280; return r;
}

static jvalue jni_return_720(JObj *self, const jvalue *args, const JMethod *m) {
    jvalue r; r.i = 720; return r;
}

static jvalue jni_return_sd_folder(JObj *self, const jvalue *args, const JMethod *m) {
    jvalue r; r.l = jni_str("sdmc:/switch/nova3/"); return r;
}

static jvalue jni_return_obb_folder(JObj *self, const jvalue *args, const JMethod *m) {
    jvalue r; r.l = jni_str("sdmc:/switch/nova3/obb/"); return r;
}

static jvalue jni_return_save_folder(JObj *self, const jvalue *args, const JMethod *m) {
    jvalue r; r.l = jni_str("sdmc:/switch/nova3/save/"); return r;
}

static jvalue jni_return_1070(JObj *self, const jvalue *args, const JMethod *m) { jvalue r; r.l = jni_str("1070"); return r; }
static jvalue jni_return_en(JObj *self, const jvalue *args, const JMethod *m) {
    jvalue r; r.l = jni_str("en"); return r;
}

static jvalue jni_return_float_1024(JObj *self, const jvalue *args, const JMethod *m) {
    jvalue r; r.f = 1024.0f; return r;
}
static jvalue jni_return_float_2(JObj *self, const jvalue *args, const JMethod *m) {
    jvalue r; r.f = 2.0f; return r;
}
static jvalue jni_return_null(JObj *self, const jvalue *args, const JMethod *m) {
    jvalue r; r.l = NULL; return r;
}

#define M_STR(NAME) { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", NAME, "()Ljava/lang/String;", jni_empty_string }
#define M_STR_SIG(NAME, SIG) { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", NAME, SIG, jni_empty_string }
#define M_INT_1(NAME) { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", NAME, "()I", jni_return_1 }
#define M_INT_0(NAME) { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", NAME, "()I", jni_return_0 }
#define M_INT_SIG_0(NAME, SIG) { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", NAME, SIG, jni_return_0 }
#define M_BOOL_1(NAME) { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", NAME, "()Z", jni_return_1 }
#define M_BOOL_0(NAME) { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", NAME, "()Z", jni_return_0 }
#define M_BOOL_SIG_0(NAME, SIG) { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", NAME, SIG, jni_return_0 }

const JMethodDef jni_method_defs[] = {
    { "java/lang/String", "getBytes", "()[B", jni_return_null },
    { "java/lang/String", "length", "()I", jni_return_0 },
    M_STR("GetUserID"),
    M_STR("GetPushNotificationType"),
    M_STR("GetCPUPartInfo"),
    M_STR("GetDeviceFirmware"),
    M_STR("GetDeviceIdentifier"),
    { "com/gameloft/android/ANMP/GloftN3HM/GL2JNIActivity", "getVersion", "()Ljava/lang/String;", jni_return_1070 }, 
    { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", "GetUDID", NULL, jni_return_1070 },
    M_STR("GetPhoneIP"),
    M_STR("GetPhoneMAC"),
    M_STR("getAndroidId"),
    M_STR("getAndroidID"),
    M_STR("getIMEI"),
    M_STR("getSerial"),
    M_STR("getSerialNo"),
    M_STR("getDeviceFirmware"),
    M_STR("getMacAddress"),
    M_STR("getDeviceIMEI"),
    M_STR("getHDIDFV"),
    M_STR("getHDIDFVVersion"),
    M_STR("getGoogleAdId"),
    M_STR("d1"),
    M_STR("getDeviceName"),
    M_STR("getPhoneManufacturer"),
    M_STR("getPhoneModel"),
    M_STR("getPhoneDevice"),
    M_STR("getPhoneProduct"),
    M_STR("retrieveDeviceCarrier"),
    M_STR("retrieveDeviceCountry"),
    M_STR("retrieveDeviceRegion"),
    M_STR("retrieveDeviceLanguage"),
    M_STR("getPhoneCarrier"),
    M_STR("retrieveCPUSerial"),
    M_STR("GetProfilesStr"),
    M_STR("GetResProfileName"),
    M_STR("GetSimCountryCode"),
    M_STR("getDeviceUserAgent"),
    
    { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", "getSaveFolder", "()Ljava/lang/String;", jni_return_save_folder },
    { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", "getSDFolder", "()Ljava/lang/String;", jni_return_sd_folder },
    { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", "GetSDFolder", "()Ljava/lang/String;", jni_return_sd_folder },
    { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", "getOBBFolder", "()Ljava/lang/String;", jni_return_obb_folder },
    { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", "getOBBFullPath", "()Ljava/lang/String;", jni_return_obb_folder },

    M_STR("getRegionFormat"),
    M_STR("getLocaleCountry"),
    M_STR("getLocaleLanguage"),
    { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", "GetPhoneLanguage", "()Ljava/lang/String;", jni_return_en },
    M_STR("getRedirectUrl"),
    M_STR("getGameAPIAchivementID"),
    M_STR("getGameAPILeaderboardID"),
    M_STR("getPackageName"),
    
    // New ones from log:
    M_STR_SIG("GetUDID", "(Z)Ljava/lang/String;"),
    M_STR_SIG("GetMAC", "(I)Ljava/lang/String;"),
    M_STR("GetPhoneModel"),
    { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", "GetDeviceName", "()Ljava/lang/String;", jni_return_1070 },
    M_STR("GetDeviceFirmware"),

    { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", "GetWindowWidth", "()I", jni_return_1280 },
    { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", "GetWindowHeight", "()I", jni_return_720 },
        { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", "GetPhoneWidth", "()I", jni_return_1280 },
        { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", "GetPhoneHeight", "()I", jni_return_720 },
    M_INT_0("GetFreeSpaceInKBytes"),
    M_INT_1("GetMaxCPUCore"),
    M_INT_0("ComputeNumUnreadNews"),
    M_INT_0("ParseWSLang"),
    M_INT_0("getGoogleAdIdStatus"),
    M_INT_0("HasPushNotification"),
    
    M_INT_1("GetPhoneCPUCores"),
    M_INT_0("UseEffectSpec"),
    M_INT_0("IsWifiEnabled"),
    M_INT_0("Is3gEnabled"),
    M_INT_0("HasGyroscope"),
    M_INT_0("IsKeyboardVisible"),
    M_INT_SIG_0("GetInt", "(Ljava/lang/String;I)I"),

    M_BOOL_1("HasConnectivity"),
    M_BOOL_1("isSlideEnabled"),
    M_BOOL_0("IsMobileConnection"),
    M_BOOL_0("IsOpenIGP"),
    M_BOOL_0("initTV"),
    M_BOOL_0("isZEUSDevice"),
    M_BOOL_0("FinishLoadWS"),
    M_BOOL_0("setCurrentContext"),
    M_BOOL_0("IsHTCPhone"),
    M_BOOL_0("IsTegra4"),
    M_BOOL_0("IsAmazonNub"),
    M_BOOL_0("IsNewDay"),
    M_BOOL_SIG_0("IsFirstTimeLaunch", "(Ljava/lang/String;)Z"),
    
    // Returns byte array
    { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", "GetFileFromURL", "(Ljava/lang/String;)[B", jni_return_null },
    { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", "GetKeyboardText", "()[B", jni_return_null },

    { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", "GetPhoneMemory", "()F", jni_return_float_1024 },
    { "com/gameloft/android/ANMP/GloftN3HM/GL2JNILib", "GetPhoneCPUFreq", "()F", jni_return_float_2 },
    { NULL, NULL, NULL, NULL }
};

const JFieldDef jni_field_defs[] = { {NULL, NULL, NULL, 0} };
const char *const jni_class_supers[][2] = { {NULL, NULL} };
const char *const jni_missing_classes[] = { NULL };
