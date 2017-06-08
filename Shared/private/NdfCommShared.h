#pragma once

///
/// The following two structures the same as structures defined
/// in the NdfCommUm.h
/// ��� ��������� �������� ��� ����, ����� �� �������� ����������
/// ���������� shared ����� ������ ��� ���������� ���� ��������.
/// ������ ����� �� ������ ������� ������������ �� ������������.
/// � ����� NdfCommMisc.c ����������� �������� ��������� ��������
/// ���� ��������.
///

typedef struct _NDFCOMMP_MESSAGE_HEADER
{
	ULONG ReplyLength;
	ULONGLONG MessageId;
} NDFCOMMP_MESSAGE_HEADER, *PNDFCOMMP_MESSAGE_HEADER;

typedef struct _NDFCOMMP_REPLY_HEADER
{
	NTSTATUS Status;
	ULONGLONG MessageId;
} NDFCOMMP_REPLY_HEADER, *PNDFCOMMP_REPLY_HEADER;

///
/// Device type the same as FILE_DEVICE_UNKNOWN
///
#define NDFCOMM_DEVICE_TYPE (0x00000022)
#define NDFCOMM_MSG_DEVICE_NAME_FMT (L"\\Device\\NdfCommMsg_%s")
#define NDFCOMM_MSG_SYMLINK_NAME_FMT (L"\\DosDevices\\NdfCommMsg_%s")

#define NDFCOMM_MSG_MAX_CCH_NAME_SIZE (64)

#define NDFCOMM_EA_NAME ("NDFCOMMSIG")
#define NDFCOMM_EA_NAME_WITH_NULL_LENGTH (sizeof(NDFCOMM_EA_NAME))
#define NDFCOMM_EA_NAME_NOT_NULL_LENGTH (sizeof(NDFCOMM_EA_NAME) - 1)

#define NdfCommAdd2Ptr(P,I) ((PVOID)((PUCHAR)(P) + (I)))

///
/// Control codes
///
#define NDFCOMM_CTL_CODE(X) CTL_CODE(NDFCOMM_DEVICE_TYPE, 0x800 + X, METHOD_NEITHER, FILE_ANY_ACCESS)

#define NDFCOMM_GETMESSAGE NDFCOMM_CTL_CODE(0)
#define NDFCOMM_SENDMESSAGE NDFCOMM_CTL_CODE(1)
#define NDFCOMM_REPLYMESSAGE NDFCOMM_CTL_CODE(2)