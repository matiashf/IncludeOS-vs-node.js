#! /bin/bash
make IncludeOS_service.img
exec ${INCLUDEOS_HOME-$HOME/IncludeOS_install}/etc/run.sh IncludeOS_service.img

