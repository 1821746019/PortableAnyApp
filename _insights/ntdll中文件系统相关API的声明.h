
NTSTATUS NtCancelIoFile(
	_In_ HANDLE FileHandle,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock
);

NTSTATUS NtCreateDirectoryObject(
	_Out_ PHANDLE DirectoryHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes
);

NTSTATUS NtCreateFile(
	_Out_ PHANDLE FileHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_opt_ PLARGE_INTEGER AllocationSize,
	_In_ ULONG FileAttributes,
	_In_ ULONG ShareAccess,
	_In_ ULONG CreateDisposition,
	_In_ ULONG CreateOptions,
	_In_opt_ PVOID EaBuffer,
	_In_ ULONG EaLength
);

NTSTATUS NtCreateIoCompletion(
	_Out_ PHANDLE IoCompletionHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_opt_ ULONG Count
);

NTSTATUS NtCreatePagingFile(
	_In_ PUNICODE_STRING PageFileName,
	_In_ PLARGE_INTEGER MinimumSize,
	_In_ PLARGE_INTEGER MaximumSize,
	_In_ ULONG Priority
);

NTSTATUS NtCreateSymbolicLinkObject(
	_Out_ PHANDLE LinkHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_ PUNICODE_STRING LinkTarget
);

NTSTATUS NtDeleteFile(
	_In_ POBJECT_ATTRIBUTES ObjectAttributes
);

NTSTATUS NtFlushBuffersFile(
	_In_ HANDLE FileHandle,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock
);

NTSTATUS NtFlushBuffersFileEx(
	_In_ HANDLE FileHandle,
	_In_ ULONG Flags,
	_In_ PVOID Parameters,
	_In_ ULONG ParametersSize,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock
);

NTSTATUS NtLockFile(
	_In_ HANDLE FileHandle,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ PLARGE_INTEGER ByteOffset,
	_In_ PLARGE_INTEGER Length,
	_In_ ULONG Key,
	_In_ BOOLEAN FailImmediately,
	_In_ BOOLEAN ExclusiveLock
);

NTSTATUS NtNotifyChangeDirectoryFile(
	_In_ HANDLE DirectoryHandle,
	_In_opt_ HANDLE Event,
	_In_opt_ PIO_APC_ROUTINE ApcRoutine,
	_In_opt_ PVOID ApcContext,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_Out_ PVOID Buffer,
	_In_ ULONG Length,
	_In_ ULONG CompletionFilter,
	_In_ BOOLEAN WatchTree
);

NTSTATUS NtOpenDirectoryObject(
	_Out_ PHANDLE DirectoryHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes
);

NTSTATUS NtOpenFile(
	_Out_ PHANDLE FileHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ ULONG ShareAccess,
	_In_ ULONG OpenOptions
);

NTSTATUS NtOpenIoCompletion(
	_Out_ PHANDLE IoCompletionHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes
);

NTSTATUS NtOpenSymbolicLinkObject(
	_Out_ PHANDLE LinkHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes
);

NTSTATUS NtQueryAttributesFile(
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_ PFILE_BASIC_INFORMATION FileInformation
);

NTSTATUS NtQueryDirectoryFile(
	_In_ HANDLE FileHandle,
	_In_opt_ HANDLE Event,
	_In_opt_ PIO_APC_ROUTINE ApcRoutine,
	_In_opt_ PVOID ApcContext,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_Out_ PVOID FileInformation,
	_In_ ULONG Length,
	_In_ FILE_INFORMATION_CLASS FileInformationClass,
	_In_ BOOLEAN ReturnSingleEntry,
	_In_opt_ PUNICODE_STRING FileName,
	_In_ BOOLEAN RestartScan
);

NTSTATUS NtQueryDirectoryObject(
	_In_ HANDLE DirectoryHandle,
	_Out_ PVOID Buffer,
	_In_ ULONG Length,
	_In_ BOOLEAN ReturnSingleEntry,
	_In_ BOOLEAN RestartScan,
	_Inout_ PULONG Context,
	_Out_opt_ PULONG ReturnLength
);

NTSTATUS NtQueryEaFile(
	_In_ HANDLE FileHandle,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_Out_ PVOID Buffer,
	_In_ ULONG Length,
	_In_ BOOLEAN ReturnSingleEntry,
	_In_opt_ PVOID EaList,
	_In_ ULONG EaListLength,
	_In_opt_ PULONG EaIndex,
	_In_ BOOLEAN RestartScan
);

NTSTATUS NtQueryFullAttributesFile(
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_ PFILE_NETWORK_OPEN_INFORMATION FileInformation
);

NTSTATUS NtQueryInformationFile(
	_In_ HANDLE FileHandle,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_Out_ PVOID FileInformation,
	_In_ ULONG Length,
	_In_ FILE_INFORMATION_CLASS FileInformationClass
);

