put_param_int(gs_param_list *plist, gs_param_name pname, int *pvalue, int minval, int maxval, int ecode)
{       int code, value;
        switch ( code = param_read_int(plist, pname, &value) )
        {
        default:
                return code;
        case 1:
                return ecode;
        case 0:
                if ( value < minval || value > maxval )
                   param_signal_error(plist, pname, gs_error_rangecheck);
                *pvalue = value;
                return (ecode < 0 ? ecode : 1);
        }
}