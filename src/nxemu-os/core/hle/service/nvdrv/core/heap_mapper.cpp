// SPDX-FileCopyrightText: 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <mutex>

#include "yuzu_common/range_sets.h"
#include "yuzu_common/range_sets.inc"
#include "yuzu_common/yuzu_assert.h"
#include "core/hle/service/nvdrv/core/heap_mapper.h"
#include <nxemu-module-spec/video.h>

namespace Service::Nvidia::NvCore {

struct HeapMapper::HeapMapperInternal {
    HeapMapperInternal(IVideo & video_) : video{video_} {}
    ~HeapMapperInternal() = default;

    Common::RangeSet<VAddr> m_temporary_set;
    Common::OverlapRangeSet<VAddr> m_mapped_ranges;
    IVideo & video;
    std::mutex m_guard;
};

HeapMapper::HeapMapper(VAddr start_vaddress, DAddr start_daddress, size_t size, Core::Asid asid,
                       IVideo & video)
    : m_vaddress{start_vaddress}, m_daddress{start_daddress}, m_size{size}, m_asid{asid} {
    m_internal = std::make_unique<HeapMapperInternal>(video);
}

HeapMapper::~HeapMapper() {
    // Unmap whatever has been mapped.
    m_internal->m_mapped_ranges.ForEach([this](VAddr start_addr, VAddr end_addr, s32 count) {
        const size_t sub_size = end_addr - start_addr;
        const size_t offset = start_addr - m_vaddress;
        m_internal->video.MemoryUnmap(m_daddress + offset, sub_size);
    });
}

DAddr HeapMapper::Map(VAddr start, size_t size) {
    std::scoped_lock lk(m_internal->m_guard);
    // Add the mapping range to a temporary range set.
    m_internal->m_temporary_set.Clear();
    m_internal->m_temporary_set.Add(start, size);

    // Remove anything that's already mapped from the temporary range set.
    m_internal->m_mapped_ranges.ForEachInRange(
        start, size, [this](VAddr start_addr, VAddr end_addr, s32) {
            m_internal->m_temporary_set.Subtract(start_addr, end_addr - start_addr);
        });

    // Map anything that has not been mapped yet.
    m_internal->m_temporary_set.ForEach([this](VAddr start_addr, VAddr end_addr) {
        const size_t sub_size = end_addr - start_addr;
        const size_t offset = start_addr - m_vaddress;
        m_internal->video.MemoryMap(m_daddress + offset, m_vaddress + offset, sub_size, m_asid.id, false);
    });

    // Add the mapping range to the split map, to register the map and overlaps.
    m_internal->m_mapped_ranges.Add(start, size);
    m_internal->m_temporary_set.Clear();
    return m_daddress + static_cast<DAddr>(start - m_vaddress);
}

void HeapMapper::Unmap(VAddr start, size_t size) {
    std::scoped_lock lk(m_internal->m_guard);

    // Just subtract the range and whatever is deleted, unmap it.
    m_internal->m_mapped_ranges.Subtract(start, size, [this](VAddr start_addr, VAddr end_addr) {
        const size_t sub_size = end_addr - start_addr;
        const size_t offset = start_addr - m_vaddress;
        m_internal->video.MemoryUnmap(m_daddress + offset, sub_size);
    });
}

} // namespace Service::Nvidia::NvCore
