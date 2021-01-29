/*
** Copyright (c) 2021 LunarG, Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and associated documentation files (the "Software"),
** to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense,
** and/or sell copies of the Software, and to permit persons to whom the
** Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
** DEALINGS IN THE SOFTWARE.
*/

#ifndef MEMORY_RESOURCE_H
#define MEMORY_RESOURCE_H

#include <iostream>
#include <memory>
#include <vector>

// Monotonic Memory Resource
// This is a memory resource that is intended to be used with a std::allocator replacement
// for stdlib container allocations. As such, it can be called to allocate the memory for the
// objects in a container, as well as for other allocations performed by the container itself.
// Because the container can request memory of any type, this is an untyped allocator.
class MonotonicMemoryResource {
  public:
    // block_size is the size of the individual memory blocks allocated. The number of blocks increases as needed to
    // fit requested allocations, and blocks are freed using an appropriate call to Clear or upon destruction of this
    // MonotonicMemoryResource.
    MonotonicMemoryResource(std::size_t block_size)
        : block_size_(block_size), current_block_(0), current_block_free_bytes_(block_size) {}
    ~MonotonicMemoryResource() { Clear(true); }

    // "Frees" all previously allocated objects. Depending on free_system_memory, system memory blocks are either
    // reused for new calls to Allocate or freed and re-created as needed. Oversized allocations are freed from system
    // memory
    void Clear(bool free_system_memory);

    void* Allocate(std::size_t object_bytes, std::size_t alignment_bytes);
    size_t BlocksInUse() { return memory_blocks_.size(); }

  private:
    void* AllocateToBlock(std::size_t object_bytes, std::size_t alignment_bytes);

    std::vector<std::unique_ptr<unsigned char[]>> memory_blocks_;
    std::vector<std::unique_ptr<unsigned char[]>> oversized_allocations_;
    const std::size_t block_size_;
    std::size_t current_block_;
    std::size_t current_block_free_bytes_;
};

#endif  // MEMORY_RESOURCE_H
