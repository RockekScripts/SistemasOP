if [ -d "tmp" ]; then
	rm -R -f tmp
	echo "./tmp directory deleted... [OK]"
fi

mkdir ./tmp

cp ./src/fuseLib.c ./tmp/fuseLib.c	
cp ./src/fuseLib.c ./mount-point/fuseLib.c
cp ./src/myFS.h ./tmp/myFS.h	
cp ./src/myFS.h ./mount-point/myFS.h


read -p "Pulsa enter para ver el siguiente paso"
echo
./my-fsck-static-64 virtual-disk
echo
read -p "Pulsa enter para ver el siguiente paso"
echo
DifMyFS=$(diff ./mount-point/myFS.h ./tmp/myFS.h) 
DifFuseLib=$(diff ./mount-point/fuseLib.c ./tmp/fuseLib.c)
echo "Comparando las copias de MOUNT y TMP"
if [ "$DifMyFS" == "" ]; 
then echo "Los archivos son iguales, está BIEN"
else echo "Los archivos son diferentes, esta MAL"
fi

if [ "$DifFuseLib" == "" ]; 
then echo "Los archivos son iguales, está BIEN"
else echo "Los archivos son diferentes, esta MAL"
fi

truncate --size=-4096 ./tmp/fuseLib.c
truncate --size=-4096 ./mount-point/fuseLib.c

read -p "Pulsa enter para ver el siguiente paso"
echo
./my-fsck-static-64 virtual-disk
echo
echo "Comparando las copia truncada de MOUNT y el original de SRC"
DifMyFSTr=$(diff ./src/fuseLib.c ./mount-point/fuseLib.c)

if [ "$DifMyFSTr" == "" ];
then echo "Los archivos son iguales, está MAL"
else echo "Los archivos son diferentes, esta BIEN"
fi

read -p "Pulsa enter para ver el siguiente paso"
cp file3.txt ./mount-point/texto.txt
echo
./my-fsck-static-64 virtual-disk
echo
DifTexto=$(diff file3.txt ./mount-point/texto.txt)
echo "Comparando la copia del tercer archivo de MOUNT y su original"
if [ "$DifTexto" == "" ]; 
then echo "Los archivos son iguales, está BIEN"
else echo "Los archivos son diferentes, esta MAL"
fi

read -p "Pulsa enter para ver el siguiente paso"
truncate --size=+4096 ./tmp/myFS.h
truncate --size=+4096 ./mount-point/myFS.h

echo
./my-fsck-static-64 virtual-disk
echo
DifFuseLibTr=$(diff ./src/myFS.h ./mount-point/myFS.h)
echo "Comparando la copia de fuseLib.c con mas tamaño de MOUNT y el original de SRC"

if [ "$DifFuseLibTr" == "" ]; 
then echo "Los archivos son iguales, está MAL"
else echo "Los archivos son diferentes, esta BIEN"
fi



