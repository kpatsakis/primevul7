stack_slot_get_type (ILStackDesc *value)
{
	return value->stype & RAW_TYPE_MASK;
}