#ifndef MACRO_INCLUDED

#include "plugin.hpp"
#include "strclass.hpp"
#include "regclass.hpp"

#define MakePtr(Type, Base, Offset) ((Type)(DWORD(Base) + (DWORD)(Offset)))

#define GROUPNAMELEN 128
#define CAPTIONLEN 128
#define TITLELEN 64
#define MINVERSION 0x0001004600050712i64 // 1.70.5.1810


//----
HINSTANCE hInstance;
PluginStartupInfo Info;
FarStandardFunctions FSF;
OSVERSIONINFO vi;


//----
char PluginRootKey[128];
char FarKey[128];               // default "\\Software\\Far"
char FarUsersKey[128];          // default "\\Software\\Far\\Users"
char KeyMacros[128];            // default "\\Software\\Far\\KeyMacros"
char FarUserName[MAX_PATH_LEN];
char FarFullName[MAX_PATH_LEN]; // default "C:\\Program Files\\Far\\Far.exe"


//----
int OpenFrom;
static int AltPressed=FALSE,CtrlPressed=FALSE,ShiftPressed=FALSE;
unsigned __int64 FarVersion;


//----
const char *HKCU="HKEY_CURRENT_USER";
const char *KeyMacros_KEY="KeyMacros";
const char *Module_KEY="MacroView";
const char *Plugins_KEY="\\Plugins";
const char *Default_KEY="\\Software\\Far";
const char *Users_KEY="\\Software\\Far\\Users";


//----
char *MacroGroupShort[]=
{
  "Dialog","Disks","Editor","Help","Info","MainMenu","Menu",
  "QView","Search","Shell","Tree","Viewer","Other","Common",
  "FindFolder","UserMenu",
};


//----
// ������� � ���� ������� �������� ����� ������� � �������
// MacroGroupShort, ���� �������� �������� ������ ��� �������
// ������
int GroupIndex[]=
{
  -1,9,-1,-1,-1,2,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,
};


//----
enum GroupNameConvert
{
  GRP_TOLONGNAME,
  GRP_TOSHORTNAME,
};

enum
{
  DM_NONE,
  DM_DELETED,
  DM_DEACTIVATED,
};

enum
{
  KB_COMMON,
  KB_ALT,
  KB_CTRL,
  KB_SHIFT,
  KB_DIALOG,
  KB_SHIFTDIALOG,
};

enum
{
  MAC_MENUACTIVE,
  MAC_EDITACTIVE,
  MAC_DELETEACTIVE,
  MAC_EXPORTACTIVE,
  MAC_COPYACTIVE,
  MAC_ERRORACTIVE,
};

enum EditMode
{
  EM_NONE,
  EM_INSERT,
  EM_EDIT,
};


//----
struct InitDialogItem
{
  int Type;
  int X1,Y1,X2,Y2;
  int Focus;
  union {
    int Selected;
    char *History;
    char *Mask;
    struct FarList *ListItems;
    int ListPos;
    CHAR_INFO *VBuf;
  } Param;
  int Flags;
  int DefaultButton;
  union {
    char *Data;
  } Data;
};


struct Config
{
  int AddDescription;
  int AutomaticSave;
  int ViewShell;
  int ViewViewer;
  int ViewEditor;
  int UseHighlight;
  int StartDependentSort;
  int LongGroupNames;
  int MenuCycle;
  int DblClick;
  int GroupDivider;
  int SaveOnStart;
};


struct MenuData
{
  char Group[16];
  char Key[32];
};


//----
BOOL InterceptDllCall(HMODULE hLocalModule,const char* c_szDllName,const char* c_szApiName,
                      PVOID pApiNew,PVOID* p_pApiOrg);

typedef (WINAPI *TReadConsoleInput)(HANDLE hConsole,PINPUT_RECORD ir,DWORD nNumber,LPDWORD nNumberOfRead);
typedef (WINAPI *TPeekConsoleInput)(HANDLE hConsole,PINPUT_RECORD ir,DWORD nNumber,LPDWORD nNumberOfRead);
TReadConsoleInput p_fnReadConsoleInputOrgA;
TReadConsoleInput p_fnReadConsoleInputOrgW;
TPeekConsoleInput p_fnPeekConsoleInputOrgA;
TPeekConsoleInput p_fnPeekConsoleInputOrgW;

long WINAPI MacroDialogProc(HANDLE hDlg, int Msg,int Param1,long Param2);
long WINAPI MenuDialogProc(HANDLE hDlg, int Msg,int Param1,long Param2);
long WINAPI DefKeyDialogProc(HANDLE hDlg, int Msg,int Param1,long Param2);
long WINAPI CopyDialogProc(HANDLE hDlg, int Msg,int Param1,long Param2);
//BOOL WINAPI ProcessKey(PINPUT_RECORD ir);
BOOL __fastcall ProcessPeekKey(PINPUT_RECORD ir);


//----
char *__fastcall AllTrim(char *S);
char *__fastcall UnQuoteText(char *S);
char *__fastcall QuoteText(char *S,BOOL Force=FALSE);
char *__fastcall GetMsg(int MsgId);
char *__fastcall CheckFirstBackSlash(char *S,BOOL mustbe);
char *__fastcall CheckLen(char *S,unsigned ln,BOOL AddDots=TRUE);
char *__fastcall CheckRLen(char *S,unsigned ln,BOOL AddDots=TRUE);
int   __fastcall CmpStr(const char *String1,const char *String2,int ln1=-1,int ln2=-1);
WORD  __fastcall CharToUpper(WORD ch);
unsigned __int64 __fastcall GetFarVersion();


