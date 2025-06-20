#!/bin/bash

for i in {1..50} ; do printf "\n" ; done
cmake --build build --target update_docs_on_droplet
