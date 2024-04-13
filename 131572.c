static uint32 get_pai_entry_val(struct pai_entry *paie)
{
	switch (paie->owner_type) {
		case UID_ACE:
			DEBUG(10,("get_pai_entry_val: uid = %u\n", (unsigned int)paie->unix_ug.uid ));
			return (uint32)paie->unix_ug.uid;
		case GID_ACE:
			DEBUG(10,("get_pai_entry_val: gid = %u\n", (unsigned int)paie->unix_ug.gid ));
			return (uint32)paie->unix_ug.gid;
		case WORLD_ACE:
		default:
			DEBUG(10,("get_pai_entry_val: world ace\n"));
			return (uint32)-1;
	}
}