//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <vector>             // std::vector
#include <algorithm>          // std::sort
#include <map>                // std::map
#include <System.SysUtils.hpp> // FormatFloat




#include "UPrincipal.h"
#include "USobre.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

#pragma resource "*.dfm"
TFormSimulador *FormSimulador;


int proximoPID = 0;

struct Processo {
    String pid;
    int chegada;
    int duracao;
    int fim;
    int turnaround;
    float turnaround_normalizado;
};

// Carregamento dos processos no struct
std::vector<Processo> LerProcessos() {
    std::vector<Processo> processos;

    for (int i = 1; i < FormSimulador->GridEntrada->RowCount; i++) {
        Processo p;
        p.pid = FormSimulador->GridEntrada->Cells[0][i];
        p.chegada = StrToInt(FormSimulador->GridEntrada->Cells[1][i]);
        p.duracao = StrToInt(FormSimulador->GridEntrada->Cells[2][i]);
        processos.push_back(p);
    }

    return processos;
}

// Executa simulação do FCFS
void SimularFCFS() {
    auto processos = LerProcessos();

    // Ordena por tempo de chegada
    std::sort(processos.begin(), processos.end(), [](const Processo &a, const Processo &b) {
        return a.chegada < b.chegada;
    });

    // Estimar tempo total da simulação
    int tempoEstimado = 0;
    for (const auto& p : processos)
        tempoEstimado += p.duracao;

    // === Preparar Grid de Gantt ===
    int numProcessos = processos.size();
    FormSimulador->GridGantt->RowCount = numProcessos + 1;  // Linha 0 = tempo
    FormSimulador->GridGantt->ColCount = tempoEstimado + 1; // Coluna 0 = PID

    // Cabeçalho de tempo
    FormSimulador->GridGantt->Cells[0][0] = "";
    for (int t = 1; t <= tempoEstimado; ++t)
        FormSimulador->GridGantt->Cells[t][0] = IntToStr(t);

    // === Ordenar os PIDs alfabeticamente para o Gantt ===
    std::vector<String> pids;
    for (const auto& p : processos)
        pids.push_back(p.pid);
    std::sort(pids.begin(), pids.end());

    // Mapear PID → linha no grid
    std::map<String, int> pidParaLinha;
    for (int i = 0; i < pids.size(); ++i)
    {
        FormSimulador->GridGantt->Cells[0][i + 1] = pids[i];
        pidParaLinha[pids[i]] = i + 1;
    }

    // === Simulação FCFS ===
    int tempo = 0;
    for (auto& p : processos) {
        if (tempo < p.chegada)
            tempo = p.chegada;

        p.fim = tempo + p.duracao;
        p.turnaround = p.fim - p.chegada;
        p.turnaround_normalizado = (float)p.turnaround / p.duracao;

        // Preencher execução no Gantt
        int linha = pidParaLinha[p.pid];
        for (int t = tempo; t < p.fim; ++t)
            FormSimulador->GridGantt->Cells[t + 1][linha] = "x";

        tempo = p.fim;
    }




    // === Preencher resultados ===
    FormSimulador->GridResultados->RowCount = numProcessos + 1;
    FormSimulador->GridResultados->Cells[0][0] = "PID";
    FormSimulador->GridResultados->Cells[1][0] = "Fim";
    FormSimulador->GridResultados->Cells[2][0] = "Turnaround";
    FormSimulador->GridResultados->Cells[3][0] = "T. Normalizado";

    for (int i = 0; i < processos.size(); ++i)
    {
        FormSimulador->GridResultados->Cells[0][i + 1] = processos[i].pid;
        FormSimulador->GridResultados->Cells[1][i + 1] = IntToStr(processos[i].fim);
        FormSimulador->GridResultados->Cells[2][i + 1] = IntToStr(processos[i].turnaround);
        FormSimulador->GridResultados->Cells[3][i + 1] = FormatFloat("0.00", processos[i].turnaround_normalizado);
    }

    // Redesenhar o Gantt com cores
    FormSimulador->GridGantt->Invalidate();
}




