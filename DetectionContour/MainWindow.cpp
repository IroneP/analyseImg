// Project
#include "MainWindow.h"
#include "ui_MainWindow.h"

// Qt
#include <QFileDialog>
#include <QMessageBox>

// Project
#include "Pipeline.h"

// OpenCV
#ifdef WIN32
    #include <opencv/cv.hpp>
#else
    #include <cv.h>
#endif

/******************************************************************************
 *
 ******************************************************************************/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _pipeline( NULL )
{
    ui->setupUi( this );

    // Hide widgets and resize for the moment
    resize( 0, 0 );

    // UI setup (Qt bug)
    ui->_hysteresisHighThresholdSpinBox->setValue( 50 );
}

/******************************************************************************
 *
 ******************************************************************************/
MainWindow::~MainWindow()
{
    delete ui;
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::setPipeline( Pipeline* pPipeline )
{
    _pipeline = pPipeline;
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__imageLoaderToolButton_clicked( bool pChecked )
{
    // Try to open image file
    QString filename = QFileDialog::getOpenFileName( this, "Choose an image file", QString( "./Images" ), tr( "Image Files (*.jpg *.png)" ) );
    if ( ! filename.isEmpty() )
    {
        // Retrieve image info
        unsigned int imageWidth = 0;
        unsigned int imageHeight = 0;
        cv::Mat image = cv::imread( filename.toLatin1().constData(), 0/*grayscale*/ );
        if( image.data )
        {
            imageWidth = image.cols;
            imageHeight = image.rows;
        }

        // Update image info widgets
        ui->_imageFilenameLineEdit->setText( filename );
        ui->_imageNbRowsEdit->setText( QString::number( imageHeight ) );
        ui->_imageNbColumnsLineEdit->setText( QString::number( imageWidth ) );

        // Update pipeline
        if ( _pipeline != NULL )
        {
            _pipeline->setImageFilename( filename.toLatin1().constData() );
        }
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__imageVisualizationCheckBox_stateChanged( int pState )
{
    // Update pipeline
    if ( _pipeline != NULL )
    {
        _pipeline->setImageVisible( pState == Qt::Checked );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__gradientGroupBox_toggled( bool pOn )
{
   if ( _pipeline != NULL )
   {
       _pipeline->setUseGradient( pOn );
   }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__gradientVisualizationCheckBox_stateChanged( int pState )
{
    // Update pipeline
    if ( _pipeline != NULL )
    {
        _pipeline->setGradientVisible( pState == Qt::Checked );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__filterKernelTypeComboBox_currentIndexChanged( int pIndex )
{
    if ( _pipeline != NULL )
    {
        _pipeline->setFilterKernelType( static_cast< Pipeline::FilterKernelType >( pIndex ) );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__filterDirectionTypeRadioButton_toggled( bool pChecked )
{
    if ( _pipeline != NULL )
    {
        Pipeline::DirectionalFilteringType directionalFilteringType = pChecked ? Pipeline::eBidirectional : Pipeline::eMultidirectional;
        _pipeline->setDirectionalFilteringType( directionalFilteringType );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__gradientNormComboBox_currentIndexChanged( int pIndex )
{
    if ( _pipeline != NULL )
    {
        _pipeline->setNormType( static_cast< Pipeline::NormType >( pIndex ) );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__thresholdGroupBox_toggled( bool pOn )
{
    if ( _pipeline != NULL )
    {
        _pipeline->setUseThreshold( pOn );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__thresholdTypeComboBox_currentIndexChanged( int pIndex )
{
    if ( _pipeline != NULL )
    {
        _pipeline->setThresholdType( static_cast< Pipeline::ThresholdType >( pIndex ) );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__globalThresholdSpinBox_valueChanged( int i )
{
    if ( _pipeline != NULL )
    {
        _pipeline->setGlobalThresholdValidPixelPercentage( i );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__localThresholdSpinBox_valueChanged( int i )
{
    if ( _pipeline != NULL )
    {
        _pipeline->setLocalThresholdWindowSize( i );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__hysteresisHighThresholdSpinBox_valueChanged( int i )
{
    if ( _pipeline != NULL )
    {
        _pipeline->setHysteresisThresholdHighValidPixelPercentage( i );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__hysteresisLowThresholdSpinBox_valueChanged( int i )
{
    if ( _pipeline != NULL )
    {
        _pipeline->setHysteresisThresholdLowValidPixelPercentage( i );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__localExtremaCheckBox_stateChanged( int pState )
{
    // Update pipeline
    if ( _pipeline != NULL )
    {
        _pipeline->setUseLocalExtrema( pState == Qt::Checked );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__edgeGroupBox_toggled( bool pOn )
{
    if ( _pipeline != NULL )
    {
        _pipeline->setUseEdgeExtraction( pOn );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__edgeMethodComboBox_currentIndexChanged( int pIndex )
{
    if ( _pipeline != NULL )
    {
    //    _pipeline->setUseEdgeClosure( static_cast< Pipeline::EdgeClosureType >( pIndex ) );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__edgeClosureCheckBox_stateChanged( int pState )
{
    // Update pipeline
    if ( _pipeline != NULL )
    {
        _pipeline->setUseEdgeClosure( pState == Qt::Checked );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__edgeClosureNbIterationsSpinBox_valueChanged( int i )
{
    // Update pipeline
    if ( _pipeline != NULL )
    {
        _pipeline->setEdgeClosureNbIterations( i );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__houghSegmentGroupBox_toggled( bool pOn )
{
   if ( _pipeline != NULL )
   {
       _pipeline->setHoughSegmentDetection( pOn );
   }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__houghSegmentCriteriaSpinBox_valueChanged( int i )
{
    if ( _pipeline != NULL )
    {
        _pipeline->setHoughSegmentCriteria( i );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__houghThresholdCheckBox_stateChanged( int pState )
{
    // Update pipeline
    if ( _pipeline != NULL )
    {
        _pipeline->setHoughSegmentThreshold( pState == Qt::Checked );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__houghFollowGradientDirectionCheckBox( int pState )
{
    // Update pipeline
    if ( _pipeline != NULL )
    {
        _pipeline->setHoughFollowGradientDirection( pState == Qt::Checked );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__houghCircleGroupBox_toggled( bool pOn )
{
   if ( _pipeline != NULL )
   {
       _pipeline->setHoughCircleDetection( pOn );
   }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__houghCircleFixedRadiusCheckBox_stateChanged( int pState )
{
    // Update pipeline
    if ( _pipeline != NULL )
    {
        _pipeline->setHoughCircleUseFixedRadius( pState == Qt::Checked );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__houghCircleCriteriaSpinBox_valueChanged( int i )
{
    if ( _pipeline != NULL )
    {
        _pipeline->setHoughCircleCriteria( i );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__houghCircleThresholdCheckBox_stateChanged( int pState )
{
    // Update pipeline
    if ( _pipeline != NULL )
    {
        _pipeline->setHoughCircleThresholdVotes( pState == Qt::Checked );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__houghCircleThresholdSpinBox_valueChanged( int i )
{
    if ( _pipeline != NULL )
    {
        _pipeline->setHoughCircleThresholdVotesValue( i );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__useBinaryDisplayCheckBox_stateChanged( int pState )
{
    // Update pipeline
    if ( _pipeline != NULL )
    {
        _pipeline->setUseBinaryDisplay( pState == Qt::Checked );
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void MainWindow::on__applyPushButton_clicked( bool pChecked )
{
    // Warning
    if ( ui->_imageFilenameLineEdit->text().isEmpty() )
    {
        QMessageBox::warning( this, tr( "Warning" ), tr( "Please, choose an image file." ) );
        return;
    }

    if ( _pipeline != NULL )
    {
        // Launch the main Edge Detection execution pipeline
        _pipeline->execute();

        // Update user interface
        ui->_globalThresholdLineEdit->setText( QString::number( _pipeline->getGlobalThresholdValue() ) );
        ui->_hysteresisHighThresholdLineEdit->setText( QString::number( _pipeline->getHysteresisThresholdHighValue() ) );
        ui->_hysteresisLowThresholdLineEdit->setText( QString::number( _pipeline->getHysteresisThresholdLowValue() ) );
    }
}
