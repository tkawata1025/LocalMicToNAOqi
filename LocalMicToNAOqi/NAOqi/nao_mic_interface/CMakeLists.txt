cmake_minimum_required(VERSION 2.8)
project(nao_mic_interface)

find_package(qibuild)


qi_create_lib(NaoMicInterface SHARED 
	"nao_mic_interface.h"
	"nao_mic_interface.cpp"
	)


	
qi_use_lib(NaoMicInterface ALCOMMON ALAUDIO ALPROXIES)
#qi_install_header("nao_mic_interface.h")
