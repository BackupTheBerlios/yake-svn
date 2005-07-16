#!/bin/bash

libtoolize --force --copy
autoheader
aclocal
automake -a
autoconf

