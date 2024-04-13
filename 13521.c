is_ex_cmdchar(cmdarg_T *cap)
{
    return cap->cmdchar == ':'
	|| cap->cmdchar == K_COMMAND
	|| cap->cmdchar == K_SCRIPT_COMMAND;
}