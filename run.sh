#!/bin/bash
make
cp ./bin/namics ./inputs/
cd inputs/
./namics lipid.in
