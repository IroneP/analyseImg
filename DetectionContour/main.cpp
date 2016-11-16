/*
 * Image processing : edge detection
 *
 * Authors : Pascal Guehl, Clement Picq
 */

/** 
 * @version 1.0
 */

 /******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Qt
#include <QCoreApplication>

// System
#include <cstdio>
#include <iostream>

// OpenCV
//#include <cv.h>
//#include <highgui.h>

// Project
#include "Algorithm.h"
#include "MainWindow.h"
#include "Pipeline.h"

// Qt
#include <QApplication>

/******************************************************************************
 ****************************** NAMESPACE SECTION *****************************
 ******************************************************************************/

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ***************************** METHOD DEFINITION ******************************
 ******************************************************************************/

/******************************************************************************
 * Main entry program
 *
 * @param pArgc number of arguments
 * @param pArgv list of arguments
 *!=
 * @return exit code
 ******************************************************************************/
int main( int pArgc, char** pArgv )
{
    // Qt application
    QApplication application( pArgc, pArgv );

    // Initialize a pipeline
    Pipeline::setApplicationPath( application.applicationDirPath().toLatin1().constData() );
    Pipeline* pipeline = new Pipeline();

    // Main window
    MainWindow w;
    w.setPipeline( pipeline );
    w.show();

    // Qt main event loop
    int status = application.exec();

    // Free OpenCV resources
    cv::destroyAllWindows();

    // Exit program
    return status;
    //return EXIT_SUCCESS;
}
