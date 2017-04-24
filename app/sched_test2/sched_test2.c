#include <hellfire.h>

#define NUMBER_OF_PROCESSORS 2

void aperiodic(void){
	int i = 0;
	int32_t jobs, id;
	id = hf_selfid();
	printf("I'm a aperiodic task and my id is %d and I[%d]\n", id, i++);
}

void polling(void) {
	int32_t id;
	id = hf_selfid();
	printf("I'm polling server id: %d\n", id);
	hf_polling_server();
	printf("I'm polling server id: %d\n", id);
}

void app_main(void){
	int i;
	char *taskName[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"};
	for(i=0; i < NUMBER_OF_PROCESSORS; i++){
		hf_spawn(aperiodic, 2, 2, 2, taskName[i], 1024);
	}
	for(; i < NUMBER_OF_PROCESSORS+2; i++){
		hf_spawn(aperiodic, 0, 2, 0, taskName[i], 1024);
	}
	// for(i=0; i < NUMBER_OF_PROCESSORS; i++){
		// hf_spawn(polling, 2, 2, 2, taskName[i], 1024);
	// }
}
