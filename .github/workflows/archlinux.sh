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

requires+=(
	autoconf-archive
	appstream-glib
	autoconf-archive
	desktop-file-utils
	exempi
	gcc
	git
	glib2-devel
	gobject-introspection
	imagemagick
	itstool
	lcms2
	libexif
	libjpeg-turbo
	libpeas
	make
	mate-desktop
	mate-common
	which
	yelp-tools
)

infobegin "Update system"
pacman --noconfirm -Syu
infoend

infobegin "Install dependency packages"
pacman --noconfirm -S ${requires[@]}
infoend
