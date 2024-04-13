bool WasmBinaryBuilder::maybeVisitBrOn(Expression*& out, uint32_t code) {
  BrOnOp op;
  switch (code) {
    case BinaryConsts::BrOnNull:
      op = BrOnNull;
      break;
    case BinaryConsts::BrOnNonNull:
      op = BrOnNonNull;
      break;
    case BinaryConsts::BrOnCast:
    case BinaryConsts::BrOnCastStatic:
      op = BrOnCast;
      break;
    case BinaryConsts::BrOnCastFail:
    case BinaryConsts::BrOnCastStaticFail:
      op = BrOnCastFail;
      break;
    case BinaryConsts::BrOnFunc:
      op = BrOnFunc;
      break;
    case BinaryConsts::BrOnNonFunc:
      op = BrOnNonFunc;
      break;
    case BinaryConsts::BrOnData:
      op = BrOnData;
      break;
    case BinaryConsts::BrOnNonData:
      op = BrOnNonData;
      break;
    case BinaryConsts::BrOnI31:
      op = BrOnI31;
      break;
    case BinaryConsts::BrOnNonI31:
      op = BrOnNonI31;
      break;
    default:
      return false;
  }
  auto name = getBreakTarget(getU32LEB()).name;
  if (code == BinaryConsts::BrOnCastStatic ||
      code == BinaryConsts::BrOnCastStaticFail) {
    auto intendedType = getIndexedHeapType();
    auto* ref = popNonVoidExpression();
    out = Builder(wasm).makeBrOn(op, name, ref, intendedType);
    return true;
  }
  Expression* rtt = nullptr;
  if (op == BrOnCast || op == BrOnCastFail) {
    rtt = popNonVoidExpression();
  }
  auto* ref = popNonVoidExpression();
  out = ValidatingBuilder(wasm, pos).validateAndMakeBrOn(op, name, ref, rtt);
  return true;
}