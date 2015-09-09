#include "Filter/SubstringFilter.h"

#include "plog/Log.h"

namespace Hekate
{
namespace Filter
{

SubstringFilter::SubstringFilter(const Protobuf::Proto::Client::AddFilterMessage &msg) :
    HekateFilter(msg)
{
}

void SubstringFilter::ParseInput(const char * const bytes, const int length, FilteredInput &filteredInput)
{
    if (m_msg.type() == Protobuf::Proto::Client::FilterType::SUBSTRING)
    {
        auto &msg = m_msg.string_filter();
        if (msg.length() <= length)
        {
            for (int i = 0; i < length - msg.length(); ++i)
            {
                if (memcmp(&bytes[i], msg.match().c_str(), msg.length()) == 0)
                {
                    filteredInput.hit = true;
                }
            }
        }
    }

    HekateFilter::ApplyFilter(bytes, length, filteredInput);
    HekateFilter::ParseInput(bytes, length, filteredInput);
}

}
}
