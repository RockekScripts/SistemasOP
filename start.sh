if [ -d "mount-point" ]; then
	rm -R -f mount-point
	echo "mount-point directory deleted... [OK]"
fi
rm virtual-disk

chmod +x ./my-fsck-static-64

mkdir mount-point

./fs-fuse -t 2097152 -a virtual-disk -f '-d -s mount-point'
chmod +x ./script.sh
