#include "canvas_context.hpp"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <gbm.h>
#include <xf86drm.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

namespace plexy::ui {



struct CanvasContext::GpuState {
    int drm_fd = -1;
    struct gbm_device* gbm_dev = nullptr;
    struct gbm_surface* gbm_surface = nullptr;
    struct gbm_bo* current_bo = nullptr;
    struct gbm_bo* previous_bo = nullptr;

    EGLDisplay egl_display = EGL_NO_DISPLAY;
    EGLContext egl_context = EGL_NO_CONTEXT;
    EGLSurface egl_surface = EGL_NO_SURFACE;
    EGLConfig egl_config = nullptr;

    bool owns_egl = false;  

    int dmabuf_fd = -1;
    uint32_t stride = 0;
    uint32_t format = 0;
    uint64_t modifier = 0;
};




bool CanvasContext::init_gpu(const char* drm_device) {
    gpu_ = new GpuState();

    
    const char* device = drm_device ? drm_device : "/dev/dri/renderD128";
    gpu_->drm_fd = open(device, O_RDWR);
    if (gpu_->drm_fd < 0) {
        fprintf(stderr, "[Canvas] Failed to open DRM device: %s\n", device);
        delete gpu_; gpu_ = nullptr;
        return false;
    }

    
    gpu_->gbm_dev = gbm_create_device(gpu_->drm_fd);
    if (!gpu_->gbm_dev) {
        fprintf(stderr, "[Canvas] Failed to create GBM device\n");
        close(gpu_->drm_fd);
        delete gpu_; gpu_ = nullptr;
        return false;
    }

    
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =
        (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");

    if (eglGetPlatformDisplayEXT) {
        gpu_->egl_display = eglGetPlatformDisplayEXT(
            EGL_PLATFORM_GBM_MESA, gpu_->gbm_dev, nullptr);
    }
    if (gpu_->egl_display == EGL_NO_DISPLAY) {
        gpu_->egl_display = eglGetDisplay((EGLNativeDisplayType)gpu_->gbm_dev);
    }

    if (gpu_->egl_display == EGL_NO_DISPLAY) {
        fprintf(stderr, "[Canvas] Failed to get EGL display\n");
        gbm_device_destroy(gpu_->gbm_dev);
        close(gpu_->drm_fd);
        delete gpu_; gpu_ = nullptr;
        return false;
    }

    EGLint major, minor;
    if (!eglInitialize(gpu_->egl_display, &major, &minor)) {
        fprintf(stderr, "[Canvas] Failed to initialize EGL\n");
        gbm_device_destroy(gpu_->gbm_dev);
        close(gpu_->drm_fd);
        delete gpu_; gpu_ = nullptr;
        return false;
    }

    eglBindAPI(EGL_OPENGL_API);

    
    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
    };

    EGLint num_configs;
    if (!eglChooseConfig(gpu_->egl_display, config_attribs, &gpu_->egl_config, 1, &num_configs) ||
        num_configs == 0) {
        fprintf(stderr, "[Canvas] Failed to choose EGL config\n");
        eglTerminate(gpu_->egl_display);
        gbm_device_destroy(gpu_->gbm_dev);
        close(gpu_->drm_fd);
        delete gpu_; gpu_ = nullptr;
        return false;
    }

    
    EGLint context_attribs[] = {
        EGL_CONTEXT_MAJOR_VERSION, 3,
        EGL_CONTEXT_MINOR_VERSION, 3,
        EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
        EGL_NONE
    };

    gpu_->egl_context = eglCreateContext(gpu_->egl_display, gpu_->egl_config,
                                          EGL_NO_CONTEXT, context_attribs);
    if (gpu_->egl_context == EGL_NO_CONTEXT) {
        fprintf(stderr, "[Canvas] Failed to create EGL context\n");
        eglTerminate(gpu_->egl_display);
        gbm_device_destroy(gpu_->gbm_dev);
        close(gpu_->drm_fd);
        delete gpu_; gpu_ = nullptr;
        return false;
    }

    
    gpu_->gbm_surface = gbm_surface_create(
        gpu_->gbm_dev, (uint32_t)width_, (uint32_t)height_,
        GBM_FORMAT_ARGB8888,
        GBM_BO_USE_RENDERING | GBM_BO_USE_LINEAR);

    if (!gpu_->gbm_surface) {
        fprintf(stderr, "[Canvas] Failed to create GBM surface\n");
        eglDestroyContext(gpu_->egl_display, gpu_->egl_context);
        eglTerminate(gpu_->egl_display);
        gbm_device_destroy(gpu_->gbm_dev);
        close(gpu_->drm_fd);
        delete gpu_; gpu_ = nullptr;
        return false;
    }

    
    gpu_->egl_surface = eglCreateWindowSurface(
        gpu_->egl_display, gpu_->egl_config,
        (EGLNativeWindowType)gpu_->gbm_surface, nullptr);

    if (gpu_->egl_surface == EGL_NO_SURFACE) {
        fprintf(stderr, "[Canvas] Failed to create EGL window surface\n");
        gbm_surface_destroy(gpu_->gbm_surface);
        eglDestroyContext(gpu_->egl_display, gpu_->egl_context);
        eglTerminate(gpu_->egl_display);
        gbm_device_destroy(gpu_->gbm_dev);
        close(gpu_->drm_fd);
        delete gpu_; gpu_ = nullptr;
        return false;
    }

    
    eglMakeCurrent(gpu_->egl_display, gpu_->egl_surface,
                   gpu_->egl_surface, gpu_->egl_context);

    
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "[Canvas] GLEW init failed: %s\n", glewGetErrorString(err));
    }
    
    while (glGetError() != GL_NO_ERROR) {}

    gpu_->owns_egl = true;
    gpu_->format = GBM_FORMAT_ARGB8888;

    printf("[Canvas] GPU initialized: EGL %d.%d, %s\n",
           major, minor, glGetString(GL_RENDERER));
    
    
    init_gl();
    
    return true;
}




