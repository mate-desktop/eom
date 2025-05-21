#!/usr/bin/bash

# Use grouped output messages
infobegin() {
	echo "::group::${1}"
}
infoend() {
	echo "::endgroup::"
}

# Required packages on Debian
requires=(
	ccache # Use ccache to speed up build
	meson  # Used for meson build
)

requires+=(
	autoconf-archive
	autopoint
	desktop-file-utils
	git
	gobject-introspection
	gtk-doc-tools
	libdconf-dev
	libexempi-dev
	libexif-dev
	libgirepository1.0-dev
	libglib2.0-dev
	libgtk-3-dev
	libjpeg-dev
	liblcms2-dev
	libmagickwand-dev
	libmate-desktop-dev
	libpeas-dev
	librsvg2-dev
	libstartup-notification0-dev
	libxml2-dev
	make
	mate-common
	shared-mime-info
	x11proto-core-dev
	yelp-tools
	zlib1g-dev
)

infobegin "Update system"
apt-get update -qq
infoend

infobegin "Install dependency packages"
env DEBIAN_FRONTEND=noninteractive \
	apt-get install --assume-yes \
	${requires[@]}
infoend
