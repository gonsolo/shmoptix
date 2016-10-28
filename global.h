#pragma once

#include <iostream>

namespace shmoptix {

const char space = ' ';
const char newline = '\n';

#ifdef LEADING_UNDERSCORE
std::string leading_underscore{"_"};
#else
std::string leading_underscore{""};
#endif

static llvm::LLVMContext Context;
std::unique_ptr<llvm::Module> module = std::make_unique<llvm::Module>("Shmoptix", Context);
llvm::IRBuilder<>& getBuilder();

}
