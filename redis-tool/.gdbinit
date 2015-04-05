# gdb init script
file bin/dbprint
#set args 10.26.225.16 10000 10.26.225.16 30000 60
set args 127.0.0.1 10000  redis-instance-password 0 string_name
dir src
set listsize 20
b tool.c:81
r
