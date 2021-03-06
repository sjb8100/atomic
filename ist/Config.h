﻿#ifndef ist_Config_h

//#define ist_with_EASTL
#define ist_with_OpenGL
//#define ist_with_OpenGLES
//#define ist_with_DirectX11
#define ist_with_tbb
#define ist_with_zlib
#define ist_with_png
//#define ist_with_jpeg
#define ist_with_gli // dds ファイル対応
#define ist_with_OpenAL
//#define ist_with_OpenCL
#define ist_with_oggvorbis
#define ist_with_boost_serialization

#ifndef ist_env_Master
#   define ist_enable_Assert
#   define i3d_enable_assert
//#	define ist_enable_CrashReport
#endif // ist_env_Master
#ifdef ist_env_Debug
//#   define ist_enable_memory_leak_check
// memory_leak_check は resource_leak_check も兼ねるがパフォーマンス低下が著しいので一応別に用意
//#   define i3d_enable_resource_leak_check
#endif // ist_env_Debug
#define ist_leak_check_max_callstack_size 64


#if defined(_MSC_VER)
#   define ist_env_MSVC
#elif defined(__GNUC__)
#   define ist_env_GCC
#elif defined(__clang__)
#   define ist_env_LLVM
#endif

#if defined(_WIN64)
#   define ist_env_Windows
#   define ist_env_x86
#   define ist_env_x64
#elif defined(_WIN32)
#   define ist_env_Windows
#   define ist_env_x86
#elif defined(__ANDROID__)
#   define ist_env_Android
#   define ist_env_ARM32
#else
#   error
#endif


#if defined(ist_env_MSVC)
#   define istForceInline   __forceinline
#   define istThreadLocal   __declspec(thread)
#   define istDLLExport     __declspec(dllexport)
#   define istDLLImport     __declspec(dllimport)
#   define istAlign(N)      __declspec(align(N))
#   define istRestrict      __restrict
#   define istAlignof       __alignof
#else
#   define istForceInline   inline
#   define istThreadLocal   __thread
#   define istDLLExport     __attribute__((visibility("default")))
#   define istDLLImport 
#   define istAlign(N)      __attribute__((aligned(N)))
#   define istRestrict      __restrict
#   define istAlignof       __alignof
#endif


//#define istExportSymbols

#if defined(istExportSymbols)
#   define istAPI istDLLExport
#elif defined(istImportSymbols)
#   define istAPI istDLLImport
#else
#   define istAPI
#endif // istExportSymbols


#ifdef ist_env_MSVC

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。
#define _SCL_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma warning(disable: 4819) // コードページ問題 (glm)
#pragma warning(disable: 4308) // boost serialization 対策

#endif // ist_env_MSVC

#include <stdint.h>
#include <stdio.h>
#include <boost/intrusive_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>

#ifdef ist_with_EASTL
#   include <EASTL/algorithm.h>
#   include <EASTL/sort.h>
#   include <EASTL/vector.h>
#   include <EASTL/list.h>
#   include <EASTL/set.h>
#   include <EASTL/map.h>
#   include <EASTL/string.h>
namespace stl = eastl;
#else // ist_with_EASTL
#   include <vector>
#   include <list>
#   include <map>
#   include <string>
#   include <algorithm>
namespace stl = std;
#endif // ist_with_EASTL
#include <functional>
#include <regex>

#ifdef ist_with_DirectX11
#   include <D3D11.h>
#   include <D3DX11.h>
#endif // ist_with_DirectX11

#ifdef ist_with_OpenGL
#   include <GL/glew.h>
#   ifdef ist_env_Windows
#       include <GL/wglew.h>
#       pragma comment(lib, "glew32.lib")
#       pragma comment(lib, "opengl32.lib")
#   endif // ist_env_Windows
#endif // ist_with_OpenGL

#ifdef ist_with_zlib
#   define ZLIB_DLL
#   include "zlib/zlib.h"
#   pragma comment(lib, "zdll.lib")
#endif // ist_with_zlib

#ifdef ist_with_png
#   include <libpng/png.h>
#   pragma comment(lib,"libpng15.lib")
#endif // ist_with_png

#ifdef ist_with_jpeg
#   include <jpeglib.h>
#   include <jerror.h>
#   pragma comment(lib,"libjpeg.lib")
#endif // ist_with_jpeg

#ifdef ist_with_OpenAL
#   include <AL/al.h>
#   include <AL/alc.h>
#endif // ist_with_OpenAL

#ifdef ist_with_OpenCL
#   pragma comment(lib, "OpenCL.lib")
#endif // ist_with_OpenCL


#ifdef ist_with_poco
#endif // ist_with_poco

#ifdef ist_with_boost_serialization
#pragma warning(push)
#pragma warning(disable: 4244)
#define BOOST_SERIALIZATION_DYN_LINK
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#pragma warning(pop)
#endif // ist_with_boost_serialization

#define istStaticAssert(...) BOOST_STATIC_ASSERT(__VA_ARGS__)
#define istGlobalNamespace(...) } __VA_ARGS__ namespace ist {

#include "ist/Base/Types.h"

#endif // ist_Config_h
