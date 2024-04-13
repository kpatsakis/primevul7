acl_authenticate(THD *thd, uint com_change_user_pkt_len)
{
  int res= CR_OK;
  MPVIO_EXT mpvio;
  Thd_charset_adapter charset_adapter(thd);

  LEX_STRING auth_plugin_name= default_auth_plugin_name;
  enum  enum_server_command command= com_change_user_pkt_len ? COM_CHANGE_USER
                                                             : COM_CONNECT;

  DBUG_ENTER("acl_authenticate");
  compile_time_assert(MYSQL_USERNAME_LENGTH == USERNAME_LENGTH);

  server_mpvio_initialize(thd, &mpvio, &charset_adapter);

  DBUG_PRINT("info", ("com_change_user_pkt_len=%u", com_change_user_pkt_len));

  /*
    Clear thd->db as it points to something, that will be freed when
    connection is closed. We don't want to accidentally free a wrong
    pointer if connect failed.
  */
  thd->reset_db(NULL, 0);

  if (command == COM_CHANGE_USER)
  {
    mpvio.packets_written++; // pretend that a server handshake packet was sent
    mpvio.packets_read++;    // take COM_CHANGE_USER packet into account

    /* Clear variables that are allocated */
    thd->set_user_connect(NULL);

    if (parse_com_change_user_packet(&mpvio, com_change_user_pkt_len))
    {
      if (!thd->is_error())
        login_failed_error(&mpvio, mpvio.auth_info.password_used);
      server_mpvio_update_thd(thd, &mpvio);
      DBUG_RETURN(1);
    }

    DBUG_ASSERT(mpvio.status == MPVIO_EXT::RESTART ||
                mpvio.status == MPVIO_EXT::SUCCESS);
  }
  else
  {
    /* mark the thd as having no scramble yet */
    mpvio.scramble[SCRAMBLE_LENGTH]= 1;
    
    /*
     perform the first authentication attempt, with the default plugin.
     This sends the server handshake packet, reads the client reply
     with a user name, and performs the authentication if everyone has used
     the correct plugin.
    */

    res= do_auth_once(thd, &auth_plugin_name, &mpvio);  
  }

  /*
   retry the authentication, if - after receiving the user name -
   we found that we need to switch to a non-default plugin
  */
  if (mpvio.status == MPVIO_EXT::RESTART)
  {
    DBUG_ASSERT(mpvio.acl_user);
    DBUG_ASSERT(command == COM_CHANGE_USER ||
                my_strcasecmp(system_charset_info, auth_plugin_name.str,
                              mpvio.acl_user->plugin.str));
    auth_plugin_name= mpvio.acl_user->plugin;
    res= do_auth_once(thd, &auth_plugin_name, &mpvio);
    if (res <= CR_OK)
    {
      if (auth_plugin_name.str == native_password_plugin_name.str)
        thd->variables.old_passwords= 0;
      if (auth_plugin_name.str == old_password_plugin_name.str)
        thd->variables.old_passwords= 1;
      if (auth_plugin_name.str == sha256_password_plugin_name.str)
        thd->variables.old_passwords= 2;
    }
  }

  server_mpvio_update_thd(thd, &mpvio);

  Security_context *sctx= thd->security_ctx;
  const ACL_USER *acl_user= mpvio.acl_user;

  thd->password= mpvio.auth_info.password_used;  // remember for error messages 

  /*
    Log the command here so that the user can check the log
    for the tried logins and also to detect break-in attempts.

    if sctx->user is unset it's protocol failure, bad packet.
  */
  if (mpvio.auth_info.user_name)
  {
    if (strcmp(mpvio.auth_info.authenticated_as, mpvio.auth_info.user_name))
    {
      general_log_print(thd, command, "%s@%s as %s on %s",
                        mpvio.auth_info.user_name, mpvio.auth_info.host_or_ip,
                        mpvio.auth_info.authenticated_as,
                        mpvio.db.str ? mpvio.db.str : (char*) "");
    }
    else
      general_log_print(thd, command, (char*) "%s@%s on %s",
                        mpvio.auth_info.user_name, mpvio.auth_info.host_or_ip,
                        mpvio.db.str ? mpvio.db.str : (char*) "");
  }

  if (res == CR_OK && !mpvio.can_authenticate())
  {
    res= CR_ERROR;
  }

  if (res > CR_OK && mpvio.status != MPVIO_EXT::SUCCESS)
  {
    Host_errors errors;
    DBUG_ASSERT(mpvio.status == MPVIO_EXT::FAILURE);
    switch (res)
    {
    case CR_AUTH_PLUGIN_ERROR:
      errors.m_auth_plugin= 1;
      break;
    case CR_AUTH_HANDSHAKE:
      errors.m_handshake= 1;
      break;
    case CR_AUTH_USER_CREDENTIALS:
      errors.m_authentication= 1;
      break;
    case CR_ERROR:
    default:
      /* Unknown of unspecified auth plugin error. */
      errors.m_auth_plugin= 1;
      break;
    }
    inc_host_errors(mpvio.ip, &errors);
    if (!thd->is_error())
      login_failed_error(&mpvio, mpvio.auth_info.password_used);
    DBUG_RETURN (1);
  }

  sctx->proxy_user[0]= 0;

  if (initialized) // if not --skip-grant-tables
  {
#ifndef NO_EMBEDDED_ACCESS_CHECKS
    bool is_proxy_user= FALSE;
    const char *auth_user = acl_user->user ? acl_user->user : "";
    ACL_PROXY_USER *proxy_user;
    /* check if the user is allowed to proxy as another user */
    proxy_user= acl_find_proxy_user(auth_user, sctx->get_host()->ptr(),
                                    sctx->get_ip()->ptr(),
                                    mpvio.auth_info.authenticated_as,
                                    &is_proxy_user);
    if (is_proxy_user)
    {
      ACL_USER *acl_proxy_user;

      /* we need to find the proxy user, but there was none */
      if (!proxy_user)
      {
        Host_errors errors;
        errors.m_proxy_user= 1;
        inc_host_errors(mpvio.ip, &errors);
        if (!thd->is_error())
          login_failed_error(&mpvio, mpvio.auth_info.password_used);
        DBUG_RETURN(1);
      }

      my_snprintf(sctx->proxy_user, sizeof(sctx->proxy_user) - 1,
                  "'%s'@'%s'", auth_user,
                  acl_user->host.get_host() ? acl_user->host.get_host() : "");

      /* we're proxying : find the proxy user definition */
      mysql_mutex_lock(&acl_cache->lock);
      acl_proxy_user= find_acl_user(proxy_user->get_proxied_host() ? 
                                    proxy_user->get_proxied_host() : "",
                                    mpvio.auth_info.authenticated_as, TRUE);
      if (!acl_proxy_user)
      {
        Host_errors errors;
        errors.m_proxy_user_acl= 1;
        inc_host_errors(mpvio.ip, &errors);
        if (!thd->is_error())
          login_failed_error(&mpvio, mpvio.auth_info.password_used);
        mysql_mutex_unlock(&acl_cache->lock);
        DBUG_RETURN(1);
      }
      acl_user= acl_proxy_user->copy(thd->mem_root);
      DBUG_PRINT("info", ("User %s is a PROXY and will assume a PROXIED"
                          " identity %s", auth_user, acl_user->user));
      mysql_mutex_unlock(&acl_cache->lock);
    }
#endif

    sctx->master_access= acl_user->access;
    if (acl_user->user)
      strmake(sctx->priv_user, acl_user->user, USERNAME_LENGTH - 1);
    else
      *sctx->priv_user= 0;

    if (acl_user->host.get_host())
      strmake(sctx->priv_host, acl_user->host.get_host(), MAX_HOSTNAME - 1);
    else
      *sctx->priv_host= 0;

#ifndef NO_EMBEDDED_ACCESS_CHECKS
    /*
      OK. Let's check the SSL. Historically it was checked after the password,
      as an additional layer, not instead of the password
      (in which case it would've been a plugin too).
    */
    if (acl_check_ssl(thd, acl_user))
    {
      Host_errors errors;
      errors.m_ssl= 1;
      inc_host_errors(mpvio.ip, &errors);
      if (!thd->is_error())
        login_failed_error(&mpvio, thd->password);
      DBUG_RETURN(1);
    }

    if (unlikely(mpvio.acl_user && mpvio.acl_user->password_expired
        && !(mpvio.client_capabilities & CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS)
        && disconnect_on_expired_password))
    {
      /*
        Clients that don't signal password expiration support
        get a connect error.
      */
      Host_errors errors;

      my_error(ER_MUST_CHANGE_PASSWORD_LOGIN, MYF(0));
      general_log_print(thd, COM_CONNECT, ER(ER_MUST_CHANGE_PASSWORD_LOGIN));
      if (log_warnings > 1)
        sql_print_warning("%s", ER(ER_MUST_CHANGE_PASSWORD_LOGIN));

      errors.m_authentication= 1;
      inc_host_errors(mpvio.ip, &errors);
      DBUG_RETURN(1);
    }

    /* Don't allow the user to connect if he has done too many queries */
    if ((acl_user->user_resource.questions || acl_user->user_resource.updates ||
         acl_user->user_resource.conn_per_hour ||
         acl_user->user_resource.user_conn || 
         global_system_variables.max_user_connections) &&
        get_or_create_user_conn(thd,
          (opt_old_style_user_limits ? sctx->user : sctx->priv_user),
          (opt_old_style_user_limits ? sctx->host_or_ip : sctx->priv_host),
          &acl_user->user_resource))
      DBUG_RETURN(1); // The error is set by get_or_create_user_conn()

    /*
      We are copying the connected user's password expired flag to the security
      context.
      This allows proxy user to execute queries even if proxied user password
      expires.
    */
    sctx->password_expired= mpvio.acl_user->password_expired;
#endif
  }
  else
    sctx->skip_grants();

  const USER_CONN *uc;
  if ((uc= thd->get_user_connect()) &&
      (uc->user_resources.conn_per_hour || uc->user_resources.user_conn ||
       global_system_variables.max_user_connections) &&
       check_for_max_user_connections(thd, uc))
  {
    DBUG_RETURN(1); // The error is set in check_for_max_user_connections()
  }

  DBUG_PRINT("info",
             ("Capabilities: %lu  packet_length: %ld  Host: '%s'  "
              "Login user: '%s' Priv_user: '%s'  Using password: %s "
              "Access: %lu  db: '%s'",
              thd->client_capabilities, thd->max_client_packet_length,
              sctx->host_or_ip, sctx->user, sctx->priv_user,
              thd->password ? "yes": "no",
              sctx->master_access, mpvio.db.str));

  if (command == COM_CONNECT &&
      !(thd->main_security_ctx.master_access & SUPER_ACL))
  {
    mysql_mutex_lock(&LOCK_connection_count);
    bool count_ok= (connection_count <= max_connections);
    mysql_mutex_unlock(&LOCK_connection_count);
    if (!count_ok)
    {                                         // too many connections
      release_user_connection(thd);
      statistic_increment(connection_errors_max_connection, &LOCK_status);
      my_error(ER_CON_COUNT_ERROR, MYF(0));
      DBUG_RETURN(1);
    }
  }

  /*
    This is the default access rights for the current database.  It's
    set to 0 here because we don't have an active database yet (and we
    may not have an active database to set.
  */
  sctx->db_access=0;

  /* Change a database if necessary */
  if (mpvio.db.length)
  {
    if (mysql_change_db(thd, &mpvio.db, FALSE))
    {
      /* mysql_change_db() has pushed the error message. */
      release_user_connection(thd);
      Host_errors errors;
      errors.m_default_database= 1;
      inc_host_errors(mpvio.ip, &errors);
      DBUG_RETURN(1);
    }
  }

  if (mpvio.auth_info.external_user[0])
    sctx->set_external_user(my_strdup(mpvio.auth_info.external_user, MYF(0)));


  if (res == CR_OK_HANDSHAKE_COMPLETE)
    thd->get_stmt_da()->disable_status();
  else
    my_ok(thd);

#ifdef HAVE_PSI_THREAD_INTERFACE
  PSI_THREAD_CALL(set_thread_user_host)
    (thd->main_security_ctx.user, strlen(thd->main_security_ctx.user),
    thd->main_security_ctx.host_or_ip, strlen(thd->main_security_ctx.host_or_ip));
#endif

  /* Ready to handle queries */
  DBUG_RETURN(0);
}