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
    int restante;  // tempo restante de execução
};

void LimparGridGantt() {
    for (int row = 1; row < FormSimulador->GridGantt->RowCount; row++) {
        for (int col = 1; col < FormSimulador->GridGantt->ColCount; col++) {
            FormSimulador->GridGantt->Cells[col][row] = "";
        }
    }
}

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
    //    int numProcessos = processos.size();
    //    FormSimulador->GridGantt->RowCount = numProcessos + 1;  // Linha 0 = tempo
    //    FormSimulador->GridGantt->ColCount = tempoEstimado + 1; // Coluna 0 = PID
    int numProcessos = processos.size();
    FormSimulador->GridGantt->RowCount = numProcessos + 1;
    FormSimulador->GridGantt->ColCount = tempoEstimado + 1;

    // Cabeçalho de tempo
    FormSimulador->GridGantt->Cells[0][0] = "";
    for (int t = 1; t <= tempoEstimado; ++t)
        FormSimulador->GridGantt->Cells[t][0] = IntToStr(t);

    // Limpa o conteúdo anterior
    LimparGridGantt();

    // === Ordenar os PIDs alfabeticamente para o Gantt ===
    std::vector<String> pids;
    for (const auto& p : processos)
        pids.push_back(p.pid);
    std::sort(pids.begin(), pids.end());

    // Mapear PID → linha no grid
    std::map<String, int> pidParaLinha;
    for (int i = 0; i < pids.size(); ++i) {
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

// Executa simulação do SPN
void SimularSPN()
{
    auto todos = LerProcessos();
    std::vector<Processo> fila;
    std::vector<Processo> finalizados;

    // Estimar tempo total da simulação (soma das durações)
    int tempoEstimado = 0;
    for (const auto& p : todos)
        tempoEstimado += p.duracao;

    // === Preparar Grid de Gantt ===
    int numProcessos = todos.size();
    FormSimulador->GridGantt->RowCount = numProcessos + 1;
    FormSimulador->GridGantt->ColCount = tempoEstimado + 1;

    FormSimulador->GridGantt->Cells[0][0] = "";
    for (int t = 1; t <= tempoEstimado; ++t)
        FormSimulador->GridGantt->Cells[t][0] = IntToStr(t);

    // Limpar conteúdo anterior
    LimparGridGantt();

    // Mapear PID → linha (ordem alfabética)
    std::vector<String> pids;
    for (auto& p : todos)
        pids.push_back(p.pid);
    std::sort(pids.begin(), pids.end());

    std::map<String, int> pidParaLinha;
    for (int i = 0; i < pids.size(); ++i) {
        FormSimulador->GridGantt->Cells[0][i + 1] = pids[i];
        pidParaLinha[pids[i]] = i + 1;
    }

    // Ordenar inicialmente por chegada
    std::sort(todos.begin(), todos.end(), [](const Processo &a, const Processo &b) {
        return a.chegada < b.chegada;
    });

    int tempo = 0;

    // Loop até todos os processos serem executados
    while (!todos.empty() || !fila.empty())
    {
        // Move para a fila os processos que já chegaram
        for (auto it = todos.begin(); it != todos.end();) {
            if (it->chegada <= tempo) {
                fila.push_back(*it);
                it = todos.erase(it);
            } else {
                ++it;
            }
        }

        // Se a fila estiver vazia, avance o tempo
        if (fila.empty()) {
            tempo++;
            continue;
        }

        // Escolhe o menor tempo de duração
        auto itMenor = std::min_element(fila.begin(), fila.end(), [](const Processo &a, const Processo &b) {
            return a.duracao < b.duracao;
        });

        Processo exec = *itMenor;
        fila.erase(itMenor);

        exec.fim = tempo + exec.duracao;
        exec.turnaround = exec.fim - exec.chegada;
        exec.turnaround_normalizado = (float)exec.turnaround / exec.duracao;

        int linha = pidParaLinha[exec.pid];
        for (int t = tempo; t < exec.fim; ++t)
            FormSimulador->GridGantt->Cells[t + 1][linha] = "x";

        tempo = exec.fim;
        finalizados.push_back(exec);
    }

    // Preencher Grid de Resultados
    FormSimulador->GridResultados->RowCount = finalizados.size() + 1;
    FormSimulador->GridResultados->Cells[0][0] = "PID";
    FormSimulador->GridResultados->Cells[1][0] = "Fim";
    FormSimulador->GridResultados->Cells[2][0] = "Turnaround";
    FormSimulador->GridResultados->Cells[3][0] = "T. Normalizado";

    for (int i = 0; i < finalizados.size(); ++i)
    {
        FormSimulador->GridResultados->Cells[0][i + 1] = finalizados[i].pid;
        FormSimulador->GridResultados->Cells[1][i + 1] = IntToStr(finalizados[i].fim);
        FormSimulador->GridResultados->Cells[2][i + 1] = IntToStr(finalizados[i].turnaround);
        FormSimulador->GridResultados->Cells[3][i + 1] = FormatFloat("0.00", finalizados[i].turnaround_normalizado);
    }

    // Redesenhar Gantt
    FormSimulador->GridGantt->Invalidate();
}

// Executa simulação do RR
void SimularRR()
{
    auto processos = LerProcessos();
    std::vector<Processo> fila;
    std::vector<Processo> aguardando = processos;
    std::vector<Processo> finalizados;

    int quantum = StrToIntDef(FormSimulador->edtQuantum->Text, 1);
    if (quantum <= 0) {
        ShowMessage("Informe um quantum válido (> 0)");
        return;
    }

    // Inicializa tempo restante
    for (auto& p : aguardando)
        p.restante = p.duracao;

    // Estimar tempo total
    int tempoEstimado = 0;
    for (const auto& p : processos)
        tempoEstimado += p.duracao;

    int numProcessos = processos.size();
    FormSimulador->GridGantt->RowCount = numProcessos + 1;
    FormSimulador->GridGantt->ColCount = tempoEstimado * 2 + 1;

    FormSimulador->GridGantt->Cells[0][0] = "";
    for (int t = 1; t < FormSimulador->GridGantt->ColCount; ++t)
        FormSimulador->GridGantt->Cells[t][0] = IntToStr(t);

    LimparGridGantt();

    // Mapeia PID → linha (ordem alfabética)
    std::vector<String> pids;
    for (const auto& p : processos)
        pids.push_back(p.pid);
    std::sort(pids.begin(), pids.end());

    std::map<String, int> pidParaLinha;
    for (int i = 0; i < pids.size(); ++i) {
        FormSimulador->GridGantt->Cells[0][i + 1] = pids[i];
        pidParaLinha[pids[i]] = i + 1;
    }

    std::sort(aguardando.begin(), aguardando.end(), [](const Processo& a, const Processo& b) {
        return a.chegada < b.chegada;
    });

    int tempo = 0;

    while (!aguardando.empty() || !fila.empty()) {
        // Primeiro: mover todos que chegaram até o tempo atual
        for (auto it = aguardando.begin(); it != aguardando.end(); ) {
            if (it->chegada <= tempo) {
                fila.push_back(*it);
                it = aguardando.erase(it);
            } else {
                ++it;
            }
        }

        // Se a fila ainda estiver vazia, avance o tempo
        if (fila.empty()) {
            tempo++;
            continue;
        }

        // Executa o primeiro da fila
        Processo p = fila.front();
        fila.erase(fila.begin());

        int executar = std::min(quantum, p.restante);
        int linha = pidParaLinha[p.pid];

        for (int t = tempo; t < tempo + executar; ++t)
            FormSimulador->GridGantt->Cells[t + 1][linha] = "x";

        tempo += executar;
        p.restante -= executar;

        // Agora, checa se novos processos chegaram neste tempo
        for (auto it = aguardando.begin(); it != aguardando.end(); ) {
            if (it->chegada <= tempo) {
                fila.push_back(*it);
                it = aguardando.erase(it);
            } else {
                ++it;
            }
        }

        // Processo atual volta ao fim da fila, se não terminou
        if (p.restante > 0) {
            fila.push_back(p);
        } else {
            p.fim = tempo;
            p.turnaround = p.fim - p.chegada;
            p.turnaround_normalizado = (float)p.turnaround / p.duracao;
            finalizados.push_back(p);
        }
    }

    // Preencher Grid de Resultados
    FormSimulador->GridResultados->RowCount = finalizados.size() + 1;
    FormSimulador->GridResultados->Cells[0][0] = "PID";
    FormSimulador->GridResultados->Cells[1][0] = "Fim";
    FormSimulador->GridResultados->Cells[2][0] = "Turnaround";
    FormSimulador->GridResultados->Cells[3][0] = "T. Normalizado";

    for (int i = 0; i < finalizados.size(); ++i) {
        FormSimulador->GridResultados->Cells[0][i + 1] = finalizados[i].pid;
        FormSimulador->GridResultados->Cells[1][i + 1] = IntToStr(finalizados[i].fim);
        FormSimulador->GridResultados->Cells[2][i + 1] = IntToStr(finalizados[i].turnaround);
        FormSimulador->GridResultados->Cells[3][i + 1] = FormatFloat("0.00", finalizados[i].turnaround_normalizado);
    }

    FormSimulador->GridGantt->Invalidate();
}


// Executa SRT
void SimularSRT()
{
    auto todos = LerProcessos();
    std::vector<Processo> prontos;
    std::vector<Processo> finalizados;

    // Inicializa restante
    for (auto& p : todos)
        p.restante = p.duracao;

    // Estimar tempo máximo
    int tempoEstimado = 0;
    for (const auto& p : todos)
        tempoEstimado += p.duracao;

    int numProcessos = todos.size();
    FormSimulador->GridGantt->RowCount = numProcessos + 1;
    FormSimulador->GridGantt->ColCount = tempoEstimado + 1;

    FormSimulador->GridGantt->Cells[0][0] = "";
    for (int t = 1; t < FormSimulador->GridGantt->ColCount; ++t)
        FormSimulador->GridGantt->Cells[t][0] = IntToStr(t);

    LimparGridGantt();

    // Mapeia PID → linha (ordem alfabética)
    std::vector<String> pids;
    for (const auto& p : todos)
        pids.push_back(p.pid);
    std::sort(pids.begin(), pids.end());

    std::map<String, int> pidParaLinha;
    for (int i = 0; i < pids.size(); ++i) {
        FormSimulador->GridGantt->Cells[0][i + 1] = pids[i];
        pidParaLinha[pids[i]] = i + 1;
    }

    int tempo = 0;

    while (!todos.empty() || !prontos.empty()) {
        // Move para prontos os que chegaram
        for (auto it = todos.begin(); it != todos.end(); ) {
            if (it->chegada <= tempo) {
                prontos.push_back(*it);
                it = todos.erase(it);
            } else {
                ++it;
            }
        }

        if (prontos.empty()) {
            tempo++;
            continue;
        }

        // Escolhe o processo com menor restante
        auto itMenor = std::min_element(prontos.begin(), prontos.end(), [](const Processo& a, const Processo& b) {
            return a.restante < b.restante;
        });

        Processo& atual = *itMenor;
        int linha = pidParaLinha[atual.pid];

        // Executa 1 unidade
        FormSimulador->GridGantt->Cells[tempo + 1][linha] = "x";
        atual.restante--;
        tempo++;

        // Se terminou, registra fim e remove
        if (atual.restante == 0) {
            atual.fim = tempo;
            atual.turnaround = atual.fim - atual.chegada;
            atual.turnaround_normalizado = (float)atual.turnaround / atual.duracao;
            finalizados.push_back(atual);
            prontos.erase(itMenor);
        }
    }

    // Preencher resultados
    FormSimulador->GridResultados->RowCount = finalizados.size() + 1;
    FormSimulador->GridResultados->Cells[0][0] = "PID";
    FormSimulador->GridResultados->Cells[1][0] = "Fim";
    FormSimulador->GridResultados->Cells[2][0] = "Turnaround";
    FormSimulador->GridResultados->Cells[3][0] = "T. Normalizado";

    for (int i = 0; i < finalizados.size(); ++i) {
        FormSimulador->GridResultados->Cells[0][i + 1] = finalizados[i].pid;
        FormSimulador->GridResultados->Cells[1][i + 1] = IntToStr(finalizados[i].fim);
        FormSimulador->GridResultados->Cells[2][i + 1] = IntToStr(finalizados[i].turnaround);
        FormSimulador->GridResultados->Cells[3][i + 1] = FormatFloat("0.00", finalizados[i].turnaround_normalizado);
    }

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
    GridEntrada->Cells[2][0] = "Serviço";

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
        ShowMessage("Insira valores válidos para chegada e tempo de serviço.");
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
    } else if (cmbAlgoritmo->ItemIndex == 1) {
        SimularRR();
        GridGantt->Invalidate();
    } else if (cmbAlgoritmo->ItemIndex == 2) {
        SimularSPN();
        GridGantt->Invalidate();
    } else if (cmbAlgoritmo->ItemIndex == 3) {
        SimularSRT();
        GridGantt->Invalidate();
    }
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
