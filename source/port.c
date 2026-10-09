#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <switch.h>

#include "rt_boot.h" // Provides port_load and port_run declarations
#include "so_util.h" // From android32, for so_module loading
#include "jni.h" // From android32 JNI fake layer
#include "gl_layer.h" // OpenGL ES / EGL provided by android32
#define EGL_NO_X11
#include <EGL/egl.h>
extern int egl_init_port(void);
extern void egl_swap_port(void);
#include "rt_pad.h" // Android32 gamepad input
#include "util.h" // Android32 debug logging
#include "dcr_setup.h" // Android32 setup pipeline
#include "dcr_boost.h"

#define printf debugPrintf

/* Globals */
so_module so_mod;
static int nova_installer_lock3_value = 1;
static int nova_installer_lock4_value = 1;

// JNI definitions are in port_jni.c

// Function pointers for JNI functions we need to hook (from N.O.V.A 3)
typedef int (*jni_on_load_fn)(void *jvm, void *reserved);
typedef void (*jni_void_fn)(void *env, void *clazz);
typedef void (*jni_bool_fn)(void *env, void *clazz, unsigned char value);
typedef void (*jni_int_fn)(void *env, void *clazz, int value);
typedef void (*jni_size_fn)(void *env, void *clazz, int width, int height);
typedef void (*jni_paths_fn)(void *env, void *clazz, void* obb_path, void* data_path, void* home_path, void* temp_path);
typedef void (*jni_touch_fn)(void *env, void *clazz, int action, int x, int y, int pointer_id);
typedef void (*jni_vec2_fn)(void *env, void *clazz, float x, float y);
typedef void (*jni_vec3_fn)(void *env, void *clazz, float x, float y, float z);

// Function pointers
static jni_int_fn nova_key_down;
static jni_int_fn nova_key_up;
static jni_touch_fn nova_touch;
static jni_vec2_fn nova_left_stick;
typedef void* (*nova3_level_get_fn)(void);
typedef void* (*nova3_get_player_component_fn)(void *level);
typedef void (*nova3_weapon_manager_unaim_fn)(void *manager);

static nova3_level_get_fn nova3_ads_level_get = NULL;
static nova3_get_player_component_fn nova3_ads_get_player_component = NULL;
static nova3_weapon_manager_unaim_fn nova3_ads_weapon_manager_unaim = NULL;

static void *nova3_ads_current_weapon(void **manager_output) {
    if (manager_output) *manager_output = NULL;
    if (!nova3_ads_level_get || !nova3_ads_get_player_component) return NULL;
    void *level = nova3_ads_level_get();
    if (!level) return NULL;
    uint8_t *player = (uint8_t *)nova3_ads_get_player_component(level);
    if (!player) return NULL;
    uint8_t *manager = *(uint8_t **)(player + 0x170U);
    if (!manager) return NULL;

    uint8_t *entries = *(uint8_t **)(manager + 0x148U);
    uint8_t *entries_end = *(uint8_t **)(manager + 0x14CU);
    int current_index = *(int *)(manager + 0x160U);
    if (!entries || !entries_end || (uintptr_t)entries_end < (uintptr_t)entries ||
        current_index < 0 || (current_index * 24U) >= (uintptr_t)(entries_end - entries))
        return NULL;

    if (manager_output) *manager_output = manager;
    void *w = *(void **)(entries + current_index * 24U + 12U);
    if ((uintptr_t)w < 0x10000 || (uintptr_t)w == 0xFFFFFFFF) return NULL;
    return w;
}

static int nova3_ads_shoulder_held = 0;

static int nova3_ads_hold_begin(void) {
    nova3_ads_shoulder_held = 1;
    void *weapon = nova3_ads_current_weapon(NULL);
    int already_aimed = weapon && *((uint8_t *)weapon + 0x61U) != 0U;
    return already_aimed ? 0 : 1;
}

