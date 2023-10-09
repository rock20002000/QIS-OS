#!/bin/sh

cd bsp
git log develop ^origin/develop
cd ../kernel
git log develop ^origin/develop
cd ../sdk
git log develop ^origin/develop
cd ../userspace
git log develop ^origin/develop
cd ..
