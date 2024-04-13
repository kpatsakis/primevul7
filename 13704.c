excmd_get_cmdidx(char_u *cmd, int len)
{
    cmdidx_T idx;

    if (!one_letter_cmd(cmd, &idx))
	for (idx = (cmdidx_T)0; (int)idx < (int)CMD_SIZE;
		idx = (cmdidx_T)((int)idx + 1))
	    if (STRNCMP(cmdnames[(int)idx].cmd_name, cmd, (size_t)len) == 0)
		break;

    return idx;
}