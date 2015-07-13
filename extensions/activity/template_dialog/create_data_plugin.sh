#script to create a sample dummy data plugin use it with the following arguments.


ClassName=$1
EngineName=`echo $ClassName | tr '[:upper:]' '[:lower:]'`
GardName=` echo $ClassName | tr '[:lower:]' '[:upper:]'`

echo $GardName;
echo $EngineName;

files="template_dialog.h template_dialog.cpp template_plugin_impl.cpp template_plugin_impl.h template_dialog.desktop CMakeLists.txt"

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


