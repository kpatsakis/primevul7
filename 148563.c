pk_transaction_state_to_string (PkTransactionState state)
{
	if (state == PK_TRANSACTION_STATE_NEW)
		return "new";
	if (state == PK_TRANSACTION_STATE_WAITING_FOR_AUTH)
		return "waiting-for-auth";
	if (state == PK_TRANSACTION_STATE_READY)
		return "ready";
	if (state == PK_TRANSACTION_STATE_RUNNING)
		return "running";
	if (state == PK_TRANSACTION_STATE_FINISHED)
		return "finished";
	if (state == PK_TRANSACTION_STATE_ERROR)
		return "error";
	return NULL;
}