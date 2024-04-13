stack_slot_is_managed_pointer (ILStackDesc *value)
{
	return (value->stype & POINTER_MASK) == POINTER_MASK;
}