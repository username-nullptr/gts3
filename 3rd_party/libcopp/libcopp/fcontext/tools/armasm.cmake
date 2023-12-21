if(MSVC)
  list(APPEND fcontext_sources
      libcopp/fcontext/asm/${fcontext_src_name_make}.asm
      libcopp/fcontext/asm/${fcontext_src_name_jump}.asm
      libcopp/fcontext/asm/${fcontext_src_name_ontop}.asm
  )
  set(MASMFound FALSE)
  enable_language(ASM_MASM)
  if(CMAKE_ASM_MASM_COMPILER_WORKS)
    set(MASMFound TRUE)
  else()
    message("-- enable masm failed")
    message(FATAL_ERROR "enable ASM_MASM failed")
  endif(CMAKE_ASM_MASM_COMPILER_WORKS)
else()
  list(APPEND fcontext_sources
      libcopp/fcontext/asm/${fcontext_src_name_make}.asm
      libcopp/fcontext/asm/${fcontext_src_name_jump}.asm
      libcopp/fcontext/asm/${fcontext_src_name_ontop}.asm
  )
endif()
