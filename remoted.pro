#-------------------------------------------------
#
# Project created by QtCreator 2010-10-11T19:25:25
#
#-------------------------------------------------

win32 {
	INCLUDEPATH += $$PWD/../libssh2-1.2.7/include/
	LIBS     += -L$$PWD/../libssh2-1.2.7/lib/ -llibgpg-error -lwsock32
}

!win32 {
	LIBS	+= -lgpg-error
}

macx {
	DEFINES += __DARWIN_64_BIT_INO_T
}

LIBS	+= -lssh2

QT       += core gui network xml script

TARGET = remoted
TEMPLATE = app

SOURCES += \
    editor/linenumberwidget.cpp \
    editor/editor.cpp \
    editor/codeeditor.cpp \
    file/unsavedchangesdialog.cpp \
    file/sshfile.cpp \
    file/openfiletreeview.cpp \
    file/openfiletreemodel.cpp \
    file/openfilemanager.cpp \
    file/openfileitemdelegate.cpp \
    file/location.cpp \
    file/filelist.cpp \
    file/filedialog.cpp \
    file/basefile.cpp \
    main/tools.cpp \
    main/searchbar.cpp \
    main/mainwindow.cpp \
    main/main.cpp \
	main/json.cpp \
    options/optionsdialog.cpp \
    ssh/passworddialog.cpp \
    ssh/sshconnectingdialog.cpp \
    ssh/sshconnection.cpp \
    ssh/sshhost.cpp \
    ssh/sshremotecontroller.cpp \
    ssh/sshrequest.cpp \
    ssh/serverconfigdlg.cpp \
    syntax/syntaxdefinition.cpp \
    syntax/syntaxrule.cpp \
    syntax/syntaxhighlighter.cpp \
    syntax/syntaxdefxmlhandler.cpp \
    syntax/syntaxblockdata.cpp \
    file/localfile.cpp \
    website/sitemanager.cpp \
    syntax/syntaxdefmanager.cpp

HEADERS  += \
    editor/linenumberwidget.h \
    editor/editor.h \
    editor/codeeditor.h \
    file/unsavedchangesdialog.h \
    file/sshfile.h \
    file/openfiletreeview.h \
    file/openfiletreemodel.h \
    file/openfilemanager.h \
    file/openfileitemdelegate.h \
    file/location.h \
    file/filelist.h \
    file/filedialog.h \
    file/basefile.h \
    main/tools.h \
    main/searchbar.h \
    main/mainwindow.h \
    main/globaldispatcher.h \
	main/json.h \
    options/optionsdialog.h \
    ssh/sshrequest.h \
    ssh/sshremotecontroller.h \
    ssh/sshhost.h \
    ssh/sshconnection.h \
    ssh/sshconnectingdialog.h \
    ssh/serverconfigdlg.h \
    ssh/passworddialog.h \
    syntax/syntaxrule.h \
    syntax/syntaxhighlighter.h \
    syntax/syntaxdefinition.h \
    syntax/syntaxdefxmlhandler.h \
    syntax/syntaxblockdata.h \
    file/localfile.h \
    website/sitemanager.h \
    syntax/syntaxdefmanager.h

