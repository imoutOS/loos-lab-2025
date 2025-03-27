#! /bin/sh
#
# cppcheck.sh
# Copyright (C) 2024 LoOS Developers
#
# Distributed under terms of the GNU AGPLv3 license.
#

CPPCHECK=cppcheck

# set -x
$CPPCHECK --xml \
    --check-level=normal \
    --enable=all \
    --error-exitcode=1 \
    --platform=unix64 \
    -i fs/lwext4/ \
    -i toolchain/ \
    . \
    $@ \
    2>build/report.xml

cppcheck-htmlreport --file build/report.xml --report-dir build
