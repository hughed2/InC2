#!/bin/bash

# Make sure we're running from the base directory
dirname=${PWD##*/}
if [ $dirname = 'tools' ]
then
  cd ..
fi

if [[ -z "${SPACK_REPOS}" ]]; then
  spackRepoDir="$SPACK_ROOT/var/spack/repos/hpmusic"
else
  spackRepoDir="${SPACK_REPOS}/hpmusic"
fi

# Make sure our repo exists
if [ ! -d "$spackRepoDir" ]; then
  spack repo create $spackRepoDir hpmusic
  spack repo add $spackRepoDir
fi

# Now make sure our package exists
if [ ! -d "$spackRepoDir/packages/inc" ]; then
  spack create --skip-editor --repo $spackRepoDir --name inc
fi

# Move our tarball over
rm -f inc.tar.gz
tar -czf inc.tar.gz *
mv inc.tar.gz $spackRepoDir/packages/inc/

# Make sure package.py is updated
cp tools/package.py.TEMPLATE $spackRepoDir/packages/inc/package.py
tarhash=`md5sum $spackRepoDir/packages/inc/inc.tar.gz | awk '{print $1}'`
sed -i'.TEMPLATE' -e "s/<VERSION>/$tarhash/g" $spackRepoDir/packages/inc/package.py
