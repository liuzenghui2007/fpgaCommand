Package: quirc:x64-osx -> 1.1#3

**Host Environment**

- Host: x64-osx
- Compiler: AppleClang 14.0.3.14030022
-    vcpkg-tool version: 2023-08-09-9990a4c9026811a312cb2af78bf77f3d9d288416
    vcpkg-scripts version: 962e5e39f 2023-09-08 (2 weeks ago)

**To Reproduce**

`vcpkg install `
**Failure logs**

```
-- Downloading https://github.com/dlbeer/quirc/archive/7e7ab596e4d0988faf1c12ae89c354b114181c40.tar.gz -> dlbeer-quirc-7e7ab596e4d0988faf1c12ae89c354b114181c40.tar.gz...
[DEBUG] To include the environment variables in debug output, pass --debug-env
[DEBUG] Trying to load bundleconfig from /Users/liu/vcpkg/vcpkg-bundle.json
[DEBUG] Failed to open: /Users/liu/vcpkg/vcpkg-bundle.json
[DEBUG] Bundle config: readonly=false, usegitregistry=false, embeddedsha=nullopt, deployment=Git, vsversion=nullopt
[DEBUG] Metrics enabled.
[DEBUG] Feature flag 'binarycaching' unset
[DEBUG] Feature flag 'compilertracking' unset
[DEBUG] Feature flag 'registries' unset
[DEBUG] Feature flag 'versions' unset
[DEBUG] Feature flag 'dependencygraph' unset
[DEBUG] 1000: popen( curl --fail -L https://github.com/dlbeer/quirc/archive/7e7ab596e4d0988faf1c12ae89c354b114181c40.tar.gz --create-dirs --output /Users/liu/vcpkg/downloads/dlbeer-quirc-7e7ab596e4d0988faf1c12ae89c354b114181c40.tar.gz.70413.part 2>&1)
[DEBUG] 1000: cmd_execute_and_stream_data() returned 56 after 217044656 us
error: Failed to download from mirror set
error: https://github.com/dlbeer/quirc/archive/7e7ab596e4d0988faf1c12ae89c354b114181c40.tar.gz: curl failed to download with exit code 56
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed

curl: (56) Recv failure: Operation timed out


[DEBUG] /Users/runner/work/1/s/src/vcpkg/base/downloads.cpp(1053): 
[DEBUG] Time in subprocesses: 217044656us
[DEBUG] Time in parsing JSON: 17us
[DEBUG] Time in JSON reader: 0us
[DEBUG] Time in filesystem: 581us
[DEBUG] Time in loading ports: 0us
[DEBUG] Exiting after 3.6 min (217047826us)

CMake Error at scripts/cmake/vcpkg_download_distfile.cmake:32 (message):
      
      Failed to download file with error: 1
      If you are using a proxy, please check your proxy setting. Possible causes are:
      
      1. You are actually using an HTTP proxy, but setting HTTPS_PROXY variable
         to `https://address:port`. This is not correct, because `https://` prefix
         claims the proxy is an HTTPS proxy, while your proxy (v2ray, shadowsocksr
         , etc..) is an HTTP proxy. Try setting `http://address:port` to both
         HTTP_PROXY and HTTPS_PROXY instead.
      
      2. If you are using Windows, vcpkg will automatically use your Windows IE Proxy Settings
         set by your proxy software. See https://github.com/microsoft/vcpkg-tool/pull/77
         The value set by your proxy might be wrong, or have same `https://` prefix issue.
      
      3. Your proxy's remote server is out of service.
      
      If you've tried directly download the link, and believe this is not a temporary
      download server failure, please submit an issue at https://github.com/Microsoft/vcpkg/issues
      to report this upstream download server failure.
      

Call Stack (most recent call first):
  scripts/cmake/vcpkg_download_distfile.cmake:270 (z_vcpkg_download_distfile_show_proxy_and_fail)
  scripts/cmake/vcpkg_from_github.cmake:106 (vcpkg_download_distfile)
  ports/quirc/portfile.cmake:3 (vcpkg_from_github)
  scripts/ports.cmake:147 (include)



```
**Additional context**

<details><summary>vcpkg.json</summary>

```
{
  "$schema": "https://raw.githubusercontent.com/microsoft/vcpkg-tool/main/docs/vcpkg.schema.json",
  "name": "my-application",
  "version": "0.15.2",
  "dependencies": [
    "boost-pfr",
    "libusb",
    "numcpp",
    "eigen3",
    "opencv",
    "nlohmann-json"
  ]
}

```
</details>
