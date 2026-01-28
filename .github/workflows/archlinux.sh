#!/usr/bin/bash

# Use grouped output messages
infobegin() {
	echo "::group::${1}"
}
infoend() {
	echo "::endgroup::"
}

# Required packages on Archlinux
requires=(
	ccache # Use ccache to speed up build
	clang  # Build with clang on Archlinux
	meson  # Used for meson build
)

# https://gitlab.archlinux.org/archlinux/packaging/packages/eom
requires+=(
	autoconf-archive
	dbus-glib
	exempi
	gcc
	gettext
	git
	glib2-devel
	gobject-introspection
	gobject-introspection-runtime
	itstool
	lcms2
	libexif
	libjpeg-turbo
	libpeas
	make
	mate-common
	mate-desktop
	which
	yelp-tools
)

infobegin "Update system"
pacman --noconfirm -Syu
infoend

infobegin "Install dependency packages"
pacman --noconfirm -S ${requires[@]}
infoend
