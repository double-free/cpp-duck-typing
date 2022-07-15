FROM arm64v8/gcc

RUN sed -i "s@http://deb.debian.org@https://mirror.sjtu.edu.cn@g" /etc/apt/sources.list && apt-get update
RUN apt-get -y --no-install-recommends install \
      cmake \
      clang-format \
      gdb \
      libgtest-dev \
      less

WORKDIR /app/build
CMD ["bash"]
