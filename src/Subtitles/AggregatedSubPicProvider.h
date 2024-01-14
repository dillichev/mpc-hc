#pragma once

#include "../SubPic/SubPicProviderImpl.h"
#include "RTS.h"
#include <vector>

/*
 * Construct a subtitles' pic provider from multiple underlying providers.
 * Redirect all but rendering to 1st provider in the list.
 */
class __declspec(uuid("462c0d95-5829-4628-a9b6-d18f4b34419e"))
        CAggregatedSubPicProvider : public CSubPicProviderImpl, public ISubStream {
public:
    CAggregatedSubPicProvider(CCritSec* pLock) : CSubPicProviderImpl(pLock) {};
    virtual ~CAggregatedSubPicProvider() {};

    void setProviders(const std::vector<CRenderedTextSubtitle*>& providers);

    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    // IPersist
    STDMETHODIMP GetClassID(CLSID* pClassID);

    // ISubStream
    STDMETHODIMP_(int) GetStreamCount();
    STDMETHODIMP GetStreamInfo(int i, WCHAR** ppName, LCID* pLCID);
    STDMETHODIMP_(int) GetStream();
    STDMETHODIMP SetStream(int iStream);
    STDMETHODIMP Reload();
    STDMETHODIMP SetSourceTargetInfo(CString yuvMatrix, int targetBlackLevel, int targetWhiteLevel);

    // ISubPicProvider
    STDMETHODIMP_(POSITION) GetStartPosition(REFERENCE_TIME rt, double fps);
    STDMETHODIMP_(POSITION) GetNext(POSITION pos);
    STDMETHOD_(bool, IsAnimated)(POSITION pos);
    STDMETHODIMP_(REFERENCE_TIME) GetStart(POSITION pos, double fps);
    STDMETHODIMP_(REFERENCE_TIME) GetStop(POSITION pos, double fps);
    STDMETHODIMP Render(SubPicDesc& spd, REFERENCE_TIME rt, double fps, RECT& bbox);

private:
    std::vector<CRenderedTextSubtitle*> m_providers;
};
