#include "Filter/ByteFilter.h"

#include "plog/Log.h"

namespace Hekate
{
namespace Filter
{

ByteFilter::ByteFilter(const Protobuf::Proto::Client::AddFilterMessage &msg) :
    HekateFilter(msg)
{
}

void ByteFilter::ParseInput(const char * const bytes, const int length, FilteredInput &filteredInput)
{
    if (m_msg.type() == Protobuf::Proto::Client::FilterType::BYTE)
    {
        auto &msg = m_msg.byte_filter();
        for (int i = 0; i < msg.byte_count(); ++i)
        {
            if (msg.byte_index(i) <= length && bytes[msg.byte_index(i)] == msg.byte_value(i))
            {
                filteredInput.hit = true;
            }
            else
            {
                filteredInput.hit = false;
                break;
            }
        }
    }

    HekateFilter::ApplyFilter(bytes, length, filteredInput);
    HekateFilter::ParseInput(bytes, length, filteredInput);
}

}
}
