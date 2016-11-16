/*
 * Image processing : edge detection
 *
 * Authors : Pascal Guehl, Clement Picq
 */
 
/**
 * @version 1.0
 */

#include "Pipeline.h"

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// System
#include <cassert>

// STL
#include <algorithm>
#include <fstream>
#include <iostream>

// Project
#include "Image.h"
#include "Algorithm.h"
#include "Hough.h"
#include "PerformanceTimer.h"
#include "Filter.h"

/******************************************************************************
 ****************************** NAMESPACE SECTION *****************************
 ******************************************************************************/

// STL
using namespace std;

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/
 
/**
 * Application path
 */
string Pipeline::_applicationPath = string();

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ***************************** METHOD DEFINITION ******************************
 ******************************************************************************/

/******************************************************************************
 * Set application path
 *
 * @param pPath application path
 ******************************************************************************/
void Pipeline::setApplicationPath( const char* pPath )
{
    _applicationPath = pPath;
}

/******************************************************************************
 * Constructor
 ******************************************************************************/
Pipeline::Pipeline()
:   _gradientModule()
,   _gradientSlope()
,   _thresholdType( eGlobalThreshold )
,   _filterKernelType( ePrewittKernel )
,   _image( NULL )
,   _filters()
,   _isMultidirectional( false )
,   _directionalFilteringType( eBidirectional )
,   _imageFilename()
,   _visualizeImage( true )
,   _visualizeGradient( false )
,   _normType( eLInfinity )
,   _useEdgeExtraction( false )
,   _useEdgeClosure( false )
,   _visualizeEdges( false )
,   _useThreshold( false )
,   _useLocalExtrema( false )
,   _visualizeThreshold( false )
,   _useGradient( false )
,   _globalThresholdValidPixelPercentage( 60 )
,   _globalThresholdValue( 0 )
,   _localThresholdWindowSize( 15 )
,   _hysteresisThresholdHighValidPixelPercentage( 50 )
,   _hysteresisThresholdLowValidPixelPercentage( 75 )
,   _hysteresisThresholdHighValue( 0 )
,   _hysteresisThresholdLowValue( 0 )
,   _useBinaryDisplay( false )
,   _edgeClosureNbIterations( 5 )
,   _useHoughSegmentDetection( false )
,   _useHoughCircleDetection( false )
,   _houghSegmentCriteria( 2 )
,   _houghCircleCriteria( 2 )
,   _houghSegmentThreshold( false )
,   _houghFollowGradientDirection( false )
,   _houghCircleThresholdVotes( false )
,   _houghCircleThresholdVotesValue( 1 )
,   _useHoughCircleFixedRadius( true )
{
    hough = new Hough();
}

/******************************************************************************
 * Desstructor
 ******************************************************************************/
Pipeline::~Pipeline()
{
    // TODO
    // check openCV reference counters
    assert( false );

    delete _image;
    _image = NULL;
}

/******************************************************************************
 * Print info
 ******************************************************************************/
void Pipeline::print()
{
    // LOG
    cout << "PIPELINE info" << endl;
}

/******************************************************************************
 * Execute pipeline
 ******************************************************************************/
