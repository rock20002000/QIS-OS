#!/bin/bash
cp sabrelite_gdbinit_tplt .sabrelite_gdbinit

sed -i "s/CFG_GDB_SVR_IP/$1/g" .sabrelite_gdbinit
sed -i "s/CFG_GDB_SVR_PORT/$2/g" .sabrelite_gdbinit

$3 -command=./.sabrelite_gdbinit 
