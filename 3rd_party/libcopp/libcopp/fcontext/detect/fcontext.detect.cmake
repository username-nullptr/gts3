# ========== set os platform ==========
# os_platform can be set to arm/arm64/i386/x86_64/combined/mips/ppc32/ppc64/sparc/sparc64
if(NOT os_platform)
  if(NOT CMAKE_SYSTEM_PROCESSOR)
    set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_HOST_SYSTEM_PROCESSOR})
  endif()
  if(ANDROID)
    if(CMAKE_ANDROID_ARCH_ABI)
      string(TOLOWER ${CMAKE_ANDROID_ARCH_ABI} os_platform)
    elseif(ANDROID_ABI)
      string(TOLOWER ${ANDROID_ABI} os_platform)
    endif()
  elseif(CMAKE_OSX_ARCHITECTURES)
    string(TOLOWER ${CMAKE_OSX_ARCHITECTURES} os_platform)
  endif()
  if(NOT os_platform)
    string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} os_platform)
  endif()
  if(os_platform MATCHES "unknown|i386|i686|x86_64|amd64")
    if(CMAKE_SIZEOF_VOID_P MATCHES 8)
      set(CMAKE_SYSTEM_PROCESSOR "x86_64")
      set(os_platform "x86_64")
    else()
      set(CMAKE_SYSTEM_PROCESSOR "i386")
      set(os_platform "i386")
    endif()
  elseif(os_platform MATCHES "^arm(v5|v7|v7s|eabi)")
    set(os_platform "arm")
  elseif(os_platform MATCHES "^arm64" OR os_platform MATCHES "^aarch64")
    set(os_platform "arm64")
  endif()

  if(os_platform STREQUAL "unknown")
    echowithcolor(
      COLOR
      RED
      "Can not detect the processor, please add -Dos_platform=arm/arm64/i386/x86_64/combined/mips32/mips64/ppc32/ppc64/loongarch64 to help us to find target processor"
    )
    message(FATAL_ERROR "-Dos_platform is required")
  endif()

endif()

# patch for old -Dos_platform=mips to -Dos_platform=mips32
if(os_platform STREQUAL "mips")
  set(os_platform "mips32")
endif()

# ========== set abi ==========
if(NOT fcontext_abi)
  set(fcontext_abi "sysv")

  if(os_platform STREQUAL "arm" OR os_platform STREQUAL "arm64")
    set(fcontext_abi "aapcs")
  elseif(os_platform STREQUAL "mips32")
    set(fcontext_abi "o32")
  elseif(os_platform STREQUAL "mips64")
    set(fcontext_abi "n64")
  elseif(
    WIN32
    OR WINCE
    OR WINDOWS_PHONE
    OR WINDOWS_STORE
    OR MINGW
    OR CYGWIN)
    set(fcontext_abi "ms")
  endif()
endif()

# ========== set binary format ==========
# fcontext_abi can be set to elf/macho/pe/xcoff
if(NOT bin_format)
  set(bin_format "elf")

  if(WIN32
     OR WINCE
     OR WINDOWS_PHONE
     OR WINDOWS_STORE
     OR MINGW
     OR CYGWIN)
    set(bin_format "pe")
  elseif(APPLE)
    set(bin_format "macho")
  elseif(AIX) # cmake not supported now
    set(bin_format "xcoff")
  endif()
endif()

# ========== set as tool ==========
# fcontext_as_tool can be set to gas/armasm/masm
if(NOT fcontext_as_tool)
  set(fcontext_as_tool "gas")
  set(fcontext_as_out_suffix "o")

  if(WIN32 OR MINGW)
    set(fcontext_as_out_suffix "obj")
  endif()
  if(MSVC)
    if("${os_platform}" STREQUAL "arm" OR "${os_platform}" STREQUAL "arm64")
      set(fcontext_as_tool "armasm")
    else()
      set(fcontext_as_tool "masm")
    endif()
  elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" AND bin_format STREQUAL "pe")
    set(fcontext_as_tool "clang_gas")
  endif()

endif()
