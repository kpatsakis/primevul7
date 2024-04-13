int check_directory(struct dir *dir)
{
	int i;
	struct dir_ent *ent;

	if(dir->dir_count < 2)
		return TRUE;

	for(ent = dir->dirs, i = 0; i < dir->dir_count - 1; ent = ent->next, i++)
		if(strcmp(ent->name, ent->next->name) >= 0)
			return FALSE;

	return TRUE;
}