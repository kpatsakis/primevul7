stack_slot_get_name (ILStackDesc *value)
{
	return type_names [value->stype & TYPE_MASK];
}