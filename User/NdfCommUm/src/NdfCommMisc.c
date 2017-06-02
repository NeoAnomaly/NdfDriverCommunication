#include "NdfCommUm.h"
#include "NdfCommNtApi.h"
#include "NdfCommShared.h"

#include <strsafe.h>
#include <intsafe.h>

HRESULT
NdfCommunicationpBuildMsgDeviceName(
    __in LPCWSTR DriverName,
    __inout PUNICODE_STRING DeviceName
);

const ULONG acc = FILE_READ_DATA | FILE_WRITE_DATA | SYNCHRONIZE;
const size_t ea = sizeof(FILE_FULL_EA_INFORMATION);
const size_t off = FIELD_OFFSET(FILE_FULL_EA_INFORMATION, EaName[0]);
const size_t t = sizeof(FILE_FULL_EA_INFORMATION) + sizeof(char) + 24;
const size_t test = FIELD_OFFSET(FILE_FULL_EA_INFORMATION, EaName[0]) + sizeof(char) + 24;

HRESULT
NdfCommunicationpBuildMsgDeviceName(
    __in LPCWSTR DriverName,
    __inout PUNICODE_STRING DeviceName
)
{
    HRESULT result = S_OK;
    SIZE_T symLinkNameNotNullCbSize = 0;

    result = StringCbPrintf(
        DeviceName->Buffer,
        DeviceName->MaximumLength,
        NDFCOMM_MSG_SYMLINK_NAME_FMT,
        DriverName
    );

    if (SUCCEEDED(result))
    {
        result = StringCbLength(
            DeviceName->Buffer,
            DeviceName->MaximumLength,
            &symLinkNameNotNullCbSize
        );

        if (SUCCEEDED(result))
        {
            DeviceName->Length = (USHORT)symLinkNameNotNullCbSize;
            if (DeviceName->Length > (NDFCOMM_MSG_MAX_CCH_NAME_SIZE * sizeof(WCHAR)))
            {
                result = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
            }
        }
    }    

    return result;
}

HRESULT
WINAPI
NdfCommunicationConnect(
    __in LPCWSTR DriverName,
    __in_opt LPCVOID Context,
    __in USHORT ContextSize,
    __out HANDLE* Connection
)
{
    HRESULT result = S_OK;
    NTSTATUS status = 0;
    PFILE_FULL_EA_INFORMATION eaInfo = NULL;
    ULONG eaBufferCbSize = 0;   
    OBJECT_ATTRIBUTES objectAttributes = { 0 };
    IO_STATUS_BLOCK iosb = { 0 };
    DECLARE_UNICODE_STRING_SIZE(deviceName, NDFCOMM_MSG_MAX_CCH_NAME_SIZE);

    if (!Connection)
    {
        return E_INVALIDARG;
    }

    if (Context && !ContextSize)
    {
        return E_INVALIDARG;
    }

    if (!Context && ContextSize)
    {
        return E_INVALIDARG;
    }

    *Connection = INVALID_HANDLE_VALUE;

    result = CommunicationpInitNtApi();
    if (FAILED(result))
    {
        return result;
    }

    result = NdfCommunicationpBuildMsgDeviceName(DriverName, &deviceName);
    if (FAILED(result))
    {
        return result;
    }

    ///
    /// The value field begins after the end of the EaName field of 
    /// the FILE_FULL_EA_INFORMATION structure, including a single null character.
    ///
    /// The null character is not included in the EaNameLength field
    ///
    eaBufferCbSize = 
        FIELD_OFFSET(FILE_FULL_EA_INFORMATION, EaName[0])
        + NDFCOMM_EA_NAME_WITH_NULL_LENGTH
        + ContextSize;

    eaInfo = HeapAlloc(GetProcessHeap(), 0, eaBufferCbSize);
    if (!eaInfo)
    {
        return E_OUTOFMEMORY;
    }

    eaInfo->NextEntryOffset = 0;
    eaInfo->Flags = 0;
    eaInfo->EaNameLength = NDFCOMM_EA_NAME_NOT_NULL_LENGTH;
    eaInfo->EaValueLength = ContextSize;    
    RtlCopyMemory(eaInfo->EaName, NDFCOMM_EA_NAME, NDFCOMM_EA_NAME_WITH_NULL_LENGTH);
    RtlCopyMemory(NdfCommAdd2Ptr(eaInfo->EaName, NDFCOMM_EA_NAME_WITH_NULL_LENGTH), Context, ContextSize);

    InitializeObjectAttributes(
        &objectAttributes,
        &deviceName,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
    );

    status = NtCreateFile(
        Connection,
        FILE_READ_DATA | FILE_WRITE_DATA | SYNCHRONIZE,
        &objectAttributes,
        &iosb,
        NULL,
        0,
        0,
        FILE_OPEN_IF,
        0,
        eaInfo,
        eaBufferCbSize
    );

    HeapFree(GetProcessHeap(), 0, eaInfo);

    if (!NT_SUCCESS(status))
    {
        result = HRESULT_FROM_NT(status);
    }

    return result;
}

HRESULT
WINAPI
NdfCommunicationSendMessage(
    __in HANDLE Connection,
    __in_opt LPVOID InputBuffer,
    __in ULONG InputBufferSize,
    __out_opt LPVOID OutputBuffer,
    __in ULONG OutputBufferSize,
    __out PULONG BytesReturned
)
{
    HRESULT result = S_OK;

    if (!DeviceIoControl(
        Connection,
        NDFCOMM_SENDMESSAGE,
        InputBuffer,
        InputBufferSize,
        OutputBuffer,
        OutputBufferSize,
        BytesReturned,
        NULL
    ))
    {
        result = HRESULT_FROM_WIN32(GetLastError());
    }

    return result;
}