FROM monkeydb:base as builder


COPY . /usr/src/project
WORKDIR /usr/src/project/build_coverage

RUN pwd
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make coverage