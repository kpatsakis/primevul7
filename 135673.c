void add_object(struct object *obj,
		struct object_array *p,
		struct name_path *path,
		const char *name)
{
	add_object_array(obj, path_name(path, name), p);
}