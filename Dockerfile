FROM alpine:X.XX

# Install build tools (gcc, make, libc-dev)
RUN apk add --no-cache build-base
WORKDIR /app
COPY src/alpi_reach.c .
RUN gcc -static -o alpireach alpi_reach.c -Wall -O2