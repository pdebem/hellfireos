#include <hellfire.h>

void aperiodic(void){
	int32_t jobs, id;

	id = hf_selfid();
	for(;;){
		printf("I'm a aperiodic task and my id is %d\n", id);
	}
}

void polling(void) {
	int32_t id;
	id = hf_selfid();

	for(;;) {
		hf_polling_server();
		printf("I'm polling server\n");
	}
}

void app_main(void){
	hf_spawn(aperiodic, 0, 1, 0, "task a", 2048);
	hf_spawn(polling, 1, 1, 1, "task b", 2048);
}