static void nova3_ads_hold_end(void) {
    if (!nova3_ads_shoulder_held) return;
    nova3_ads_shoulder_held = 0;

    void *manager = NULL;
    void *weapon = nova3_ads_current_weapon(&manager);
    int aimed = weapon && *((uint8_t *)weapon + 0x61U) != 0U;
    if (aimed && manager && nova3_ads_weapon_manager_unaim)
        nova3_ads_weapon_manager_unaim(manager);
}
static jni_vec3_fn nova_right_stick;
static jni_void_fn native_step;
static int *nova_power_status = NULL;
static uint8_t *nova_power_status_ba = NULL;
static uint8_t *nova_moga_pro = NULL;
static uint8_t *nova_is_moga = NULL;

static uintptr_t required_symbol(const char *name) {
    uintptr_t address = so_try_find_addr_rx(&so_mod, name);
    if (!address) {
        printf("N.O.V.A. 3 is missing required export %s.\n", name);
    }
    return address;
}

// ---------------------------------------------------------
// Required hooks by android32: port_load and port_run
// ---------------------------------------------------------

int port_load(const char *apk_path) {
    printf("[port] Loading N.O.V.A. 3 APK and resolving symbols...\n");
    
    // This tells android32 to read the port_setup_plan and extract libNOVA3_neon.so
    dcr_setup_from_apk(apk_path);

    #include <stdint.h>
    int load_res = so_load(&so_mod, "libNOVA3_neon.so", NULL, SIZE_MAX);
    if (load_res != 0) {
        #include <errno.h>
        printf("[port] Failed to load libNOVA3_neon.so, error code: %d, errno: %s\n", load_res, strerror(errno));
        return -1;
    }

    so_relocate(&so_mod);
    so_resolve(&so_mod, NULL, 0, 0);
    so_fix_kuser_helpers(&so_mod);
    so_finalize(&so_mod);
    so_execute_init_array(&so_mod);

    printf("[port] N.O.V.A. 3 ARMv7 module mapped successfully.\n");
    return 0; // Success
}

static int s_game_started = 0;

int port_cpu_boost_set(int on) {
    (void)on;
    return 0; // Disable FastLoad boost completely: prevents GPU throttling to 76MHz and saves battery
}