bool CanvasContext::init_with_egl(void* egl_display, void* egl_context) {
    gpu_ = new GpuState();
    gpu_->egl_display = (EGLDisplay)egl_display;
    gpu_->egl_context = (EGLContext)egl_context;
    gpu_->owns_egl = false;  

    printf("[Canvas] Using existing EGL context (server mode)\n");
    
    
    init_gl();
    
    return true;
}



void CanvasContext::swap_buffers() {
    if (!gpu_ || !gpu_->owns_egl) return;  
    if (!gpu_->egl_display || !gpu_->egl_surface) return;

    
    glFinish();

    
    if (!eglSwapBuffers(gpu_->egl_display, gpu_->egl_surface)) {
        fprintf(stderr, "[Canvas] eglSwapBuffers failed\n");
        return;
    }

    
    if (gpu_->previous_bo) {
        gbm_surface_release_buffer(gpu_->gbm_surface, gpu_->previous_bo);
    }

    
    gpu_->current_bo = gbm_surface_lock_front_buffer(gpu_->gbm_surface);
    if (!gpu_->current_bo) {
        fprintf(stderr, "[Canvas] Failed to lock front buffer\n");
        return;
    }

    
    if (gpu_->dmabuf_fd >= 0) {
        close(gpu_->dmabuf_fd);
        gpu_->dmabuf_fd = -1;
    }

    
    gpu_->dmabuf_fd = gbm_bo_get_fd(gpu_->current_bo);
    gpu_->stride = gbm_bo_get_stride(gpu_->current_bo);
    gpu_->format = gbm_bo_get_format(gpu_->current_bo);
    gpu_->modifier = gbm_bo_get_modifier(gpu_->current_bo);

    gpu_->previous_bo = gpu_->current_bo;
}



void CanvasContext::resize_gpu(int width, int height) {
    if (!gpu_ || !gpu_->owns_egl) return;
    
    
    if (gpu_->current_bo) {
        gbm_surface_release_buffer(gpu_->gbm_surface, gpu_->current_bo);
        gpu_->current_bo = nullptr;
    }
    if (gpu_->previous_bo) {
        gbm_surface_release_buffer(gpu_->gbm_surface, gpu_->previous_bo);
        gpu_->previous_bo = nullptr;
    }
    
    
    if (gpu_->dmabuf_fd >= 0) {
        close(gpu_->dmabuf_fd);
        gpu_->dmabuf_fd = -1;
    }
    
    
    if (gpu_->egl_surface != EGL_NO_SURFACE) {
        eglDestroySurface(gpu_->egl_display, gpu_->egl_surface);
        gpu_->egl_surface = EGL_NO_SURFACE;
    }
    
    
    if (gpu_->gbm_surface) {
        gbm_surface_destroy(gpu_->gbm_surface);
        gpu_->gbm_surface = nullptr;
    }
    
    
    gpu_->gbm_surface = gbm_surface_create(
        gpu_->gbm_dev, (uint32_t)width, (uint32_t)height,
        GBM_FORMAT_ARGB8888,
        GBM_BO_USE_RENDERING | GBM_BO_USE_LINEAR);
    
    if (!gpu_->gbm_surface) {
        fprintf(stderr, "[Canvas] Failed to recreate GBM surface for resize\n");
        return;
    }
    
    
    gpu_->egl_surface = eglCreateWindowSurface(
        gpu_->egl_display, gpu_->egl_config,
        (EGLNativeWindowType)gpu_->gbm_surface, nullptr);
    
    if (gpu_->egl_surface == EGL_NO_SURFACE) {
        fprintf(stderr, "[Canvas] Failed to recreate EGL surface for resize\n");
        gbm_surface_destroy(gpu_->gbm_surface);
        gpu_->gbm_surface = nullptr;
        return;
    }
    
    
    eglMakeCurrent(gpu_->egl_display, gpu_->egl_surface,
                   gpu_->egl_surface, gpu_->egl_context);
    
    
    glViewport(0, 0, width, height);
}



int CanvasContext::get_dmabuf_fd() const {
    if (!gpu_ || !gpu_->current_bo) return -1;
    return gpu_->dmabuf_fd;
}

uint32_t CanvasContext::get_stride() const {
    if (!gpu_ || !gpu_->current_bo) return (uint32_t)(width_ * 4);
    return gpu_->stride;
}

uint32_t CanvasContext::get_format() const {
    if (!gpu_) return 0x34325258; 
    return gpu_->format;
}

uint64_t CanvasContext::get_modifier() const {
    if (!gpu_ || !gpu_->current_bo) return 0;
    return gpu_->modifier;
}

} 
