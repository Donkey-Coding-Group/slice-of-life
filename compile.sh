#!/bin/sh -x

if [ "$CC" = "" ]; then
    CC=gcc
fi

FLAGS=
if [ "$DEBUG" = "yes" ]; then
    FLAGS="$FLAGS -g"
fi

$CC src/*.c $FLAGS -o main.out
