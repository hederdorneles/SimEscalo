//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "USobre.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmSobre *frmSobre;
//---------------------------------------------------------------------------
__fastcall TfrmSobre::TfrmSobre(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmSobre::btnSairClick(TObject *Sender)
{
    frmSobre->Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmSobre::lblUrlClick(TObject *Sender)
{
    ShellExecute(0, L"open", L"https://github.com/hederdorneles/SimEscalo", 0, 0, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

