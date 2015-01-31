#script to create a sample dummy data plugin use it with the following arguments.


ClassName=$1
EngineName=`echo $ClassName | tr '[:upper:]' '[:lower:]'`
GardName=` echo $ClassName | tr '[:lower:]' '[:upper:]'`

echo $GardName;
echo $EngineName;

files="template.h template.cpp templateinterface.h templateinterface.cpp template.desktop CMakeLists.txt"

mkdir $EngineName;

for file_name in $files 
do
	NewFileName=`echo $file_name | sed "s/template/$EngineName/g";`
	echo $NewFileName;
	cat $file_name > $EngineName/$NewFileName;
	cd $EngineName;
	sed -i.bak "s/Template/"$ClassName"/g" $NewFileName;
	sed -i.bak "s/template/"$EngineName"/g" $NewFileName;
	sed -i.bak "s/TEMPLATE/"$GardName"/g" $NewFileName;
	rm $NewFileName.bak
	cd ..
done