//---------------------------------------------------------------------------
__fastcall TFormSimulador::TFormSimulador(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormSimulador::FormCreate(TObject *Sender)
{
// === CONFIGURA GRID DE ENTRADA ===
    GridEntrada->ColCount = 3;
    GridEntrada->RowCount = 1; // Cabeçalho + primeira linha

    GridEntrada->Cells[0][0] = "PID";
    GridEntrada->Cells[1][0] = "Chegada";
    GridEntrada->Cells[2][0] = "Duração";

    GridEntrada->ColWidths[0] = 50;
    GridEntrada->ColWidths[1] = 70;
    GridEntrada->ColWidths[2] = 70;

    // === CONFIGURA GRID DE RESULTADOS ===
    GridResultados->ColCount = 4;
    GridResultados->RowCount = 1; // Cabeçalho inicialmente

    GridResultados->Cells[0][0] = "PID";
    GridResultados->Cells[1][0] = "Fim";
    GridResultados->Cells[2][0] = "Turnaround";
    GridResultados->Cells[3][0] = "T. Normalizado";

    GridResultados->ColWidths[0] = 50;
    GridResultados->ColWidths[1] = 60;
    GridResultados->ColWidths[2] = 80;
    GridResultados->ColWidths[3] = 130;

    // === CONFIGURA GRID DE GANTT ===
    GridGantt->ColCount = 1;
    GridGantt->RowCount = 2; // Linha 0: tempo, Linha 1: processo (inicialmente vazio)

    GridGantt->Cells[0][0] = "t";
    GridGantt->Cells[0][1] = "";

    GridGantt->ColWidths[0] = 30;

    // === CONFIGURA COMBOBOX DE ALGORITMOS ===
    cmbAlgoritmo->Items->Clear();
    cmbAlgoritmo->Items->Add("FCFS");
    cmbAlgoritmo->Items->Add("RR");
    cmbAlgoritmo->Items->Add("SPN");
    cmbAlgoritmo->Items->Add("SRT");
    cmbAlgoritmo->ItemIndex = 0; // Seleciona FCFS por padrão

    // === ESCONDE QUANTUM SE NÃO FOR RR ===
    edtQuantum->Visible = (cmbAlgoritmo->ItemIndex == 1); // Mostra só se RR
    Label2->Visible = edtQuantum->Visible;



    // Entradas de teste
    GridEntrada->RowCount = 4; // 1 cabeçalho + 3 processos

    GridEntrada->Cells[0][1] = "A";
    GridEntrada->Cells[1][1] = "0";
    GridEntrada->Cells[2][1] = "5";

    GridEntrada->Cells[0][2] = "B";
    GridEntrada->Cells[1][2] = "2";
    GridEntrada->Cells[2][2] = "3";

    GridEntrada->Cells[0][3] = "C";
    GridEntrada->Cells[1][3] = "4";
    GridEntrada->Cells[2][3] = "1";
}
//---------------------------------------------------------------------------
void __fastcall TFormSimulador::Sobre1Click(TObject *Sender)
{
    frmSobre->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormSimulador::Sair1Click(TObject *Sender)
{
     Application->Terminate();
}
//---------------------------------------------------------------------------
void __fastcall TFormSimulador::cmbAlgoritmoChange(TObject *Sender)
{
    edtQuantum->Visible = (cmbAlgoritmo->ItemIndex == 1); // Mostra só se RR
}
//---------------------------------------------------------------------------
void __fastcall TFormSimulador::btnAdicionarClick(TObject *Sender) {
    int chegada, duracao;

    // Validação
    if (!TryStrToInt(edtChegada->Text, chegada) || !TryStrToInt(edtServico->Text, duracao)) {
        ShowMessage("Insira valores válidos para chegada e duração.");
        return;
    }

    // Conta processos existentes (exclui a linha de cabeçalho)
    int processosInseridos = GridEntrada->RowCount - 1;

    // Gera o nome do processo (A, B, C, ...)
    char pidChar = 'A' + processosInseridos;
    String pid = String(pidChar);
    proximoPID++;

    // Adiciona nova linha se necessário
    if (GridEntrada->RowCount < processosInseridos + 2) {
        GridEntrada->RowCount = processosInseridos + 2;
    }

    int linha = processosInseridos + 1; // linha após o cabeçalho

    // Preenche os dados na linha correta
    GridEntrada->Cells[0][linha] = pid;
    GridEntrada->Cells[1][linha] = IntToStr(chegada);
    GridEntrada->Cells[2][linha] = IntToStr(duracao);

    // Limpa os campos
    edtChegada->Clear();
    edtServico->Clear();
    edtChegada->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormSimulador::btnLimparClick(TObject *Sender)
{
    // Limpa o GridEntrada
    GridEntrada->RowCount = 1;
    edtChegada->Clear();
    edtServico->Clear();
    edtChegada->SetFocus();
    proximoPID = 0;

    // Limpa o GridGantt
    GridGantt->RowCount = 2;
    GridGantt->ColCount = 1;
    GridGantt->Cells[0][0] = "t";
    GridGantt->Cells[0][1] = "";

    // Limpa o GridResultados
    GridResultados->RowCount = 1;
    GridResultados->Cells[0][0] = "PID";
    GridResultados->Cells[1][0] = "Fim";
    GridResultados->Cells[2][0] = "Turnaround";
    GridResultados->Cells[3][0] = "T. Normalizado";

    // Redesenhar
    GridGantt->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TFormSimulador::btnSimularClick(TObject *Sender)
{
    if (cmbAlgoritmo->ItemIndex == 0) { // FCFS
        SimularFCFS();
        GridGantt->Invalidate();  // força redesenho com o OnDrawCell
    }
    else
        ShowMessage("Algoritmo ainda não implementado.");
}
//---------------------------------------------------------------------------
void __fastcall TFormSimulador::GridGanttDrawCell(TObject *Sender, System::LongInt ACol,
          System::LongInt ARow, TRect &Rect, TGridDrawState State)
{
    TStringGrid *grid = dynamic_cast<TStringGrid*>(Sender);
    AnsiString texto = grid->Cells[ACol][ARow];

    // === CABEÇALHOS: primeira linha (tempo) e primeira coluna (nomes de processos) ===
    if (ARow == 0 || ACol == 0)
    {
        grid->Canvas->Brush->Color = clBtnFace;  // cor de fundo padrão
        grid->Canvas->FillRect(Rect);
        grid->Canvas->TextOut(Rect.Left + 4, Rect.Top + 2, texto);
        return;
    }

    // === CORPO DO GRID: células internas ===
    if (texto == "x")
        grid->Canvas->Brush->Color = clRed;
    else
        grid->Canvas->Brush->Color = clWhite;

    grid->Canvas->FillRect(Rect);
    // Nenhum texto é desenhado nas células de execução
}
//---------------------------------------------------------------------------
