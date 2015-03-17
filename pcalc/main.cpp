#if !defined _DEBUG
#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif _DEBUG


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "parser.h"

using namespace nsParser;

HWND hComboBox	 = 0;
HWND hHexStatic  = 0;
HWND hDecStatic  = 0;
HWND hOctStatic  = 0;
HWND hBinStatic  = 0;
HWND hCharStatic = 0;


//--------------------------------------------------------------------
inline void chSETDLGICONS(HWND hwnd, int idi) {
   SendMessage(hwnd, WM_SETICON, TRUE,  (LPARAM) 
      LoadIcon((HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 
         MAKEINTRESOURCE(idi)));
   SendMessage(hwnd, WM_SETICON, FALSE, (LPARAM) 
      LoadIcon((HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 
      MAKEINTRESOURCE(idi)));
}

//--------------------------------------------------------------------
void InitDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	chSETDLGICONS(hwnd, IDI_CALC);

	hComboBox   = GetDlgItem(hwnd, IDC_EXPCOMBO);
	hHexStatic  = GetDlgItem(hwnd, IDC_HEXSTATIC);
	hDecStatic  = GetDlgItem(hwnd, IDC_DECSTATIC);
	hOctStatic  = GetDlgItem(hwnd, IDC_OCTSTATIC);
	hBinStatic  = GetDlgItem(hwnd, IDC_BINSTATIC);
	hCharStatic = GetDlgItem(hwnd, IDC_CHARSTATIC);

	SetFocus(hComboBox);
}

//--------------------------------------------------------------------
void ConverToBin(unsigned long int iDec, char *str)
{
	long long int t;
	register char i;
	char temp[35];

	if(iDec == 0)
	{
		#pragma warning(disable:4996)
		_snprintf(str, 255, "Binary\t\t: %s", "0");
		#pragma warning(default:4996)
		return;
	}

	for(i = 0, t = 1; t <= iDec; t <<= 1, ++i)
		if(t & iDec)
			temp[i] = '1';
		else
			temp[i] = '0';

	temp[i] = 0;

	char *d, *c;
	size_t len = strlen(temp);
	d = temp;
	c = &temp[len - 1];
	len >>= 1;

	for(; d < &temp[len]; ++d, --c )
	{
		char tmp;
		tmp = *d;
		*d = *c;
		*c = tmp;
	}

	#pragma warning(disable:4996)
	_snprintf(str, 255, "Binary\t\t: %s", temp);
	#pragma warning(default:4996)
}

//--------------------------------------------------------------------
void CalcExpression()
{
	char sExpression[256];
	char sTemp[256];
	ComboBox_GetText(hComboBox,(LPSTR) &sExpression, 255);
	if(strlen(sExpression) == 0)
		return;

	ComboBox_InsertString(hComboBox, 0, sExpression);

	//Clear all static
	Static_SetText(hHexStatic, NULL);
	Static_SetText(hDecStatic, NULL);
	Static_SetText(hOctStatic, NULL);
	Static_SetText(hBinStatic, NULL);
	Static_SetText(hCharStatic, NULL);

	CParser ob;
	try
	{
		double res = ob.eval_exp(sExpression);
		double a = floor(res);	
		if(fabs(res-a) < 1e-8)
		{
			#pragma warning(disable:4996)
			_snprintf(sTemp, 255, "Hex\t\t: 0x%X", (unsigned long int) res);
			Static_SetText(hHexStatic, sTemp);
			_snprintf(sTemp, 255, "Decimal\t\t: %d", (long long int) res);
			Static_SetText(hDecStatic, sTemp);
			_snprintf(sTemp, 255, "Octal\t\t: 0%o", (unsigned long int) res);
			Static_SetText(hOctStatic, sTemp);
			//Binary
			ConverToBin((unsigned long int) res, sTemp);
			Static_SetText(hBinStatic, sTemp);
			_snprintf(sTemp, 255, "Character\t: \'%c\'", (char) res);
			Static_SetText(hCharStatic, sTemp);
			#pragma warning(default:4996)
		}
		else
		{
			#pragma warning(disable:4996)
			_snprintf(sTemp, 255,"%e", res);
			#pragma warning(default:4996)
			Static_SetText(hHexStatic, sTemp);
		}
	}
	catch(CalcEcx ecx)
	{
		Static_SetText(hHexStatic,(LPSTR) ecx.geterror());
	}
}

//--------------------------------------------------------------------
void CommandProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
		case IDOK:
			CalcExpression();
			break;
		case IDCANCEL:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case IDC_ONTOPCHECK:
			SetWindowPos(hwnd, IsDlgButtonChecked(hwnd, IDC_ONTOPCHECK) 
			? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			break;
		default: ;
	}
}

//--------------------------------------------------------------------

int WINAPI Dlg_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_CLOSE:
			EndDialog(hwnd, 0);
			break;
		case WM_INITDIALOG:
			InitDlgProc(hwnd, uMsg, wParam, lParam);
			break;
		case WM_COMMAND:
			CommandProc(hwnd, uMsg, wParam, lParam);
		default: ;
	}
	return false;
}

//--------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, Dlg_Proc, NULL);
	return 0;
}