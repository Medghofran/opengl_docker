#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/glew.h>
#include <GL/glxew.h>
#include <cmath>
#include <iostream>

// display device configuration attributes
constexpr EGLint configAttribs[] = {EGL_SURFACE_TYPE,
                                    EGL_PBUFFER_BIT,
                                    EGL_BLUE_SIZE,
                                    8,
                                    EGL_GREEN_SIZE,
                                    8,
                                    EGL_RED_SIZE,
                                    8,
                                    EGL_DEPTH_SIZE,
                                    8,
                                    EGL_RENDERABLE_TYPE,
                                    EGL_OPENGL_BIT,
                                    EGL_NONE};

constexpr int pbufferWidth = 400;
constexpr int pbufferHeight = 400;
constexpr EGLint pbufferAttribs[] = {
    EGL_WIDTH, pbufferWidth, EGL_HEIGHT, pbufferHeight, EGL_NONE,
};

bool init_dpy() {
  // get available display servers using egl
  static const int MAX_DEVICES = 4;
  EGLDeviceEXT eglDevs[MAX_DEVICES];
  EGLint numDevices;
  PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT = // manually loading functions
      (PFNEGLQUERYDEVICESEXTPROC)eglGetProcAddress("eglQueryDevicesEXT");

  eglQueryDevicesEXT(MAX_DEVICES, eglDevs, &numDevices);
  std::cout << "detected " << numDevices << " devices" << std::endl;

  auto eglGetPlatformDisplayEXT =
      (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress(
          "eglGetPlatformDisplayEXT");

  // 1. initialize egl
  EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (eglDpy == EGL_NO_DISPLAY) {
    std::cout << "NO EGL DISPLAY WAS FOUND" << std::endl;
    return false;
  }
  EGLint major, minor;
  auto _init_res = eglInitialize(eglDpy, &major, &minor);
  if (!_init_res) {
    std::cout << "FAILED TO INITIALIZE EGL" << std::endl;
    return false;
  }
  // 2. choose the available configuration from egl devices
  EGLint numConfigs;
  EGLConfig eglCfg;
  eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);

  EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg, pbufferAttribs);

  // 4. bind opengl API
  if (!eglBindAPI(EGL_OPENGL_API)) {
    std::cerr << "FAILED BINDING THE OPENGL API" << std::endl;
    return false;
  }

  // 5. Create a context and make it current
  EGLint ctxattrib[] = {EGL_CONTEXT_MAJOR_VERSION,
                        1,
                        EGL_CONTEXT_MINOR_VERSION,
                        5,
                        EGL_CONTEXT_OPENGL_PROFILE_MASK,
                        EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
                        EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE,
                        EGL_TRUE};
  EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT, nullptr);
  if (eglCtx == EGL_NO_CONTEXT) {
    auto error_code = eglGetError();
    std::cout << "FAILED TO CREATE CONTEXT " << error_code << std::endl;
    return false;
  }
  auto result = eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);
  if (!result) {
    std::cout << "FAILED TO MAKE CONTEXT CURRENT" << std::endl;
    return false;
  }

  std::cout << "GL VERSION IN USE: " << glGetString(GL_VERSION) << std::endl;

  // 6. intialize glew and use opengl functions
  glewExperimental = GL_TRUE;
  auto init_result = glewInit();
  if (init_result != GLEW_OK) {
    std::cout << "GLEW INITIALIZATION ERROR::" << init_result << std::endl;
    printf("GLEW_ERROR:%s\n", glewGetErrorString(init_result));
  }

  return true;
}

// draw params
GLuint fb, color, depth;
float const light_dir[] = {1, 1, 1, 0};
float const light_color[] = {1, 0.95, 0.9, 1};

// create buffers to render to
void init() {
  // create the framebuffer
  glGenFramebuffers(1, &fb);
  glBindFramebuffer(GL_FRAMEBUFFER, fb);
  // create the texture buffer
  glGenTextures(1, &color);
  glBindTexture(GL_TEXTURE_2D, color);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pbufferWidth, pbufferHeight, 0, GL_RGB,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         color, 0);
  // create the render buffer
  glGenRenderbuffers(1, &depth);
  glBindRenderbuffer(GL_RENDERBUFFER, depth);
}

void draw() {
  glBindFramebuffer(GL_FRAMEBUFFER, fb);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, color);

  glViewport(0, 0, pbufferWidth, pbufferHeight);

  std::cout << "testing shader creation" << std::endl;
  unsigned int _gShader = glCreateShader(GL_VERTEX_SHADER);
  std::cout << "shader id " << _gShader << std::endl;

  glBegin(GL_POINTS);
  glVertex3d(0.5, 0.5, 0.0);
  glVertex3d(-0.5, 0.5, 0.0);
  glVertex3d(0.5, -0.5, 0.0);
  glVertex3d(-0.5, -0.5, 0.0);
  glEnd();

  glClearColor(0.2, 0.8, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void to_bmp(unsigned char *_data) {

  FILE *f;
  int w = 400, h = 400;
  int filesize =
      54 + 3 * w * h; // w is your image width, h is image height, both int
  unsigned char bmpfileheader[14] = {'B', 'M', 0, 0,  0, 0, 0,
                                     0,   0,   0, 54, 0, 0, 0};
  unsigned char bmpinfoheader[40] = {40, 0, 0, 0, 0, 0, 0,  0,
                                     0,  0, 0, 0, 1, 0, 24, 0};
  unsigned char bmppad[3] = {0, 0, 0};
  bmpfileheader[2] = (unsigned char)(filesize);
  bmpfileheader[3] = (unsigned char)(filesize >> 8);
  bmpfileheader[4] = (unsigned char)(filesize >> 16);
  bmpfileheader[5] = (unsigned char)(filesize >> 24);
  bmpinfoheader[4] = (unsigned char)(w);
  bmpinfoheader[5] = (unsigned char)(w >> 8);
  bmpinfoheader[6] = (unsigned char)(w >> 16);
  bmpinfoheader[7] = (unsigned char)(w >> 24);
  bmpinfoheader[8] = (unsigned char)(h);
  bmpinfoheader[9] = (unsigned char)(h >> 8);
  bmpinfoheader[10] = (unsigned char)(h >> 16);
  bmpinfoheader[11] = (unsigned char)(h >> 24);
  f = fopen("img.bmp", "wb");
  fwrite(bmpfileheader, 1, 14, f);
  fwrite(bmpinfoheader, 1, 40, f);
  for (int i = 0; i < h; i++) {
    fwrite(_data + (w * (h - i - 1) * 3), 3, w, f);
  }
}

int main(int argc, char *argv[]) {

  if (!init_dpy()) {
    return -1;
  }

  init();
  // draw scene
  draw();

  // read scene
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fb);

  // save the output to an image
  constexpr int _SIZE = 400 * 400 * 3;
  GLubyte _pixels[_SIZE];
  std::cout << color << std::endl;
  glGetTextureImage(color, 0, GL_RGB, GL_UNSIGNED_BYTE, sizeof(_pixels),
                    _pixels);

  to_bmp(_pixels);
  return 0;
}