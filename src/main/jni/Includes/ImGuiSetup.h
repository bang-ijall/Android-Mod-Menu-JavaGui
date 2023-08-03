#include <dlfcn.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "ImGui/backends/imgui_impl_android.h"
#include "Dobby/dobby.h"

bool setup;
int glWidth, glHeight;

EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &glWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &glHeight);
    if (!setup) {
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float) glWidth, (float) glHeight);
        ImGui_ImplOpenGL3_Init();
        ImFontConfig font_cfg;
        font_cfg.SizePixels = 22.0f;
        io.Fonts->AddFontDefault(&font_cfg);
        setup = true;
    }
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(glWidth, glHeight);
    ImGui::NewFrame();
    DrawESP(glWidth, glHeight);
    ImGui::EndFrame();
    ImGui::Render();
    glViewport(0, 0, (int) io.DisplaySize.x, (int) io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return old_eglSwapBuffers(dpy, surface);
}

void ImGuiHook() {
    sleep(5);
    DobbyHook(dlsym(dlopen(OBFUSCATE("libEGL.so"), 4), OBFUSCATE("eglSwapBuffers")), (void *) hook_eglSwapBuffers, (void **) &old_eglSwapBuffers);
}
