FROM alpine:X.XX

# Install build tools (gcc, make, libc-dev)
RUN apk add --no-cache build-base
WORKDIR /app
COPY src/alpiReach.c .
RUN gcc -static -o alpireach alpiReach.c -Wall -O2