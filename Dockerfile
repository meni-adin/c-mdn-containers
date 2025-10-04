FROM ubuntu

RUN echo "APT::Get::Assume-Yes \"true\";\nAPT::Get::force-yes \"true\";\n" > /etc/apt/apt.conf

RUN apt update
RUN apt install cmake
RUN apt install g++
RUN apt install gcc
RUN apt install git
RUN apt install ninja-build
RUN apt install python3
RUN apt install valgrind
RUN apt install clang-tidy

ARG REPO_NAME=c-mdn-containers

WORKDIR /root/
COPY . ${REPO_NAME}

ARG CACHEBUST=1
RUN git clone --depth 1 https://github.com/google/googletest.git _github.com/google/googletest

WORKDIR /root/${REPO_NAME}
CMD ["python3", "scripts/build_and_test.py"]
