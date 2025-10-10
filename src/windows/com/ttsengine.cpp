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

#include <windows.h>
#include <sapiddk.h>
#include <sperror.h>

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/speak_lib.h>

#include <new>
#include <errno.h>
#include <stdlib.h>
#include <wchar.h>

extern "C" ULONG ObjectCount;

static HRESULT espeak_status_to_hresult(espeak_ng_STATUS status)
{
	switch (status)
	{
	case ENS_OK: return S_OK;
	case EACCES: return E_ACCESSDENIED;
	case EINVAL: return E_INVALIDARG;
	case ENOENT: return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
	case ENOMEM: return E_OUTOFMEMORY;
	default:     return E_FAIL;
	}
}

struct TtsEngine
	: public ISpObjectWithToken
	, public ISpTTSEngine
{
	TtsEngine();
	~TtsEngine();

	// IUnknown

	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	HRESULT __stdcall QueryInterface(REFIID iid, void **object);

	// ISpObjectWithToken

	HRESULT __stdcall GetObjectToken(ISpObjectToken **token);
	HRESULT __stdcall SetObjectToken(ISpObjectToken *token);

	// ISpTTSEngine

	HRESULT __stdcall
	Speak(DWORD flags,
	      REFGUID formatId,
	      const WAVEFORMATEX *format,
	      const SPVTEXTFRAG *textFragList,
	      ISpTTSEngineSite *site);

	HRESULT __stdcall
	GetOutputFormat(const GUID *targetFormatId,
	                const WAVEFORMATEX *targetFormat,
	                GUID *formatId,
	                WAVEFORMATEX **format);

	int OnEvent(short *data, int samples, espeak_EVENT *events);
private:
	HRESULT GetStringValue(LPCWSTR key, char *&value);

	ULONG refCount;
	ISpObjectToken *objectToken;
	ISpTTSEngineSite *site;
};

static int
espeak_callback(short *data, int samples, espeak_EVENT *events)
{
	TtsEngine *engine = (TtsEngine *)events->user_data;
	return engine->OnEvent(data, samples, events);
}

TtsEngine::TtsEngine()
	: refCount(1)
	, objectToken(NULL)
	, site(NULL)
{
	InterlockedIncrement(&ObjectCount);
}

TtsEngine::~TtsEngine()
{
	InterlockedDecrement(&ObjectCount);
	if (objectToken)
		objectToken->Release();
}

ULONG __stdcall TtsEngine::AddRef()
{
	return InterlockedIncrement(&refCount);
}

ULONG __stdcall TtsEngine::Release()
{
	ULONG ret = InterlockedDecrement(&refCount);
	if (ret == 0)
		delete this;
	return ret;
}

HRESULT __stdcall TtsEngine::QueryInterface(REFIID iid, void **object)
{
	*object = NULL;
	if (IsEqualIID(iid, IID_IUnknown) || IsEqualIID(iid, IID_ISpTTSEngine))
		*object = (ISpTTSEngine *)this;
	else if (IsEqualIID(iid, IID_ISpObjectWithToken))
		*object = (ISpObjectWithToken *)this;
	else
		return E_NOINTERFACE;

	this->AddRef();
	return S_OK;
}

HRESULT __stdcall TtsEngine::GetObjectToken(ISpObjectToken **token)
{
	if (!token)
		return E_POINTER;

	*token = objectToken;
	if (objectToken) {
		objectToken->AddRef();
		return S_OK;
	}
	return S_FALSE;
}

HRESULT __stdcall TtsEngine::SetObjectToken(ISpObjectToken *token)
{
	if (!token)
		return E_INVALIDARG;

	if (objectToken)
		return SPERR_ALREADY_INITIALIZED;

	objectToken = token;
	objectToken->AddRef();

	char *path = NULL;
	GetStringValue(L"Path", path);
	espeak_ng_InitializePath(path);
	if (path)
		free(path);

	espeak_ng_STATUS status;
	status = espeak_ng_Initialize(NULL);
	if (status == ENS_OK)
		status = espeak_ng_InitializeOutput(ENOUTPUT_MODE_SYNCHRONOUS, 100, NULL);

	espeak_SetSynthCallback(espeak_callback);

	char *voiceName = NULL;
	if (SUCCEEDED(GetStringValue(L"VoiceName", voiceName))) {
		if (status == ENS_OK)
			status = espeak_ng_SetVoiceByName(voiceName);
		free(voiceName);
	}

	return espeak_status_to_hresult(status);
}

