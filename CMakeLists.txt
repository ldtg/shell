cmake_minimum_required(VERSION 3.8)
project(shell)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES builtin.h builtin.c createcmd.h createcmd.c defs.h exec.h exec.c freecmd.h freecmd.c parsing.h parsing.c
        printstatus.h printstatus.c readline.h readline.c runcmd.h runcmd.c sh.c types.h utils.h utils.c)
add_executable(shell ${SOURCE_FILES})