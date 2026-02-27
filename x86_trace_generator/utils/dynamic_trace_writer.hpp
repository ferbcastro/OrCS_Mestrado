#ifndef SINUCA3_GENERATOR_DYNAMIC_FILE_HPP_
#define SINUCA3_GENERATOR_DYNAMIC_FILE_HPP_

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
 * @file dynamic_trace_writer.hpp
 * @details A dynamic trace stores the basic blocks that were executed and
 * thread events (e.g. when a thread reaches a barrier). The trace reader uses
 * this info to simulate an execution. This file defines the DynamicTraceWriter
 * class which encapsulates the dynamic trace file and the methods that may
 * modify it.
 */

#include "file_handler.hpp"
#include "../../utils/macros.hpp"

extern "C" {
#include <zlib.h>
}

/** @brief Check dynamic_trace_writer.hpp documentation for details */
class DynamicTraceWriter {
  private:
    gzFile file;

  public:
    inline DynamicTraceWriter() : file(NULL) {};
    inline ~DynamicTraceWriter() {
        if (file) {
            gzclose(this->file);
        }
    }

    /** @brief  */
    int OpenFile(const char* sourceDir, const char* img, int tid);
    /**
     * @brief Add thread event record to the trace file.
     * @param type Event type
    */
    int AddThreadEvent(ThreadEventType evType);
    /** @brief Add the identifier of basic block executed. */
    int AddBasicBlockTag(unsigned int basicBlockTag);
};

#endif
