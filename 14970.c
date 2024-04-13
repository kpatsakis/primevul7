schema_destroy( void )
{
	oidm_destroy();
	oc_destroy();
	at_destroy();
	mr_destroy();
	mru_destroy();
	syn_destroy();

	if( schema_init_done ) {
		ldap_pvt_thread_mutex_destroy( &ad_index_mutex );
		ldap_pvt_thread_mutex_destroy( &ad_undef_mutex );
		ldap_pvt_thread_mutex_destroy( &oc_undef_mutex );
	}
}