# Note: These version based 'plugins' are not an actual Qt plugins,
# they are just regular runtime loaded libraries
include(../qpluginbase.pri)

CONFIG  -= plugin

MMP_RULES += NOEXPORTLIBRARY

defBlock = \
    "$${LITERAL_HASH}ifdef WINSCW" \
    "DEFFILE ../bwins/qts60plugin.def" \
    "$${LITERAL_HASH}else" \
    "DEFFILE ../eabi/qts60plugin.def" \
    "$${LITERAL_HASH}endif"

MMP_RULES += defBlock