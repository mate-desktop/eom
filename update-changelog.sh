#!/bin/bash
set -e
if [ -d "${MESON_SOURCE_ROOT}/.git" ]; then
	GIT_DIR="${MESON_SOURCE_ROOT}/.git" git log --stat > $MESON_DIST_ROOT/ChangeLog
fi
