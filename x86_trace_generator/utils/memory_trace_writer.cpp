//
// Copyright (C) 2024  HiPES - Universidade Federal do Paraná
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

/**
 * @file memory_trace_writer.cpp
 * @details Implementation of MemoryTraceFile class.
 */

#include "memory_trace_writer.hpp"

#include "file_handler.hpp"
#include <cstdio>
#include <cstring>

extern "C" {
#include <alloca.h>
}

int MemoryTraceWriter::OpenFile(const char* sourceDir, const char* imageName,
                                int tid) {
    unsigned long bufferSize;

    bufferSize = GetPathTidInSize(sourceDir, "memory", imageName);
    char* path = (char*)alloca(bufferSize);
    FormatPathTidIn(path, sourceDir, imageName, MEMORY_TRACE_SUFFIX, tid, bufferSize);
    this->file = gzopen(path, "w");
    if (this->file == NULL) {
        printf("Failed to alloc this->file\n");
        return 1;
    }

    return 0;
}

int MemoryTraceWriter::AddNumberOfMemOperations(unsigned int numMemOps) {
    static char numOpsFormattedStr[sizeof('$') + MAX_U32_DIGITS + sizeof('\n') + sizeof('\0')];
    sprintf(numOpsFormattedStr, "$%u\n", numMemOps);
    unsigned long len = strlen(numOpsFormattedStr);
    return (gzwrite(this->file, numOpsFormattedStr, (unsigned int)len) != (int)len);
}

int MemoryTraceWriter::AddMemOp(unsigned long address, unsigned int size,
                                bool isLoadOp, int bbl) {
    const char* operation = (isLoadOp) ? "R" : "W";
    char* memoryOperation = (char*)alloca(strlen(operation) + sizeof(' ') +
                                        MAX_U32_DIGITS + sizeof(' ') +
                                        MAX_U64_DIGITS + sizeof(' ') +
                                        MAX_U32_DIGITS + sizeof('\n') + sizeof('\0'));

    sprintf(memoryOperation, "%s %u %lu %u\n", operation, size, address, bbl);
    unsigned long len = strlen(memoryOperation);
    return (gzwrite(this->file, memoryOperation, (unsigned int)len) != (int)len);
}
