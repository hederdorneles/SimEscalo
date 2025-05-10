object FormSimulador: TFormSimulador
  Left = 0
  Top = 0
  Caption = 'Simulador de Escalonamento'
  ClientHeight = 595
  ClientWidth = 892
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  Menu = MainMenu1
  Position = poScreenCenter
  OnCreate = FormCreate
  TextHeight = 15
  object Label1: TLabel
    Left = 1
    Top = 211
    Width = 73
    Height = 21
    Caption = 'Algoritmo:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Segoe UI'
    Font.Style = []
    ParentFont = False
  end
  object Label2: TLabel
    Left = 277
    Top = 211
    Width = 69
    Height = 21
    Caption = 'Quantum:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Segoe UI'
    Font.Style = []
    ParentFont = False
  end
  object btnSimular: TSpeedButton
    Left = 473
    Top = 200
    Width = 105
    Height = 49
    Cursor = crHandPoint
    Caption = 'Simular'
    StyleElements = [seFont, seClient]
    OnClick = btnSimularClick
  end
  object Label3: TLabel
    Left = 1
    Top = 6
    Width = 147
    Height = 21
    Caption = 'Entrada de Processos'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Segoe UI'
    Font.Style = []
    ParentFont = False
  end
  object Label4: TLabel
    Left = 1
    Top = 251
    Width = 66
    Height = 21
    Caption = 'Execu'#231#227'o:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Segoe UI'
    Font.Style = []
    ParentFont = False
  end
  object Label5: TLabel
    Left = 1
    Top = 424
    Width = 72
    Height = 21
    Caption = 'Resultado:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Segoe UI'
    Font.Style = []
    ParentFont = False
  end
  object Label6: TLabel
    Left = 1
    Top = 28
    Width = 233
    Height = 21
    Caption = 'Tempo de Chegada (Arrival Time):'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Segoe UI'
    Font.Style = []
    ParentFont = False
  end
  object Label7: TLabel
    Left = 425
    Top = 28
    Width = 125
    Height = 21
    Caption = 'Tempo de Servi'#231'o:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Segoe UI'
    Font.Style = []
    ParentFont = False
  end
  object btnLimpar: TSpeedButton
    Left = 815
    Top = 56
    Width = 73
    Height = 145
    Caption = 'Limpar'
    StyleElements = [seFont, seClient]
    OnClick = btnLimparClick
  end
  object btnAdicionar: TSpeedButton
    Left = 696
    Top = 17
    Width = 113
    Height = 33
    Caption = 'Adicionar Processo'
    StyleElements = [seFont, seClient]
    OnClick = btnAdicionarClick
  end
  object GridEntrada: TStringGrid
    Left = 1
    Top = 56
    Width = 808
    Height = 146
    ColCount = 3
    FixedCols = 0
    RowCount = 2
    TabOrder = 0
  end
  object cmbAlgoritmo: TComboBox
    Left = 80
    Top = 213
    Width = 179
    Height = 23
    TabOrder = 1
    OnChange = cmbAlgoritmoChange
    Items.Strings = (
      'FCFS'
      'RR'
      'SPN'
      'SRT')
  end
  object edtQuantum: TEdit
    Left = 352
    Top = 213
    Width = 81
    Height = 23
    TabOrder = 2
    Text = '1'
  end
  object GridGantt: TStringGrid
    Left = 1
    Top = 272
    Width = 857
    Height = 137
    DefaultDrawing = False
    TabOrder = 3
    OnDrawCell = GridGanttDrawCell
  end
  object GridResultados: TStringGrid
    Left = 1
    Top = 445
    Width = 425
    Height = 129
    ColCount = 4
    RowCount = 2
    TabOrder = 4
  end
  object edtChegada: TEdit
    Left = 240
    Top = 28
    Width = 121
    Height = 23
    TabOrder = 5
  end
  object edtServico: TEdit
    Left = 556
    Top = 28
    Width = 121
    Height = 23
    TabOrder = 6
  end
  object MainMenu1: TMainMenu
    Left = 729
    Top = 504
    object Arquivo1: TMenuItem
      Caption = '&Arquivo'
      object Sair1: TMenuItem
        Caption = 'Sai&r'
        ShortCut = 27
        OnClick = Sair1Click
      end
    end
    object Sobre1: TMenuItem
      Caption = '&Sobre'
      ShortCut = 112
      OnClick = Sobre1Click
    end
  end
end
