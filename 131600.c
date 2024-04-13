static bool unix_ex_wire_to_tagtype(unsigned char wire_tt, SMB_ACL_TAG_T *p_tt)
{
	switch (wire_tt) {
		case SMB_POSIX_ACL_USER_OBJ:
			*p_tt = SMB_ACL_USER_OBJ;
			break;
		case SMB_POSIX_ACL_USER:
			*p_tt = SMB_ACL_USER;
			break;
		case SMB_POSIX_ACL_GROUP_OBJ:
			*p_tt = SMB_ACL_GROUP_OBJ;
			break;
		case SMB_POSIX_ACL_GROUP:
			*p_tt = SMB_ACL_GROUP;
			break;
		case SMB_POSIX_ACL_MASK:
			*p_tt = SMB_ACL_MASK;
			break;
		case SMB_POSIX_ACL_OTHER:
			*p_tt = SMB_ACL_OTHER;
			break;
		default:
			return False;
	}
	return True;
}