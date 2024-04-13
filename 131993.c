void key_type_put(struct key_type *ktype)
{
	up_read(&key_types_sem);
}