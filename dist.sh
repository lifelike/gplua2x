#!/bin/sh

version=`cat version.txt`
name=gplua2x-$version
d=/tmp/gplua2x$$/$name

mkdir -p $d \
&& cp -ruv *.png *.c *.h build Makefile* version.txt *.lua license.txt \
    index.html $d \
&& cd $d \
&& find . -name .svn|xargs rm -rf \
&& perl -pe s/VERSION/$version/ -i index.html \
&& cd .. \
&& tar zcvf /tmp/$name.tar.gz $name \
&& scp $d/index.html /tmp/$name.tar.gz $1