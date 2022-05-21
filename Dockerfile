##################################################
# Section 1: Build the application

FROM ubuntu:18.04 as builder
MAINTAINER Christian Berger christian.berger@gu.se

ARG DEBIAN_FRONTEND=noninteractive


#Update package list, upgrade ubuntu and then add dependencies.
RUN apt-get update -y && \
        apt-get upgrade -y && \
        apt-get dist-upgrade && \
        apt-get install -y --no-install-recommends \
        cmake \
        build-essential \
        gcovr \
        ca-certificates \
        clang-tools \
        libopencv-dev

ADD . /opt/sources
ADD src /opt/sources
WORKDIR /opt/sources

RUN cd /opt/sources && \
    mkdir build && \
    cd build && mkdir scan && \
    cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/tmp .. && \
    make && make install && make test && cp lightning /tmp && \
    scan-build -o ./scan --keep-empty make && cp -r scan /tmp && \
    gcovr -r .. --xml-pretty --exclude-unreachable-branches -e ".*/.hpp" --print-summary -o coverage.xml && cp coverage.xml /tmp && \
    gcovr -r .. --html --exclude-unreachable-branches -e ".*/.hpp" --print-summary -o cov.html && cp cov.html /tmp

##################################################
# Section 2: Bundle the application.

#As we have a statically linked binary we can execute it "distroless", make the image from "scratch", meaning empty. Update and upgrade instructions were removed, as they are unnecessary and would also not work in a distroless image
FROM ubuntu:18.04 as main
MAINTAINER Christian Berger christian.berger@gu.se

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y && \   
        apt-get install -y --no-install-recommends \
        libopencv-core3.2 \
        libopencv-highgui3.2 \
        libopencv-imgproc3.2


WORKDIR /usr/bin
COPY --from=builder /tmp/bin/lightning .
# This is the entrypoint when starting the Docker container; hence, this Docker image is automatically starting our software on its creation
COPY --from=builder /tmp/coverage.xml .
COPY --from=builder /tmp/cov.html .
COPY --from=builder /tmp/scan ./scan
ENTRYPOINT ["/usr/bin/lightning"]
