#include "filesystem_interfaces.h"
#include <nxemu-module-spec/system_loader.h>

template<typename InterfaceType>
InterfacePtr<InterfaceType>::InterfacePtr() : 
    m_ptr(nullptr) 
{
}

template<typename InterfaceType>
InterfacePtr<InterfaceType>::InterfacePtr(InterfaceType* ptr) : 
    m_ptr(ptr) 
{
}

template<typename InterfaceType>
InterfacePtr<InterfaceType>::InterfacePtr(InterfacePtr&& other) noexcept
    : m_ptr(other.m_ptr) 
{
    other.m_ptr = nullptr;
}

template<typename InterfaceType>
InterfacePtr<InterfaceType>::~InterfacePtr() 
{
    if (m_ptr) 
    {
        m_ptr->Release();
        m_ptr = nullptr;
    }
}

template<typename InterfaceType>
InterfacePtr<InterfaceType> & InterfacePtr<InterfaceType>::operator=(InterfaceType * ptr)
{
    if (m_ptr != ptr && m_ptr != nullptr)
    {
        m_ptr->Release();
    }
    m_ptr = ptr;
    return *this;
}

template<typename InterfaceType>
InterfacePtr<InterfaceType>& InterfacePtr<InterfaceType>::operator=(InterfacePtr&& other) noexcept 
{
    if (this != &other) 
    {
        if (m_ptr) {
            m_ptr->Release();
        }
        m_ptr = other.m_ptr;
        other.m_ptr = nullptr;
    }
    return *this;
}

template<typename InterfaceType>
InterfaceType** InterfacePtr<InterfaceType>::GetAddressForSet() 
{
    if (m_ptr) 
    {
        m_ptr->Release();
        m_ptr = nullptr;
    }
    return &m_ptr;
}

template<typename InterfaceType>
InterfaceType* InterfacePtr<InterfaceType>::operator->() const 
{
    return m_ptr;
}

template<typename InterfaceType>
InterfaceType& InterfacePtr<InterfaceType>::operator*() const 
{
    return *m_ptr;
}

template<typename InterfaceType>
InterfacePtr<InterfaceType>::operator bool() const 
{
    return m_ptr != nullptr;
}

std::vector<uint8_t> IVirtualFilePtr::ReadAllBytes() const
{
    uint64_t dataSize = m_ptr->GetSize();
    if (dataSize == 0)
    {
        return {};
    }
    std::vector<uint8_t> data;
    data.resize(dataSize);
    dataSize = m_ptr->ReadBytes(data.data(), data.size(), 0);
    if (dataSize < data.size())
    {
        data.resize(dataSize);
    }
    return data;
}

template class InterfacePtr<IVirtualDirectory>;
template class InterfacePtr<IVirtualFile>;
template class InterfacePtr<IFileSysNCA>;
template class InterfacePtr<ISaveDataFactory>;
template class InterfacePtr<IRomFsController>;
template class InterfacePtr<IFileSysNACP>;
