#include "Capstone/ScopedCapstoneInstructions.h"

#include "Capstone/CapstoneDll.h"

#include "plog/Log.h"

namespace Hekate
{
namespace Capstone
{

ScopedCapstoneInstructions::ScopedCapstoneInstructions() :
    m_pInstructions{ nullptr },
    m_count{ 0 }
{
}

ScopedCapstoneInstructions::~ScopedCapstoneInstructions()
{
    if (IsValid())
    {
        Hekate::Capstone::cs_free(m_pInstructions, m_count);
    }
}

const size_t ScopedCapstoneInstructions::Count() const
{
    return m_count;
}

const cs_insn * const ScopedCapstoneInstructions::Instructions() const
{
    return m_pInstructions;
}

size_t * const ScopedCapstoneInstructions::CountPtr()
{
    return &m_count;
}

cs_insn **ScopedCapstoneInstructions::InstructionsPtr()
{
    return &m_pInstructions;
}

const bool ScopedCapstoneInstructions::IsValid() const
{
    return Instructions() != nullptr && Count() > 0;
}

}
}