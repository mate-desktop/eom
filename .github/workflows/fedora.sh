#!/usr/bin/bash

# Use grouped output messages
infobegin() {
	echo "::group::${1}"
}
infoend() {
	echo "::endgroup::"
}

# Required packages on Fedora
requires=(
	ccache # Use ccache to speed up build
	meson  # Used for meson build
)

requires+=(
	autoconf-archive
	desktop-file-utils
	exempi-devel
	gcc
	git
	gobject-introspection-devel
	gtk3-devel
	ImageMagick-devel
	lcms2-devel
	libappstream-glib-devel
	libexif-devel
	libjpeg-turbo-devel
	libpeas1-devel
	librsvg2-devel
	libxml2-devel
	make
	mate-common
	mate-desktop-devel
	redhat-rpm-config
	zlib-devel
)

infobegin "Update system"
dnf update -y
infoend

infobegin "Install dependency packages"
dnf install -y ${requires[@]}
infoend
