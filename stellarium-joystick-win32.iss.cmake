; Inno Setup script for generating an executable installation package on Windows
; Created with Inno Setup 5.5.4
; @INNO_SETUP_WARNING@

[Setup]
; The plug-in is installed in Stellarium's existing installation directory.
; This means that AppId should match the AppId of the Stellarium installer.
; See the Inno Setup knowledge base page on creating an update/add-on installer.
; The default value of AppId is AppName, so...
AppId=Stellarium
; Default value, but nevertheless set explicitly
UsePreviousAppDir=yes
; Using the same uninstaller as the original Stellarium
CreateUninstallRegKey=no
UpdateUninstallLogAppName=no


AppComments=This plug-in is still an experiment-in-progress. Details may change suddenly and drastically.
AppCopyright=Copyright (C) 2014 Bogdan Marinov (GNU GPL version 2 or later)
AppName=Joystick Plug-in for Stellarium
AppPublisher=Bogdan Marinov
;AppPublisherURL=
AppUpdatesURL=https://github.com/daggerstab/stellarium-joystick/releases
AppVersion=@JOYSTICKSUPPORT_VERSION@
; Version of the installer file?
VersionInfoVersion=@JOYSTICKSUPPORT_VERSION@
; This is later overwritten?
DefaultDirName={pf}\Stellarium

;BackColor=
;BackColor2=
;BackColorDirection=
;InfoBeforeFile=
LicenseFile=LICENSE.txt
OutputBaseFilename=stellarium-joystick-@JOYSTICKSUPPORT_VERSION@-win32-setup
; Keep the output in the same directory as the script that generates it
OutputDir=@CMAKE_BINARY_DIR@


[Tasks]
Name: loadatstartup; Description: "Load the plug-in at Stellarium start-up"; GroupDescription: "For the current user only:"; Check: FileExists(ExpandConstant('{userappdata}\Stellarium\config.ini'))


[Files]
; Installs the plug-in for all users, while still allowing separate settings.
Source: "libJoystickSupport.dll"; DestDir: "{app}\modules\JoystickSupport"; 
Source: "@SDL2_DLL@"; DestDir: "{app}";

Source: "README.txt"; DestDir: "{app}\modules\JoystickSupport"; Flags: isreadme touch
Source: "LICENSE.txt"; DestDir: "{app}\modules\JoystickSupport"; Flags: touch
; TODO: AUTHORS file?

; SDL2_DOCS_DIR will have to be passed manually to CMake...
Source: "@SDL2_DOCS_DIR@\README-SDL.txt"; DestDir: "{app}";
; Should follow the same naming pattern as the plug-in's license file, so...
; TODO: Remember to rename this if you rename the main file to COPYING.
Source: "@SDL2_DOCS_DIR@\COPYING.txt"; DestDir: "{app}"; DestName: "LICENSE-SDL.txt";


[INI]
Filename: "{userappdata}\Stellarium\config.ini"; Section: "plugins_load_at_startup"; Key: "JoystickSupport"; String: "true"; Tasks: loadatstartup;


[Run]
Filename: "{app}\stellarium.exe"; Description: "Start Stellarium"; Flags: postinstall skipifsilent unchecked


[Code]
function NextButtonClick(CurPageID: Integer): Boolean;
begin
	if CurPageID = wpSelectDir then begin
		if FileExists(WizardDirValue + '\stellarium.exe') then
			Result := True
		else begin
			MsgBox('The selected directory is not an existing Stellarium installation. It does not contain a ''stellarium.exe'' file.', mbError, MB_OK);
			Result := False;
		end;
	end else
		Result := True;
end;
