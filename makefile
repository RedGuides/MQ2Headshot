!include "../global.mak"

ALL : "$(OUTDIR)\MQ2Headshot.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2Headshot.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2Headshot.dll"
	-@erase "$(OUTDIR)\MQ2Headshot.exp"
	-@erase "$(OUTDIR)\MQ2Headshot.lib"
	-@erase "$(OUTDIR)\MQ2Headshot.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2Headshot.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2Headshot.dll" /implib:"$(OUTDIR)\MQ2Headshot.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2Headshot.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2Headshot.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2Headshot.dep")
!INCLUDE "MQ2Headshot.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2Headshot.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2Headshot.cpp

"$(INTDIR)\MQ2Headshot.obj" : $(SOURCE) "$(INTDIR)"

