cmake_minimum_required(VERSION 3.21)
project(exam06 C)

set(CMAKE_C_STANDARD 99)

add_executable(mini_serv mini_serv.c)
add_executable(mini_serv2 mini_serv2.c)
add_executable(mini_serv3 mini_serv3.c)
add_executable(test MyCode/mini_serv.c)
#add_executable(backup Backup/mini_serv.c Backup/mini_serv2.c Backup/mini_serv3.c)
