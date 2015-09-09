#pragma once

#include "Protobuf/Proto/Client/HekateClientProto.pb.h"

#include <memory>
#include <set>

namespace Hekate
{
namespace Filter
{

typedef struct HekateFilteredInput
{
    HekateFilteredInput() :
        id{ 0 },
        bytes{ nullptr },
        length{ 0 },
        hit{ false },
        newData{ false },
        breakOnHit{ false }
    {
    }

    HekateFilteredInput(const HekateFilteredInput &copy)
    {
        *this = copy;
    }

    HekateFilteredInput &operator=(const HekateFilteredInput &copy)
    {
        id = copy.id;
        bytes = std::unique_ptr<char[]>(new char[copy.length]);
        (void)memcpy(bytes.get(), copy.bytes.get(), copy.length);

        length = copy.length;
        hit = copy.hit;
        newData = copy.newData;
        breakOnHit = copy.breakOnHit;

        return *this;
    }

    int id;
    std::unique_ptr<char[]> bytes;
    int length;
    bool hit;
    bool newData;
    bool breakOnHit;

} FilteredInput;

class HekateFilter
{
public:
    HekateFilter();
    HekateFilter(const Protobuf::Proto::Client::AddFilterMessage &msg);

    virtual ~HekateFilter() = default;

    void AddFilter(HekateFilter * const pFilter);
    const bool RemoveFilter(const int filterId);

    virtual void ParseInput(const char * const bytes, const int length, FilteredInput &filteredInput);

    const int Id() const;

private:
    std::unique_ptr<HekateFilter> m_pNextFilter;
    int m_id;

    static const int TOPMOST_FILTER = -1;

protected:
    void ApplyFilter(const char * const bytes, const int length, FilteredInput &filteredInput);
    Protobuf::Proto::Client::AddFilterMessage m_msg;
};

}
}

