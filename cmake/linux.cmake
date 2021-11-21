message("\n** Configuring Host Environment for CTest**")
message("** Selected Kit: ${CMAKE_C_COMPILER} **")
message("** Selected Kit: ${CMAKE_CXX_COMPILER} **\n")


set(CMAKE_SYSTEM_NAME		Linux)
set(CMAKE_SYSTEM_VERSION	1)
set(CMAKE_SYSTEM_PROCESSOR	Linux)
set(BUILD_NAME            build)

# binutils language settings
# compilers are set by the select kit in vscode (CMAKE_C_COMPILER/CMAKE_CXX_COMPILER)
set(CMAKE_OBJCOPY			objcopy)
set(CMAKE_OBJDUMP			objdump)
set(CMAKE_SIZE			    size)

# common build settings
set(COMMON_FLAGS "${OPTIM_LVL} ${DEBUG_LVL} -pedantic -Wall -Wextra -Wfloat-equal -Wshadow -fmessage-length=0 -ffunction-sections -fdata-sections -ffreestanding -fno-builtin")
set(CMAKE_EXE_LINKER_FLAGS  "" CACHE INTERNAL "exe link flags")

# C compiler settings
set(C_FLAGS "")
set(CMAKE_C_FLAGS "${COMMON_FLAGS} ${C_FLAGS}" CACHE INTERNAL "c compiler flags")
enable_language(C)

# c++ compiler settings
set(CPP_FLAGS "")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} ${CPP_FLAGS}" CACHE INTERNAL "cpp compiler flags")
enable_language(CXX)