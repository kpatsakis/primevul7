static void gc_boundary(struct object_array *array)
{
	unsigned nr = array->nr;
	unsigned alloc = array->alloc;
	struct object_array_entry *objects = array->objects;

	if (alloc <= nr) {
		unsigned i, j;
		for (i = j = 0; i < nr; i++) {
			if (objects[i].item->flags & SHOWN)
				continue;
			if (i != j)
				objects[j] = objects[i];
			j++;
		}
		for (i = j; i < nr; i++)
			objects[i].item = NULL;
		array->nr = j;
	}
}