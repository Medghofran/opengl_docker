FROM alpine:3.11      

# Install all needed build deps for Mesa
RUN set -xe; \
    apk add --no-cache \
    autoconf \
    automake \
    cmake \
    bison \
    meson \
    build-base \
    libdrm \
    expat-dev \
    glew \ 
    glew-dev \ 
    xkeyboard-config \ 
    flex \
    elfutils \
    gettext \
    git \
    glproto \
    libtool \
    llvm8 \
    llvm8-dev \ 
    llvm8-libs \ 
    xrandr \
    libxrandr-dev \ 
    py-pip \ 
    py-mako \
    xorg-server-dev python3-dev \
    xdpyinfo \
    xvfb \ 
    expat \ 
    zlib-dev;

RUN pip install Mako

RUN set -xe; \
    mkdir -p /var/tmp/build; \
    cd /var/tmp/build; \
    git clone https://gitlab.freedesktop.org/mesa/mesa.git;


# BUILD MESA (SOFTWARE RENDERING DRIVERS).
RUN set -xe; \
    cd /var/tmp/build/mesa; \
    git checkout mesa-19.0.0; \
    libtoolize; \
    autoreconf --install; \
    ./configure \
    --enable-glx=gallium-xlib \
    --with-gallium-drivers=swrast,swr \
    --enable-dri \
    --enable-gbm \
    --enable-egl \
    --disable-glx \
    --enable-gallium-osmesa \
    --enable-autotools \
    --enable-llvm \
    --with-llvm-prefix=/usr/lib/llvm8 \
    --prefix=/usr/local; \
    make -j12; \
    make install;

# COPY ACTIVATE DISPLAY ENTRY POINT SCRIPT.
COPY ./config/entrypoint.sh /usr/local/bin/entrypoint.sh


# SETUP DISPLAY ENV VARS.
ENV DISPLAY=":99" \
    GALLIUM_DRIVER="llvmpipe" \
    LIBGL_ALWAYS_SOFTWARE="1" \
    LP_DEBUG="" \
    LP_NO_RAST="false" \
    LP_NUM_THREADS="" \
    LP_PERF="" \
    MESA_VERSION="${MESA_VERSION}" \
    XVFB_WHD="1920x1080x24"

# COPY THE SOURCE CODE TO THE HOME FOLDER 
COPY ./src /home/root/src/
COPY ./CMakeLists.txt /home/root/CMakeLists.txt

COPY ./config/basic /usr/share/X11/xkb/compat/basic

# ACTIVATE THE DISPLAY SERVER AT LOGIN.
RUN chmod +x /usr/local/bin/entrypoint.sh
# ENTRYPOINT ["sudo /usr/local/bin/entrypoint.sh"]
