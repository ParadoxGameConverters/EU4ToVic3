echo on
rem Copy converter data files
copy "Data_Files\log.txt" "..\Release\EU4ToVic3\"
copy "Data_Files\configuration-example.txt" "..\Release\EU4ToVic3\"
copy "Resources\msvcp140_codecvt_ids.dll" "..\Release\EU4ToVic3\"
copy "Resources\vcruntime140_1.dll" "..\Release\EU4ToVic3\"
copy "Resources\rakaly.dll" "..\Release\EU4ToVic3\"
copy "Resources\VC_redist.x64.exe" "..\Release\EU4ToVic3\"
copy "Data_Files\GeneralFAQ-READ.ME.FIRST.txt" "..\Release\"

mkdir "..\Release\Configuration"
copy "Data_Files\fronter-configuration.txt" "..\Release\Configuration\"
copy "Data_Files\fronter-options.txt" "..\Release\Configuration\"
copy "Data_Files\*.yml" "..\Release\Configuration\"

mkdir "..\Release\Docs"
copy "Data_Files\ReadMe.txt" "..\Release\Docs\"
copy "Data_Files\FAQ.txt" "..\Release\Docs\"
copy "Data_Files\license.txt" "..\Release\Docs\"
copy "Data_Files\after_converting.txt" "..\Release\Docs\"

rem Create Configurables
del "..\Release\EU4ToVic3\configurables" /Q
rmdir "..\Release\EU4ToVic3\configurables" /S /Q
xcopy "Data_Files\configurables" "..\Release\EU4ToVic3\configurables" /Y /E /I

rem Create Blank Mod
del "..\Release\EU4ToVic3\blankMod" /Q
rmdir "..\Release\EU4ToVic3\blankMod" /S /Q
xcopy "Data_Files\blankMod" "..\Release\EU4ToVic3\blankMod" /Y /E /I
