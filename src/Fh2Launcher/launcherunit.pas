unit LauncherUnit;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, StdCtrls,
  ComCtrls;

type

  { TForm1 }

  TForm1 = class(TForm)
    Button1: TButton;
    cbxFullScreen: TCheckBox;
    cbxMusic: TCheckBox;
    cbxSound: TCheckBox;
    cbxRes: TComboBox;
    editFh2Exe: TEdit;
    PageControl1: TPageControl;
    TabSheet1: TTabSheet;
    TabSheet2: TTabSheet;
    TabSheet3: TTabSheet;
    tbMusic: TTrackBar;
    tbSound: TTrackBar;
    procedure Button1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private

  public

  end;

var
  Form1: TForm1;

implementation

{$R *.lfm}

{ TForm1 }

procedure TForm1.FormCreate(Sender: TObject);
var
  resText: String;
begin
   resText:= IntToStr(Screen.Width) + 'x'+IntToStr(Screen.Height);    
   cbxRes.ItemIndex:=0;
   cbxRes.Items.BeginUpdate;
   cbxRes.Items.Insert(0, resText);
   cbxRes.Items.EndUpdate;
end;

procedure TForm1.Button1Click(Sender: TObject);
var
  sl : TStringList;
begin
  sl:= TStringList.Create;


  if(cbxSound.Enabled) then
  begin
    sl.Add('sound = on');
    sl.Add('sound volume = '+IntToStr(tbSound.Position));
  end;

  if(cbxMusic.Enabled) then
  begin
    sl.Add('sound = on');
    sl.Add('sound volume = '+IntToStr(tbSound.Position));
  end;

  if(cbxFullScreen.Checked) then
  begin
    sl.Add('fullscreen = on');
  end
  else
  begin
    sl.Add('fullscreen = off');
  end;
  sl.Add('videomode = ' + cbxRes.Text);


  sl.Add('heroes speed = 10');
  sl.Add('ai speed = 10');
  sl.Add('battle speed = 10');

  sl.SaveToFile('fheroes2.cfg');
  sl.Free;

  SysUtils.ExecuteProcess(editFh2Exe.Text, '');
end;

end.

