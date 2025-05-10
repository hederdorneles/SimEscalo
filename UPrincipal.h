//---------------------------------------------------------------------------

#ifndef UPrincipalH
#define UPrincipalH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "trayicon.h"
#include <Vcl.Grids.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.Menus.hpp>
//---------------------------------------------------------------------------
class TFormSimulador : public TForm
{
__published:	// IDE-managed Components
    TStringGrid *GridEntrada;
    TComboBox *cmbAlgoritmo;
    TLabel *Label1;
    TLabel *Label2;
    TEdit *edtQuantum;
    TSpeedButton *btnSimular;
    TStringGrid *GridGantt;
    TStringGrid *GridResultados;
    TMainMenu *MainMenu1;
    TMenuItem *Arquivo1;
    TMenuItem *Sair1;
    TMenuItem *Sobre1;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label5;
    TLabel *Label6;
    TLabel *Label7;
    TEdit *edtChegada;
    TEdit *edtServico;
    TSpeedButton *btnLimpar;
    TSpeedButton *btnAdicionar;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall Sobre1Click(TObject *Sender);
    void __fastcall Sair1Click(TObject *Sender);
    void __fastcall cmbAlgoritmoChange(TObject *Sender);
    void __fastcall btnAdicionarClick(TObject *Sender);
    void __fastcall btnLimparClick(TObject *Sender);
    void __fastcall btnSimularClick(TObject *Sender);
    void __fastcall GridGanttDrawCell(TObject *Sender, System::LongInt ACol, System::LongInt ARow,
          TRect &Rect, TGridDrawState State);
private:	// User declarations
public:		// User declarations
    __fastcall TFormSimulador(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormSimulador *FormSimulador;
//---------------------------------------------------------------------------
#endif
