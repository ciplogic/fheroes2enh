unit LauncherUnit;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, StdCtrls,
  ComCtrls, ExtCtrls;

type

  { TForm1 }

  TForm1 = class(TForm)
    Button1: TButton;
    Button2: TButton;
    cbxFullScreen: TCheckBox;
    cbxMusic: TCheckBox;
    cbxRes: TComboBox;
    cbxSound: TCheckBox;
    editFh2Exe: TEdit;
    Image1: TImage;
    Label1: TLabel;
    OpenDialog1: TOpenDialog;
    PageControl1: TPageControl;
    TabSheet1: TTabSheet;
    TabSheet3: TTabSheet;
    tbMusic: TTrackBar;
    tbSound: TTrackBar;
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure cbxMusicChange(Sender: TObject);
    procedure cbxSoundChange(Sender: TObject);
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
    sl.Add('music = on');
    sl.Add('music volume = '+IntToStr(tbSound.Position));
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

procedure TForm1.Button2Click(Sender: TObject);
begin
  if not OpenDialog1.Execute then exit;
  editFh2Exe.Caption:= OpenDialog1.FileName;
end;

procedure TForm1.cbxMusicChange(Sender: TObject);
begin
  tbMusic.Enabled:= cbxMusic.Checked;
end;

procedure TForm1.cbxSoundChange(Sender: TObject);
begin
  tbSound.Enabled:= cbxSound.Enabled;
end;

end.

