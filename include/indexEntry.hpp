#pragma once

struct IndexEntry {
	uint32_t offset;
	uint32_t size;

	IndexEntry() : offset(0), size(0) {}

	IndexEntry(uint32_t offset, uint32_t size) : offset(offset), size(size) {}

	bool operator==(const IndexEntry& other) const {
		return offset == other.offset && size == other.size;
	}
};

namespace std {
    template <>
    struct hash<IndexEntry> {
        std::size_t operator()(const IndexEntry& entry) const noexcept {
            std::size_t h1 = std::hash<uint32_t>{}(entry.offset);
            std::size_t h2 = std::hash<uint32_t>{}(entry.size);
            return h1 ^ (h2 << 1);
        }
    };
}