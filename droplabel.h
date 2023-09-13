#ifndef DROPLABEL_H
#define DROPLABEL_H

#include <QDragEnterEvent>
#include <QLabel>
#include <QMimeData>

class DropLabel : public QLabel
{
    Q_OBJECT
public:
    DropLabel(QWidget *parent = nullptr);

    void dragEnterEvent(QDragEnterEvent* event) override;

    void dragMoveEvent(QDragMoveEvent* event) override;

    void dropEvent(QDropEvent* event) override;

signals:
    void fileSelected(QString fileName);
};

#endif // DROPLABEL_H
