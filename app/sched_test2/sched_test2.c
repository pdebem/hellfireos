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
	printf("Krnl Task %d\n", hf_selfid());
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
		if (k == 0){
			hf_yield();
		}
		printf("Capacidade da fila  %d\n", hf_queue_count(krnl_ps_queue));

		if (krnl_task->capacity == 0)
			hf_kill(hf_selfid());
		else
			krnl_task->capacity--;

		krnl_task = hf_queue_remhead(krnl_ps_queue);
		printf("Current task: %d", krnl_task->id);
		printf("Capacidade da fila agora  %d\n", hf_queue_count(krnl_ps_queue));
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
	printf("I'm a APERIODIC task and my ID is %d\n", hf_selfid());
	delay_ms(1);			// do not hog the CPU!
	hf_yield();
}


void best_effort(void){
	printf("I'm a BEST EFFORT task and my ID is %d\n", hf_selfid());
	delay_ms(1);			// do not hog the CPU!
	hf_yield();
}



void real_time(void){
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

	hf_spawn(aperiodic, 0, 2, 0, "A1", 1024);
	hf_spawn(aperiodic, 0, 2, 0, "A2", 1024);
	hf_spawn(aperiodic, 0, 2, 0, "A3", 1024);
	hf_spawn(best_effort, 0, 0, 0, "BF", 1024);
	hf_spawn(polling_server, 3, 4, 3, "polling", 1024);
	hf_spawn(real_time, 2, 1, 2, "RT", 1024);
}
