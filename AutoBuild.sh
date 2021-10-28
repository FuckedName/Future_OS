clear

cp /mnt/hgfs/Share/code/Split/A.c MdeModulePkg/Application/A/
if [ $? -ne 0 ]; then
    exit 0;
fi

#EmulatorPkg/build.sh -a IA32
#if [ $? -ne 0 ]; then
#    exit 0;
#fi

EmulatorPkg/build.sh -a X64
if [ $? -ne 0 ]; then
    exit 0;
fi


cp ./Build/EmulatorX64/DEBUG_GCC5/X64/A.efi /mnt/hgfs/Share/code/Split/A_X64_`date +%Y_%m_%d__%H_%M_%S`.efi
#cp ./Build/EmulatorIA32/DEBUG_GCC5/IA32/A.efi /mnt/hgfs/Share/code/FragrantOS/A_`date +%Y_%m_%d__%H_%M_%S`.efi


