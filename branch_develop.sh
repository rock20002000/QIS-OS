#!/bin/sh

cd bsp
git switch develop
cd ../kernel
git switch develop
git branch
cd ../sdk
git switch develop
git branch
cd ../userspace
git switch develop
git branch
cd ..
