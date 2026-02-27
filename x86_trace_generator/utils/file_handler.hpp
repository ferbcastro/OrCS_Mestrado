#ifndef SINUCA3_SINUCA_TRACER_FILE_HANDLER_HPP_
#define SINUCA3_SINUCA_TRACER_FILE_HANDLER_HPP_

//
// Copyright (C) 2025  HiPES - Universidade Federal do Paraná
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
 * @file
 * @brief Common trace file handling API.
 */

extern "C" {
#include <alloca.h>
#include <stdint.h>
}

#include "../../utils/macros.hpp"

#define _PACKED __attribute__((packed))

const int MAX_U64_DIGITS = 20;
const int MAX_U32_DIGITS = 10;

const int MAX_IMAGE_NAME_SIZE = 255;
const int RECORD_ARRAY_SIZE = 10000;
const int CURRENT_TRACE_VERSION = 1;
const unsigned char MAGIC_NUMBER = 187;

const char STATIC_TRACE_SUFFIX[] = "stat_out.gz";
const char DYNAMIC_TRACE_SUFFIX[] = "dyn_out.gz";
const char MEMORY_TRACE_SUFFIX[] = "mem_out.gz";

const char TRACE_TARGET_X86[] = "X86";
const char TRACE_TARGET_ARM[] = "ARM";
const char TRACE_TARGET_RISCV[] = "RISCV";

enum TargetArch : uint8_t { TargetArchX86, TargetArchARM, TargetArchRISCV };

enum ThreadEventType : uint32_t {
    ThreadEventBarrierSync,
    ThreadEventCriticalStart,
    ThreadEventCriticalEnd,
    ThreadEventAbruptEnd
};

enum MemoryRecordType : uint8_t {
    MemoryRecordHeader,
    MemoryRecordLoad,
    MemoryRecordStore
};

/**
 * @brief Get max size of the formatted path string that includes the thread id.
 * @param sourceDir Complete path to the directory that stores the traces.
 * @param
 * @param
 */
unsigned long GetPathTidInSize(const char *sourceDir, const char *prefix,
                               const char *suffix);

/**
 * @brief Format the path in dest string including the thread id.
 * @param sourceDir Complete path to the directory that stores the traces.
 * @param prefix
 * @param
 * @param tid Thread identier
 * @param destSize Max capacity of dest string.
 */
void FormatPathTidIn(char *dest, const char *sourceDir, const char *prefix,
                     const char *suffix, int tid, long destSize);

/**
 * @brief Get size of the formatted path string without the thread id.
 * @param sourceDir Complete path to the directory that stores the traces.
 * @param prefix 'dynamic', 'memory' or 'static'
 * @param
 */
unsigned long GetPathTidOutSize(const char *sourceDir, const char *prefix,
                                const char *suffix);

/**
 * @brief Format the path in dest string without the thread id.
 * @param sourceDir Complete path to the directory that stores the traces.
 * @param prefix
 * @param
 * @param destSize Max capacity of dest string.
 */
void FormatPathTidOut(char *dest, const char *sourceDir, const char *prefix,
                      const char *suffix, long destSize);

#endif
