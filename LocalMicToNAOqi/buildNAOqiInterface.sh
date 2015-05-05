
cd NAOqi

qibuild configure --release nao_mic_interface
qibuild install --release nao_mic_interface ./build
#qibuild configure nao_mic_interface -c tool-2.0.6
#qibuild install  nao_mic_interface ./build  -c tool-2.0.6

cd ..
