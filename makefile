#
# Main File:        main.c
# This File:        makefile
# Other Files:      main.c, queue.c, queue.h
# Semester:         CS 537 Fall 2018
# Lecture:          Lec 001
# Authors:          Zhengqi Liu, Tian Zheng
# CS Login:         zhengqi, tian
# NetID:            mliu292, tzheng24
#

CC = gcc
WARNING_FLAGS = -Wall -Wextra -lpthread
EXE = prodcomm
SCAN_BUILD_DIR = scan-build-out
  
all: prodcomm.o Queue.o
    $(CC) -o $(EXE) prodcomm.o Queue.o

main.o: prodcomm.c Queue.h
    $(CC) $(WARNING_FLAGS) -c prodcomm.c
    
queue.o: Queue.c Queue.h
    $(CC) $(WARNING_FLAGS) -c Queue.c
    
clean:
    rm -f $(EXE) *.o
    rm -rf $(SCAN_BUILD_DIR)

#
# Run the Clang Static Analyzer
#
scan-build: clean
    scan-build -o $(SCAN_BUILD_DIR) make

#
# View the one scan available using firefox
#
scan-view: scan-build
    firefox -new-window $(SCAN_BUILD_DIR)/*/index.html 
