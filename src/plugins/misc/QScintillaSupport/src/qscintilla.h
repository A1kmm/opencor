//==============================================================================
// QScintilla class
//==============================================================================

#ifndef QSCINTILLA_H
#define QSCINTILLA_H

//==============================================================================

#include "qscintillasupportglobal.h"

//==============================================================================

#include "Qsci/qsciscintilla.h"

//==============================================================================

namespace OpenCOR {
namespace QScintillaSupport {

//==============================================================================

class QSCINTILLASUPPORT_EXPORT QScintilla : public QsciScintilla
{
public:
    explicit QScintilla(const QString &pContents, const bool &pReadOnly,
                        QsciLexer *pLexer = 0, QWidget *pParent = 0);

protected:
    virtual void dragEnterEvent(QDragEnterEvent *pEvent);
};

//==============================================================================

}   // namespace QScintillaSupport
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================