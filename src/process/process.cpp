#include <Windows.h>
#include <Wbemidl.h>
#include <comdef.h>

import std;

#pragma comment(lib, "wbemuuid.lib")

using namespace std;


void checkProcess(const std::wstring_view& proc_path) {
	std::wcout << L"Process started: " << proc_path << std::endl;
}
class EventSink : public IWbemObjectSink
{
    LONG m_lRef;
    bool bDone;

public:
    EventSink() : m_lRef(0), bDone(false) {}
    virtual ~EventSink() {}

    virtual ULONG STDMETHODCALLTYPE AddRef() {
        return InterlockedIncrement(&m_lRef);
    }

    virtual ULONG STDMETHODCALLTYPE Release() {
        LONG lRef = InterlockedDecrement(&m_lRef);
        if (lRef == 0)
            delete this;
        return lRef;
    }

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) {
        if (riid == IID_IUnknown || riid == IID_IWbemObjectSink) {
            *ppv = (IWbemObjectSink*)this;
            AddRef();
            return WBEM_S_NO_ERROR;
        }
        else return E_NOINTERFACE;
    }

    virtual HRESULT STDMETHODCALLTYPE Indicate(
        LONG lObjectCount,
        IWbemClassObject __RPC_FAR* __RPC_FAR* apObjArray)
    {
        HRESULT hr = S_OK;

        for (int i = 0; i < lObjectCount; i++) {
            IWbemClassObject* pObj = apObjArray[i];
            VARIANT vtProp;

            // Get the value of the Name property
            hr = pObj->Get(L"TargetInstance", 0, &vtProp, 0, 0);
            if (!FAILED(hr)) {
                IWbemClassObject* pClassObject = NULL;
                hr = vtProp.punkVal->QueryInterface(IID_IWbemClassObject, (void**)&pClassObject);
                if (SUCCEEDED(hr)) {
                    VARIANT vtProcessPath;
                    hr = pClassObject->Get(L"ExecutablePath", 0, &vtProcessPath, 0, 0);
                    if (SUCCEEDED(hr) && vtProcessPath.vt == VT_BSTR) {
                        std::wstring_view proc_path(vtProcessPath.bstrVal);
                        checkProcess(proc_path);  // Call the function to check the process
                        VariantClear(&vtProcessPath);
                    }
                    pClassObject->Release();
                }
                VariantClear(&vtProp);
            }
        }

        return WBEM_S_NO_ERROR;
    }

    virtual HRESULT STDMETHODCALLTYPE SetStatus(
        /* [in] */ LONG lFlags,
        /* [in] */ HRESULT hResult,
        /* [in] */ BSTR strParam,
        /* [in] */ IWbemClassObject __RPC_FAR* pObjParam) {
        if (lFlags == WBEM_STATUS_COMPLETE) {
            printf("Call complete. hResult = 0x%X\n", hResult);
            bDone = true;
        }
        else if (lFlags == WBEM_STATUS_PROGRESS) {
            printf("Call in progress.\n");
        }
        return WBEM_S_NO_ERROR;
    }
};


int main() {
    getchar();
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize COM library. Error code = " << std::hex << hr << std::endl;
        return 1;
    }

    hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize security. Error code = " << std::hex << hr << std::endl;
        CoUninitialize();
        return 1;
    }

    IWbemLocator* pLocator = NULL;
    hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLocator);
    if (FAILED(hr)) {
        std::cerr << "Failed to create IWbemLocator object. Error code = " << std::hex << hr << std::endl;
        CoUninitialize();
        return 1;
    }

    IWbemServices* pServices = NULL;
    hr = pLocator->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pServices);
    if (FAILED(hr)) {
        std::cerr << "Could not connect to WMI namespace. Error code = " << std::hex << hr << std::endl;
        pLocator->Release();
        CoUninitialize();
        return 1;
    }

    IUnsecuredApartment* pUnsecApp = NULL;
    hr = CoCreateInstance(CLSID_UnsecuredApartment, NULL, CLSCTX_LOCAL_SERVER, IID_IUnsecuredApartment, (void**)&pUnsecApp);
    if (FAILED(hr)) {
        std::cerr << "Failed to create unsecured apartment. Error code = " << std::hex << hr << std::endl;
        pServices->Release();
        pLocator->Release();
        CoUninitialize();
        return 1;
    }

    IWbemObjectSink* pSink = new EventSink(); // EventSink is your custom class implementing IWbemObjectSink
    IUnknown* pStubUnk = NULL;
    pUnsecApp->CreateObjectStub(pSink, &pStubUnk);

    IWbemObjectSink* pStubSink = NULL;
    pStubUnk->QueryInterface(IID_IWbemObjectSink, (void**)&pStubSink);

    hr = pServices->ExecNotificationQueryAsync(_bstr_t(L"WQL"), _bstr_t(L"SELECT * FROM __InstanceCreationEvent WHERE TargetInstance ISA 'Win32_Process'"), WBEM_FLAG_SEND_STATUS, NULL, pStubSink);
    if (FAILED(hr)) {
        std::cerr << "Failed to setup async notification query. Error code = " << std::hex << hr << std::endl;
        pStubUnk->Release();
        pSink->Release();
        pUnsecApp->Release();
        pServices->Release();
        pLocator->Release();
        CoUninitialize();
        return 1;
    }

    std::cout << "Successfully setup WMI event notification for process creation." << std::endl;

    // Prevent the application from exiting immediately
    std::cin.get();

    pStubUnk->Release();
    pSink->Release();
    pUnsecApp->Release();
    pServices->Release();
    pLocator->Release();
    CoUninitialize();
    return 0;
}
