bool WasmBinaryBuilder::maybeVisitSIMDLoadStoreLane(Expression*& out,
                                                    uint32_t code) {
  SIMDLoadStoreLaneOp op;
  size_t lanes;
  switch (code) {
    case BinaryConsts::V128Load8Lane:
      op = Load8LaneVec128;
      lanes = 16;
      break;
    case BinaryConsts::V128Load16Lane:
      op = Load16LaneVec128;
      lanes = 8;
      break;
    case BinaryConsts::V128Load32Lane:
      op = Load32LaneVec128;
      lanes = 4;
      break;
    case BinaryConsts::V128Load64Lane:
      op = Load64LaneVec128;
      lanes = 2;
      break;
    case BinaryConsts::V128Store8Lane:
      op = Store8LaneVec128;
      lanes = 16;
      break;
    case BinaryConsts::V128Store16Lane:
      op = Store16LaneVec128;
      lanes = 8;
      break;
    case BinaryConsts::V128Store32Lane:
      op = Store32LaneVec128;
      lanes = 4;
      break;
    case BinaryConsts::V128Store64Lane:
      op = Store64LaneVec128;
      lanes = 2;
      break;
    default:
      return false;
  }
  auto* curr = allocator.alloc<SIMDLoadStoreLane>();
  curr->op = op;
  readMemoryAccess(curr->align, curr->offset);
  curr->index = getLaneIndex(lanes);
  curr->vec = popNonVoidExpression();
  curr->ptr = popNonVoidExpression();
  curr->finalize();
  out = curr;
  return true;
}