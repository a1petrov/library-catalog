#pragma once
#include <QWidget>

class CatalogWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief CatalogWidget
     * главное окно приложения
     * @param parent
     */
    CatalogWidget(QWidget *parent = nullptr);
    /**
     * @brief readSettings
     * чтение настроек геометрии главного окна и последнего пути к файлу каталога
     */
    void readSettings();
    /**
     * @brief writeSettings
     * сохранение настроек геометрии главного окна и последнего пути к файлу каталога
     */
    void writeSettings() const;

private:
    /**
     * @brief clearModel
     * метод очистки модели(удаление всех строк, кроме заголовка)
     * @return
     * очистилась/не очистилась
     */
    bool clearModel(class QAbstractItemModel *);
    /**
     * @brief filterRegExpChanged
     * обработчик изменения регулярного выражения для фильтрации(поиска)
     */
    void filterRegExpChanged();
    /**
     * @brief loadCatalog
     * загрузка каталога по указанному пути
     * @param path
     * путь к CSV-файлу каталога
     * @return
     * true - если загрузка прошла успешно, false - в случае ошибки
     */
    bool loadCatalog(QString path = QString());
    /**
     * @brief saveCatalog
     * сохранения текущего каталога в файл
     * @param path
     * путь к CSV-файлу каталога
     * @return
     * true - файл успешно сохранен, false - иначе
     */
    bool saveCatalog(const QString &);

private:
    /**
     * @brief m_baseModel
     * модель базовая модель для данных каталога
     */
    QAbstractItemModel *m_baseModel;
    /**
     * @brief m_proxyModel
     * proxy модель, обеспечивающая возможность сортировки и фильтрации
     */
    class QSortFilterProxyModel *m_proxyModel;
    /**
     * @brief m_catalogView
     * представление каталога в табличном виде
     */
    class QTreeView *m_catalogView;
    /**
     * @brief m_saveBtn
     * кнопка сохранения каталога
     */
    class QPushButton *m_saveBtn;
    /**
     * @brief m_filterEdit
     * поле ввода регулярного выражения для фильтра
     */
    class QLineEdit* m_filterEdit;
    /**
     * @brief m_filterColCombo
     * выпадающий список для выбора столбца таблицы по которому будет производиться фильтрация
     */
    class QComboBox* m_filterColCombo;
    /**
     * @brief m_caseSensCheck
     * флаг установки чувствительности к регистру для фильтра
     */
    class QCheckBox* m_caseSensCheck;
    /**
     * @brief m_recentPath
     * последний использованный путь для загрузки каталога
     */
    QString m_recentPath;
};
