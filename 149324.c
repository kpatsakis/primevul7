
private uint32_t
do_ops(struct magic *m, intmax_t lhs, intmax_t off)
{
	intmax_t offset;
	if (off) {
		switch (m->in_op & FILE_OPS_MASK) {
		case FILE_OPAND:
			offset = lhs & off;
			break;
		case FILE_OPOR:
			offset = lhs | off;
			break;
		case FILE_OPXOR:
			offset = lhs ^ off;
			break;
		case FILE_OPADD:
			offset = lhs + off;
			break;
		case FILE_OPMINUS:
			offset = lhs - off;
			break;
		case FILE_OPMULTIPLY:
			offset = lhs * off;
			break;
		case FILE_OPDIVIDE:
			offset = lhs / off;
			break;
		case FILE_OPMODULO:
			offset = lhs % off;
			break;
		}
	} else
		offset = lhs;
	if (m->in_op & FILE_OPINVERSE)
		offset = ~offset;
