//==============================================================================
// CellML annotation view metadata normal view details widget
//==============================================================================

#ifndef CELLMLANNOTATIONVIEWMETADATANORMALVIEWDETAILSWIDGET_H
#define CELLMLANNOTATIONVIEWMETADATANORMALVIEWDETAILSWIDGET_H

//==============================================================================

#include "cellmlfile.h"
#include "commonwidget.h"

//==============================================================================

#include <QScrollArea>

//==============================================================================

namespace Ui {
    class CellMLAnnotationViewMetadataNormalViewDetailsWidget;
}

//==============================================================================

class QGridLayout;
class QLabel;
class QStackedWidget;

//==============================================================================

namespace OpenCOR {
namespace CellMLAnnotationView {

//==============================================================================

class CellMLAnnotationViewWidget;

//==============================================================================

class CellMLAnnotationViewMetadataNormalViewDetailsWidget : public QScrollArea,
                                                            public Core::CommonWidget
{
    Q_OBJECT

private:
    enum Information {
        None,
        First,
        Any,
        Last
    };

    enum Type {
        No,
        Qualifier,
        Resource,
        Id
    };

public:
    explicit CellMLAnnotationViewMetadataNormalViewDetailsWidget(CellMLAnnotationViewWidget *pParent);
    ~CellMLAnnotationViewMetadataNormalViewDetailsWidget();

    virtual void retranslateUi();

    void updateGui(iface::cellml_api::CellMLElement *pElement,
                   const QString &pRdfTripleInformation = QString(),
                   const Type &pType = No,
                   const Information &pLookupInformation = First,
                   const int &pVerticalScrollBarPosition = 0,
                   const bool &pRetranslate = false);

    void addRdfTriple(CellMLSupport::CellMLFileRdfTriple *pRdfTriple);

private:
    CellMLSupport::CellMLFile *mCellMLFile;

    Ui::CellMLAnnotationViewMetadataNormalViewDetailsWidget *mGui;

    QStackedWidget *mWidget;

    QWidget *mGridWidget;
    QGridLayout *mGridLayout;

    ObjRef<iface::cellml_api::CellMLElement> mElement;

    QString mRdfTripleInformation;
    Type mType;

    Information mLookupInformation;

    int mVerticalScrollBarPosition;
    int mNeighbourRow;

    QMap<QObject *, CellMLSupport::CellMLFileRdfTriple *> mRdfTriplesMapping;

    QLabel *mCurrentResourceOrIdLabel;

    void genericLookup(const QString &pRdfTripleInformation = QString(),
                       const Type &pType = No,
                       const bool &pRetranslate = false);

    QString rdfTripleInformation(const int &pRow) const;

Q_SIGNALS:
    void qualifierLookupRequested(const QString &pQualifier,
                                  const bool &pRetranslate);
    void resourceLookupRequested(const QString &pResource,
                                 const bool &pRetranslate);
    void idLookupRequested(const QString &pResource, const QString &pId,
                           const bool &pRetranslate);
    void noLookupRequested();

    void rdfTripleRemoved(CellMLSupport::CellMLFileRdfTriple *pRdfTriple);

private Q_SLOTS:
    void on_actionCopy_triggered();

    void disableLookupInformation();

    void lookupQualifier(const QString &pRdfTripleInformation);
    void lookupResource(const QString &pRdfTripleInformation);
    void lookupId(const QString &pRdfTripleInformation);

    void removeRdfTriple();

    void showNeighbourRdfTriple();
    void showLastRdfTriple();

    void trackVerticalScrollBarPosition(const int &pPosition);

    void showCustomContextMenu(const QPoint &pPosition);
};

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
