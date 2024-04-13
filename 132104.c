auth_spa_init(auth_instance *ablock)
{
auth_spa_options_block *ob =
  (auth_spa_options_block *)(ablock->options_block);

/* The public name defaults to the authenticator name */

if (ablock->public_name == NULL) ablock->public_name = ablock->name;

/* Both username and password must be set for a client */

if ((ob->spa_username == NULL) != (ob->spa_password == NULL))
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG_FOR, "%s authenticator:\n  "
      "one of client_username and client_password cannot be set without "
      "the other", ablock->name);
ablock->client = ob->spa_username != NULL;

/* For a server we have just one option */

ablock->server = ob->spa_serverpassword != NULL;
}