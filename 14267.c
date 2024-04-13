void WasmBinaryBuilder::validateHeapTypeUsingChild(Expression* child,
                                                   HeapType heapType) {
  if (child->type == Type::unreachable) {
    return;
  }
  if ((!child->type.isRef() && !child->type.isRtt()) ||
      !HeapType::isSubType(child->type.getHeapType(), heapType)) {
    throwError("bad heap type: expected " + heapType.toString() +
               " but found " + child->type.toString());
  }
}