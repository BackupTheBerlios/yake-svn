#!/bin/bash

libtoolize --force --copy
aclocal
autoheader
automake -a
autoconf

