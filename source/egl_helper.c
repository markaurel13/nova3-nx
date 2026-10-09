#define EGL_NO_X11
#include <EGL/egl.h>
#include <switch.h>
#include <stdio.h>

extern void debugPrintf(const char* fmt, ...);
extern void dcr_window_prepare(void);

static EGLDisplay s_display;
static EGLSurface s_surface;
static EGLContext s_context;

int egl_init_port(void) {
    debugPrintf("[egl] Starting EGL initialization...\n");
    extern void log_console_close(void);
    log_console_close();
    dcr_window_prepare();

    s_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (!s_display) { debugPrintf("[egl] eglGetDisplay failed\n"); return 0; }
    
    EGLint maj, min;
    if (!eglInitialize(s_display, &maj, &min)) { debugPrintf("[egl] eglInitialize failed\n"); return 0; }
    eglBindAPI(EGL_OPENGL_ES_API);
    
    static const EGLint cfg_attrs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_DEPTH_SIZE, 24, EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };
    EGLConfig cfg;
    EGLint n;
    if (!eglChooseConfig(s_display, cfg_attrs, &cfg, 1, &n) || n < 1) { debugPrintf("[egl] eglChooseConfig failed\n"); return 0; }
    
    s_surface = eglCreateWindowSurface(s_display, cfg, (EGLNativeWindowType)nwindowGetDefault(), NULL);
    if (!s_surface) { debugPrintf("[egl] eglCreateWindowSurface failed\n"); return 0; }

    static const EGLint ctx_attrs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    s_context = eglCreateContext(s_display, cfg, EGL_NO_CONTEXT, ctx_attrs);
    if (!s_context) { debugPrintf("[egl] eglCreateContext failed\n"); return 0; }
    
    if (!eglMakeCurrent(s_display, s_surface, s_surface, s_context)) { debugPrintf("[egl] eglMakeCurrent failed\n"); return 0; }
    
    // Lock presentation to 30 FPS (interval = 2 on 60Hz display)
    eglSwapInterval(s_display, 2);
    
    debugPrintf("[egl] EGL initialized successfully (30 FPS VSync lock active)!\n");
    return 1;
}

void egl_set_swap_interval(int interval) {
    if (s_display) {
        eglSwapInterval(s_display, interval);
    }
}

extern void dcr_boost_first_picture(void);
extern void dcr_boost_launch_tick(void);

void egl_swap_port(void) {
    if (s_display && s_surface) {
        static int first_frame = 1;
        if (first_frame) {
            first_frame = 0;
            dcr_boost_first_picture();
        }
        eglSwapBuffers(s_display, s_surface);
        dcr_boost_launch_tick();
    }
}
