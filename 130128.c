copy_stack_value (ILStackDesc *to, ILStackDesc *from)
{
	to->stype = from->stype;
	to->type = from->type;
	to->method = from->method;
}