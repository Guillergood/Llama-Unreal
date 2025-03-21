# Llama Unreal

An Unreal plugin for [llama.cpp](https://github.com/ggml-org/llama.cpp) to support embedding local LLMs in your projects.

Fork is modern re-write from [upstream](https://github.com/mika314/UELlama) to support latest API, including: GPULayers, advanced sampling (MinP, Miro, etc), Jinja templates, chat history, partial rollback & context reset, regeneration, and more. Defaults to Vulkan build on windows for wider hardware support at about ~10% perf loss compared to CUDA backend on token generation speed. 


[Discord Server](https://discord.gg/qfJUyxaW4s)

# Install & Setup

1. [Download Latest Release](https://github.com/getnamo/Llama-Unreal/releases) Ensure to use the `Llama-Unreal-UEx.x-vx.x.x.7z` link which contains compiled binaries, *not* the Source Code (zip) link.
2. Create new or choose desired unreal project.
3. Browse to your project folder (project root)
4. Copy *Plugins* folder from .7z release into your project root.
5. Plugin should now be ready to use.

# How to use - Basics

NB: Early days of API, unstable.

Everything is wrapped inside a [`ULlamaComponent`](https://github.com/getnamo/Llama-Unreal/blob/5b149eabccd2832fb630bb08f0d9f0c14325ed82/Source/LlamaCore/Public/LlamaComponent.h#L237) which interfaces internally via [`FLlama`](https://github.com/getnamo/Llama-Unreal/blob/5b149eabccd2832fb630bb08f0d9f0c14325ed82/Source/LlamaCore/Private/LlamaComponent.cpp#L87).

1) Setup your [`ModelParams`](https://github.com/getnamo/Llama-Unreal/blob/5b149eabccd2832fb630bb08f0d9f0c14325ed82/Source/LlamaCore/Public/LlamaComponent.h#L273) of type [`FLLMModelParams`](https://github.com/getnamo/Llama-Unreal/blob/5b149eabccd2832fb630bb08f0d9f0c14325ed82/Source/LlamaCore/Public/LlamaComponent.h#L165) 

3) Call `LoadModel`

2) Call [`InsertPromptTemplated`](https://github.com/getnamo/Llama-Unreal/blob/5b149eabccd2832fb630bb08f0d9f0c14325ed82/Source/LlamaCore/Public/LlamaComponent.h#L307) (or [`InsertRawPrompt`](https://github.com/getnamo/Llama-Unreal/blob/5b149eabccd2832fb630bb08f0d9f0c14325ed82/Source/LlamaCore/Public/LlamaComponent.h#L290) if you're doing raw input style without formatting.

3) You should receive replies via [`OnNewTokenGenerated`](https://github.com/getnamo/Llama-Unreal/blob/5b149eabccd2832fb630bb08f0d9f0c14325ed82/Source/LlamaCore/Public/LlamaComponent.h#L252) callback

Explore [LlamaComponent.h](https://github.com/getnamo/Llama-Unreal/blob/main/Source/LlamaCore/Public/LlamaComponent.h) for detailed API.


# Llama.cpp Build Instructions

To do custom backends or support platforms not currently supported you can follow these build instruction. Note that these build instructions should be run from the cloned llama.cpp root directory, not the plugin root.

### Basic Build Steps
1. clone [Llama.cpp](https://github.com/ggml-org/llama.cpp)
2. build using commands given below e.g. for Vulkan
```
mkdir build
cd build/
cmake .. -DGGML_VULKAN=ON -DGGML_NATIVE=OFF
cmake --build . --config Release -j --verbose
```
3. Include: After build 
- Copy `{llama.cpp root}/include`
- Copy `{llama.cpp root}/ggml/include`
- into `{plugin root}/ThirdParty/LlamaCpp/Include`
- Copy `{llama.cpp root}/common/` `common.h` and `sampling.h` 
- into `{plugin root}/ThirdParty/LlamaCpp/Include/common`

4. Libs: Assuming `{llama.cpp root}/build` as `{build root}`. 

- Copy `{build root}/src/Release/llama.lib`, 
- Copy `{build root}/common/Release/common.lib`, 
- Copy `{build root}/ggml/src/Release/` `ggml.lib`, `ggml-base.lib` & `ggml-cpu.lib`, 
- Copy `{build root}/ggml/src/Release/ggml-vulkan/Release/ggml-vulkan.lib` 
- into `{plugin root}/ThirdParty/LlamaCpp/Lib/Win64`

5. Dlls: 
- Copy `{build root}/bin/Release/` `ggml.dll`, `ggml-base.dll`, `ggml-cpu.dll`, `ggml-vulkan.dll`, & `llama.dll` 
- into `{plugin root}/ThirdParty/LlamaCpp/Binaries/Win64`
6. Build plugin

### Current Version
Current Plugin [Llama.cpp](https://github.com/ggml-org/llama.cpp) was built from git has/tag: [b4879](https://github.com/ggml-org/llama.cpp/tree/b4879)

NB: use `-DGGML_NATIVE=OFF` to ensure wider portability.


### Windows build
With the following build commands for windows (cpu build only, CUDA ignored, see upstream for GPU version):

#### CPU Only

```
mkdir build
cd build/
cmake .. -DGGML_NATIVE=OFF
cmake --build . --config Release -j --verbose
```
#### Vulkan

see https://github.com/ggml-org/llama.cpp/blob/b4762/docs/build.md#git-bash-mingw64

e.g. once [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) has been installed run.

```
mkdir build
cd build/
cmake .. -DGGML_VULKAN=ON -DGGML_NATIVE=OFF
cmake --build . --config Release -j --verbose
```

#### CUDA

ATM built for CUDA 12.4 runtime

- Use `cuda` branch if you want cuda enabled.
- We build statically due to dll runtime load bug so you need to copy `cudart.lib` `cublas.lib` and `cuda.lib` into your libraries/win64 path. These are ignored atm.
- Ensure `bTryToUseCuda = true;` is set in LlamaCore.build.cs to add CUDA libs to build.
- NB help wanted: Ideally this needs a variant that build with `-DBUILD_SHARED_LIBS=ON`

```
mkdir build
cd build
cmake .. -DGGML_CUDA=ON -DGGML_NATIVE=OFF
cmake --build . --config Release -j --verbose
```

### Mac build

```
mkdir build
cd build/
cmake .. -DBUILD_SHARED_LIBS=ON
cmake --build . --config Release -j --verbose
```

### Android build

For Android build see: https://github.com/ggerganov/llama.cpp/blob/master/docs/android.md#cross-compile-using-android-ndk

```
mkdir build-android
cd build-android
export NDK=<your_ndk_directory>
cmake -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-23 -DCMAKE_C_FLAGS=-march=armv8.4a+dotprod ..
$ make
```

Then the .so or .lib file was copied into e.g. `ThirdParty/LlamaCpp/Win64/cpu` directory and all the .h files were copied to the `Includes` directory.
