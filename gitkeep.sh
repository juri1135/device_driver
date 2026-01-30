#!/bin/sh

iterate_dir(){
	filelist=`ls -A $1`
	for file in $filelist
		#일단 하위 디렉토리 있으면 들어가	
		do filelist=`ls -A $1/$file`
			#만약 비어 있는 디렉토리면 gitkeep file 만들어
			if [ "$filelist" = "" ]; then
				touch $1/$file/.gitkeep
				echo "create"			
				#비어 있지 않다면 하위로 더 들어가
			elif [ -d $1/$file ]; then
				iterate_dir $1/$file
			fi
	done
}

if [ $# -ne 1 ];
then echo "Error"
exit 1
fi
iterate_dir $1
