//---------------------------------------------------------------------------

#ifndef USobreH
#define USobreH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
//---------------------------------------------------------------------------
class TfrmSobre : public TForm
{
__published:	// IDE-managed Components
    TMemo *Memo1;
    TSpeedButton *btnSair;
    void __fastcall btnSairClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TfrmSobre(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmSobre *frmSobre;
//---------------------------------------------------------------------------
#endif
