#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

bool checkIfAccountNumber(char* text, int len) {
	int countNumbers = 0;
	for (int i = 0; i < len; i++) {
		//printf("i: %d", i);
		if (text[i] >= '0' && text[i] <= '9')
		{
			//printf(" %c ", text[i]);
			countNumbers++;
		}
	}


	if (countNumbers == 26 )
		return true;
	else
		return false;
}

int main() {
	char text[] = "nie wklejaj";
	/*char test[] = "PL49 1020 2892 2276 3005 0000 0000";
	int len = sizeof(test) / sizeof(test[0]);
	printf("result: %d\n", checkIfAccountNumber(test, len));
	printf("\n######################\n"); */

	bool bAvail = IsClipboardFormatAvailable(CF_TEXT);
	if (bAvail) {
		OpenClipboard(NULL);

		HANDLE hData = GetClipboardData(CF_TEXT);
		if (hData == nullptr)
		{
			return 0;
		}
		char* pszText = static_cast<char*>(GlobalLock(hData));
		if (pszText != nullptr)
		{
			printf("Clipoard contains: %s, len is %d\n", pszText, strlen(pszText));
			if (checkIfAccountNumber(pszText, strlen(pszText) ) ) {
				EmptyClipboard();
				HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, sizeof(text));
				memcpy(GlobalLock(memory), text, sizeof(text));
				GlobalUnlock(memory);
				SetClipboardData(CF_TEXT, memory);
			}

		}
		else {
			printf("<no data>");
		}
		GlobalUnlock(hData);
		CloseClipboard();

		OpenClipboard(NULL);
		hData = GetClipboardData(CF_TEXT);
		pszText = static_cast<char*>(GlobalLock(hData));
		printf("Clipoard contains: %s\n", pszText);
		CloseClipboard();
	}
	return 0;
}
