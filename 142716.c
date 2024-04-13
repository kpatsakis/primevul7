_clone_pdu(netsnmp_pdu *pdu, int drop_err)
{
    netsnmp_pdu    *newpdu;
    newpdu = _clone_pdu_header(pdu);
    newpdu = _copy_pdu_vars(pdu, newpdu, drop_err, 0, 10000);   /* skip none, copy all */

    return newpdu;
}