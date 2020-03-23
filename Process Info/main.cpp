#include "Process Info.h"
#include <iostream>

int main()
{
	HANDLE hProc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, GetCurrentProcessId());

	ProcessInfo info;
	if (!info.SetProcess(hProc))
	{
		printf("ProcessInfo::SetProcess failed\n");

		return 0;
	}
	
	printf("Process (%08d)\n", info.GetPID());
	printf("\tPEB        : %p\n", info.GetPEB());
	printf("\tPEB entry  : %p\n", info.GetLdrEntry(GetModuleHandle(NULL)));
	printf("\tEntrypoint : %p\n", info.GetEntrypoint());
	printf("\tPlatform   : %s\n", info.IsNative() ? "native" : "wow64");

	printf("\n");

	if (info.FirstThread())
	{
		do
		{
			printf("Thread (%08d)\n", info.GetThreadId());

			void * start_address = nullptr;
			if (!info.GetThreadStartAddress(start_address))
			{
				printf("\tStartaddress : failed to query startaddress\n");
			}
			else
			{
				printf("\tStartaddress : %p\n", start_address);
			}			

			THREAD_STATE state;
			KWAIT_REASON reason;
			if (!info.GetThreadState(state, reason))
			{
				printf("\tThread state : failed to query thread state\n");
				printf("\tWait reason  : failed to query wait reason\n");
			}
			else
			{
				printf("\tThread state : %s\n", state == THREAD_STATE::Running ? "Running" : "Waiting");
				printf("\tWait reason  : %02d\n", (int)reason);
			}

			bool bAlertable = info.IsThreadInAlertableState();
			printf("\tIs alertable : %s\n", bAlertable ? "Yes" : "No");

			bool bWoker = info.IsThreadWorkerThread();
			printf("\tIs worker    : %s\n", bWoker ? "Yes" : "No");

			printf("\n");

		} while (info.NextThread());
	}
	else
	{
		printf("ProcessInfo::FirstThread failed\n");
	}

	return 0;
}