NTSTATUS NtQueryIoCompletion(
	_In_ HANDLE IoCompletionHandle,
	_In_ IO_COMPLETION_INFORMATION_CLASS IoCompletionInformationClass,
	_Out_ PVOID IoCompletionInformation,
	_In_ ULONG IoCompletionInformationLength,
	_Out_opt_ PULONG ReturnLength
);

NTSTATUS NtQueryOleDirectoryFile(
	_In_ HANDLE DirectoryHandle,
	_Out_ PVOID Buffer,
	_In_ ULONG Length,
	_In_ BOOLEAN ReturnSingleEntry,
	_In_ BOOLEAN RestartScan,
	_Inout_ PULONG Context,
	_Out_opt_ PULONG ReturnLength
);

NTSTATUS NtQueryQuotaInformationFile(
	_In_ HANDLE FileHandle,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_Out_ PVOID Buffer,
	_In_ ULONG Length,
	_In_ BOOLEAN ReturnSingleEntry,
	_In_opt_ PVOID SidList,
	_In_ ULONG SidListLength,
	_In_opt_ PSID StartSid,
	_In_ BOOLEAN RestartScan
);

NTSTATUS NtQuerySymbolicLinkObject(
	_In_ HANDLE LinkHandle,
	_Inout_ PUNICODE_STRING LinkTarget,
	_Out_opt_ PULONG ReturnedLength
);

NTSTATUS NtQueryVolumeInformationFile(
	_In_ HANDLE FileHandle,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_Out_ PVOID FsInformation,
	_In_ ULONG Length,
	_In_ FS_INFORMATION_CLASS FsInformationClass
);

NTSTATUS NtReadFile(
	_In_ HANDLE FileHandle,
	_In_opt_ HANDLE Event,
	_In_opt_ PIO_APC_ROUTINE ApcRoutine,
	_In_opt_ PVOID ApcContext,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_Out_ PVOID Buffer,
	_In_ ULONG Length,
	_In_opt_ PLARGE_INTEGER ByteOffset,
	_In_opt_ PULONG Key
);

NTSTATUS NtReadFileScatter(
	_In_ HANDLE FileHandle,
	_In_opt_ HANDLE Event,
	_In_opt_ PIO_APC_ROUTINE ApcRoutine,
	_In_opt_ PVOID ApcContext,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ FILE_SEGMENT_ELEMENT SegmentArray[],
	_In_ ULONG Length,
	_In_opt_ PLARGE_INTEGER ByteOffset,
	_In_opt_ PULONG Key
);

NTSTATUS NtRemoveIoCompletion(
	_In_ HANDLE IoCompletionHandle,
	_Out_ PVOID* KeyContext,
	_Out_ PVOID* ApcContext,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_opt_ PLARGE_INTEGER Timeout
);

NTSTATUS NtSetEaFile(
	_In_ HANDLE FileHandle,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ PVOID Buffer,
	_In_ ULONG Length
);

NTSTATUS NtSetInformationFile(
	_In_ HANDLE FileHandle,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ PVOID FileInformation,
	_In_ ULONG Length,
	_In_ FILE_INFORMATION_CLASS FileInformationClass
);

NTSTATUS NtSetIoCompletion(
	_In_ HANDLE IoCompletionHandle,
	_In_ PVOID KeyContext,
	_In_ PVOID ApcContext,
	_In_ NTSTATUS IoStatus,
	_In_ ULONG_PTR IoStatusInformation
);

NTSTATUS NtSetQuotaInformationFile(
	_In_ HANDLE FileHandle,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ PVOID Buffer,
	_In_ ULONG Length
);

NTSTATUS NtSetVolumeInformationFile(
	_In_ HANDLE FileHandle,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ PVOID FsInformation,
	_In_ ULONG Length,
	_In_ FS_INFORMATION_CLASS FsInformationClass
);

NTSTATUS NtTranslateFilePath(
	_In_ PFILE_PATH InputFilePath,
	_In_ ULONG OutputType,
	_Out_ PFILE_PATH OutputFilePath
);

NTSTATUS NtUnlockFile(
	_In_ HANDLE FileHandle,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ PLARGE_INTEGER ByteOffset,
	_In_ PLARGE_INTEGER Length,
	_In_ ULONG Key
);

NTSTATUS NtWriteFile(
	_In_ HANDLE FileHandle,
	_In_opt_ HANDLE Event,
	_In_opt_ PIO_APC_ROUTINE ApcRoutine,
	_In_opt_ PVOID ApcContext,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ PVOID Buffer,
	_In_ ULONG Length,
	_In_opt_ PLARGE_INTEGER ByteOffset,
	_In_opt_ PULONG Key
);

NTSTATUS NtWriteFileGather(
	_In_ HANDLE FileHandle,
	_In_opt_ HANDLE Event,
	_In_opt_ PIO_APC_ROUTINE ApcRoutine,
	_In_opt_ PVOID ApcContext,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ FILE_SEGMENT_ELEMENT SegmentArray[],
	_In_ ULONG Length,
	_In_opt_ PLARGE_INTEGER ByteOffset,
	_In_opt_ PULONG Key
);

