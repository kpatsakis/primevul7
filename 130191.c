stack_slot_is_this_pointer (ILStackDesc *value)
{
	return (value->stype & THIS_POINTER_MASK) == THIS_POINTER_MASK;
}