typedef unsigned int M_PROCESS_ID;
struct process
{
	const char* name;
	M_PROCESS_ID id;
	M_PROCESS_ID parent_id;

	M_PAGE_ENTRY* page_directory;
	M_PROCESS_STATE state;
	M_SCHEDULER_TIME time_remaining;
	M_SCHEDULER_PRIORITY priority;
	struct stack kernel_stack;
	struct stack user_stack;

	struct file_descriptor file;
};

void create_process(struct process process_decriptor);
void kill_process(M_PROCESS_ID id);
void schedule_process(M_PROCESS_ID id, M_PROCESS_STATE state, M_SCHEDULER_TIME time, M_SCHEDULER_PRIORITY priority);
void set_current_process(M_PROCESS_ID id);



	
	

