#include "Capstone/ScopedCapstoneHandle.h"

#include "Capstone/CapstoneDll.h"

#include "plog/Log.h"

namespace Hekate
{
namespace Capstone
{

ScopedCapstoneHandle::ScopedCapstoneHandle(const cs_arch architecture, const cs_mode mode)
{
    const cs_err status = Hekate::Capstone::cs_open(architecture, mode, &m_handle);
    
    if (status != CS_ERR_OK)
    {
        LOG_ERROR << "Could not open Capstone handle. Error = " <<
            status;
    }
    else
    {
        LOG_DEBUG << "Capstone handle successfully initialized.";

        m_bIsValid = true;
    }
}

ScopedCapstoneHandle::~ScopedCapstoneHandle()
{
    if (IsValid())
    {
        cs_err status = Hekate::Capstone::cs_close(&m_handle);
        if (status != CS_ERR_OK)
        {
            LOG_WARNING << "Could not close a valid Capstone handle. Error = " <<
                status;
        }
    }
    else
    {
        LOG_WARNING << "Stopping attempt to close an invalid Capstone handle.";
    }
}

const csh &ScopedCapstoneHandle::Get() const 
{
    return m_handle;
}

const csh &ScopedCapstoneHandle::operator()() const
{
    return Get();
}

const bool ScopedCapstoneHandle::IsValid() const
{
    return m_bIsValid;
}

}
}