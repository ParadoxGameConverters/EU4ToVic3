echo on
rem Copy converter data files
copy "Data_Files\log.txt" "..\Debug\EU4ToVic3\"
copy "Data_Files\configuration-example.txt" "..\Debug\EU4ToVic3\"
copy "Resources\msvcp140_codecvt_ids.dll" "..\Debug\EU4ToVic3\"
copy "Resources\vcruntime140_1.dll" "..\Debug\EU4ToVic3\"
copy "Resources\rakaly.dll" "..\Debug\EU4ToVic3\"
copy "Resources\VC_redist.x64.exe" "..\Debug\"
copy "Data_Files\GeneralFAQ-READ.ME.FIRST.txt" "..\Debug\"

mkdir "..\Debug\Configuration"
copy "Data_Files\fronter-configuration.txt" "..\Debug\Configuration\"
copy "Data_Files\fronter-options.txt" "..\Debug\Configuration\"
copy "Data_Files\*.yml" "..\Debug\Configuration\"

mkdir "..\Debug\Docs"
copy "Data_Files\ReadMe.txt" "..\Debug\Docs\"
copy "Data_Files\FAQ.txt" "..\Debug\Docs\"
copy "Data_Files\license.txt" "..\Debug\Docs\"
copy "Data_Files\after_converting.txt" "..\Debug\Docs\"

rem Create Configurables
del "..\Debug\EU4ToVic3\configurables" /Q
rmdir "..\Debug\EU4ToVic3\configurables" /S /Q
xcopy "Data_Files\configurables" "..\Debug\EU4ToVic3\configurables" /Y /E /I

rem Create Blank Mod
del "..\Debug\EU4ToVic3\blankMod" /Q
rmdir "..\Debug\EU4ToVic3\blankMod" /S /Q
xcopy "Data_Files\blankMod" "..\Debug\EU4ToVic3\blankMod" /Y /E /I
