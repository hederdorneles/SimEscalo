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