bool Pipeline::execute()
{
    bool statusOK = false;

    // First, check parameters validity
    assert( ! _imageFilename.empty() );
    if ( _imageFilename.empty() )
    {
        cout << "ERROR : Image filename is empty" << endl;
        return false;
    }

    // LOG
    cout << "---------------------------------------------------------------------------" << endl;
    cout << "PIPELINE execute" << endl;

    // Free OpenCV resources
    cv::destroyAllWindows();

    // Finalize filters
    statusOK = finalizeFilters();
    assert( statusOK );
    if ( ! statusOK )
    {
        cout << "ERROR finalizeFilters()" << endl;
        return false;
    }

    // Timers initialization
    PerformanceTimer timer;
    PerformanceTimer::Event processEvent = timer.createEvent();
    PerformanceTimer::Event gradientEvent = timer.createEvent();
    PerformanceTimer::Event thresholdEvent = timer.createEvent();
    PerformanceTimer::Event localExtremaEvent = timer.createEvent();
    PerformanceTimer::Event edgeExtractionEvent = timer.createEvent();
    PerformanceTimer::Event edgeClosureEvent = timer.createEvent();
    PerformanceTimer::Event houghSegmentDetectionEvent = timer.createEvent();
    PerformanceTimer::Event houghCircleDetectionEvent = timer.createEvent();
    float processTime = 0.0f;
    float gradientTime = 0.0f;
    float thresholdTime = 0.0f;
    float localExtremaTime = 0.0f;
    float edgeExtractionTime = 0.0f;
    float edgeClosureTime = 0.0f;
    float houghSegmentDetectionTime = 0.0f;
    float houghCircleDetectionTime = 0.0f;

    // TIMER start
    timer.startEvent( processEvent );

    // Initialize parameters
    const int NbDirection = _isMultidirectional ? 4 : 2;

    // Initialize filters
    statusOK = initializeFilters();
    assert( statusOK );
    if ( ! statusOK )
    {
        cout << "ERROR initializeFilters()" << endl;
        return false;
    }

    // Read image file
    cv::Mat image = cv::imread( _imageFilename, 0/*grayscale*/ );
    image.convertTo( image, CV_32F );
    // Image visualization
    if ( _visualizeImage )
    {
        algorithm::displayMat( "Input Image", image, false);
    }

    // Gradient
    if ( _useGradient )
    {
        // Compute directional components of gradient
        //  - apply dedicated kernels
        char title[] = "Gradient GX";
        for ( int i = 0; i < NbDirection; i++ )
        {
            title[ 10 ] = i + '0';

            timer.startEvent( gradientEvent );
            _gradient[ i ] = algorithm::filter( image, _kernelDirection[ i ] );
            timer.stopEvent( gradientEvent );
            gradientTime += timer.getEventDuration( gradientEvent );

            //algorithm::displayMat( title, gradient[ i ], false );
        }
        // Gradient visualization
        if ( _visualizeGradient )
        {
            for ( int i = 0; i < NbDirection; i++ )
            {
                // Need to normalize to [0,255] before visualization
                //algorithm::normalize( gradient[ i ] );  // BEWARE : normalize modify gradient !!!!!!!!!!
                title[ 10 ] = i + '0';
                //algorithm::displayMat( title, gradient[ i ], false );

                timer.startEvent( gradientEvent );
                cv::Mat tmp = algorithm::normalizeData( _gradient[ i ] );
                timer.stopEvent( gradientEvent );
                gradientTime += timer.getEventDuration( gradientEvent );

                // Visualization
                algorithm::displayMat( title, tmp, false );
            }
        }

        // Retrieve gradient : module and slope
        switch ( _normType )
        {
            case eLInfinity:
                {
                    // L-infinity norm
                    timer.startEvent( gradientEvent );
                    _module = algorithm::moduleLinf( _gradient, NbDirection );
                    timer.stopEvent( gradientEvent );
                    gradientTime += timer.getEventDuration( gradientEvent );

                    // Visualization
                    algorithm::displayMat( "Gradient - module (L-infinity norm)", _module, false );
                }
                break;

            case eL1:
                {
                    // L1 norm
                    timer.startEvent( gradientEvent );
                    _module = algorithm::moduleL1( _gradient, NbDirection );
                    timer.stopEvent( gradientEvent );
                    gradientTime += timer.getEventDuration( gradientEvent );

                    // Visualization
                    algorithm::displayMat( "Gradient - module (L1 norm)", _module, false );
                }
                break;

            default:
                // TODO : L2 norm
                assert( false );
                break;
        }

        // Threshold
        if ( _useThreshold )
        {
            // LOG
            cout << "\nApply THRESHOLD" << endl;

            switch ( _thresholdType )
            {
                case eUserDefinedThreshold:
                    //const int threshold = 80;
                    //moduleThreshold = algorithm::threshold( module, threshold );
                    break;

                case eGlobalThreshold:
                    {
                        // LOG
                        cout << "- Global method (histogram)" << endl;

                        // LOG
                        cout << "- percentage of valid pixels : " << _globalThresholdValidPixelPercentage << endl;

                        timer.startEvent( thresholdEvent );
                        _globalThresholdValue = algorithm::globalThreshold( _module, static_cast< float >( _globalThresholdValidPixelPercentage ) );
                        timer.stopEvent( thresholdEvent );
                        thresholdTime += timer.getEventDuration( thresholdEvent );

                        // LG
                        cout << "- determined threshold value : " << _globalThresholdValue << endl;

                        // Apply threshold
                        timer.startEvent( thresholdEvent );
                        _moduleThreshold = algorithm::applyThreshold( _module, _globalThresholdValue );
                        timer.stopEvent( thresholdEvent );
                        thresholdTime += timer.getEventDuration( thresholdEvent );
                    }
                    break;

                case eLocalThreshold:
                    {
                        // LOG
                        cout << "- Local method (neighborhood mean)" << endl;
                        cout << "- window size: " << _localThresholdWindowSize << endl;

                        timer.startEvent( thresholdEvent );
                        _moduleThreshold = algorithm::localThresholdEnhanced( _module, _localThresholdWindowSize );
                        timer.stopEvent( thresholdEvent );
                        thresholdTime += timer.getEventDuration( thresholdEvent );
                    }
                    break;

                case eHysteresisThreshold:
                    {
                        // LOG
                        cout << "- Hysteris method" << endl;
                        cout << "- percentage of valid pixels (high value): " << _hysteresisThresholdHighValidPixelPercentage << endl;
                        cout << "- percentage of valid pixels (low value): " << _hysteresisThresholdLowValidPixelPercentage << endl;

                        algorithm::_highThresholdPercent = static_cast< float >(_hysteresisThresholdHighValidPixelPercentage );
                        algorithm::_lowThresholdPercent = static_cast< float >( _hysteresisThresholdLowValidPixelPercentage );
                        timer.startEvent( thresholdEvent );
                        _moduleThreshold = algorithm::hysteresis( _module, _hysteresisThresholdHighValue, _hysteresisThresholdLowValue );
                        timer.stopEvent( thresholdEvent );
                        thresholdTime += timer.getEventDuration( thresholdEvent );
                    }
                    break;

                default:
                    // TODO: handle error
                    assert( false );
                    break;
            }

            // Visualization
            algorithm::displayMat( "Gradient - Threshold (module)", _moduleThreshold , _useBinaryDisplay );

            // Gradient slope
            timer.startEvent( gradientEvent );
            _pente = algorithm::pente( _gradient, NbDirection, _moduleThreshold );
            timer.stopEvent( gradientEvent );
            gradientTime += timer.getEventDuration( gradientEvent );
            // Gradient slope visualization
            timer.startEvent( gradientEvent );
            _penteColor = algorithm::colorMat( _pente, _moduleThreshold );
            timer.stopEvent( gradientEvent );
            gradientTime += timer.getEventDuration( gradientEvent );
            algorithm::displayMat( "Gradient - Slope", _penteColor, false );

            // Extract local extrema
            if ( _useLocalExtrema )
            {
                // LOG
                cout << "\nApply LOCAL EXTREMA" << endl;

                timer.startEvent( localExtremaEvent );
                _localExtrema = algorithm::localExtremum( _pente, _moduleThreshold );
                timer.stopEvent( localExtremaEvent );
                localExtremaTime += timer.getEventDuration( localExtremaEvent );

                // Visualization
                algorithm::displayMat( "Local Extrema", _localExtrema , _useBinaryDisplay );

                // Hough Transform segment detection
                if ( _useHoughSegmentDetection )
                {
                    // LOG
                    cout << "\nApply HOUGH Transform - Segment Detection" << endl;

                    timer.startEvent( houghSegmentDetectionEvent );

                    cv::Mat accumulator = hough->CreateSegmentAccumulator( _localExtrema );
                    int segmentCriteria = _houghSegmentCriteria;
                    segmentCriteria = hough->segmentThreshold( accumulator, 30 );
                    cv::Mat affiche = hough->getSegmentFromAccumulator( accumulator, _localExtrema.rows, _localExtrema.cols, segmentCriteria/*nbMinPoints*/);
                    cv::imshow( "Hough Transform: segment detection", affiche );
                    hough->limitSegment( affiche, _moduleThreshold );

                    timer.stopEvent( houghSegmentDetectionEvent );
                    houghSegmentDetectionTime += timer.getEventDuration( houghSegmentDetectionEvent );

                    // Visualization
                    if(_useBinaryDisplay){
                        cv::Mat binaryMat = algorithm::toBinary( affiche );
                        cv::imshow( "Limited Hough Transform: SEGMENT detection", binaryMat );
                        binaryMat.~Mat();
                    }else
                        cv::imshow( "Limited Hough Transform: SEGMENT detection", affiche );
                }

                // Hough Transform segment detection
                if ( _useHoughCircleDetection )
                {
                    // LOG
                    cout << "\nApply HOUGH Transform - Circle Detection" << endl;

                    timer.startEvent( houghCircleDetectionEvent );

                    // Circular Hough Transform initialization
                    const float circleRadius = static_cast< float >( _houghCircleCriteria );
                    unsigned int houghCircleThresholdVoteCriteria = 1;
                    if ( _houghCircleThresholdVotes )
                    {
                        houghCircleThresholdVoteCriteria = _houghCircleThresholdVotesValue;

                        // LOG
                        printf( "\t - use threshold votes: %d", houghCircleThresholdVoteCriteria );
                    }

                    // Generate the Hough circle accumulator
                    cv::Mat accumulator;
                    if ( _useHoughCircleFixedRadius )
                    {
                         accumulator = hough->generateCircleAccumulator( _localExtrema, circleRadius );

                         // LOG
                         printf( "\t - fixed radius: %f", circleRadius );
                    }
                    else
                    {
                        // LOG
                        printf( "\t - NON-fixed radius: %f", circleRadius );

                        accumulator = hough->generateCircleAccumulator( _localExtrema );
                    }
                    // Visualization
                    if ( _useBinaryDisplay )
                    {
                        cv::imshow( "Hough Accumulator - CIRCLE", algorithm::toBinary( accumulator ) );
                    }
                    else
                    {
                        // 3D attention !!!!
                       // imshow( "Hough Accumulator - CIRCLE", accumulator );
                    }

                    cv::Mat extractedCircles; // BEWARE: not used => OpenCV draw features are used => TODO: fixe that !
                    if ( _useHoughCircleFixedRadius )
                    {
                        // LOG
                        printf( "\t - Extract circles - with fixed radius: %f", circleRadius );

                        extractedCircles = hough->extractCirclesFromAccumulator( accumulator, circleRadius, houghCircleThresholdVoteCriteria, image.rows, image.cols );
                    }
                    else
                    {
                        // LOG
                        printf( "\t - Extract circles - with NON-fixed radius: %f" );

                        extractedCircles = hough->extractCirclesFromAccumulator( accumulator, houghCircleThresholdVoteCriteria, image.rows, image.cols );
                    }

                    timer.stopEvent( houghCircleDetectionEvent );
                    houghCircleDetectionTime += timer.getEventDuration( houghCircleDetectionEvent );

                    // Visualization
                    //cv::imshow( "Limited Hough Transform: CIRCLE detection", affiche );
                }
            }

            // Edge management
            if ( _useEdgeExtraction )
            {
                // LOG
                cout << "\nApply EDGE EXTRACTION" << endl;

                // Extract edges
                timer.startEvent( edgeExtractionEvent );
                std::vector< algorithm::Edge > listEdges = algorithm::freemanEncoding( _localExtrema );
                timer.stopEvent( edgeExtractionEvent );
                edgeExtractionTime += timer.getEventDuration( edgeExtractionEvent );

                // LOG
                cout << "- extracted edges: " << listEdges.size() << endl;

                // Visualization
                //if ( _visualizeEdges )
                //{
                    // Close edges/contours
                    timer.startEvent( edgeExtractionEvent );
                    _edges = algorithm::traceEdges( listEdges, image.rows, image.cols );
                    timer.stopEvent( edgeExtractionEvent );
                    edgeExtractionTime += timer.getEventDuration( edgeExtractionEvent );

                    // Visualize
                    cv::imshow( "Edge Extraction", _edges );
                //}

                if ( _useEdgeClosure )
                {
                    // LOG
                    cout << "\nApply EDGE CLOSURE" << endl;

                    // Close contours
                    algorithm::edgesClosure(listEdges, _localExtrema, _pente, _edgeClosureNbIterations );

                    // Close edges/contours
                    timer.startEvent( edgeClosureEvent );
                    _edges = algorithm::traceEdges( listEdges, image.rows, image.cols );
                    timer.stopEvent( edgeClosureEvent );
                    edgeClosureTime += timer.getEventDuration( edgeClosureEvent );

                    // Visualize
                    cv::imshow( "Closed Edges", _edges );
                }
            }
        }
        else
        {
            timer.startEvent( gradientEvent );
            _pente = algorithm::pente( _gradient, NbDirection, _module );
            timer.stopEvent( gradientEvent );
            gradientTime += timer.getEventDuration( gradientEvent );
            // Gradient slope visualization
            timer.startEvent( gradientEvent );
            _penteColor = algorithm::colorMat( _pente, _module );
            timer.stopEvent( gradientEvent );
            gradientTime += timer.getEventDuration( gradientEvent );
            algorithm::displayMat( "Gradient - Slope", _penteColor, false);
        }

    }

    // TIMER stop
    timer.stopEvent( processEvent );
    processTime = timer.getEventDuration( processEvent );
    cout << "\nPIPELINE process time: " << processTime << " ms" << endl;
    cout << "- gradient             : " << gradientTime << " ms" << " - " << ( ( gradientTime / processTime ) * 100.0f ) << " %" << endl;
    cout << "- threshold            : " << thresholdTime << " ms" << " - " << ( ( thresholdTime / processTime ) * 100.0f ) << " %" << endl;
    cout << "- local extrema        : " << localExtremaTime << " ms" << " - " << ( ( localExtremaTime / processTime ) * 100.0f ) << " %" << endl;
    cout << "- edge extraction      : " << edgeExtractionTime << " ms" << " - " << ( ( edgeExtractionTime / processTime ) * 100.0f ) << " %" << endl;
    cout << "- edge closure         : " << edgeClosureTime << " ms" << " - " << ( ( edgeClosureTime / processTime ) * 100.0f ) << " %" << endl;
    cout << "- Hough (segment)      : " << houghSegmentDetectionTime << " ms" << " - " << ( ( houghSegmentDetectionTime / processTime ) * 100.0f ) << " %" << endl;
    cout << "- Hough (circle)       : " << houghCircleDetectionTime << " ms" << " - " << ( ( houghCircleDetectionTime / processTime ) * 100.0f ) << " %" << endl;

    // Visualization
    // Destroy temporary object
    image.~Mat();

    // LOG
    cout << "---------------------------------------------------------------------------" << endl;

    return true;
}

