FROM ubuntu:latest
LABEL authors="balin"

RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    libboost-all-dev \
    libtbb-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN ls -l
RUN cmake -S . -B build
RUN cmake --build build --target redis_server

EXPOSE 6379

CMD ["./build/redis_server"]