OTHER_FILES += \
    slaves/slave.py \
    slaves/slave.pl \
    syntaxdefs/perl.xml \
    syntax/test.pl \
    syntaxdefs/zonnon.xml \
    syntaxdefs/yaml.xml \
    syntaxdefs/yacc.xml \
    syntaxdefs/yacas.xml \
    syntaxdefs/xul.xml \
    syntaxdefs/xslt.xml \
    syntaxdefs/xorg.xml \
    syntaxdefs/xmldebug.xml \
    syntaxdefs/xml.xml \
    syntaxdefs/xharbour.xml \
    syntaxdefs/winehq.xml \
    syntaxdefs/vrml.xml \
    syntaxdefs/vhdl.xml \
    syntaxdefs/verilog.xml \
    syntaxdefs/velocity.xml \
    syntaxdefs/uscript.xml \
    syntaxdefs/txt2tags.xml \
    syntaxdefs/tibasic.xml \
    syntaxdefs/texinfo.xml \
    syntaxdefs/tcl.xml \
    syntaxdefs/systemc.xml \
    syntaxdefs/stata.xml \
    syntaxdefs/sql.xml \
    syntaxdefs/sql-postgresql.xml \
    syntaxdefs/sql-mysql.xml \
    syntaxdefs/spice.xml \
    syntaxdefs/sml.xml \
    syntaxdefs/sisu.xml \
    syntaxdefs/sieve.xml \
    syntaxdefs/sgml.xml \
    syntaxdefs/sci.xml \
    syntaxdefs/scheme.xml \
    syntaxdefs/scala.xml \
    syntaxdefs/sather.xml \
    syntaxdefs/ruby.xml \
    syntaxdefs/rsiidl.xml \
    syntaxdefs/rpmspec.xml \
    syntaxdefs/roff.xml \
    syntaxdefs/rib.xml \
    syntaxdefs/rhtml.xml \
    syntaxdefs/rexx.xml \
    syntaxdefs/rapidq.xml \
    syntaxdefs/r.xml \
    syntaxdefs/qml.xml \
    syntaxdefs/python.xml \
    syntaxdefs/purebasic.xml \
    syntaxdefs/prolog.xml \
    syntaxdefs/progress.xml \
    syntaxdefs/povray.xml \
    syntaxdefs/postscript.xml \
    syntaxdefs/pike.xml \
    syntaxdefs/picsrc.xml \
    syntaxdefs/php.xml \
    syntaxdefs/pgn.xml \
    syntaxdefs/pango.xml \
    syntaxdefs/opal.xml \
    syntaxdefs/octave.xml \
    syntaxdefs/ocaml.xml \
    syntaxdefs/objectivecpp.xml \
    syntaxdefs/objectivec.xml \
    syntaxdefs/noweb.xml \
    syntaxdefs/nemerle.xml \
    syntaxdefs/nasm.xml \
    syntaxdefs/mup.xml \
    syntaxdefs/monobasic.xml \
    syntaxdefs/modula-2.xml \
    syntaxdefs/modelica.xml \
    syntaxdefs/mips.xml \
    syntaxdefs/mergetagtext.xml \
    syntaxdefs/mediawiki.xml \
    syntaxdefs/maxima.xml \
    syntaxdefs/matlab.xml \
    syntaxdefs/mason.xml \
    syntaxdefs/mandoc.xml \
    syntaxdefs/makefile.xml \
    syntaxdefs/mab.xml \
    syntaxdefs/m3u.xml \
    syntaxdefs/lua.xml \
    syntaxdefs/lsl.xml \
    syntaxdefs/lpc.xml \
    syntaxdefs/logtalk.xml \
    syntaxdefs/literate-haskell.xml \
    syntaxdefs/lilypond.xml \
    syntaxdefs/lex.xml \
    syntaxdefs/ldif.xml \
    syntaxdefs/latex.xml \
    syntaxdefs/kbasic.xml \
    syntaxdefs/jsp.xml \
    syntaxdefs/json.xml \
    syntaxdefs/javascript.xml \
    syntaxdefs/javadoc.xml \
    syntaxdefs/java.xml \
    syntaxdefs/ini.xml \
    syntaxdefs/inform.xml \
    syntaxdefs/ilerpg.xml \
    syntaxdefs/idl.xml \
    syntaxdefs/idconsole.xml \
    syntaxdefs/html.xml \
    syntaxdefs/haxe.xml \
    syntaxdefs/haskell.xml \
    syntaxdefs/grammar.xml \
    syntaxdefs/go.xml \
    syntaxdefs/gnuassembler.xml \
    syntaxdefs/glsl.xml \
    syntaxdefs/gettext.xml \
    syntaxdefs/gdl.xml \
    syntaxdefs/gdb.xml \
    syntaxdefs/gap.xml \
    syntaxdefs/fstab.xml \
    syntaxdefs/freebasic.xml \
    syntaxdefs/fortran.xml \
    syntaxdefs/fgl-per.xml \
    syntaxdefs/fgl-4gl.xml \
    syntaxdefs/ferite.xml \
    syntaxdefs/euphoria.xml \
    syntaxdefs/erlang.xml \
    syntaxdefs/email.xml \
    syntaxdefs/eiffel.xml \
    syntaxdefs/e.xml \
    syntaxdefs/dtd.xml \
    syntaxdefs/doxygenlua.xml \
    syntaxdefs/doxygen.xml \
    syntaxdefs/djangotemplate.xml \
    syntaxdefs/diff.xml \
    syntaxdefs/desktop.xml \
    syntaxdefs/debiancontrol.xml \
    syntaxdefs/debianchangelog.xml \
    syntaxdefs/d.xml \
    syntaxdefs/cue.xml \
    syntaxdefs/css.xml \
    syntaxdefs/cs.xml \
    syntaxdefs/cpp.xml \
    syntaxdefs/component-pascal.xml \
    syntaxdefs/commonlisp.xml \
    syntaxdefs/coldfusion.xml \
    syntaxdefs/cmake.xml \
    syntaxdefs/clipper.xml \
    syntaxdefs/cisco.xml \
    syntaxdefs/changelog.xml \
    syntaxdefs/cgis.xml \
    syntaxdefs/cg.xml \
    syntaxdefs/c.xml \
    syntaxdefs/boo.xml \
    syntaxdefs/bmethod.xml \
    syntaxdefs/bibtex.xml \
    syntaxdefs/bash.xml \
    syntaxdefs/awk.xml \
    syntaxdefs/asterisk.xml \
    syntaxdefs/asp.xml \
    syntaxdefs/asn1.xml \
    syntaxdefs/asm6502.xml \
    syntaxdefs/asm-m68k.xml \
    syntaxdefs/asm-dsp56k.xml \
    syntaxdefs/asm-avr.xml \
    syntaxdefs/apache.xml \
    syntaxdefs/ansys.xml \
    syntaxdefs/ansic89.xml \
    syntaxdefs/ample.xml \
    syntaxdefs/alert.xml \
    syntaxdefs/alert_indent.xml \
    syntaxdefs/ahdl.xml \
    syntaxdefs/ada.xml \
    syntaxdefs/actionscript.xml \
    syntaxdefs/abc.xml \
    syntaxdefs/abap.xml

FORMS += \
    file/filedialog.ui \
    main/searchbar.ui \
    options/optionsdialog.ui \
    ssh/sshconnectingdialog.ui \
    ssh/serverconfigdlg.ui \
    ssh/passworddialog.ui

RESOURCES += \
	resources.qrc
