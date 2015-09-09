#pragma once

#include "Filter/HekateFilter.h"

namespace Hekate
{
namespace Filter
{

class SubstringFilter final : public HekateFilter
{
public:
    SubstringFilter() = delete;

    SubstringFilter(const Protobuf::Proto::Client::AddFilterMessage &msg);

    ~SubstringFilter() = default;

    void ParseInput(const char * const bytes, const int length, FilteredInput &filteredInput) override;
};

}
}
