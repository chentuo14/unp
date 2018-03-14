#!/bin/bash

for((i=0;i<2;i++));
do
$(pwd)/client >/dev/null &;
done
