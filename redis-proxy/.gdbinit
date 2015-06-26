#FILE     : .gdbinit
#DESC     : it will be used by gdb
#AUTHOR        : v1.0 written by stockszhao
#DATE     : on Oct 16, 2012
#MOD        : modified by * on June 2, 2012

define    list_info
    echo ----cur args (info args)-------------------------------------
    echo "\n"
    info args
    echo "\n"
    echo "\n"
    echo ----cur breakpoints (info break)-----------------------------
    echo "\n"
    info break
    echo "\n"
    echo "\n"
    echo ----cur watchpoints (info watchpoints)-----------------------
    echo "\n"
    info watchpoints
    echo "\n"
    echo "\n"
    echo ----cur local variables (info locals)------------------------
    echo "\n"
    info locals
    echo "\n"
    echo "\n"
    echo ----cur frame (info frame)-----------------------------------
    echo "\n"
    info frame
    echo "\n"
    echo "\n"
    echo ----back trace (statck info) (bt)----------------------------
    echo "\n"
    bt
    echo "\n"
    echo ----over.(show global var: file::gv or func::gv)--
    echo "\n"
end

define    debug_int_signal
    handle SIGINT pass
end

define    debug_valgrind
    target remote | vgdb
end

define    debug_thread
    set target-async 1
    set pagination off
    set non-stop on
end

define  start_threads
    set scheduler-locking off
end

define  stop_threads
    set scheduler-locking on
end

dir ./src/
file bin/t_conf
#set args  0.0.0.0 8000 1
set args ./conf/test.conf
#to see struct in a pretty style
set print pretty on
set listsize 25
b dict_uninit
info break
#debug_valgrind
r