HRESULT __stdcall
TtsEngine::Speak(DWORD flags,
                 REFGUID formatId,
                 const WAVEFORMATEX *format,
                 const SPVTEXTFRAG *textFragList,
                 ISpTTSEngineSite *site)
{
	if (!site || !textFragList)
		return E_INVALIDARG;

	this->site = site;

	while (textFragList != NULL)
	{
		DWORD actions = site->GetActions();
		if (actions & SPVES_ABORT)
			return S_OK;

		switch (textFragList->State.eAction)
		{
		case SPVA_Speak:
			espeak_ng_Synthesize(textFragList->pTextStart, 0, 0, POS_CHARACTER, 0, espeakCHARS_WCHAR, NULL, this);
			break;
		case SPVA_Silence:
			{
				// Insert silence for the specified duration in milliseconds
				int silence_ms = textFragList->State.SilenceMSecs;
				if (silence_ms > 0) {
					// Generate silence using SSML break tag
					wchar_t break_tag[64];
					swprintf(break_tag, 64, L"<break time=\"%dms\"/>", silence_ms);
					espeak_ng_Synthesize(break_tag, 0, 0, POS_CHARACTER, 0, espeakCHARS_WCHAR | espeakSSML, NULL, this);
				}
			}
			break;
		case SPVA_Pronounce:
			// The text contains phonetic pronunciation (IPA or other format)
			// eSpeak uses [[phonemes]] notation for phoneme input
			if (textFragList->pTextStart) {
				// Wrap the phonemes in [[ ]] notation for espeak
				size_t len = wcslen(textFragList->pTextStart);
				wchar_t *phoneme_text = (wchar_t *)malloc((len + 5) * sizeof(wchar_t));
				if (phoneme_text) {
					swprintf(phoneme_text, len + 5, L"[[%s]]", textFragList->pTextStart);
					espeak_ng_Synthesize(phoneme_text, 0, 0, POS_CHARACTER, 0, espeakCHARS_WCHAR, NULL, this);
					free(phoneme_text);
				}
			}
			break;
		case SPVA_Bookmark:
			// Fire a bookmark event
			if (textFragList->pTextStart) {
				// Convert wide char bookmark name to UTF-8
				size_t len = wcslen(textFragList->pTextStart);
				char *bookmark = (char *)malloc(len * 4 + 1); // UTF-8 can be up to 4 bytes per char
				if (bookmark) {
					wcstombs(bookmark, textFragList->pTextStart, len * 4 + 1);
					espeak_ng_SynthesizeMark(NULL, 0, bookmark, 0, espeakCHARS_WCHAR, NULL, this);
					free(bookmark);
					// Notify SAPI that we've completed the bookmark
					site->CompleteSkip(0);
				}
			}
			break;
		case SPVA_SpellOut:
			// Spell out text character by character
			if (textFragList->pTextStart) {
				const wchar_t *text = textFragList->pTextStart;
				while (*text) {
					espeak_ng_SpeakCharacter(*text);
					text++;
				}
			}
			break;
		case SPVA_ParseUnknownTag:
			// Handle unknown XML/SSML tags - typically just skip them
			// The text in pTextStart contains the unknown tag
			// We can either ignore it or try to synthesize the content
			if (textFragList->pTextStart) {
				// Just speak the text content, ignoring the tag
				espeak_ng_Synthesize(textFragList->pTextStart, 0, 0, POS_CHARACTER, 0, espeakCHARS_WCHAR, NULL, this);
			}
			break;
		}

		textFragList = textFragList->pNext;
	}

	return S_OK;
}

HRESULT __stdcall
TtsEngine::GetOutputFormat(const GUID *targetFormatId,
                           const WAVEFORMATEX *targetFormat,
                           GUID *formatId,
                           WAVEFORMATEX **format)
{
	*format = (WAVEFORMATEX *)CoTaskMemAlloc(sizeof(WAVEFORMATEX));
	if (!*format)
		return E_OUTOFMEMORY;
	(*format)->wFormatTag = WAVE_FORMAT_PCM;
	(*format)->nChannels = 1;
	(*format)->nBlockAlign = 2;
	(*format)->nSamplesPerSec = 22050;
	(*format)->wBitsPerSample = 16;
	(*format)->nAvgBytesPerSec = (*format)->nSamplesPerSec * (*format)->nBlockAlign;
	(*format)->cbSize = 0;
	*formatId = SPDFID_WaveFormatEx;
	return S_OK;
}

int
TtsEngine::OnEvent(short *data, int samples, espeak_EVENT *events)
{
	DWORD actions = site->GetActions();
	if (actions & SPVES_ABORT)
		return 1;

	if (data)
		site->Write(data, samples * 2, NULL);
	return 0;
}

HRESULT TtsEngine::GetStringValue(LPCWSTR key, char *&value)
{
	if (!objectToken)
		return E_FAIL;

	LPWSTR wvalue = NULL;
	HRESULT hr = objectToken->GetStringValue(key, &wvalue);
	if (FAILED(hr))
		return hr;

	size_t len = wcslen(wvalue);
	value = (char *)malloc(len + 1);
	if (!value) {
		CoTaskMemFree(wvalue);
		return E_OUTOFMEMORY;
	}

	wcstombs(value, wvalue, len + 1);
	CoTaskMemFree(wvalue);

	return S_OK;
}

extern "C" HRESULT __stdcall TtsEngine_CreateInstance(IClassFactory *iface, IUnknown *outer, REFIID iid, void **object)
{
	if (outer != NULL)
		return CLASS_E_NOAGGREGATION;

	TtsEngine *engine = new (std::nothrow) TtsEngine();
	if (!engine)
		return E_OUTOFMEMORY;

	HRESULT ret = engine->QueryInterface(iid, object);
	engine->Release();
	return ret;
}
