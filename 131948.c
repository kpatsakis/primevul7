void LIRGenerator::do_Convert(Convert* x) {
  // flags that vary for the different operations and different SSE-settings
  bool fixed_input = false, fixed_result = false, round_result = false, needs_stub = false;

  switch (x->op()) {
    case Bytecodes::_i2l: // fall through
    case Bytecodes::_l2i: // fall through
    case Bytecodes::_i2b: // fall through
    case Bytecodes::_i2c: // fall through
    case Bytecodes::_i2s: fixed_input = false;       fixed_result = false;       round_result = false;      needs_stub = false; break;

    case Bytecodes::_f2d: fixed_input = UseSSE == 1; fixed_result = false;       round_result = false;      needs_stub = false; break;
    case Bytecodes::_d2f: fixed_input = false;       fixed_result = UseSSE == 1; round_result = UseSSE < 1; needs_stub = false; break;
    case Bytecodes::_i2f: fixed_input = false;       fixed_result = false;       round_result = UseSSE < 1; needs_stub = false; break;
    case Bytecodes::_i2d: fixed_input = false;       fixed_result = false;       round_result = false;      needs_stub = false; break;
    case Bytecodes::_f2i: fixed_input = false;       fixed_result = false;       round_result = false;      needs_stub = true;  break;
    case Bytecodes::_d2i: fixed_input = false;       fixed_result = false;       round_result = false;      needs_stub = true;  break;
    case Bytecodes::_l2f: fixed_input = false;       fixed_result = UseSSE >= 1; round_result = UseSSE < 1; needs_stub = false; break;
    case Bytecodes::_l2d: fixed_input = false;       fixed_result = UseSSE >= 2; round_result = UseSSE < 2; needs_stub = false; break;
    case Bytecodes::_f2l: fixed_input = true;        fixed_result = true;        round_result = false;      needs_stub = false; break;
    case Bytecodes::_d2l: fixed_input = true;        fixed_result = true;        round_result = false;      needs_stub = false; break;
    default: ShouldNotReachHere();
  }

  LIRItem value(x->value(), this);
  value.load_item();
  LIR_Opr input = value.result();
  LIR_Opr result = rlock(x);

  // arguments of lir_convert
  LIR_Opr conv_input = input;
  LIR_Opr conv_result = result;
  ConversionStub* stub = NULL;

  if (fixed_input) {
    conv_input = fixed_register_for(input->type());
    __ move(input, conv_input);
  }

  assert(fixed_result == false || round_result == false, "cannot set both");
  if (fixed_result) {
    conv_result = fixed_register_for(result->type());
  } else if (round_result) {
    result = new_register(result->type());
    set_vreg_flag(result, must_start_in_memory);
  }

  if (needs_stub) {
    stub = new ConversionStub(x->op(), conv_input, conv_result);
  }

  __ convert(x->op(), conv_input, conv_result, stub);

  if (result != conv_result) {
    __ move(conv_result, result);
  }

  assert(result->is_virtual(), "result must be virtual register");
  set_result(x, result);
}