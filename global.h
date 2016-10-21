#pragma once

using std::cout;
using std::cerr;
using std::endl;
const char space = ' ';
const char newline = '\n';

static llvm::LLVMContext Context;
std::unique_ptr<llvm::Module> module = std::make_unique<llvm::Module>("shmoptix module", Context);
static llvm::IRBuilder<> Builder(Context);
