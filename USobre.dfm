object frmSobre: TfrmSobre
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Sobre'
  ClientHeight = 186
  ClientWidth = 513
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  Position = poScreenCenter
  TextHeight = 15
  object btnSair: TSpeedButton
    Left = 184
    Top = 144
    Width = 113
    Height = 22
    Caption = 'Sair'
    OnClick = btnSairClick
  end
  object Memo1: TMemo
    Left = 72
    Top = 8
    Width = 433
    Height = 121
    Enabled = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Segoe UI'
    Font.Style = []
    Lines.Strings = (
      'Institito Federal de S'#227'o Paulo - IFSP Campos do Jord'#227'o'
      'An'#225'lise e Desenvolvimento de Sistemas'
      'Arquitetura de Computadores'
      'Simulador de escalonamento'
      'Autor: Heder Dorneles Soares'
      ''
      '')
    ParentFont = False
    TabOrder = 0
  end
end
