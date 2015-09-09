#pragma once

#include "Filter/HekateFilter.h"

namespace Hekate
{
namespace Filter
{

class LengthFilter final : public HekateFilter
{
public:
    LengthFilter() = delete;

    LengthFilter(const Protobuf::Proto::Client::AddFilterMessage &msg);

    ~LengthFilter() = default;

    void ParseInput(const char * const bytes, const int length, FilteredInput &filteredInput) override;
};

}
}
