; â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
;  IzoClone VST3 Installer
;  Built with NSIS (Nullsoft Scriptable Install System) - free & open source
;
;  To compile:
;    1. Download NSIS from https://nsis.sourceforge.io (free)
;    2. Place this file in the same folder as your build output
;    3. Right-click this file â†’ "Compile NSIS Script"
;    4. Output: IzoClone_Setup_v1.0.0.exe
; â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

!define PRODUCT_NAME        "IzoClone"
!define PRODUCT_VERSION     "1.0.0"
!define PRODUCT_PUBLISHER   "YourStudio"
!define PRODUCT_WEB_SITE    "https://yourstudio.com"
!define PRODUCT_UNINST_KEY  "Software\Microsoft\Windows\CurrentVersion\Uninstall\`${PRODUCT_NAME}"
!define VST3_SYSTEM_DIR     "`$COMMONFILES\VST3"
!define VST3_PLUGIN_FILE    "IzoClone.vst3"

; Modern UI
!include "MUI2.nsh"
!include "LogicLib.nsh"

; â”€â”€ Installer Settings â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
Name                "`${PRODUCT_NAME} `${PRODUCT_VERSION}"
OutFile             "IzoClone_Setup_v`${PRODUCT_VERSION}.exe"
InstallDir          "`${VST3_SYSTEM_DIR}"
InstallDirRegKey    HKLM "`${PRODUCT_UNINST_KEY}" "InstallLocation"
RequestExecutionLevel admin
SetCompressor       lzma
SetCompressorDictSize 32

; â”€â”€ UI Appearance â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
!define MUI_ABORTWARNING
!define MUI_ICON                    "installer\icon.ico"
!define MUI_UNICON                  "installer\icon.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "installer\banner.bmp"    ; 164x314 BMP
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP      "installer\header.bmp"    ; 150x57 BMP
!define MUI_HEADERIMAGE_RIGHT

; Colours matching IzoClone dark amber theme
!define MUI_BGCOLOR         "0D0D12"
!define MUI_TEXTCOLOR       "D8D8E0"

; â”€â”€ Pages â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE       "LICENSE.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

; â”€â”€ Version Info (shows in Properties â†’ Details) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
VIProductVersion                    "`${PRODUCT_VERSION}.0"
VIAddVersionKey "ProductName"       "`${PRODUCT_NAME}"
VIAddVersionKey "ProductVersion"    "`${PRODUCT_VERSION}"
VIAddVersionKey "CompanyName"       "`${PRODUCT_PUBLISHER}"
VIAddVersionKey "FileVersion"       "`${PRODUCT_VERSION}"
VIAddVersionKey "FileDescription"   "IzoClone VST3 Mastering Plugin Installer"
VIAddVersionKey "LegalCopyright"    "Copyright Â© 2025 `${PRODUCT_PUBLISHER}"

; â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
;  COMPONENTS
; â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
InstType "Full Install"
InstType "VST3 Only"

Section "VST3 Plugin (Required)" SecVST3
    SectionIn 1 2
    SectionIn RO  ; Required â€” can't be unchecked

    SetOutPath "`${VST3_SYSTEM_DIR}"
    SetOverwrite on

    ; The VST3 is actually a folder (bundle) on Windows
    ; Copy the entire .vst3 bundle directory
    File /r "build\IzoClone_artefacts\Release\VST3\`${VST3_PLUGIN_FILE}"

    ; Write uninstall registry entries
    WriteRegStr   HKLM "`${PRODUCT_UNINST_KEY}" "DisplayName"     "`${PRODUCT_NAME} `${PRODUCT_VERSION}"
    WriteRegStr   HKLM "`${PRODUCT_UNINST_KEY}" "UninstallString" "`$INSTDIR\Uninstall_IzoClone.exe"
    WriteRegStr   HKLM "`${PRODUCT_UNINST_KEY}" "DisplayVersion"  "`${PRODUCT_VERSION}"
    WriteRegStr   HKLM "`${PRODUCT_UNINST_KEY}" "Publisher"       "`${PRODUCT_PUBLISHER}"
    WriteRegStr   HKLM "`${PRODUCT_UNINST_KEY}" "URLInfoAbout"    "`${PRODUCT_WEB_SITE}"
    WriteRegStr   HKLM "`${PRODUCT_UNINST_KEY}" "InstallLocation" "`$INSTDIR"
    WriteRegDWORD HKLM "`${PRODUCT_UNINST_KEY}" "NoModify"        1
    WriteRegDWORD HKLM "`${PRODUCT_UNINST_KEY}" "NoRepair"        1

    WriteUninstaller "`$INSTDIR\Uninstall_IzoClone.exe"

    ; Create presets folder in user Documents
    CreateDirectory "`$DOCUMENTS\IzoClone\Presets"

SectionEnd

Section "Documentation & Presets" SecDocs
    SectionIn 1

    SetOutPath "`$DOCUMENTS\IzoClone"
    File "README.md"

    ; Factory presets (if you export them as files)
    SetOutPath "`$DOCUMENTS\IzoClone\Presets"
    ; File /r "factory_presets\*.mfpreset"   ; Uncomment when you have preset files

SectionEnd

Section "Desktop Shortcut to Preset Folder" SecShortcut
    SectionIn 1

    CreateShortcut "`$DESKTOP\IzoClone Presets.lnk" \
                   "`$DOCUMENTS\IzoClone\Presets"

SectionEnd

; â”€â”€ Section Descriptions â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT `${SecVST3}     "Installs IzoClone.vst3 to the system VST3 folder. Required."
    !insertmacro MUI_DESCRIPTION_TEXT `${SecDocs}     "Installs the README and creates a presets folder in your Documents."
    !insertmacro MUI_DESCRIPTION_TEXT `${SecShortcut} "Creates a desktop shortcut to your IzoClone presets folder."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; â”€â”€ Init: Check Windows version â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
Function .onInit
    ; Require Windows 10 or later (VST3 and AVX2 work best on Win10+)
    `${If} `${AtLeastWin10}
        ; All good
    `${Else}
        MessageBox MB_ICONSTOP|MB_OK \
            "IzoClone requires Windows 10 or later.`$\nYour system does not meet this requirement."
        Abort
    `${EndIf}

    ; Check if already installed and warn
    ReadRegStr `$0 HKLM "`${PRODUCT_UNINST_KEY}" "DisplayVersion"
    `${If} `$0 != ""
        MessageBox MB_YESNO|MB_ICONQUESTION \
            "IzoClone `${PRODUCT_VERSION} is already installed.`$\n`$\nDo you want to reinstall?" \
            IDYES +2
        Abort
    `${EndIf}
FunctionEnd

; â”€â”€ Uninstaller â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
Section "Uninstall"

    ; Remove VST3 bundle
    RMDir /r "`${VST3_SYSTEM_DIR}\`${VST3_PLUGIN_FILE}"

    ; Remove uninstaller itself
    Delete "`$INSTDIR\Uninstall_IzoClone.exe"

    ; Remove registry entries
    DeleteRegKey HKLM "`${PRODUCT_UNINST_KEY}"

    ; Remove desktop shortcut
    Delete "`$DESKTOP\IzoClone Presets.lnk"

    ; NOTE: We intentionally do NOT delete `$DOCUMENTS\IzoClone
    ; so the user keeps their saved presets after uninstalling.
    MessageBox MB_ICONINFORMATION|MB_OK \
        "IzoClone has been uninstalled.`$\n`$\nYour presets in Documents\IzoClone have been kept."

SectionEnd

