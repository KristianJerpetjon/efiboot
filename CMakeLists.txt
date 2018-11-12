cmake_minimum_required(VERSION 2.8)
project(efi)
SET(ARCH "x86_64")

#ARCH            = $(shell uname -m | sed s,i[3456789]86,ia32,)

include_directories(
  /usr/include/efi
  /usr/include/efi/${ARCH}
  /usr/include/efi/protocol
)

##set flags
SET(CFLAGS "-fno-stack-protector -fpic -fshort-wchar -mno-red-zone -Wall")
  #""-Wshadow -Wunused -Werror -Werror-implicit-function-declaration")
if (${ARCH} STREQUAL "x86_64")
  SET(CFLAGS "${CFLAGS} -DEFI_FUNCTION_WRAPPER")
else()
  SET(CFLAGS ${CFLAGS} "-m32")
endif()

if (${ARCH} STREQUAL "x86_64")
  SET(LIB "lib64")
else()
  SET(LIB "lib32")
endif()

SET(EFILIB /usr/${LIB})
#SET(EFILIB /usr/)
#SET(CMAKE_C_FLAGS "-m64 -mno-red-zone")
#SET(CMAKE_C_FLAGS ${CFLAGS})
#SET(CMAKE_SHARED_LINKER_FLAGS "-nostdlib -znocombreloc -T ${EFILIB}/elf_${ARCH}_efi.lds -shared -Bsymbolic")
#set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-dll -Wl,--subsystem,10")



#how do i do this in cmake..
##check how to set ldflags properly
add_library(hello OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/main.c)
set_source_files_properties(main.c PROPERTIES COMPILE_FLAGS ${CFLAGS})

#target_link_libraries(hello "-nostdlib -znocombreloc -T ${EFILIB}/elf_${ARCH}_efi.lds -shared -Bsymbolic" efi gnuefi ${EFILIB}/crt0-efi-${ARCH}.o)
SET(LD_FLAGS -nostdlib -znocombreloc -T ${EFILIB}/elf_${ARCH}_efi.lds -shared -Bsymbolic -L ${EFILIB} -L ${EFILIB} ${EFILIB}/crt0-efi-${ARCH}.o)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/hello.so
  COMMAND ld ${LD_FLAGS} $<TARGET_OBJECTS:hello> -o ${CMAKE_CURRENT_BINARY_DIR}/hello.so -lefi -lgnuefi
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Creating hello.so"
  DEPENDS hello
  VERBATIM
  )

add_custom_target(libhello DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/hello.so )

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/hello.efi
  COMMAND objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-${ARCH} hello.so hello.efi
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Creating hello.efi"
  DEPENDS libhello
  VERBATIM
)
#ALL makes it allways build this target
add_custom_target(hello-efi ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/hello.efi )
#install(FILES src/some_opencl_kernel.cl DESTINATION bin)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hello.efi DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/boot/EFI)
