#!/bin/bash

for i in *.{wav,aif}; do
  sox "$i" -b 16 -r 48k -t raw -c 1 converted/"${i%.*}".raw
done
