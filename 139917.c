void thread_stack_cache_init(void)
{
	thread_stack_cache = kmem_cache_create("thread_stack", THREAD_SIZE,
					      THREAD_SIZE, 0, NULL);
	BUG_ON(thread_stack_cache == NULL);
}