FROM gcc:11.2.0 as builder

# sudo 可能な docker ユーザー (グループ:docker, パスワード:docker) を作成する。
RUN groupadd docker \
    && useradd -g docker -d /home/docker -s /bin/bash docker \
    && echo "docker:docker" | chpasswd \
    && gpasswd -a docker sudo \
    && mkdir /home/docker \
    && chown docker:docker /home/docker

# タイムゾーン設定
ENV TZ=Asia/Tokyo

RUN wget https://github.com/Kitware/CMake/releases/download/v3.23.1/cmake-3.23.1-linux-x86_64.tar.gz
RUN tar xvzf cmake-3.23.1-linux-x86_64.tar.gz
RUN cd cmake-3.23.1-linux-x86_64 && cp -f bin/* /usr/bin/ && cp -rf share/* /usr/share/

RUN apt-get update; \
	apt-get -y upgrade; \
	apt-get -y install binutils-dev uuid-dev libssl-dev \
	curl git python3-pip  build-essential libssl-dev libffi-dev python3-dev doxygen

ADD . /home/docker/myapp/
WORKDIR /home/docker/myapp/
RUN chown docker:docker -R /home/docker

USER docker
ENV TERM xterm
ENV HOME /home/docker
ENV LANG C.UTF-8
RUN curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py; \
	python3 get-pip.py --user; \
	pip3 install gil --user;

# .bashrc
COPY ./docker/bashrc /home/docker/.bashrc

ENV CFLAG -Wno-error
ENV CXXFLAGS -Wno-error
RUN export CFLAGS="-Wno-error"
RUN export CXXFLAGS="-Wno-error"
RUN /home/docker/.local/bin/gil update && rm -rf temp && cd build && cat ./unix.sh && ./unix.sh


# FROM alpine:3.12

# COPY --from=builder /app/main /bin/main
# COPY . .

# RUN gcc -o myapp main.c
# CMD ["./myapp"]
