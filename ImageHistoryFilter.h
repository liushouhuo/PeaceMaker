#pragma once
#include "common.h"
#include "StackWalker.h"

//
// Maximum amount of STACK_RETURN_INFO to have in the process execution stack return history.
//
#define MAX_STACK_RETURN_HISTORY 50

#define IMAGE_NAME_TAG 'nImP'
#define PROCESS_HISTORY_TAG 'hPmP'
#define STACK_HISTORY_TAG 'hSmP'

typedef struct ImageLoadHistoryEntry
{
	LIST_ENTRY ListEntry;					// The list entry to iterate multiple images in a process.
	UNICODE_STRING ImageFileName;			// The full image file name of loaded image.
	PSTACK_RETURN_INFO CallerStackHistory;	// A variable-length array of the stack that loaded the image.
	USHORT CallerStackHistorySize;			// The size of the variable-length stack history array.
} IMAGE_LOAD_HISTORY_ENTRY, *PIMAGE_LOAD_HISTORY_ENTRY;

typedef struct ProcessHistoryEntry
{
	LIST_ENTRY ListEntry;						// The list entry to iterate multiple process histories.

	HANDLE CallerId;							// The process id of the caller process.
	PUNICODE_STRING CallerImageFileName;		// OPTIONAL: The image file name of the caller process.

	HANDLE ParentId;							// The process id of the alleged parent process.
	PUNICODE_STRING ParentImageFileName;		// OPTIONAL: The image file name of the alleged parent process.

	HANDLE ProcessId;							// The process id of the executed process.
	PUNICODE_STRING ProcessImageFileName;		// The image file name of the executed process.
	
	ULONG EpochExecutionTime;					// Process execution time in seconds since 1970.
	BOOLEAN ProcessTerminated;					// Whether or not the process has terminated.

	PSTACK_RETURN_INFO CallerStackHistory;		// A variable-length array of the stack that started the process.
	USHORT CallerStackHistorySize;				// The size of the variable-length stack history array.

	PIMAGE_LOAD_HISTORY_ENTRY ImageLoadHistory;	// A linked-list of loaded images and their respective stack histories.
	EX_PUSH_LOCK ImageLoadHistoryLock;			// The lock protecting the linked-list of loaded images.
} PROCESS_HISTORY_ENTRY, *PPROCESS_HISTORY_ENTRY;

class ImageHistoryFilter
{

	static VOID CreateProcessNotifyRoutine (
		HANDLE ParentId,
		HANDLE ProcessId,
		BOOLEAN Create
		);

	static VOID LoadImageNotifyRoutine (
		PUNICODE_STRING FullImageName,
		HANDLE ProcessId,
		PIMAGE_INFO ImageInfo
		);

	static StackWalker walker; // Stack walking utility.
	static PPROCESS_HISTORY_ENTRY ProcessHistoryHead; // Linked-list of process history objects.
	static EX_PUSH_LOCK ProcessHistoryLock;	// Lock protecting the ProcessHistory linked-list.
	static BOOLEAN destroying; // This boolean indicates to functions that a lock should not be held as we are in the process of destruction.

	static BOOLEAN GetProcessImageFileName (
		_In_ HANDLE ProcessId,
		_Out_ PUNICODE_STRING* ImageFileName
		);

	static VOID AddProcessToHistory (
		_In_ HANDLE ProcessId,
		_In_ HANDLE ParentId
		);

	static VOID TerminateProcessInHistory (
		_In_ HANDLE ProcessId
		);

public:
	ImageHistoryFilter (
		_Out_ NTSTATUS* InitializeStatus
		);
	~ImageHistoryFilter();


};