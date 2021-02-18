#!/bin/sh
./build.sh && ./run-rdice.sh sched.algorithm=rr pgalloc.algorithm=buddy $*
