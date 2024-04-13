void WasmBinaryBuilder::readNames(size_t payloadLen) {
  BYN_TRACE("== readNames\n");
  auto sectionPos = pos;
  uint32_t lastType = 0;
  while (pos < sectionPos + payloadLen) {
    auto nameType = getU32LEB();
    if (lastType && nameType <= lastType) {
      std::cerr << "warning: out-of-order name subsection: " << nameType
                << std::endl;
    }
    lastType = nameType;
    auto subsectionSize = getU32LEB();
    auto subsectionPos = pos;
    if (nameType == BinaryConsts::UserSections::Subsection::NameModule) {
      wasm.name = getInlineString();
    } else if (nameType ==
               BinaryConsts::UserSections::Subsection::NameFunction) {
      auto num = getU32LEB();
      NameProcessor processor;
      for (size_t i = 0; i < num; i++) {
        auto index = getU32LEB();
        auto rawName = getInlineString();
        auto name = processor.process(rawName);
        auto numFunctionImports = functionImports.size();
        if (index < numFunctionImports) {
          functionImports[index]->setExplicitName(name);
        } else if (index - numFunctionImports < functions.size()) {
          functions[index - numFunctionImports]->setExplicitName(name);
        } else {
          std::cerr << "warning: function index out of bounds in name section, "
                       "function subsection: "
                    << std::string(rawName.str) << " at index "
                    << std::to_string(index) << std::endl;
        }
      }
    } else if (nameType == BinaryConsts::UserSections::Subsection::NameLocal) {
      auto numFuncs = getU32LEB();
      auto numFunctionImports = functionImports.size();
      for (size_t i = 0; i < numFuncs; i++) {
        auto funcIndex = getU32LEB();
        Function* func = nullptr;
        if (funcIndex < numFunctionImports) {
          func = functionImports[funcIndex];
        } else if (funcIndex - numFunctionImports < functions.size()) {
          func = functions[funcIndex - numFunctionImports];
        } else {
          std::cerr
            << "warning: function index out of bounds in name section, local "
               "subsection: "
            << std::to_string(funcIndex) << std::endl;
        }
        auto numLocals = getU32LEB();
        NameProcessor processor;
        for (size_t j = 0; j < numLocals; j++) {
          auto localIndex = getU32LEB();
          auto rawLocalName = getInlineString();
          if (!func) {
            continue; // read and discard in case of prior error
          }
          auto localName = processor.process(rawLocalName);
          if (localIndex < func->getNumLocals()) {
            func->localNames[localIndex] = localName;
          } else {
            std::cerr << "warning: local index out of bounds in name "
                         "section, local subsection: "
                      << std::string(rawLocalName.str) << " at index "
                      << std::to_string(localIndex) << " in function "
                      << std::string(func->name.str) << std::endl;
          }
        }
      }
    } else if (nameType == BinaryConsts::UserSections::Subsection::NameType) {
      auto num = getU32LEB();
      NameProcessor processor;
      for (size_t i = 0; i < num; i++) {
        auto index = getU32LEB();
        auto rawName = getInlineString();
        auto name = processor.process(rawName);
        if (index < types.size()) {
          wasm.typeNames[types[index]].name = name;
        } else {
          std::cerr << "warning: type index out of bounds in name section, "
                       "type subsection: "
                    << std::string(rawName.str) << " at index "
                    << std::to_string(index) << std::endl;
        }
      }
    } else if (nameType == BinaryConsts::UserSections::Subsection::NameTable) {
      auto num = getU32LEB();
      NameProcessor processor;
      for (size_t i = 0; i < num; i++) {
        auto index = getU32LEB();
        auto rawName = getInlineString();
        auto name = processor.process(rawName);
        auto numTableImports = tableImports.size();
        auto setTableName = [&](Table* table) {
          for (auto& segment : elementSegments) {
            if (segment->table == table->name) {
              segment->table = name;
            }
          }
          table->setExplicitName(name);
        };

        if (index < numTableImports) {
          setTableName(tableImports[index]);
        } else if (index - numTableImports < tables.size()) {
          setTableName(tables[index - numTableImports].get());
        } else {
          std::cerr << "warning: table index out of bounds in name section, "
                       "table subsection: "
                    << std::string(rawName.str) << " at index "
                    << std::to_string(index) << std::endl;
        }
      }
    } else if (nameType == BinaryConsts::UserSections::Subsection::NameElem) {
      auto num = getU32LEB();
      NameProcessor processor;
      for (size_t i = 0; i < num; i++) {
        auto index = getU32LEB();
        auto rawName = getInlineString();
        auto name = processor.process(rawName);

        if (index < elementSegments.size()) {
          elementSegments[index]->setExplicitName(name);
        } else {
          std::cerr << "warning: elem index out of bounds in name section, "
                       "elem subsection: "
                    << std::string(rawName.str) << " at index "
                    << std::to_string(index) << std::endl;
        }
      }
    } else if (nameType == BinaryConsts::UserSections::Subsection::NameMemory) {
      auto num = getU32LEB();
      for (size_t i = 0; i < num; i++) {
        auto index = getU32LEB();
        auto rawName = getInlineString();
        if (index == 0) {
          wasm.memory.setExplicitName(escape(rawName));
        } else {
          std::cerr << "warning: memory index out of bounds in name section, "
                       "memory subsection: "
                    << std::string(rawName.str) << " at index "
                    << std::to_string(index) << std::endl;
        }
      }
    } else if (nameType == BinaryConsts::UserSections::Subsection::NameData) {
      auto num = getU32LEB();
      for (size_t i = 0; i < num; i++) {
        auto index = getU32LEB();
        auto rawName = getInlineString();
        if (index < wasm.memory.segments.size()) {
          wasm.memory.segments[i].name = rawName;
        } else {
          std::cerr << "warning: memory index out of bounds in name section, "
                       "memory subsection: "
                    << std::string(rawName.str) << " at index "
                    << std::to_string(index) << std::endl;
        }
      }
    } else if (nameType == BinaryConsts::UserSections::Subsection::NameGlobal) {
      auto num = getU32LEB();
      NameProcessor processor;
      for (size_t i = 0; i < num; i++) {
        auto index = getU32LEB();
        auto rawName = getInlineString();
        auto name = processor.process(rawName);
        auto numGlobalImports = globalImports.size();
        if (index < numGlobalImports) {
          globalImports[index]->setExplicitName(name);
        } else if (index - numGlobalImports < globals.size()) {
          globals[index - numGlobalImports]->setExplicitName(name);
        } else {
          std::cerr << "warning: global index out of bounds in name section, "
                       "global subsection: "
                    << std::string(rawName.str) << " at index "
                    << std::to_string(index) << std::endl;
        }
      }
    } else if (nameType == BinaryConsts::UserSections::Subsection::NameField) {
      auto numTypes = getU32LEB();
      for (size_t i = 0; i < numTypes; i++) {
        auto typeIndex = getU32LEB();
        bool validType =
          typeIndex < types.size() && types[typeIndex].isStruct();
        if (!validType) {
          std::cerr << "warning: invalid field index in name field section\n";
        }
        auto numFields = getU32LEB();
        NameProcessor processor;
        for (size_t i = 0; i < numFields; i++) {
          auto fieldIndex = getU32LEB();
          auto rawName = getInlineString();
          auto name = processor.process(rawName);
          if (validType) {
            wasm.typeNames[types[typeIndex]].fieldNames[fieldIndex] = name;
          }
        }
      }
    } else {
      std::cerr << "warning: unknown name subsection with id "
                << std::to_string(nameType) << " at " << pos << std::endl;
      pos = subsectionPos + subsectionSize;
    }
    if (pos != subsectionPos + subsectionSize) {
      throwError("bad names subsection position change");
    }
  }
  if (pos != sectionPos + payloadLen) {
    throwError("bad names section position change");
  }
}