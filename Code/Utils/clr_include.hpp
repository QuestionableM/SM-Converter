#pragma once

#if (_MANAGED == 1) || (_M_CEE == 1)
#define SM_MANAGED_CODE _Pragma("managed")
#define SM_UNMANAGED_CODE _Pragma("unmanaged")
#else
#define SM_MANAGED_CODE
#define SM_UNMANAGED_CODE
#endif