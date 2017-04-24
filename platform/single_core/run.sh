rm *.o image.*
clear
make image > debugMakeImage
../../usr/sim/hf_risc_sim/hf_risc_sim image.bin > debugRunning
