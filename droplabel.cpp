#include "droplabel.h"

DropLabel::DropLabel(QWidget *parent) : QLabel(parent)
{
    setAcceptDrops(true);
}

void DropLabel::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void DropLabel::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void DropLabel::dropEvent(QDropEvent *event) {
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        if(urlList.size() > 0){
            QString name = urlList[0].toLocalFile();
            emit fileSelected(name);
        }

        event->acceptProposedAction();
    }
}
