#include "Filter/HekateFilter.h"

#include "plog/Log.h"

namespace Hekate
{
namespace Filter
{

HekateFilter::HekateFilter() :
    m_pNextFilter{ nullptr },
    m_id{ TOPMOST_FILTER }
{
}

HekateFilter::HekateFilter(const Protobuf::Proto::Client::AddFilterMessage &msg) :
    m_msg{ msg },
    m_pNextFilter{ nullptr }
{
    m_id = m_msg.filter_id();
}

void HekateFilter::AddFilter(HekateFilter * const pFilter)
{
    if (m_pNextFilter != nullptr)
    {
        m_pNextFilter->AddFilter(pFilter);
    }
    else
    {
        m_pNextFilter = std::unique_ptr<HekateFilter>(pFilter);
    }
}

const bool HekateFilter::RemoveFilter(const int filterId)
{
    if (m_pNextFilter == nullptr)
    {
        return false;
    }
    if (m_pNextFilter != nullptr && m_pNextFilter->Id() == filterId)
    {
        m_pNextFilter = std::move(m_pNextFilter->m_pNextFilter);
        return true;
    }
    else
    {
        return m_pNextFilter->RemoveFilter(filterId);
    }
}

void HekateFilter::ParseInput(const char * const bytes, const int length, FilteredInput &filteredInput)
{
    if (!filteredInput.hit && m_pNextFilter != nullptr)
    {
        m_pNextFilter->ParseInput(bytes, length, filteredInput);
    }
}

void HekateFilter::ApplyFilter(const char * const bytes, const int length, FilteredInput &filteredInput)
{
    if (filteredInput.hit)
    {
        if (m_msg.has_substitute())
        {
            LOG_VERBOSE << "Found substitute rule for filter 0x"
                << std::hex << m_msg.filter_id();

            std::unique_ptr<char[]> newBytes(new char[length]);
            (void)memcpy(newBytes.get(), bytes, length);

            for (int i = 0; i < m_msg.substitute().byte_count(); ++i)
            {
                newBytes[m_msg.substitute().byte_index(i)] = (char)m_msg.substitute().byte_value(i);
            }

            filteredInput.bytes = std::move(newBytes);
            filteredInput.length = length;
            filteredInput.newData = true;
        }
        else if (m_msg.has_replacement())
        {
            LOG_VERBOSE << "Found replacement rule for filter 0x"
                << std::hex << m_msg.filter_id();

            const int newLength = m_msg.replacement().length();
            std::unique_ptr<char[]> newBytes(new char[newLength]);
            (void)memcpy(newBytes.get(), m_msg.replacement().buffer().c_str(), newLength);

            filteredInput.bytes = std::move(newBytes);
            filteredInput.length = newLength;
            filteredInput.newData = true;
        }
        else
        {
            LOG_VERBOSE << "Filter was hit but no rules for substitute/replace exist. "
                << "Filter id = 0x" << std::hex << m_msg.filter_id();
        }

        filteredInput.id = m_msg.filter_id();
        filteredInput.breakOnHit = m_msg.break_on_hit();
    }
}

const int HekateFilter::Id() const
{
    return m_id;
}

}
}
