//===- BasicBlockCompiler.h -  -------*- C++ -*-===//
//
// Copyright (c) 2016, Intel Corporation
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// Neither the name of the Intel Corporation nor the names of its contributors
// may be used to endorse or promote products derived from this software
// without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//===----------------------------------------------------------------------===//
//
//  This file contains some hooks for compiling basic blocks. Implementation is 
//  left to the particular compiler.  
//
//===----------------------------------------------------------------------===//
// Author: kefleming (Kermin Fleming)
//===----------------------------------------------------------------------===//
//
// This file contains the shared typedefs for FPGA analyses

#ifndef LLVM_BASIC_BLOCK_COMPILER_H
#define LLVM_BASIC_BLOCK_COMPILER_H

#include "llvm/Pass.h"
#include "llvm/IR/PassManager.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/ValueMap.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Transforms/Utils/ValueMapper.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/breadth_first_search.hpp>
//#include <boost/graph/graphviz.hpp>

#include "BasicBlockDumper.h"

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <map>
#include <list>
#include <string>

using namespace llvm;

typedef struct {
  int area;
  int latency;
  int ii;
} BlockMetadata;

#define ASSUME_PIPELINE 1

void compileBlock(BasicBlock *BB);

// Export a C interface so that dynamic linking is a bit simpler

extern std::map<BasicBlock*, BlockMetadata> *metaMap;

extern "C" {

  int getBlockArea(BasicBlock *BB) {
    if (metaMap == NULL ) {
      metaMap = new std::map<BasicBlock*, BlockMetadata>();
    }

    auto it = metaMap->find(BB);
    if (it == metaMap->end()) {
        compileBlock(BB);
    }

    return metaMap->find(BB)->second.area;
  }

  int getBlockLatency(BasicBlock *BB) {
    if (metaMap == NULL ) {
      metaMap = new std::map<BasicBlock*, BlockMetadata>();
    }

    auto it = metaMap->find(BB);
    if (it == metaMap->end()) {
        compileBlock(BB);
    }

    return metaMap->find(BB)->second.latency;
  }

 int getBlockII(BasicBlock *BB) {
    if (metaMap == NULL ) {
      metaMap = new std::map<BasicBlock*, BlockMetadata>();
    }

    auto it = metaMap->find(BB);
    if (it == metaMap->end()) {
        compileBlock(BB);
    }

#ifdef ASSUME_PIPELINE
    return metaMap->find(BB)->second.ii;
#else
    return metaMap->find(BB)->second.latency;
#endif
    
  }
    
}



#endif