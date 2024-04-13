void squashfs_closedir(struct dir *dir)
{
	struct dir_ent *ent = dir->dirs;

	while(ent) {
		struct dir_ent *tmp = ent;

		ent = ent->next;
		free(tmp->name);
		free(tmp);
	}

	free(dir);
}