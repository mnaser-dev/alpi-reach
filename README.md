# AlpiReach

**AlpiReach** is a tiny C-based utility to test **network reachability** from within **Alpine Linux** or other minimal containers. It allows you to check whether a given **host** and **port** are reachable over TCP, similar to tools like `telnet`, `nc`, or `curl`, but without their heavy dependencies.

> 🟢 Lightweight • 🐳 Container-Friendly • 📦 No Dependencies • 🧱 Ideal for Alpine Linux

---

## 🔧 Features

- ✅ Written in **C**, compiles to a **small static binary**
- ✅ Designed for **minimalist environments** like **Alpine Linux**
- ✅ Works in **Docker** containers, Kubernetes, embedded devices
- ✅ Clear success/failure messages
- ✅ Supports both **IPv4** and **IPv6**
- ✅ Non-blocking socket with a **3-second timeout**

---

## 📦 Usage

```bash
alpireach [optional:timeout_seconds, default:3] <hostname> <port>
```

**Example:**

```bash
alpireach google.com 443
```

**Output:**

```
SUCCESS: Port 443 on google.com is reachable.
```

or

```
FAILURE: Could not connect to google.com on port 443.
```

---

## 🏗️ Build Instructions

To compile the tool:

```bash
cd scripts
./build.sh <alpine-image-version>
```

### Example:

```bash
cd scripts
./build.sh 3.20
```

This will generate a **static binary** under folder: `build` that you can copy into **Alpine-based Docker containers**.

The output executable binary file name will be: alpireach\_\<alpine-image-version> , example: alpireach_3.20 , you can rename it, if needed.

> 💡 Static linking ensures no dependency mismatch.

to validate run `./test`

```bash
cd scripts
./test.sh <alpine-image-version>
```

### Example:

```bash
cd scrips
./test.sh 3.20
```

the test script will run the compiled binary inside an alpine container with given version

---

## 🐳 Using in Docker/Containers

Copy the binary into your container image or volume:

Example:

```dockerfile
COPY ./alpireach_3.20 /usr/local/bin/
RUN chmod +x /usr/local/bin/alpireach_3.20
```

Or during runtime:

Example:

```bash
docker cp alpireach_3.20 mycontainer:/usr/bin/
```

---

## 🧪 Use Cases

- Checking service availability in **Alpine containers**
- Lightweight **CI/CD checks**
- Kubernetes **liveness/readiness probes**
- Debugging **network DNS/TCP issues** from inside containers

---

## 🔍 Keywords

`alpine network check` • `minimal connectivity tool` • `alpine tcp check`
`lightweight telnet alternative` • `network reachability alpine linux`
`tiny tcp port check container` • `static binary alpine linux`
`docker health check tool` • `minimal C network utility`

---

## ⚖️ License

MIT License. No warranty. Use at your own risk.
See [LICENSE](LICENSE) file for details.

---

## 🤝 Contributions

PRs, issues, and improvements are welcome!
