#!/bin/bash

###############################################################
# Important values to set

###############################################################
# Gather some information from the git repo
GITTAG=`git describe --tags | cut -d'-' -f1 | cut -d'v' -f2`
GITSH1=`git log --pretty=format:'%h' -1`
RELESEVERSION=$GITTAG

echo "pack botrace :: $RELESEVERSION from commit $GITSH"

###############################################################
# Get archive from git and but it into subfolder
#
mkdir "botrace-$RELESEVERSION"

cd ..
git archive master --prefix="botrace-$RELESEVERSION/" | gzip > release-script/botrace-$RELESEVERSION/botrace-$RELESEVERSION.tar.gz
cd release-script

###############################################################
# unpack all files again and change some stuff in the config.pri
#
cd "botrace-$RELESEVERSION"
tar -xf botrace-$RELESEVERSION.tar.gz

cat botrace-$RELESEVERSION/config.pri | \
sed "s/#RELEASEBUILD/RELEASEBUILD=$GITTAG/g" |
sed "s/#GITTAG/GITTAG=$GITTAG/g" |
sed "s/#GITREV/GITREV=""/g" |
sed "s/#GITSH1/GITSH1=$GITSH1/g" |
sed "s/#GAME_VERSION/GAME_VERSION=$RELESEVERSION/g" > botrace-$RELESEVERSION/config.pri_old
rm botrace-$RELESEVERSION/config.pri
mv botrace-$RELESEVERSION/config.pri_old botrace-$RELESEVERSION/config.pri

###############################################################
# pack the files again
#
tar -czf botrace-$RELESEVERSION.tar.gz botrace-$RELESEVERSION
rm -r botrace-$RELESEVERSION

SH1=`sha1sum botrace-$RELESEVERSION.tar.gz | cut -d' ' -f1`
FILESIZE=`ls -l botrace-$RELESEVERSION.tar.gz | awk '{print $5}'`
cd ..

###############################################################
# Add the important files for the packaging
#
cat botrace.spec_tmpl | \
sed "s/##RELESEVERSION##/$RELESEVERSION/g" > botrace-$RELESEVERSION/botrace.spec

cat botrace.dsc_tmpl | \
sed "s/##RELESEVERSION##/$RELESEVERSION/g" |
sed "s/##SH1##/$SH1/g" |
sed "s/##FILESIZE##/$FILESIZE/g" > botrace-$RELESEVERSION/botrace.dsc

cp ../CHANGELOG botrace-$RELESEVERSION/debian.changelog

cp debian.control botrace-$RELESEVERSION/
cp debian.rules botrace-$RELESEVERSION/


echo "done"