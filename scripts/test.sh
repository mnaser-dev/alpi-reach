# Check if argument is provided
if [ -z "$1" ]; then
  echo "Usage: $0 <alpine version, e.g: 3.20>"
  exit 1
fi
docker run -it -v $(pwd)/../build:/app alpine:$1 /app/alpireach_$1 google.com 443
