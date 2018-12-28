CWD=$(pwd | sed 's#.*/##')
pwd=$(pwd)
debug(){
	if [ -d "build" ]; then
		echo "Build directory is present"
	else 
		mkdir build
	fi
	cd build
	cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_DISTRIBUTABLE_LIBS=Off ..
	make
}

help(){
	echo "Usage: "
	echo "	Debug";
}


case "$1" in
	debug)
		debug
		;;
	*)
		help
		;;
esac
