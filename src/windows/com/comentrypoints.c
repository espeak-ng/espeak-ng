/*
 * Copyright (C) 2016 Reece H. Dunn
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see: <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#define INITGUID

#include <windows.h>

// {61D23633-CE59-4101-8158-569FC6B51B49}
DEFINE_GUID(CLSID_TtsEngine, 0x61d23633, 0xce59, 0x4101, 0x81, 0x58, 0x56, 0x9f, 0xc6, 0xb5, 0x1b, 0x49);

extern HRESULT __stdcall TtsEngine_CreateInstance(IClassFactory *iface, IUnknown *outer, REFIID iid, void **object);

ULONG ObjectCount = 0;
static ULONG LockCount = 0;

static ULONG __stdcall ClassFactory_AddRef(IClassFactory *iface)
{
	return 1;
}

static ULONG __stdcall ClassFactory_Release(IClassFactory *iface)
{
	return 1;
}

static HRESULT __stdcall ClassFactory_QueryInterface(IClassFactory *iface, REFIID iid, void **object)
{
	if (IsEqualGUID(iid, &IID_IUnknown) || IsEqualGUID(iid, &IID_IClassFactory)) {
		*object = iface;
		iface->lpVtbl->AddRef(iface);
		return S_OK;
	}

	*object = NULL;
	return E_NOINTERFACE;
}

static HRESULT __stdcall ClassFactory_LockServer(IClassFactory *iface, BOOL lock)
{
	if (lock)
		InterlockedIncrement(&LockCount);
	else
		InterlockedDecrement(&LockCount);
	return S_OK;
}

static const IClassFactoryVtbl TtsEngine_ClassFactoryVtbl = {
	ClassFactory_QueryInterface,
	ClassFactory_AddRef,
	ClassFactory_Release,
	TtsEngine_CreateInstance,
	ClassFactory_LockServer
};
static IClassFactory TtsEngine_ClassFactory = { &TtsEngine_ClassFactoryVtbl };

HRESULT __stdcall DllGetClassObject(REFCLSID classId, REFIID iid, void **object)
{
	if (IsEqualCLSID(classId, &CLSID_TtsEngine))
		return ClassFactory_QueryInterface(&TtsEngine_ClassFactory, iid, object);

	*object = NULL;
	return CLASS_E_CLASSNOTAVAILABLE;
}

HRESULT __stdcall DllCanUnloadNow(void)
{
	return (ObjectCount == 0 && LockCount == 0) ? S_OK : S_FALSE;
}
