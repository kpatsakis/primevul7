usm_store_users(int majorID, int minorID, void *serverarg, void *clientarg)
{
    /*
     * figure out our application name 
     */
    char           *appname = (char *) clientarg;
    if (appname == NULL) {
        appname = netsnmp_ds_get_string(NETSNMP_DS_LIBRARY_ID, 
					NETSNMP_DS_LIB_APPTYPE);
    }

    /*
     * save the user base 
     */
    usm_save_users("usmUser", appname);

    /*
     * never fails 
     */
    return SNMPERR_SUCCESS;
}