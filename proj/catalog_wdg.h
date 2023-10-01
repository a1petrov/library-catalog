#pragma once
#include <QWidget>

class CatalogWidget : public QWidget
{
    Q_OBJECT

public:
    CatalogWidget(QWidget *parent = nullptr);

    void readSettings();
    void writeSettings() const;

private:
    bool clearModel(class QAbstractItemModel *);

    void filterRegExpChanged();

    bool loadCatalog(QString path = QString());
    bool saveCatalog(const QString &);

private:
    QAbstractItemModel *m_baseModel;
    class QSortFilterProxyModel *m_proxyModel;

    class QTreeView *m_catalogView;
    class QPushButton *m_saveBtn;
    class QLineEdit* m_filterEdit;
    class QComboBox* m_filterColCombo;
    class QCheckBox* m_caseSensCheck;

    QString m_recentPath;
};
