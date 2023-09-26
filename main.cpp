#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#include <windows.h>
#include "skin.h"
#include "panel.h"
#include "plugin.h"
#include "main.h"

#define GSPEED 200 //500

bool IsGameStarted;
UINT plTimer = 0;
int mode;
fg field;

enum {  //id of menu items
	    mnuSep, mnuCont, mnuNewGame, mnuOptions, mnuExit, mnuAccept,
			mnuCellSize, mnuDefaults, mnuFieldW, mnuFieldH
		 };

#define SZ(A) (sizeof(A)/sizeof(A[0]))

menu_item pmenu[] = {
	{
		mnuCont,
	  TEXT("����������"),
	  MT_BUTTON,
	  TEXT("���������� ������� ����"),
	  (WPARAM) &IsGameStarted
	},
	{
		mnuNewGame,
		TEXT("����� ����"),
		MT_BUTTON,
	  TEXT("������ ����� ����"),
		MENU_SHOW
	},
	{
		mnuSep,
		(TCHAR*) NULL,
		MT_SEPARATOR,
	  (TCHAR*) NULL,
		0
	},
	{
		mnuOptions,
		TEXT("���������"),
		MT_BUTTON,
	  TEXT("�������� ��������� ���� (������� ���� ����� ��������)"),
		MENU_SHOW
	},
	{
		mnuExit,
		TEXT("�����"),
		MT_BUTTON,
	  TEXT("�������� ����"),
		MENU_SHOW
	}
};

menu_item poptions[] = {
	{
		mnuCellSize,
		TEXT("������ �����"),
		MT_OPTION,
	  TEXT("������ ������ �� ����"),
		(WPARAM) CELL_SIZE
	},
	{
		mnuFieldW,
		TEXT("������ ����"),
		MT_OPTION,
	  TEXT("���������� ����� �� ���� �� ������"),
		(WPARAM) FIELD_WIDTH
	},
	{
		mnuFieldH,
		TEXT("������ ����"),
		MT_OPTION,
	  TEXT("���������� ����� �� ���� �� ������"),
		(WPARAM) FIELD_HEIGHT
	},
  {
		mnuSep,
		(TCHAR*) NULL,
		MT_SEPARATOR,
		(TCHAR*) NULL,
		0
	},
	{
		mnuDefaults,
		TEXT("�����"),
		MT_BUTTON,
	  TEXT("������� ��������� �� ���������"),
		MENU_SHOW
	},
	{
		mnuAccept,
		TEXT("�������"),
		MT_BUTTON,
	  TEXT("������� ��������� � ��������� � ������� ����"),
		MENU_SHOW
	}
};

VOID CALLBACK timer_proc (HWND, UINT, UINT, DWORD)
{
	plAction (action_timer);
  skinUpdateScreen ();
}

void key_proc (int key)
{

	if (mode == MODE_MENU)
		switch (key)
  	{
    	case VK_UP:     menuSelectNext(true);       break;
    	case VK_DOWN:   menuSelectNext(false);      break;
    	case VK_RIGHT:
				if (menuGetSelectedItem()->type == MT_OPTION) {
					optIncrease ((int) menuGetSelectedItem()->wParam);
					skinUpdateScreen(); }
			break;
			case VK_LEFT:
				if (menuGetSelectedItem()->type == MT_OPTION)	{
					optDecrease ((int) menuGetSelectedItem()->wParam);
					skinUpdateScreen(); }
			break;
    	case VK_ESCAPE:
				if (IsGameStarted == true) conSetMode (MODE_GAME);
			 break;
    	case 13:
				switch (menuGetSelectedId())
				{
					case mnuCont:
						conSetMode (MODE_GAME);
						break;
					case mnuNewGame:
						plRestart ();
						conSetMode (MODE_GAME);
						IsGameStarted = true;
						break;
					case mnuOptions:
							menuSet(poptions, SZ(poptions));
						break;
					case mnuExit:
						PostQuitMessage (0);
						break;
					case mnuAccept:
            IsGameStarted = false;
            plInit (optGetValue(FIELD_WIDTH), optGetValue(FIELD_HEIGHT), 1);
            field.resize(optGetValue(FIELD_WIDTH), optGetValue(FIELD_HEIGHT), 0);
            skinRebuild();
            menuSet(pmenu, SZ(pmenu));
						break;
					case mnuDefaults:
            optDefaults();
						break;
						
				}
			break;
    }
    
  else if (mode == MODE_GAME)
    switch (key)
  	{
			case VK_ESCAPE: conSetMode (MODE_MENU); break;
    
			case VK_LEFT:  plAction (action_left);     break;
    	case VK_RIGHT: plAction (action_right);    break;
    	case VK_UP:    plAction (action_up);       break;
    	case VK_DOWN:  plAction (action_down);     break;
    	case VK_SPACE: plAction (action_special1); break;
  	}

  skinUpdateScreen ();
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE , LPSTR , int )
{
	optInit();
	menuInit();
	panelInit(3);
	skinInit(hInstance);
	
	field.resize(optGetValue(FIELD_WIDTH), optGetValue(FIELD_HEIGHT), 0);
	plInit(optGetValue(FIELD_WIDTH), optGetValue(FIELD_HEIGHT), 1);

	IsGameStarted = false;
	
	menuSet(pmenu, SZ(pmenu));
	mode = MODE_MENU;
 	skinUpdateScreen ();

 	MSG messages;
	while (GetMessage (&messages, NULL, 0, 0))
	{
		if (messages.message == WM_KEYDOWN) key_proc (messages.wParam);
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}
	return messages.wParam;
}

int conGetMode ()
{
	return mode;
}

fg* conGetField ()
{
	return &field;
}

void conSetMode (int new_mode)
{
	if (new_mode == mode) return;
	switch (new_mode)
	{
		case MODE_GAME:
			plTimer = SetTimer (NULL, 0, GSPEED, timer_proc);
      mode = MODE_GAME;
		break;
		case MODE_MENU:
      if (plTimer) KillTimer (NULL, plTimer);
			menuSet(pmenu, SZ(pmenu));
			mode = MODE_MENU;
		break;
  }
  skinUpdateScreen ();
}

void conCreateCells (int count, COLORREF *colors)
{
	skinCreateCells (count, colors);
}
