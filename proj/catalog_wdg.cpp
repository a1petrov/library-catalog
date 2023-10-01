#include "catalog_wdg.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDebug>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSaveFile>
#include <QSettings>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTextStream>
#include <QTreeView>

#define SAMPLE_INIT

namespace {
const QStringList HDR_FIELDS{QObject::tr("Name"),
                             QObject::tr("Author"),
                             QObject::tr("Publisher"),
                             QObject::tr("Year")};
constexpr QChar DELIM(';');
}; // namespace
//=================================================================================================

CatalogWidget::CatalogWidget(QWidget *parent /* = nullptr*/)
    : QWidget(parent)
    , m_baseModel(new QStandardItemModel(0, HDR_FIELDS.size(), this))
    , m_proxyModel(new QSortFilterProxyModel(this))
    , m_recentPath(QDir::homePath())
{
    qobject_cast<QStandardItemModel *>(m_baseModel)->setHorizontalHeaderLabels(HDR_FIELDS);
    m_proxyModel->setSourceModel(m_baseModel);

    m_proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);

    setWindowTitle(tr("Library Catalog"));

    auto *mainLayout = new QVBoxLayout(this);

    auto catalogGroup = new QGroupBox(tr("Books List"));
    mainLayout->addWidget(catalogGroup);
    auto gridLayout = new QGridLayout(catalogGroup);

    int row(0), col(0);
    gridLayout->addWidget(m_catalogView = new QTreeView(catalogGroup), row++, col, 1, -1);
    m_catalogView->setModel(m_proxyModel); // Use proxy model for view
    m_catalogView->setRootIsDecorated(false);
    m_catalogView->setAlternatingRowColors(true);
    m_catalogView->setSortingEnabled(true);
    m_catalogView->sortByColumn(1, Qt::AscendingOrder);

    auto lbl = new QLabel(tr("Filter pattern:"), catalogGroup);
    gridLayout->addWidget(lbl, row, col++);

    gridLayout->addWidget(m_filterEdit = new QLineEdit(catalogGroup), row++, col--);
    lbl->setBuddy(m_filterEdit);
    connect(m_filterEdit, &QLineEdit::textChanged, this, &CatalogWidget::filterRegExpChanged);

    lbl = new QLabel(tr("Filter column::"), catalogGroup);
    gridLayout->addWidget(lbl, row, col++);

    gridLayout->addWidget(m_filterColCombo = new QComboBox(catalogGroup), row++, col--);
    m_filterColCombo->addItems(HDR_FIELDS);
    lbl->setBuddy(m_filterColCombo);
    connect(m_filterColCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]() {
        qDebug() << m_filterColCombo->currentIndex();
        m_proxyModel->setFilterKeyColumn(-1/*m_filterColCombo->currentIndex()*/);
    });

    gridLayout->addWidget(m_caseSensCheck = new QCheckBox(tr("Case sensitive filter"), catalogGroup),
                          row++,
                          col--);
    connect(m_caseSensCheck, &QAbstractButton::toggled, this, &CatalogWidget::filterRegExpChanged);

    auto btnBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    mainLayout->addWidget(btnBox);

    auto btn = btnBox->button(QDialogButtonBox::Close);
    Q_ASSERT(nullptr != btn);
    btn->setIcon(QIcon(QStringLiteral(":/images/resources/close.png")));
    btn->setText(tr("Exit"));
    connect(btnBox, &QDialogButtonBox::rejected, this, &QWidget::close);

    btn = btnBox->addButton(tr("Open Catalog..."), QDialogButtonBox::ActionRole);
    btn->setIcon(QIcon(QStringLiteral(":/images/resources/open.png")));
    connect(btn, &QPushButton::clicked, this, [=]() { loadCatalog(); });

    m_saveBtn = btnBox->addButton(tr("Save Catalog..."), QDialogButtonBox::ActionRole);
    m_saveBtn->setIcon(QIcon(QStringLiteral(":/images/resources/save.png")));
    connect(m_saveBtn, &QPushButton::clicked, this, [this]() {
        const auto path = QFileDialog::getSaveFileName(this,
                                                       tr("Open Library Catalog"),
                                                       m_recentPath,
                                                       tr("CSV files (*.csv)"));
        if (path.isEmpty())
            return;

        if (saveCatalog(path))
            QMessageBox::information(this, tr("Information"), tr("Catalog successfully saved."));
    });
    m_saveBtn->setDisabled(true);

#ifdef SAMPLE_INIT
    QString path(QStringLiteral(":/files/resources/def_lib_catalog.csv"));
    loadCatalog(path);
#endif
}
//=================================================================================================

bool CatalogWidget::clearModel(QAbstractItemModel *model)
{
    return model->removeRows(0, model->rowCount());
}
//=================================================================================================

void CatalogWidget::filterRegExpChanged()
{
    QRegExp regExp(m_filterEdit->text(),
                   m_caseSensCheck->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive,
                   QRegExp::RegExp);
    m_proxyModel->setFilterRegExp(regExp);
}
//=================================================================================================

bool CatalogWidget::loadCatalog(QString path /* = QString()*/)
{
    if (path.isEmpty()) {
        path = QFileDialog::getOpenFileName(this,
                                            tr("Open Library Catalog"),
                                            m_recentPath,
                                            tr("CSV files (*.csv)"));
        if (path.isEmpty())
            return false;
    }

    if (!QFile::exists(path)) {
        QMessageBox::critical(this, tr("Error"), tr("File %1 doesn't exist!").arg(path));
        return false;
    }

    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to open file %1!").arg(path));
        return false;
    }

    clearModel(m_baseModel);

    // Read CSV line by line and fill model data
    QTextStream in(&file);
    in.setCodec("UTF-8");
    auto model = qobject_cast<QStandardItemModel *>(m_baseModel);
    while (!in.atEnd()) {
        QString line(in.readLine());
        QList<QStandardItem *> items;
        const auto values(line.split(DELIM, QString::SkipEmptyParts));
        if (values.size() != HDR_FIELDS.size())
            continue;
        for (const auto &str : qAsConst(values))
            items << new QStandardItem(str);
        model->insertRow(m_baseModel->rowCount(), items);
    }

    file.close();
    m_recentPath = path;
    m_saveBtn->setEnabled(true);
    m_catalogView->sortByColumn(0, Qt::AscendingOrder);
    m_filterColCombo->setCurrentIndex(0);
    m_caseSensCheck->setChecked(false);
    for (int i = 0; i < model->columnCount(); ++i)
        m_catalogView->resizeColumnToContents(i);

    return true;
}
//=================================================================================================

bool CatalogWidget::saveCatalog(const QString &path)
{
    QSaveFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to open file for saving %1!").arg(path));
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    for (int row(0); row < m_baseModel->rowCount(); ++row) {
        for (int col(0); col < m_baseModel->columnCount(); ++col) {
            out << m_baseModel->data(m_baseModel->index(row, col)).toString();
            col + 1 == m_baseModel->columnCount() ? (out << "\n") : (out << DELIM);
        }
    }

    file.commit();
    m_recentPath = path;
    return true;
}
//=================================================================================================

void CatalogWidget::readSettings()
{
    QSettings s;
    m_recentPath = s.value(QStringLiteral("Recent_path"), QDir::homePath()).toString();
    if (!restoreGeometry(s.value("Geometry").toByteArray()))
        resize(640, 480);
}
//=================================================================================================

void CatalogWidget::writeSettings() const
{
    QSettings s;
    s.setValue(QStringLiteral("Recent_path"), m_recentPath);
    s.setValue(QStringLiteral("Geometry"), saveGeometry());
}
//=================================================================================================
