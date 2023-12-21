# =========== libcopp/src - fcontext : ${LIBCOPP_FCONTEXT_OS_PLATFORM} - sysv - ${LIBCOPP_FCONTEXT_BIN_FORMAT} - ${LIBCOPP_FCONTEXT_AS_TOOL} ===========
if(WIN32
   OR WINCE
   OR WINDOWS_PHONE
   OR WINDOWS_STORE
   OR MINGW
   OR CYGWIN)
  set(fcontext_src_file_make "${fcontext_src_name_make}.asm")
  set(fcontext_src_file_jump "${fcontext_src_name_jump}.asm")
  set(fcontext_src_file_ontop "${fcontext_src_name_ontop}.asm")
else()
  set(fcontext_src_file_make "${fcontext_src_name_make}.S")
  set(fcontext_src_file_jump "${fcontext_src_name_jump}.S")
  set(fcontext_src_file_ontop "${fcontext_src_name_ontop}.S")
endif()

set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp")

list(APPEND fcontext_sources
    libcopp/fcontext/asm/${fcontext_src_file_jump}
    libcopp/fcontext/asm/${fcontext_src_file_make}
    libcopp/fcontext/asm/${fcontext_src_file_ontop}
)
