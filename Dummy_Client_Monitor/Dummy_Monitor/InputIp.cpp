#include "stdafx.h"

void UpdateIpAddress()
{
	FILE *fp = fopen("ipAddress.txt", "wt");
	if ( fp == NULL) {
		puts("���Ͽ��� ����!");
		strcpy(c_ip, "127.0.0.1");
	    return;
	}

	WideCharToMultiByte(CP_ACP, 0, tc_ip, -1, c_ip, sizeof(c_ip), NULL, FALSE);

	fputs(c_ip, fp);
	fclose(fp);
}

BOOL CALLBACK InputServerIpDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	char text_file_ip[20];
	TCHAR ipTmp[20];

	// �ؽ�Ʈ���Ͽ��� ������ �ּ� �о����
	FILE *ff = fopen("ipAddress.txt", "rt");
	if ( ff == NULL) {
		puts("���Ͽ��� ����!");
		strcpy(text_file_ip,"127.0.0.1");
	} else {
		fgets(text_file_ip, 20, ff);
		fclose(ff);
	}

	MultiByteToWideChar(CP_ACP, 0, text_file_ip, strlen(text_file_ip)+1, ipTmp, strlen(text_file_ip)+1);

	switch (iMessage) {
	case WM_INITDIALOG:
		hIp=GetDlgItem(hDlg, IDC_IP);		
		SetWindowText(hIp, ipTmp);
		break;

	case WM_COMMAND:
		switch (wParam) {
		case IDOK:
			GetWindowText(hIp, tc_ip, 20);	
			UpdateIpAddress();
			EndDialog(hDlg,IDOK);
			break;

		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			break;
		}
		break;
	}

	return (DefWindowProc(hDlg, iMessage, wParam, lParam));
}