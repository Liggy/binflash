#if _MSC_VER >= 1400

#ifdef _M_IX86
   #pragma comment(linker,"/manifestdependency:\"type='win32' "   \
        "name='Microsoft.Windows.Common-Controls' "                \
        "version='6.0.0.0' "                                       \
        "processorArchitecture='x86' "                             \
        "publicKeyToken='6595b64144ccf1df' "                       \
        "language='*'\"")
#endif

#ifdef _M_AMD64
    #pragma comment(linker,"/manifestdependency:\"type='win32' "   \
        "name='Microsoft.Windows.Common-Controls' "                \
        "version='6.0.0.0' "                                       \
        "processorArchitecture='amd64' "                           \
        "publicKeyToken='6595b64144ccf1df' "                       \
        "language='*'\"")
#endif

#ifdef _M_IA64
    #pragma comment(linker,"/manifestdependency:\"type='win32' "   \
        "name='Microsoft.Windows.Common-Controls' "                \
        "version='6.0.0.0' "                                       \
        "processorArchitecture='ia64' "                            \
        "publicKeyToken='6595b64144ccf1df' "                       \
        "language='*'\"")
#endif

#endif