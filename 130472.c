dump_uses(const char *name, bool infodump)
/* dump "use=" clauses in the appropriate format */
{
    char buffer[MAX_TERMINFO_LENGTH];

    if (TcOutput())
	trim_trailing();
    _nc_SPRINTF(buffer, _nc_SLIMIT(sizeof(buffer))
		"%s%s", infodump ? "use=" : "tc=", name);
    wrap_concat1(buffer);
}