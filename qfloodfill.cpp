#include "qfloodfill.h"
#include <deque>
#include <unordered_set>
#include <stdexcept>
#include <QPainter>

std::deque<QPoint> GetPoints(QImage& image, QPoint seed)
{
    // A really simple algorithm for the time being. Feel free to improve!

    if (image.format() != QImage::Format_ARGB32_Premultiplied) {
        throw std::runtime_error("The only supported input image format is QImage::Format_ARGB32_Premultiplied");
    }

    const int width = image.width();
    const int height = image.height();

    QRgb* bits = reinterpret_cast<QRgb*>(image.bits());

    const auto getPixel = [&](int x, int y) {
        return bits[(y * width) + x];
    };

    const QRgb oldRgba = getPixel(seed.x(), seed.y());

    std::vector<unsigned char> processedAlready(width * height);

    std::deque<QPoint> backlog = { seed };
    std::deque<QPoint> points;

    while (!backlog.empty()) {
        const QPoint& point = backlog.front();
        const int x = point.x();
        const int y = point.y();
        if (x >= 0 && y >= 0 && x < width && y < height) {
            const QRgb rgba = getPixel(x, y);
            if (rgba == oldRgba) {
                unsigned char& isProcessedAlready = processedAlready[(y * width) + x];
                if (!isProcessedAlready) {
                    isProcessedAlready = true;
                    points.push_back(point);
                    backlog.push_back(QPoint(x - 1, y));
                    backlog.push_back(QPoint(x, y - 1));
                    backlog.push_back(QPoint(x + 1, y));
                    backlog.push_back(QPoint(x, y + 1));
                }
            }
        }
        backlog.pop_front();
    }

    return points;
}

void FloodFill(QPixmap& pixmap, QPoint seed, QColor newColor)
{
    QImage image = pixmap.toImage();
    if (image.format() != QImage::Format_ARGB32_Premultiplied) {
        image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    }

    const auto points = GetPoints(image, seed);

    QPainter painter(&pixmap);
    painter.setPen(newColor);
    painter.setBrush(newColor);
    painter.setCompositionMode(QPainter::CompositionMode_Source);

    for (const QPoint& point : points) {
        painter.drawPoint(point);
    }
}
