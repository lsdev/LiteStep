LSAPI_STYLESHEET = lsapi.xslt
INDEX_STYLESHEET = index.xslt

FUNCTION_FILES = \
	AddBangCommand.html \
	AddBangCommandEx.html \
	BangCommandProc.html \
	BangCommandProcEx.html \
	BitmapFromIcon.html \
	BitmapToRegion.html \
	CommandParse.html \
	CommandTokenize.html \
	EnumBangsProc.html \
	EnumBangsV2Proc.html \
	EnumLSData.html \
	EnumModulesProc.html \
	EnumRevIDsProc.html \
	Frame3D.html \
	GetLitestepWnd.html \
	GetLSBitmapSize.html \
	GetRCBool.html \
	GetRCBoolDef.html \
	GetRCColor.html \
	GetRCCoordinate.html \
	GetRCInt.html \
	GetRCLine.html \
	GetRCString.html \
	GetResStr.html \
	GetResStrEx.html \
	GetToken.html \
	is_valid_pattern.html \
	LCClose.html \
	LCOpen.html \
	LCReadNextCommand.html \
	LCReadNextConfig.html \
	LCReadNextLine.html \
	LCTokenize.html \
	LoadLSIcon.html \
	LoadLSImage.html \
	LSExecute.html \
	LSExecuteEx.html \
	LSGetImagePath.html \
	LSGetLitestepPath.html \
	LSGetVariable.html \
	LSGetVariableEx.html \
	LSLog.html \
	LSLogPrintf.html \
	LSSetVariable.html \
	match.html \
	matche.html \
	ParseBangCommand.html \
	ParseCoordinate.html \
	RemoveBangCommand.html \
	SetDesktopArea.html \
	TransparentBltLS.html \
	VarExpansion.html \
	VarExpansionEx.html

MESSAGE_FILES = \
	LM_GETREVID.html \
	LM_REFRESH.html \
	LM_REGISTERMESSAGE.html \
	LM_RELOADMODULE.html \
	LM_RESTOREDATA.html \
	LM_SAVEDATA.html \
	LM_SHELLHOOK.html \
	LM_SYSTRAY.html \
	LM_SYSTRAYREADY.html \
	LM_UNLOADMODULE.html \
	LM_UNREGISTERMESSAGE.html

MODULE_FILES = \
	initModuleEx.html \
	quitModule.html

STRUCTURE_FILES = \
	LSNOTIFYICONDATA.html

LSAPI_FILES = $(FUNCTION_FILES) $(MESSAGE_FILES) $(MODULE_FILES) $(STRUCTURE_FILES)
INDEX_FILES = index.html

.PHONY: all clean

all: $(LSAPI_FILES) $(INDEX_FILES)

clean:
	@echo Deleting generated files
	-@del $(LSAPI_FILES) $(INDEX_FILES)

$(INDEX_FILES): %.html: %.xml $(INDEX_STYLESHEET)
	@echo Processing $<
	@xsltproc --stringparam local_extension html $(INDEX_STYLESHEET) $< > $@

$(LSAPI_FILES): %.html: %.xml $(LSAPI_STYLESHEET)
	@echo Processing $<
	@xsltproc --stringparam local_extension html $(LSAPI_STYLESHEET) $< > $@
