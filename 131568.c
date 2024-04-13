SEC_DESC *get_nt_acl_no_snum( TALLOC_CTX *ctx, const char *fname)
{
	SEC_DESC *psd, *ret_sd;
	connection_struct conn;
	files_struct finfo;
	struct fd_handle fh;

	ZERO_STRUCT( conn );

	if ( !(conn.mem_ctx = talloc_init( "novfs_get_nt_acl" )) ) {
		DEBUG(0,("get_nt_acl_no_snum: talloc() failed!\n"));
		return NULL;
	}

	if (!(conn.params = TALLOC_P(conn.mem_ctx, struct share_params))) {
		DEBUG(0,("get_nt_acl_no_snum: talloc() failed!\n"));
		TALLOC_FREE(conn.mem_ctx);
		return NULL;
	}

	conn.params->service = -1;

	set_conn_connectpath(&conn, "/");

	if (!smbd_vfs_init(&conn)) {
		DEBUG(0,("get_nt_acl_no_snum: Unable to create a fake connection struct!\n"));
		conn_free_internal( &conn );
		return NULL;
        }

	ZERO_STRUCT( finfo );
	ZERO_STRUCT( fh );

	finfo.fnum = -1;
	finfo.conn = &conn;
	finfo.fh = &fh;
	finfo.fh->fd = -1;
	finfo.fsp_name = CONST_DISCARD(char *,fname);

	if (!NT_STATUS_IS_OK(posix_fget_nt_acl( &finfo, DACL_SECURITY_INFORMATION, &psd))) {
		DEBUG(0,("get_nt_acl_no_snum: get_nt_acl returned zero.\n"));
		conn_free_internal( &conn );
		return NULL;
	}

	ret_sd = dup_sec_desc( ctx, psd );

	conn_free_internal( &conn );

	return ret_sd;
}