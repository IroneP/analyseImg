#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui
{
    class MainWindow;
}

// Project
class Pipeline;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( QWidget* parent = 0 );
    ~MainWindow();
    void setPipeline( Pipeline* pPipeline );

public slots:

    // Image
    void on__imageLoaderToolButton_clicked( bool pChecked );
    void on__imageVisualizationCheckBox_stateChanged( int pState );

    // Gradient
    void on__gradientGroupBox_toggled( bool pOn );
    void on__filterKernelTypeComboBox_currentIndexChanged( int pIndex );
    void on__filterDirectionTypeRadioButton_toggled( bool pChecked );
    void on__gradientVisualizationCheckBox_stateChanged( int pState );
    void on__gradientNormComboBox_currentIndexChanged( int pIndex );

    // Threshold
    void on__thresholdGroupBox_toggled( bool pOn );
    void on__thresholdTypeComboBox_currentIndexChanged( int pIndex );
    void on__localExtremaCheckBox_stateChanged( int pState );
    void on__globalThresholdSpinBox_valueChanged( int i );
    void on__localThresholdSpinBox_valueChanged( int i );
    void on__hysteresisHighThresholdSpinBox_valueChanged( int i );
    void on__hysteresisLowThresholdSpinBox_valueChanged( int i );

    // Edges
    void on__edgeGroupBox_toggled( bool pOn );
    void on__edgeMethodComboBox_currentIndexChanged( int pIndex );
    void on__edgeClosureCheckBox_stateChanged( int pState );
    void on__edgeClosureNbIterationsSpinBox_valueChanged( int i );

    // Hough
    // - segment
    void on__houghSegmentGroupBox_toggled( bool pOn );
    void on__houghSegmentCriteriaSpinBox_valueChanged( int i );
    void on__houghThresholdCheckBox_stateChanged( int pState );
    void on__houghFollowGradientDirectionCheckBox( int pState );
    // - circle
    void on__houghCircleGroupBox_toggled( bool pOn );
    void on__houghCircleFixedRadiusCheckBox_stateChanged( int pState );
    void on__houghCircleCriteriaSpinBox_valueChanged( int i );
    void on__houghCircleThresholdCheckBox_stateChanged( int pState );
    void on__houghCircleThresholdSpinBox_valueChanged( int i );

    // Global settings
    void on__useBinaryDisplayCheckBox_stateChanged( int pState );

    // Pipeline
    void on__applyPushButton_clicked( bool pChecked );

private:
    Ui::MainWindow* ui;
    Pipeline* _pipeline;

};

#endif // MAINWINDOW_H
