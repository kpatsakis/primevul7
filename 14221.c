void WasmBinaryBuilder::processNames() {
  for (auto* func : functions) {
    wasm.addFunction(func);
  }
  for (auto& global : globals) {
    wasm.addGlobal(std::move(global));
  }
  for (auto& table : tables) {
    wasm.addTable(std::move(table));
  }
  for (auto& segment : elementSegments) {
    wasm.addElementSegment(std::move(segment));
  }

  // now that we have names, apply things

  if (startIndex != static_cast<Index>(-1)) {
    wasm.start = getFunctionName(startIndex);
  }

  for (auto* curr : exportOrder) {
    auto index = exportIndices[curr];
    switch (curr->kind) {
      case ExternalKind::Function: {
        curr->value = getFunctionName(index);
        break;
      }
      case ExternalKind::Table:
        curr->value = getTableName(index);
        break;
      case ExternalKind::Memory:
        curr->value = wasm.memory.name;
        break;
      case ExternalKind::Global:
        curr->value = getGlobalName(index);
        break;
      case ExternalKind::Tag:
        curr->value = getTagName(index);
        break;
      default:
        throwError("bad export kind");
    }
    wasm.addExport(curr);
  }

  for (auto& [index, refs] : functionRefs) {
    for (auto* ref : refs) {
      if (auto* call = ref->dynCast<Call>()) {
        call->target = getFunctionName(index);
      } else if (auto* refFunc = ref->dynCast<RefFunc>()) {
        refFunc->func = getFunctionName(index);
      } else {
        WASM_UNREACHABLE("Invalid type in function references");
      }
    }
  }

  for (auto& [index, refs] : tableRefs) {
    for (auto* ref : refs) {
      if (auto* callIndirect = ref->dynCast<CallIndirect>()) {
        callIndirect->table = getTableName(index);
      } else if (auto* get = ref->dynCast<TableGet>()) {
        get->table = getTableName(index);
      } else if (auto* set = ref->dynCast<TableSet>()) {
        set->table = getTableName(index);
      } else if (auto* size = ref->dynCast<TableSize>()) {
        size->table = getTableName(index);
      } else if (auto* grow = ref->dynCast<TableGrow>()) {
        grow->table = getTableName(index);
      } else {
        WASM_UNREACHABLE("Invalid type in table references");
      }
    }
  }

  for (auto& [index, refs] : globalRefs) {
    for (auto* ref : refs) {
      if (auto* get = ref->dynCast<GlobalGet>()) {
        get->name = getGlobalName(index);
      } else if (auto* set = ref->dynCast<GlobalSet>()) {
        set->name = getGlobalName(index);
      } else {
        WASM_UNREACHABLE("Invalid type in global references");
      }
    }
  }

  // Everything now has its proper name.

  wasm.updateMaps();
}