#include "Filter/LengthFilter.h"

#include "plog/Log.h"

namespace Hekate
{
namespace Filter
{

LengthFilter::LengthFilter(const Protobuf::Proto::Client::AddFilterMessage &msg) :
    HekateFilter(msg)
{
}

void LengthFilter::ParseInput(const char * const bytes, const int length, FilteredInput &filteredInput)
{
    if (m_msg.type() == Protobuf::Proto::Client::FilterType::LENGTH)
    {
        auto &msg = m_msg.length_filter();
        if (msg.length_type() == Protobuf::Proto::Client::LengthType::LESS &&
            length < msg.length_value())
        {
            LOG_VERBOSE << "Less than filter hit. Size was 0x"
                << std::hex << length << ". Filter value is 0x"
                << std::hex << msg.length_value();
            filteredInput.hit = true;
        }
        else if (msg.length_type() == Protobuf::Proto::Client::LengthType::EQUAL &&
            length == msg.length_value())
        {
            LOG_VERBOSE << "Equal to filter hit. Size was 0x"
                << std::hex << length << ". Filter value is 0x"
                << std::hex << msg.length_value();
            filteredInput.hit = true;
        }
        else if (msg.length_type() == Protobuf::Proto::Client::LengthType::GREATER &&
            length > msg.length_value())
        {
            LOG_VERBOSE << "Greater than filter hit. Size was 0x"
                << std::hex << length << ". Filter value is 0x"
                << std::hex << msg.length_value();
            filteredInput.hit = true;
        }
    }

    HekateFilter::ApplyFilter(bytes, length, filteredInput);
    HekateFilter::ParseInput(bytes, length, filteredInput);
}

}
}