//----
class TMacroView
{
  friend long WINAPI MacroDialogProc(HANDLE hDlg, int Msg,int Param1,long Param2);
  friend long WINAPI MenuDialogProc(HANDLE hDlg, int Msg,int Param1,long Param2);
  friend long WINAPI DefKeyDialogProc(HANDLE hDlg, int Msg,int Param1,long Param2);
  friend long WINAPI CopyDialogProc(HANDLE hDlg, int Msg,int Param1,long Param2);
  friend BOOL WINAPI myReadConsoleInputA(HANDLE hConsole,PINPUT_RECORD ir,DWORD nNumber,LPDWORD nNumberOfRead);
  friend BOOL WINAPI myReadConsoleInputW(HANDLE hConsole,PINPUT_RECORD ir,DWORD nNumber,LPDWORD nNumberOfRead);
  friend BOOL WINAPI myPeekConsoleInputA(HANDLE hConsole,PINPUT_RECORD ir,DWORD nNumber,LPDWORD nNumberOfRead);
  friend BOOL WINAPI myPeekConsoleInputW(HANDLE hConsole,PINPUT_RECORD ir,DWORD nNumber,LPDWORD nNumberOfRead);
//  friend BOOL WINAPI ProcessKey(PINPUT_RECORD ir);
  friend BOOL __fastcall ProcessPeekKey(PINPUT_RECORD ir);
  friend void __fastcall FlushInputBuffer();

  private:
    const char   *MacroText,
                 *MacroCmdHistory,
                 *MacroKeyHistory,
                 *MacroDescrHistory,
                 *MacroExpHistory,
                 *MacroCopyHistory;

    Config        Conf;
    FarDialogItem EditDialog[32];
    FarDialogItem MenuDialog[2];
    FarDialogItem DefKeyDialog[2];
    FarListItem   GroupItems[16],*ConfItems;
    FarList       GroupList,ConfList;

    BOOL          CtrlDotPressed,
                  WaitForKeyToMacro,
                  AltInsPressed,
                  HelpInvoked,
                  HelpActivated,
                  EditInMove,
				  MultiLine;
    HANDLE        hand,
                  EditDlg,
                  MenuDlg,
                  DefDlg,
                  SaveScr,
                  /*SaveBar,*/
                  hOut,
                  hIn;
    TStrList     *NameList,
                 *MacNameList,
                 *DescrList,
                 *ValueList,
                 *MenuList;

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    WIN32_FIND_DATA fData;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    char          S[MAX_PATH_LEN],
                  Str[MAX_PATH_LEN],
                  TempPath[MAX_PATH_LEN],
                  TempFileName[MAX_PATH_LEN],
                  Group[MAX_KEY_LEN],
                  Key[MAX_KEY_LEN];

    char          OldCaption[CAPTIONLEN],
                  NewCaption[CAPTIONLEN],
                  MenuTitle[TITLELEN],
                  MenuBottom[TITLELEN];

    char         *MacroData,*MacroMulti;

    int           Deactivated;
    int           ActiveMode;
    int           EditMode;
    int           MenuItemsNumber;
    int           MacroGroupsSize;
    int           KeyWidth;
    int           GroupKeyLen;
    int           MaxMenuItemLen;
    int           SelectPos;
    int           TopPos;
    int           GroupPos;
    int           UserConfPos;
    int           LastFocus;
    int           MenuX,MenuY,MenuH,MenuW;
    int           EditX1,EditY1,EditX2,EditY2;

  public:
    TMacroView();
    ~TMacroView();
    BOOL          __fastcall Config();
    void          __fastcall ReadConfig();
    int           MacroList();

  private:
    void          __fastcall InitData();
    void          __fastcall InitMacroAreas();
    void          __fastcall InitDialogs();
//    void          __fastcall ParseMenuItem(FarListGetItem *List);
    void          __fastcall WriteKeyBar(int kbType);
    BOOL          __fastcall CreateDirs(char *Dir);
    char         *ConvertGroupName(char *Group,int nWhere);
    void          InitDialogItems(InitDialogItem *Init,FarDialogItem *Item,int ItemsNumber);
    void          __fastcall InsertMacroToEditor(BOOL AllMacros);
    void          __fastcall ExportMacroToFile(BOOL AllMacros=FALSE);
    void          SwitchOver(char *Group,char *Key);
    int           DeletingMacro(char **Items,int ItemsSize,char *HelpTopic);
    BOOL          __fastcall CopyMoveMacro(int Op);
    void          MoveTildeInKey(TStrList *&NameList,BOOL doit=FALSE);
    void          PrepareDependentSort(TStrList *&NameList,BOOL doit=FALSE);
    void          __fastcall FillMenu(HANDLE hDlg,int RebuildList=TRUE);
    void          WriteRegValues(FarDialogItem *DialogItems);
    BOOL          __fastcall CopyMacro(int vKey);
    void          __fastcall ExportMacro(BOOL AllMacros=FALSE);
    BOOL          __fastcall DeleteMacro();
    void          __fastcall SetFocus(int Focus);
    int           __fastcall ShowEdit();
    BOOL          __fastcall InsertMacro();
    BOOL          __fastcall EditMacro();
};

TReg *Reg=NULL;
TMacroView *Macro=NULL;

#define MACRO_INCLUDED
#endif
