
echo -n Hello
echo
echo World
tail -n 1 Makefile


echo Begin variable test.
MAN=man
CC=gcc
VERSION=--version
LS=ls

echo $MAN $CC $LS $VERSION
echo Variable ne contienne pas toutes le caractère. Harder...
echo Command: $MAN:$VERSION:$LS

$CC --version
man $VERSION
ls --version
ls --color=auto -lh
$CC $VERSION

for i in 1 2 3 ; do echo Hello $i ; done
for i in $MAN $CC $LS ; do $i $VERSION ; done
for i in 1 2 3 ; do for j in 1 2 3 ; do echo $i $j; done ; done

gcc -c || echo test réussi.
echo -n Coco && echo Lapin
cat nofile || echo File not found twice.
tail -n 1 Makefile && echo Devrait y avoir la fin du makefile.



for i in 1 2 3 ; do for j in 1 2 3 ; do echo  $i $j; done ; done && echo Good || echo Bad


