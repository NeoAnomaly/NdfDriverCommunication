#pragma once

#include "NdfCommConcurentList.h"
#include "NdfCommKm.h"

#include <ntddk.h>

struct _NDFCOMM_GLOBALS
{
	EX_RUNDOWN_REF LibraryRundownRef;

    PDEVICE_OBJECT MessageDeviceObject;

	///
	/// Symbolic link name for our MessageDeviceObject.
	/// Used at library clean up
	///
    PUNICODE_STRING SymbolicLinkName;

	///
	/// The following section describes original IRP dispatching
	/// functions
	///
    PDRIVER_DISPATCH OriginalCreateDispatch;
    PDRIVER_DISPATCH OriginalCleanupDispatch;
    PDRIVER_DISPATCH OriginalCloseDispatch;
    PDRIVER_DISPATCH OriginalControlDispatch;

	///
	/// The following section describes driver handlers for events
	/// generated by user app
	///
	PNDFCOMM_CONNECT_NOTIFY ConnectNotifyCallback;
	PNDFCOMM_DISCONNECT_NOTIFY DisconnectNotifyCallback;
	PNDFCOMM_MESSAGE_NOTIFY MessageNotifyCallback;

	///
	///
	///
	ULONG MaxClientsCount;
	LONG ClientsCount;
	NDFCOMM_CONCURENT_LIST ClientList;

} NdfCommGlobals;
