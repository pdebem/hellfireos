#include <hellfire.h>

#define NUMBER_OF_PROCESSORS 2

mutex_t m;

/**
 * @brief Yields the current task.
 *
 * The current task gives up execution and the best effort scheduler is invoked.
 */
void polling_server(void)
{
	printf("I'm a POLLING SERVER task\n");
	int32_t rc;
	int32_t k;
	volatile int32_t status;

	status = _di();
#if KERNEL_LOG >= 1
		dprintf("hf_polling_server() %d ", (uint32_t)_read_us());
#endif
	krnl_task = &krnl_tcb[krnl_current_task];
	rc = setjmp(krnl_task->task_context);
	if (rc){
		_ei(status);
		return;
	}
	if (krnl_task->state == TASK_RUNNING)
		krnl_task->state = TASK_READY;
	if (krnl_task->pstack[0] != STACK_MAGIC)
		panic(PANIC_STACK_OVERFLOW);
	if (krnl_tasks > 0){
		//Size of queue
		k = hf_queue_count(krnl_ps_queue);
		//printf("Capacidade da fila %d", k);
		if (k == 0){
			hf_yield();
			return;
		}
		//Decrementar a capacidade dela.. quando chegar no 0 killa

		if (krnl_task->capacity == 0)
			hf_kill(hf_selfid());
		else
			krnl_task->capacity--;

		// Erro aqui
		krnl_current_task = hf_queue_remhead(krnl_ps_queue);
		// if (!krnl_current_task)
		// 	panic(PANIC_NO_TASKS_RT);
		// if (hf_queue_addtail(krnl_ps_queue, krnl_current_task))
		// 	panic(PANIC_CANT_PLACE_RT);
		// krnl_current_task = hf_queue_remhead(krnl_ps_queue);
		krnl_task->state = TASK_RUNNING;
		krnl_pcb.coop_cswitch++;
#if KERNEL_LOG >= 1
		dprintf("\n%d %d %d %d %d ", krnl_current_task, krnl_task->period, krnl_task->capacity, krnl_task->deadline, (uint32_t)_read_us());
#endif
		_restoreexec(krnl_task->task_context, status, krnl_current_task);
		panic(PANIC_UNKNOWN);
	}else{
		panic(PANIC_NO_TASKS_LEFT);
	}
	delay_ms(1);			// do not hog the CPU!
}



void aperiodic(void){
	// int i = 0;
	// int32_t jobs, id;
	// id = hf_selfid();
	printf("I'm a APERIODIC task and my ID is %d\n", hf_selfid());
	delay_ms(1);			// do not hog the CPU!
	hf_yield();
}


void best_effort(void){
	// int i = 0;
	// int32_t jobs, id;
	// id = hf_selfid();
	printf("I'm a BEST EFFORT task and my ID is %d\n", hf_selfid());
	delay_ms(1);			// do not hog the CPU!
	hf_yield();
}



void real_time(void){
	// int i = 0;
	// int32_t jobs, id;
	// id = hf_selfid();
	hf_mtxlock(&m);
	printf("I'm a REAL TIME task and my ID is %d\n", hf_selfid());
	hf_mtxunlock(&m);
	delay_ms(1);			// do not hog the CPU!
	hf_yield();
}

// void polling(void) {
// 	int32_t id;
// 	id = hf_selfid();
// 	printf("I'm polling server id: %d\n", id);
// 	hf_polling_server();
// 	printf("I'm polling server id: %d\n", id);
// }

void app_main(void){
	int i = 0;
	hf_mtxinit(&m);
	char *taskName[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"};
	for(; i < NUMBER_OF_PROCESSORS+0; ++i){
		hf_spawn(aperiodic, 0, 2, 0, taskName[i], 1024);
	}
	for(; i < NUMBER_OF_PROCESSORS+2; ++i){
		hf_spawn(best_effort, 0, 0, 0, taskName[i], 1024);
	}
	for(; i < NUMBER_OF_PROCESSORS+5; ++i){
		hf_spawn(real_time, 2, 2, 2, taskName[i], 1024);
		hf_spawn(polling_server, 3, 2, 3, taskName[i+3], 1024);
	}
}
