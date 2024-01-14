#include "stdafx.h"
#include "AggregatedSubPicProvider.h"

//
// CAggregatedSubPicProvider
//

void CAggregatedSubPicProvider::setProviders(const std::vector<CRenderedTextSubtitle*>& providers) {
    Lock();
    m_providers = providers;
    size_t cnt = m_providers.size();
    for (size_t i = 0; i < cnt; ++i) {
        CRenderedTextSubtitle* pRTS = m_providers.at(i);
        if (cnt > 1) {
            // TODO options for placement
            pRTS->SetAlignment(true, 50, 90 - 30 * i);
        }
        else {
            pRTS->SetAlignment(false, 50, 90);
        }
    }
    Unlock();
}

STDMETHODIMP CAggregatedSubPicProvider::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
    CheckPointer(ppv, E_POINTER);
    *ppv = nullptr;
    return
        QI(IPersist)
        QI(ISubStream)
        QI(ISubPicProvider)
        __super::NonDelegatingQueryInterface(riid, ppv);
}

STDMETHODIMP CAggregatedSubPicProvider::GetClassID(CLSID* pClassID)
{
    return pClassID ? *pClassID = __uuidof(CRenderedTextSubtitle), S_OK : E_POINTER;
}

STDMETHODIMP_(int) CAggregatedSubPicProvider::GetStreamCount()
{
    return 1;
}

STDMETHODIMP CAggregatedSubPicProvider::GetStreamInfo(int iStream, WCHAR** ppName, LCID* pLCID)
{
    Lock();
    HRESULT hr = m_providers.empty() ? S_OK : (*m_providers.begin())->GetStreamInfo(iStream, ppName, pLCID);
    Unlock();
    return hr;
}

STDMETHODIMP_(int) CAggregatedSubPicProvider::GetStream()
{
    return 0;
}

STDMETHODIMP CAggregatedSubPicProvider::SetStream(int iStream)
{
    return iStream == 0 ? S_OK : E_FAIL;
}

STDMETHODIMP CAggregatedSubPicProvider::Reload()
{
    Lock();
    HRESULT hr = m_providers.empty() ? S_OK : (*m_providers.begin())->Reload();
    Unlock();
    return hr;
}

STDMETHODIMP CAggregatedSubPicProvider::SetSourceTargetInfo(CString yuvVideoMatrix, int targetBlackLevel, int targetWhiteLevel)
{
    Lock();
    HRESULT hr = m_providers.empty() ? S_OK : (*m_providers.begin())->SetSourceTargetInfo(yuvVideoMatrix, targetBlackLevel, targetWhiteLevel);
    Unlock();
    return hr;
}

STDMETHODIMP_(POSITION) CAggregatedSubPicProvider::GetStartPosition(REFERENCE_TIME rt, double fps)
{
    Lock();
    if (m_providers.empty()) {
        return nullptr;
    }
    CRenderedTextSubtitle* spp = *m_providers.begin();
    POSITION pos = spp->GetStartPosition(rt, fps);
    Unlock();
    return pos;
}

STDMETHODIMP_(POSITION) CAggregatedSubPicProvider::GetNext(POSITION pos)
{
    Lock();
    POSITION ret = m_providers.empty() ? nullptr : (*m_providers.begin())->GetNext(pos);
    Unlock();
    return ret;
}

STDMETHODIMP_(REFERENCE_TIME) CAggregatedSubPicProvider::GetStart(POSITION pos, double fps)
{
    Lock();
    REFERENCE_TIME rt = m_providers.empty() ? 0 : (*m_providers.begin())->GetStart(pos, fps);
    Unlock();
    return rt;
}

STDMETHODIMP_(REFERENCE_TIME) CAggregatedSubPicProvider::GetStop(POSITION pos, double fps)
{
    Lock();
    REFERENCE_TIME rt = m_providers.empty() ? 0 : (*m_providers.begin())->GetStop(pos, fps);
    Unlock();
    return rt;
}

STDMETHODIMP_(bool) CAggregatedSubPicProvider::IsAnimated(POSITION pos)
{
    Lock();
    bool result = m_providers.empty() ? false : (*m_providers.begin())->IsAnimated(pos);
    Unlock();
    return result;
}

STDMETHODIMP CAggregatedSubPicProvider::Render(SubPicDesc& spd, REFERENCE_TIME rt, double fps, RECT& bbox)
{
    Lock();
    std::vector<CRenderedTextSubtitle*> providers(m_providers);
    Unlock();

    HRESULT hr = S_OK;
    for (CRenderedTextSubtitle* p : providers) {
        hr = p->Render(spd, rt, fps, bbox);
    }
    return hr;
}
