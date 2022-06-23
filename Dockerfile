FROM arm64v8/gcc:12.1

RUN sed -i "s@http://deb.debian.org@https://mirror.sjtu.edu.cn@g" /etc/apt/sources.list && apt-get update
RUN apt-get -y --no-install-recommends install \
      cmake \
      clang-format \
      gdb \
      less

WORKDIR /app/build
CMD ["bash"]
