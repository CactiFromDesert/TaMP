#ifndef SCHEMADIALOG_H
#define SCHEMADIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QScrollArea>
#include <QWidget>

// ── UI STUB MODE (для будущих заглушек) ───────────────────────
#ifndef UI_STUB
#define UI_STUB 1
#endif

// ── Inner canvas widget that draws the flowchart ─────────────
class FlowchartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FlowchartWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

// ── The dialog ───────────────────────────────────────────────
class SchemaDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SchemaDialog(QWidget *parent = nullptr);
    ~SchemaDialog();

private:
    QPushButton     *closeBtn;
    FlowchartWidget *canvas;
    QScrollArea     *scrollArea;
    QLabel *imageLabel;

    void setupUI();

#if UI_STUB
    // UI-only helpers (если захочешь позже отключать отрисовку)
    void enableStubMode(bool enable = true);
    bool m_stubMode = true;
#endif
};

#endif // SCHEMADIALOG_H