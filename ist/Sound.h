﻿#ifndef __ist_Sound_h__
#define __ist_Sound_h__

#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#include <deque>
#include <boost/shared_ptr.hpp>

#include "Config.h"
#include "Base.h"
#include "Sound/isdDeviceResource.h"
#include "Sound/isdDevice.h"
#include "Sound/isdBuffer.h"
#include "Sound/isdSource.h"
#include "Sound/isdListener.h"
#include "Sound/isdUtil.h"
#include "Sound/isduStream.h"
#include "Sound/isduStreamSource.h"
#ifdef __ist_with_oggvorbis__
    #include <vorbis/vorbisfile.h>
    #include "Sound/isduOggVorbis.h"
    #pragma comment(lib, "libogg.lib")
    #pragma comment(lib, "libvorbis.lib")
    #pragma comment(lib, "libvorbisfile.lib")
    //#pragma comment(lib, "libogg_static.lib")
    //#pragma comment(lib, "libvorbis_static.lib")
    //#pragma comment(lib, "libvorbisfile_static.lib")
#endif
#pragma comment(lib, "OpenAL32.lib")
//#pragma comment(lib, "EFX-Util.lib")

#endif // __ist_Sound_h__
