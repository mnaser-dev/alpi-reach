#!/bin/bash

# Check if argument is provided
if [ -z "$1" ]; then
  echo "Usage: $0 <alpine version, e.g: 3.20>"
  exit 1
fi

# Replace the first line of 'myfile' with the new FROM line
sed -i "1s|.*|FROM alpine:$1|" ../Dockerfile
mkdir -p ../build
docker build -t alpine-reach-builder ..
docker create --name temp-container-alpine-reach-builder alpine-reach-builder
docker cp temp-container-alpine-reach-builder:/app/alpireach $(pwd)/../build/alpireach_$1
docker rm temp-container-alpine-reach-builder