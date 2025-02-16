if [ -z "$1" ]; then
	echo "Please provide a path"
	exit 1
fi

if [ -d "$1" ]; then
	file_count=$(find "$1" -type f | wc -l)
	echo "Number of files in the directory: $file_count"
else
	echo "The path is not valid"
	exit 1
fi