void port_run(void) {
    appletSetCpuBoostMode(ApmCpuBoostMode_Normal);
    extern void so_patch(void);
    so_patch();

    printf("[port] Starting N.O.V.A. 3 init sequence...\n");

    // JNI call logging disabled during gameplay to eliminate MicroSD log overhead
    #include "rt_cfg.h"
    ((RtConfig*)rt_config())->log_jni = 0;

    // Initialize the android32 fake JNI subsystem
    jni_init();

    // 1. Call JNI_OnLoad
    jni_on_load_fn jni_on_load = (jni_on_load_fn)required_symbol("JNI_OnLoad");
    if (jni_on_load) {
        int jni_version = jni_on_load(g_jni_vm, NULL);
        printf("[port] JNI_OnLoad returned version 0x%08x\n", jni_version);
    }

    // 2. Resolve Gameloft Java Native Functions
    jni_paths_fn set_paths = (jni_paths_fn)required_symbol("Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_setPaths");
    jni_void_fn native_init = (jni_void_fn)required_symbol("Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_init");
    jni_void_fn native_init_gl = (jni_void_fn)required_symbol("Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_initGL");
    jni_void_fn init_view_settings = (jni_void_fn)required_symbol("Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_InitViewSettings");
    jni_size_fn native_resize = (jni_size_fn)required_symbol("Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_resize");
    jni_void_fn game_resume = (jni_void_fn)required_symbol("Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_GameResume");
    native_step = (jni_void_fn)required_symbol("Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_step");
    nova_key_down = (jni_int_fn)required_symbol("Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_OnKeyDown");
    nova_key_up = (jni_int_fn)required_symbol("Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_OnKeyUp");
    nova_touch = (jni_touch_fn)required_symbol("Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_touchEvent");
    nova_left_stick = (jni_vec2_fn)required_symbol("Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeSetPowerALeftJoystick");
    nova_right_stick = (jni_vec3_fn)required_symbol("Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeSetPowerARightJoystick");
    nova_power_status = (int *)so_try_find_addr_rx(&so_mod, "s_iPowerAStatus");
    nova_power_status_ba = (uint8_t *)so_try_find_addr_rx(&so_mod, "_ZN3glf18s_iPowerAStatus_BAE");
    nova_moga_pro = (uint8_t *)so_try_find_addr_rx(&so_mod, "_ZN3glf16b_versionMogaProE");
    nova_is_moga = (uint8_t *)so_try_find_addr_rx(&so_mod, "_ZN3glf8s_isMOGAE");
    nova3_ads_level_get = (nova3_level_get_fn)(so_mod.load_virtbase + 0x002E6248U);
    nova3_ads_get_player_component = (nova3_get_player_component_fn)(so_mod.load_virtbase + 0x002F14B4U);
    nova3_ads_weapon_manager_unaim = (nova3_weapon_manager_unaim_fn)(so_mod.load_virtbase + 0x00585FC8U);



    if (nova_power_status) *nova_power_status = 1;
    if (nova_power_status_ba) *nova_power_status_ba = 1;
    if (nova_moga_pro) *nova_moga_pro = 1;
        if (nova_is_moga) *nova_is_moga = 1;




    // Resolving inputs
    nova_key_down = (jni_int_fn)required_symbol("Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_OnKeyDown");
    nova_key_up = (jni_int_fn)required_symbol("Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_OnKeyUp");
    nova_left_stick = (jni_vec2_fn)required_symbol("Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeSetPowerALeftJoystick");
    nova_right_stick = (jni_vec3_fn)required_symbol("Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeSetPowerARightJoystick");
    nova_power_status = (int *)so_try_find_addr_rx(&so_mod, "s_iPowerAStatus");
    nova_power_status_ba = (uint8_t *)so_try_find_addr_rx(&so_mod, "_ZN3glf18s_iPowerAStatus_BAE");
    nova_moga_pro = (uint8_t *)so_try_find_addr_rx(&so_mod, "_ZN3glf16b_versionMogaProE");
    nova_is_moga = (uint8_t *)so_try_find_addr_rx(&so_mod, "_ZN3glf8s_isMOGAE");
    nova3_ads_level_get = (nova3_level_get_fn)(so_mod.load_virtbase + 0x002E6248U);
    nova3_ads_get_player_component = (nova3_get_player_component_fn)(so_mod.load_virtbase + 0x002F14B4U);
    nova3_ads_weapon_manager_unaim = (nova3_weapon_manager_unaim_fn)(so_mod.load_virtbase + 0x00585FC8U);



    if (nova_power_status) *nova_power_status = 1;
    if (nova_power_status_ba) *nova_power_status_ba = 1;
    if (nova_moga_pro) *nova_moga_pro = 1;
        if (nova_is_moga) *nova_is_moga = 1;



    // 3. Fake the DRM / Installer lock (same as PS Vita)
    int **installer_lock3 = (int **)required_symbol("lockPointer3");
    int **installer_lock4 = (int **)required_symbol("lockPointer4");
    if (installer_lock3) *installer_lock3 = &nova_installer_lock3_value;
    if (installer_lock4) *installer_lock4 = &nova_installer_lock4_value;

    // 4. Init OpenGL Context (Switch/mesa32 equivalent of gl_init())

    // 5. Fire Gameloft Init Chain
    // Use android32's actual JNI environment
    void *env = g_jni_env;
    void *gl2jni_class = jni_class("com/gameloft/android/ANMP/GloftN3HM/GL2JNILib")->obj;
    
    jni_int_fn nova_orientation = (jni_int_fn)so_try_find_addr_rx(&so_mod, "Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_orientationChanged");
    if (nova_orientation) nova_orientation(env, gl2jni_class, 2);

    if (native_init) native_init(env, gl2jni_class);
    if (native_init_gl) native_init_gl(env, gl2jni_class);
    if (init_view_settings) init_view_settings(env, gl2jni_class);

    if (set_paths) {
        printf("[port] Setting Gameloft paths...\n");
        void *obb_path  = jni_str("sdmc:/switch/nova3/obb/");
        void *data_path = jni_str("sdmc:/switch/nova3/data/files/");
        void *home_path = jni_str("sdmc:/switch/nova3/internal/");
        void *temp_path = jni_str("sdmc:/switch/nova3/cache/");
        
        set_paths(env, gl2jni_class, obb_path, data_path, home_path, temp_path);
    }
    
    if (native_resize) native_resize(env, gl2jni_class, 1280, 720); // Switch 720p resolution
    if (game_resume) game_resume(env, gl2jni_class);
    jni_bool_fn set_is_moga = (jni_bool_fn)so_try_find_addr_rx(&so_mod, "Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeSetIsMOGA");
    jni_bool_fn set_controller_power = (jni_bool_fn)so_try_find_addr_rx(&so_mod, "Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativePowerStatus");
    jni_int_fn set_moga_version = (jni_int_fn)so_try_find_addr_rx(&so_mod, "Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeMogaVersion");
    if (set_is_moga) set_is_moga(env, gl2jni_class, 1);
    if (set_controller_power) set_controller_power(env, gl2jni_class, 1);
    if (set_moga_version) set_moga_version(env, gl2jni_class, 1);


    
    printf("[port] Entering main step loop...\n");
    egl_init_port();
    s_game_started = 1;
    
    // Pre-warm GLA handle pool (3 concurrent handles per audio bank)
    extern void *b_fopen(const char *path, const char *mode);
    extern int b_fclose(void *fp);
    static const char *const prewarm_gla[] = {
        "sdmc:/switch/nova3/data/files/sounds_hi_p1.gla",
        "sdmc:/switch/nova3/data/files/sounds_hi_p2.gla",
        "sdmc:/switch/nova3/data/files/sounds_hi_p3.gla",
        "sdmc:/switch/nova3/data/files/sounds_hi_p4.gla",
        "sdmc:/switch/nova3/data/files/actors_stream.gla",
        "sdmc:/switch/nova3/data/files/weapons_stream.gla",
    };
    for (size_t i = 0; i < sizeof(prewarm_gla)/sizeof(prewarm_gla[0]); i++) {
        void *handles[3] = {NULL, NULL, NULL};
        for (int h = 0; h < 3; h++) {
            handles[h] = b_fopen(prewarm_gla[i], "rb");
        }
        for (int h = 0; h < 3; h++) {
            if (handles[h]) b_fclose(handles[h]);
        }
    }
    printf("[port] GLA handle pool pre-warmed (3 handles per audio bank).\n");
    
    static uint64_t s_frames = 0;
    while (appletMainLoop()) {
        uint64_t frame_count = ++s_frames;
        dcr_boost_frame_begin();
        
        uint64_t t_start = armGetSystemTick();
        
        // Poll inputs via android32/libnx
        static PadState pad;

        static int pad_init = 0;
        if (!pad_init) {
            padConfigureInput(1, HidNpadStyleSet_NpadStandard);
            padInitializeDefault(&pad);
            pad_init = 1;
        }
        padUpdate(&pad);
        u64 keys_down = padGetButtonsDown(&pad);
        u64 keys_up = padGetButtonsUp(&pad);
        float sticks[4];
        if (nova_power_status) *nova_power_status = 1;
        if (nova_power_status_ba) *nova_power_status_ba = 1;
        if (nova_moga_pro) *nova_moga_pro = 1;
        if (nova_is_moga) *nova_is_moga = 1;

        rt_pad_read(&pad, sticks);

        // Convert Left Stick to D-PAD (Gameloft ignored Left Joystick native binding)
        int horizontal_key = sticks[0] > 0.5f ? 22 : (sticks[0] < -0.5f ? 21 : 0);
        int vertical_key = sticks[1] > 0.5f ? 19 : (sticks[1] < -0.5f ? 20 : 0);
        
        static int sprint_macro_frame = 0;
        if (keys_down & HidNpadButton_StickL) {
            sprint_macro_frame = 1;
        }

        if (sprint_macro_frame > 0) {
            if (sprint_macro_frame >= 1 && sprint_macro_frame < 4) {
                vertical_key = 0;
            } else if (sprint_macro_frame >= 4 && sprint_macro_frame < 8) {
                vertical_key = 19;
            } else if (sprint_macro_frame >= 8 && sprint_macro_frame < 12) {
                vertical_key = 0;
            } else if (sprint_macro_frame >= 12 && sprint_macro_frame < 16) {
                vertical_key = 19;
            }
            sprint_macro_frame++;
            if (sprint_macro_frame >= 16) sprint_macro_frame = 0;
        }
        
        static int left_horizontal_key = 0;
        static int left_vertical_key = 0;

        

        if (horizontal_key != left_horizontal_key) {
            if (left_horizontal_key && nova_key_up) nova_key_up(env, gl2jni_class, left_horizontal_key);
            if (horizontal_key && nova_key_down) nova_key_down(env, gl2jni_class, horizontal_key);
            left_horizontal_key = horizontal_key;
        }
        if (vertical_key != left_vertical_key) {
            if (left_vertical_key && nova_key_up) nova_key_up(env, gl2jni_class, left_vertical_key);
            if (vertical_key && nova_key_down) nova_key_down(env, gl2jni_class, vertical_key);
            left_vertical_key = vertical_key;
        }

        // Standard Buttons
        if (nova_key_down) {
            if (keys_down & HidNpadButton_A) nova_key_down(env, gl2jni_class, 23);
            if (keys_down & HidNpadButton_B) nova_key_down(env, gl2jni_class, 227);
            if (keys_down & HidNpadButton_X) nova_key_down(env, gl2jni_class, 99);
            if (keys_down & HidNpadButton_Y) nova_key_down(env, gl2jni_class, 100);

            if (keys_down & HidNpadButton_Plus) nova_key_down(env, gl2jni_class, 108); // START
            if (keys_down & HidNpadButton_Minus) nova_key_down(env, gl2jni_class, 109); // SELECT
            if (keys_down & HidNpadButton_Up) nova_key_down(env, gl2jni_class, 19);
            if (keys_down & HidNpadButton_Down) nova_key_down(env, gl2jni_class, 20);
            if (keys_down & HidNpadButton_Left) nova_key_down(env, gl2jni_class, 21);
            if (keys_down & HidNpadButton_Right) nova_key_down(env, gl2jni_class, 22);
            if (keys_down & HidNpadButton_L) nova_key_down(env, gl2jni_class, 100);
            if (keys_down & HidNpadButton_R) nova_key_down(env, gl2jni_class, 227); // R1 (Shoot)
            if (keys_down & HidNpadButton_ZL) { if (nova3_ads_hold_begin()) { nova_key_down(env, gl2jni_class, 102); nova_key_up(env, gl2jni_class, 102); } } // L2
            if (keys_down & HidNpadButton_ZR) nova_key_down(env, gl2jni_class, 103); // R2
            if (keys_down & HidNpadButton_StickR) nova_key_down(env, gl2jni_class, 109); // R3 (Change Weapon via SELECT)
        }
        if (nova_key_up) {
            if (keys_up & HidNpadButton_A) nova_key_up(env, gl2jni_class, 23);
            if (keys_up & HidNpadButton_B) nova_key_up(env, gl2jni_class, 227);
            if (keys_up & HidNpadButton_X) nova_key_up(env, gl2jni_class, 99);
            if (keys_up & HidNpadButton_Y) nova_key_up(env, gl2jni_class, 100);

            if (keys_up & HidNpadButton_Plus) nova_key_up(env, gl2jni_class, 108);
            if (keys_up & HidNpadButton_Minus) nova_key_up(env, gl2jni_class, 109);
            if (keys_up & HidNpadButton_Up) nova_key_up(env, gl2jni_class, 19);
            if (keys_up & HidNpadButton_Down) nova_key_up(env, gl2jni_class, 20);
            if (keys_up & HidNpadButton_Left) nova_key_up(env, gl2jni_class, 21);
            if (keys_up & HidNpadButton_Right) nova_key_up(env, gl2jni_class, 22);
            if (keys_up & HidNpadButton_L) nova_key_up(env, gl2jni_class, 100);
            if (keys_up & HidNpadButton_R) nova_key_up(env, gl2jni_class, 227);
            if (keys_up & HidNpadButton_ZL) nova3_ads_hold_end();
            if (keys_up & HidNpadButton_ZR) nova_key_up(env, gl2jni_class, 103);
            if (keys_up & HidNpadButton_StickR) nova_key_up(env, gl2jni_class, 109); // R3
        }

        // Right Stick Camera (Vita acceleration with inverted Y)
        if (nova_right_stick) {
            static uint64_t right_active_since = 0;
            float r_x = sticks[2];
            float r_y = -sticks[3]; // Inverted Y-axis
            
            if (r_x != 0.0f || r_y != 0.0f) {
                uint64_t now_us = ((uint64_t)(armTicksToNs(armGetSystemTick()) / 1000));
                if (right_active_since == 0) right_active_since = now_us;
                uint64_t held_us = now_us - right_active_since;
                
                float accel = held_us >= 900000U ? 2.2f
                            : held_us >= 600000U ? 1.9f
                            : held_us >= 300000U ? 1.6f : 1.3f;
                            
                nova_right_stick(env, gl2jni_class, r_x * 7.0f, r_y * 7.0f, accel);
            } else {
                right_active_since = 0;
                nova_right_stick(env, gl2jni_class, 0.0f, 0.0f, 1.3f);
            }
        }

        
        uint64_t t_inputs = armGetSystemTick();

        HidTouchScreenState state;

        if (hidGetTouchScreenStates(&state, 1)) {
            static int last_touches = 0;
            static float last_x = 0;
            static float last_y = 0;
            if (nova_touch) {
                if (state.count > 0) {
                    for (int i = 0; i < state.count; i++) {
                        int action = (last_touches == 0) ? 0 : 2;
                        nova_touch(env, gl2jni_class, action, state.touches[i].x, state.touches[i].y, state.touches[i].finger_id);
                        last_x = state.touches[i].x;
                        last_y = state.touches[i].y;
                    }
                } else if (last_touches > 0) {
                    nova_touch(env, gl2jni_class, 1, last_x, last_y, 0); // 1=up at last coords
                }
            }
            last_touches = state.count;
        }

        
        uint64_t t_touch = armGetSystemTick();

        if (native_step) {

            native_step(env, gl2jni_class);
        }


        uint64_t t_step = armGetSystemTick();

        egl_swap_port();
        dcr_boost_frame_end(frame_count);
        
        uint64_t t_swap = armGetSystemTick();
        
        uint64_t total_us = armTicksToNs(t_swap - t_start) / 1000ULL;
        static uint64_t last_lag_log = 0;
        if (total_us > 100000ULL && (frame_count - last_lag_log > 30)) { // > 100ms threshold, rate-limited
            last_lag_log = frame_count;
            debugPrintf("[Profiler] Lag spike! Total: %llu ms | Inputs: %llu ms | Touch: %llu ms | Engine+GL: %llu ms | Swap/VSync: %llu ms\n",
                total_us / 1000ULL,
                (armTicksToNs(t_inputs - t_start) / 1000ULL) / 1000ULL,
                (armTicksToNs(t_touch - t_inputs) / 1000ULL) / 1000ULL,
                (armTicksToNs(t_step - t_touch) / 1000ULL) / 1000ULL,
                (armTicksToNs(t_swap - t_step) / 1000ULL) / 1000ULL
            );
        }

        // Periodic Performance Metrics (every 120 frames ~ 2 seconds)
        extern uint32_t g_gla_pool_hits, g_gla_pool_misses;
        static uint64_t s_perf_start = 0;
        static uint32_t s_perf_frames = 0;
        static uint64_t s_perf_total_frame_us = 0;
        static uint64_t s_perf_min_us = UINT64_MAX;
        static uint64_t s_perf_max_us = 0;
        static uint64_t s_perf_total_cpu_us = 0;
        static uint64_t s_perf_total_gpu_us = 0;
        static uint64_t s_perf_total_in_us = 0;
        static uint32_t s_perf_drops_18ms = 0;
        static uint32_t s_perf_drops_33ms = 0;
        static uint32_t s_last_hits = 0;

        if (s_perf_start == 0) s_perf_start = armGetSystemTick();

        uint64_t cpu_us = armTicksToNs(t_step - t_touch) / 1000ULL;
        uint64_t gpu_us = armTicksToNs(t_swap - t_step) / 1000ULL;
        uint64_t in_us  = armTicksToNs(t_touch - t_start) / 1000ULL;

        s_perf_frames++;
        s_perf_total_frame_us += total_us;
        s_perf_total_cpu_us += cpu_us;
        s_perf_total_gpu_us += gpu_us;
        s_perf_total_in_us += in_us;

        if (total_us < s_perf_min_us) s_perf_min_us = total_us;
        if (total_us > s_perf_max_us) s_perf_max_us = total_us;
        if (total_us > 18181ULL) s_perf_drops_18ms++;
        if (total_us > 33333ULL) s_perf_drops_33ms++;

        if (s_perf_frames >= 120) {
            uint64_t now_tick = armGetSystemTick();
            float avg_ms = (float)s_perf_total_frame_us / (float)(s_perf_frames * 1000.0f);
            float fps = (avg_ms > 0.001f) ? (1000.0f / avg_ms) : 0.0f;
            float min_ms = (float)s_perf_min_us / 1000.0f;
            float max_ms = (float)s_perf_max_us / 1000.0f;
            float cpu_ms = (float)s_perf_total_cpu_us / (float)(s_perf_frames * 1000.0f);
            float gpu_ms = (float)s_perf_total_gpu_us / (float)(s_perf_frames * 1000.0f);
            float in_ms  = (float)s_perf_total_in_us / (float)(s_perf_frames * 1000.0f);
            uint32_t hits_delta = g_gla_pool_hits - s_last_hits;

            debugPrintf("[Perf] FPS: %.1f | Frame: avg %.2f ms (min %.2f, max %.2f) | CPU: %.2f ms, GPU/VSync: %.2f ms, In: %.2f ms | Drops: %u (>18ms), %u (>33ms) | Asset Cache: +%u hits (Total: %u, Misses: %u)\n",
                fps, avg_ms, min_ms, max_ms, cpu_ms, gpu_ms, in_ms,
                s_perf_drops_18ms, s_perf_drops_33ms,
                hits_delta, g_gla_pool_hits, g_gla_pool_misses);

            s_perf_start = now_tick;
            s_perf_frames = 0;
            s_perf_total_frame_us = 0;
            s_perf_min_us = UINT64_MAX;
            s_perf_max_us = 0;
            s_perf_total_cpu_us = 0;
            s_perf_total_gpu_us = 0;
            s_perf_total_in_us = 0;
            s_perf_drops_18ms = 0;
            s_perf_drops_33ms = 0;
            s_last_hits = g_gla_pool_hits;
        }
    }

}

static const char *const nova3_libs[] = { "libNOVA3_neon.so" };
const RtSetupPlan port_setup_plan = {
  .libs = nova3_libs,
  .nlibs = 1,
  .libs_p0 = 0,
  .libs_p1 = 1000, // 0-100% of the loading bar
};

void dcr_config_load(void) {}

const char *port_path_fixup(const char *real, char *out, size_t cap) {
    if (!real || !out || cap == 0) return real;
    const char *data_prefix = "sdmc:/switch/nova3/data/";
    const char *files_prefix = "sdmc:/switch/nova3/data/files/";
    size_t data_len = strlen(data_prefix);
    size_t files_len = strlen(files_prefix);
    
    if (strncmp(real, data_prefix, data_len) == 0 && strncmp(real, files_prefix, files_len) != 0) {
        const char *sub = real + data_len;
        while (*sub == '/') sub++;
        char candidate[512];
        snprintf(candidate, sizeof(candidate), "sdmc:/switch/nova3/data/files/%s", sub);
        if (access(candidate, F_OK) == 0) {
            snprintf(out, cap, "%s", candidate);
            return out;
        }
    }
    return real;
}
