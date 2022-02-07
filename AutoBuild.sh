clear

rm -rf MdeModulePkg/Application/*
rm -rf ShellPkg/Application/*

cp /mnt/hgfs/Share/code/edk2_code/MdeModulePkg/Application/* -rf MdeModulePkg/Application/
cp /mnt/hgfs/Share/code/edk2_code/ShellPkg/Application/* -rf ShellPkg/Application/
if [ $? -ne 0 ]; then
    exit 0;
fi

#rm -rf /mnt/hgfs/Share/code/Split/*
#cp -rf MdeModulePkg/Application/A/* /mnt/hgfs/Share/code/Split/  


#EmulatorPkg/build.sh -a IA32
#if [ $? -ne 0 ]; then
#    exit 0;
#fi

#EmulatorPkg/build.sh -a IA32 run
EmulatorPkg/build.sh -a X64
if [ $? -ne 0 ]; then
    exit 0;
fi

cp ./Build/EmulatorX64/DEBUG_GCC5/X64/Shell.efi /mnt/hgfs/Share/code/output/bootx64.efi
cp ./Build/EmulatorX64/DEBUG_GCC5/X64/HelloWorld.efi /mnt/hgfs/Share/code/output/H.efi
#cp ./Build/EmulatorX64/DEBUG_GCC5/X64/EchoTcp4.efi /mnt/hgfs/Share/code/output/EchoTcp4_X64_`date +%Y_%m_%d__%H_%M_%S`.efi
cp ./Build/EmulatorX64/DEBUG_GCC5/X64/A.efi /mnt/hgfs/Share/code/output/
#cp ./Build/EmulatorX64/DEBUG_GCC5/X64/A.efi /mnt/hgfs/Share/code/output/A_X64_`date +%Y_%m_%d__%H_%M_%S`.efi
#cp ./Build/EmulatorIA32/DEBUG_GCC5/IA32/A.efi /mnt/hgfs/Share/code/FragrantOS/A_`date +%Y_%m_%d__%H_%M_%S`.efi


