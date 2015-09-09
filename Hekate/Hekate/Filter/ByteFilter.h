#pragma once

#include "Filter/HekateFilter.h"

#include <memory>

namespace Hekate
{
namespace Filter
{

class ByteFilter final : public HekateFilter
{
public:
    ByteFilter() = delete;

    ByteFilter(const Protobuf::Proto::Client::AddFilterMessage &msg);

    ~ByteFilter() = default;

    void ParseInput(const char * const bytes, const int length, FilteredInput &filteredInput) override;
};

}
}
