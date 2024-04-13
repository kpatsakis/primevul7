close_all_scripts(void)
{
    while (scriptin[0] != NULL)
	closescript();
}