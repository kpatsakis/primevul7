static struct pai_val *create_pai_val(char *buf, size_t size)
{
	char *entry_offset;
	struct pai_val *paiv = NULL;
	int i;

	if (!check_pai_ok(buf, size))
		return NULL;

	paiv = SMB_MALLOC_P(struct pai_val);
	if (!paiv)
		return NULL;

	memset(paiv, '\0', sizeof(struct pai_val));

	paiv->pai_protected = (CVAL(buf,PAI_FLAG_OFFSET) == PAI_ACL_FLAG_PROTECTED);

	paiv->num_entries = SVAL(buf,PAI_NUM_ENTRIES_OFFSET);
	paiv->num_def_entries = SVAL(buf,PAI_NUM_DEFAULT_ENTRIES_OFFSET);

	entry_offset = buf + PAI_ENTRIES_BASE;

	DEBUG(10,("create_pai_val:%s num_entries = %u, num_def_entries = %u\n",
			paiv->pai_protected ? " (pai_protected)" : "", paiv->num_entries, paiv->num_def_entries ));

	for (i = 0; i < paiv->num_entries; i++) {
		struct pai_entry *paie;

		paie = SMB_MALLOC_P(struct pai_entry);
		if (!paie) {
			free_inherited_info(paiv);
			return NULL;
		}

		paie->owner_type = (enum ace_owner)CVAL(entry_offset,0);
		switch( paie->owner_type) {
			case UID_ACE:
				paie->unix_ug.uid = (uid_t)IVAL(entry_offset,1);
				DEBUG(10,("create_pai_val: uid = %u\n", (unsigned int)paie->unix_ug.uid ));
				break;
			case GID_ACE:
				paie->unix_ug.gid = (gid_t)IVAL(entry_offset,1);
				DEBUG(10,("create_pai_val: gid = %u\n", (unsigned int)paie->unix_ug.gid ));
				break;
			case WORLD_ACE:
				paie->unix_ug.world = -1;
				DEBUG(10,("create_pai_val: world ace\n"));
				break;
			default:
				free_inherited_info(paiv);
				return NULL;
		}
		entry_offset += PAI_ENTRY_LENGTH;
		DLIST_ADD(paiv->entry_list, paie);
	}

	for (i = 0; i < paiv->num_def_entries; i++) {
		struct pai_entry *paie;

		paie = SMB_MALLOC_P(struct pai_entry);
		if (!paie) {
			free_inherited_info(paiv);
			return NULL;
		}

		paie->owner_type = (enum ace_owner)CVAL(entry_offset,0);
		switch( paie->owner_type) {
			case UID_ACE:
				paie->unix_ug.uid = (uid_t)IVAL(entry_offset,1);
				DEBUG(10,("create_pai_val: (def) uid = %u\n", (unsigned int)paie->unix_ug.uid ));
				break;
			case GID_ACE:
				paie->unix_ug.gid = (gid_t)IVAL(entry_offset,1);
				DEBUG(10,("create_pai_val: (def) gid = %u\n", (unsigned int)paie->unix_ug.gid ));
				break;
			case WORLD_ACE:
				paie->unix_ug.world = -1;
				DEBUG(10,("create_pai_val: (def) world ace\n"));
				break;
			default:
				free_inherited_info(paiv);
				return NULL;
		}
		entry_offset += PAI_ENTRY_LENGTH;
		DLIST_ADD(paiv->def_entry_list, paie);
	}

	return paiv;
}