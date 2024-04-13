finish_collect_stats (void)
{
	max_allocated_memory = MAX (max_allocated_memory, allocated_memory);
	max_working_set = MAX (max_working_set, working_set);
	total_allocated_memory += allocated_memory;
	allocated_memory = working_set = 0;
}