/******************************************************************************
 * Get the image filename
 *
 * @return image filename
 ******************************************************************************/
const char* Pipeline::getImageFilename() const
{
    return _imageFilename.c_str();
}

/******************************************************************************
 * @brief editKernel
 * @return
 ******************************************************************************/
void Pipeline::setImageFilename( const char* pFilename )
{
    _imageFilename = pFilename;
}

/******************************************************************************
 * Initialize filters
 *
 * @return a flag telling whether or not it succeeds
 ******************************************************************************/
bool Pipeline::initializeFilters()
{
    /*Filter* filter = new Filter();
    string filterPath = _applicationPath + "/" + "Filters" + "/" + "prewittFilter.txt";
    filter->load( filterPath.c_str() );
    filter->print();
    filter->editKernel();
    delete filter;
    filter = NULL;*/

    if ( _filterKernelType == 0 )
    {
        //Prewitt
        std::cout << "Used kernel: Prewitt" << std::endl;
        _matKernel = ( cv::Mat_< float >( 3, 3 ) << -1, 0, 1, -1, 0, 1, -1, 0, 1 );
    }
    else if ( _filterKernelType == 1 )
    {
        //Sobel
        std::cout << "Used kernel: Sobel" << std::endl;
        _matKernel = ( cv::Mat_< float >( 3, 3 ) << -1, 0, 1, -2, 0, 2, -1, 0, 1 );
    }
    else if ( _filterKernelType == 2 )
    {
        //Kirsh
        std::cout << "Used kernel: Kirsch" << std::endl;
        _matKernel = ( cv::Mat_< float >( 3, 3 ) << -3, -3, 5, -3, 0, 5, -3, -3, 5 );
    }
    else
    {
        std::cout << "ERROR - check used kernel" << std::endl;

        return false;
    }

    //Filter* filter = new Filter();
    //string filterPath = _applicationPath + "/" + "Filters" + "/" + "prewittFilter.txt";
    //filter->load( filterPath.c_str() );
    //filter->print();
    //MatKernel = filter->editKernel();
    //delete filter;
    //filter = NULL;

    //cv::Mat gradient[ NbDirection ];

    //cv::Mat kernelDirection[ NbDirection ];
    if( _filterKernelType == 0 )
    {  //Prewitt

        //Kernel Prewitt pour ->
        _kernelDirection[0] = (cv::Mat_<float>(3,3) << -1, 0, 1, -1, 0, 1, -1, 0, 1);
        //Kernel Prewitt pour |^
        _kernelDirection[1] = (cv::Mat_<float>(3,3) << 1, 1, 1, 0, 0, 0, -1, -1, -1);

        // Multidirectional filtering
        //if ( NbDirection == 4 )
        if ( _isMultidirectional )
        {
            //Kernel Prewitt pour />                                                                                                                                        .
            _kernelDirection[2] = (cv::Mat_<float>(3,3) << 0, 1, 1, -1, 0, 1, -1, -1, 0);
            //Kernel Prewitt pour <\                                                        .
            _kernelDirection[3] = (cv::Mat_<float>(3,3) << 1, 1, 0, 1, 0, -1, 0, -1, -1);
        }

    }
    else if ( _filterKernelType == 1 )
    {  //Sobel

        //Kernel Sobel pour ->
        _kernelDirection[0] = (cv::Mat_<float>(3,3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
        //Kernel Sobel pour |^
        _kernelDirection[1] = (cv::Mat_<float>(3,3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);

        // Multidirectional filtering
        //if ( NbDirection == 4 )
        if ( _isMultidirectional )
        {
            //Kernel Sobel pour />                                                                                                                                        .
            _kernelDirection[2] = (cv::Mat_<float>(3,3) << 0, 1, 2, -1, 0, 1, -2, -1, 0);
            //Kernel Sobel pour <\                                                        .
            _kernelDirection[3] = (cv::Mat_<float>(3,3) << 2, 1, 0, 1, 0, -1, 0, -1, -2);
        }

    }
    else if ( _filterKernelType == 2 )
    {  //Kirsh

        //Kernel Kirsch pour ->
        _kernelDirection[0] = (cv::Mat_<float>(3,3) << -3, -3, 5, -3, 0, 5, -3, -3, 5);
        //Kernel Kirsch pour |^
        _kernelDirection[1] = (cv::Mat_<float>(3,3) << 5, 5, 5, -3, 0, -3, -3, -3, -3);

        // Multidirectional filtering
        //if ( NbDirection == 4 )
        if ( _isMultidirectional )
        {
            //Kernel Kirsch pour />
            _kernelDirection[2] = (cv::Mat_<float>(3,3) << -3, 5, 5, -3, 0, 5, -3, -3, -3);
            //Kernel Kirsch pour <\                                                        .
            _kernelDirection[3] = (cv::Mat_<float>(3,3) << 5, 5, -3, 5, 0, -3, -3, -3, -3);
        }
    }

    return true;
}

/******************************************************************************
 * Finalize filters
 *
 * @return a flag telling whether or not it succeeds
 ******************************************************************************/
bool Pipeline::finalizeFilters()
{
    _matKernel.~Mat();
    const int nbDirections = _isMultidirectional ? 4 : 2;
    for ( int i = 0; i < nbDirections; i++ )
    {
        _gradient[ i ].~Mat();
        _kernelDirection[ i ].~Mat();
    }
    _module.~Mat();
    _moduleThreshold.~Mat();
    _pente.~Mat();
    _penteColor.~Mat();
    _localExtrema.~Mat();
    _edges.~Mat();

    return true;
}

/******************************************************************************
 * @brief getKernel
 * @return
 ******************************************************************************/
const cv::Mat& Pipeline::getGradientModule() const
{
    return _gradientModule;
}

/******************************************************************************
 * @brief editKernel
 * @return
 ******************************************************************************/
cv::Mat& Pipeline::editGradientModule()
{
    return _gradientModule;
}

/******************************************************************************
 * @brief getKernel
 * @return
 ******************************************************************************/
const cv::Mat& Pipeline::getGradientSlope() const
{
    return _gradientSlope;
}

/******************************************************************************
 * @brief editKernel
 * @return
 ******************************************************************************/
cv::Mat& Pipeline::editGradientSlope()
{
    return _gradientSlope;
}

/******************************************************************************
 *
 ******************************************************************************/
Pipeline::FilterKernelType Pipeline::getFilterKernelType() const
{
    return _filterKernelType;
}

/******************************************************************************
 *
 ******************************************************************************/
void Pipeline::setFilterKernelType( FilterKernelType pValue )
{
    _filterKernelType = pValue;
}

/******************************************************************************
 *
 ******************************************************************************/
Pipeline::DirectionalFilteringType Pipeline::getDirectionalFilteringType() const
{
    return _directionalFilteringType;
}

/******************************************************************************
 *
 ******************************************************************************/
void Pipeline::setDirectionalFilteringType( DirectionalFilteringType pValue )
{
    _directionalFilteringType = pValue;

    _isMultidirectional = ( pValue == eMultidirectional );
}

/******************************************************************************
 *
 ******************************************************************************/
Pipeline::NormType Pipeline::getNormType() const
{
    return _normType;
}

/******************************************************************************
 *
 ******************************************************************************/
void Pipeline::setNormType( NormType pValue )
{
    _normType = pValue;
}

/******************************************************************************
 *
 ******************************************************************************/
Pipeline::ThresholdType Pipeline::getThresholdType() const
{
    return _thresholdType;
}

/******************************************************************************
 *
 ******************************************************************************/
void Pipeline::setThresholdType( ThresholdType pValue )
{
    _thresholdType = pValue;
}

/******************************************************************************
 *
 ******************************************************************************/
bool Pipeline::isImageVisible() const
{
    return _visualizeImage;
}

/******************************************************************************
 *
 ******************************************************************************/
void Pipeline::setImageVisible( bool pFlag )
{
    _visualizeImage = pFlag;
}

/******************************************************************************
 *
 ******************************************************************************/
bool Pipeline::isGradientVisible() const
{
    return _visualizeGradient;
}

/******************************************************************************
 *
 ******************************************************************************/
void Pipeline::setGradientVisible( bool pFlag )
{
    _visualizeGradient = pFlag;
}

/******************************************************************************
 * Set the flag telling whether or not to use gradient
 *
 * @return a flag telling whether or not to use gradient
 ******************************************************************************/
void Pipeline::setUseGradient( bool pFlag )
{
    _useGradient = pFlag;
}

/******************************************************************************
 * Set the flag telling whether or not to use threshold
 *
 * @return a flag telling whether or not to use threshold
 ******************************************************************************/
void Pipeline::setUseThreshold( bool pFlag )
{
    _useThreshold = pFlag;
}

/******************************************************************************
 * Set the flag telling whether or not to use local extrema
 *
 * @return a flag telling whether or not to use local extrema
 ******************************************************************************/
void Pipeline::setUseLocalExtrema( bool pFlag )
{
    _useLocalExtrema = pFlag;
}

/******************************************************************************
 * Set the flag telling whether or not to use gradient
 *
 * @return a flag telling whether or not to use gradient
 ******************************************************************************/
void Pipeline::setUseEdgeExtraction( bool pFlag )
{
    _useEdgeExtraction = pFlag;
}

/******************************************************************************
 * Set the flag telling whether or not to use gradient
 *
 * @return a flag telling whether or not to use gradient
 ******************************************************************************/
void Pipeline::setUseEdgeClosure( bool pFlag )
{
    _useEdgeClosure = pFlag;
}

/******************************************************************************
 *
 ******************************************************************************/
int Pipeline::getGlobalThresholdValidPixelPercentage() const
{
    return _globalThresholdValidPixelPercentage;
}

/******************************************************************************
 *
 ******************************************************************************/
void Pipeline::setGlobalThresholdValidPixelPercentage( int pValue )
{
    _globalThresholdValidPixelPercentage = pValue;
}

/******************************************************************************
 *
 ******************************************************************************/
int Pipeline::getGlobalThresholdValue() const
{
    return _globalThresholdValue;
}

/******************************************************************************
 *
 ******************************************************************************/
int Pipeline::getLocalThresholdWindowSize() const
{
    return _localThresholdWindowSize;
}

/******************************************************************************
 *
 ******************************************************************************/
void Pipeline::setLocalThresholdWindowSize( int pValue )
{
    _localThresholdWindowSize = pValue;
}

/******************************************************************************
 *
 ******************************************************************************/
int Pipeline::getHysteresisThresholdHighValidPixelPercentage() const
{
    return _hysteresisThresholdHighValidPixelPercentage;
}

/******************************************************************************
 *
 ******************************************************************************/
void Pipeline::setHysteresisThresholdHighValidPixelPercentage( int pValue )
{
    _hysteresisThresholdHighValidPixelPercentage = pValue;
}

/******************************************************************************
 *
 ******************************************************************************/
int Pipeline::getHysteresisThresholdHighValue() const
{
    return _hysteresisThresholdHighValue;
}

/******************************************************************************
 *
 ******************************************************************************/
int Pipeline::getHysteresisThresholdLowValidPixelPercentage() const
{
    return _hysteresisThresholdLowValidPixelPercentage;
}

/******************************************************************************
 *
 ******************************************************************************/
void Pipeline::setHysteresisThresholdLowValidPixelPercentage( int pValue )
{
    _hysteresisThresholdLowValidPixelPercentage = pValue;
}

/******************************************************************************
 *
 ******************************************************************************/
int Pipeline::getHysteresisThresholdLowValue() const
{
    return _hysteresisThresholdLowValue;
}

/******************************************************************************
 * Get the flag to tell whether or not to display images in binary mode
 *
 * @return the flag to tell whether or not to display images in binary mode
 ******************************************************************************/
bool Pipeline::useBinaryDisplay() const
{
    return _useBinaryDisplay;
}

/******************************************************************************
 * Set the flag to tell whether or not to display images in binary mode
 *
 * @param pFlag the flag to tell whether or not to display images in binary mode
 ******************************************************************************/
void Pipeline::setUseBinaryDisplay( bool pFlag )
{
    _useBinaryDisplay = pFlag;
}

/******************************************************************************
 * Get the number of iterations used during the Edge Closure algorithm
 *
 * @return the number of iterations used during the Edge Closure algorithm
 ******************************************************************************/
unsigned int Pipeline::getEdgeClosureNbIterations() const
{
    return _edgeClosureNbIterations;
}

/******************************************************************************
 * Set the number of iterations used during the Edge Closure algorithm
 *
 * @param pValue the number of iterations used during the Edge Closure algorithm
 ******************************************************************************/
void Pipeline::setEdgeClosureNbIterations( unsigned int pValue )
{
    _edgeClosureNbIterations = pValue;
}

/******************************************************************************
 * Set the flag telling whether or not the Hough Transform for segment detection is activated
 *
 * @param pFlag the flag telling whether or not the Hough Transform for segment detection is activated
 ******************************************************************************/
void Pipeline::setHoughSegmentDetection( bool pFlag )
{
    _useHoughSegmentDetection = pFlag;
}

/******************************************************************************
 * Set the Hough Transform criteria for segment detection, i.e. min number of points to determine a segment
 *
 * @param pValue the Hough Transform criteria for segment detection, i.e. min number of points to determine a segment
 ******************************************************************************/
void Pipeline::setHoughSegmentCriteria( unsigned int pValue )
{
    _houghSegmentCriteria = pValue;
}

/******************************************************************************
 * Set the flag telling whether or not the Hough Transform for circle detection is activated
 *
 * @param pFlag the flag telling whether or not the Hough Transform for circle detection is activated
 ******************************************************************************/
void Pipeline::setHoughCircleDetection( bool pFlag )
{
    _useHoughCircleDetection = pFlag;
}

/******************************************************************************
 * Set the Hough Transform criteria for circle detection, i.e. min number of points to determine a circle
 *
 * @param pValue the Hough Transform criteria for circle detection, i.e. min number of points to determine a circle
 ******************************************************************************/
void Pipeline::setHoughCircleCriteria( unsigned int pValue )
{
    _houghCircleCriteria = pValue;
}

void Pipeline::setHoughSegmentThreshold( bool pFlag )
{
    _houghSegmentThreshold = pFlag;
}

void Pipeline::setHoughFollowGradientDirection( bool pFlag )
{
     _houghFollowGradientDirection = pFlag;
}

void Pipeline::setHoughCircleThresholdVotes( bool pFlag )
{
     _houghCircleThresholdVotes = pFlag;
}

void Pipeline::setHoughCircleThresholdVotesValue( unsigned int pValue )
{
     _houghCircleThresholdVotesValue = pValue;
}

void Pipeline::setHoughCircleUseFixedRadius( bool pFlag )
{
     _useHoughCircleFixedRadius = pFlag;
}
