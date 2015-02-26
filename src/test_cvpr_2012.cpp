/** ****************************************************************************
 *  @file    test_cvpr_2012.cpp
 *  @brief   Real-time facial pose and feature detection
 *  @author  Roberto Valle Fernandez
 *  @date    2012/01
 *  @copyright All rights reserved.
 *  Software developed by UPM PCR Group: http://www.dia.fi.upm.es/~pcr
 ******************************************************************************/

// ----------------------- INCLUDES --------------------------------------------
#include <trace.hpp>
#include <tree_node.hpp>
#include <face_forest.hpp>
#include <face_utils.hpp>

#include <opencv2/highgui.hpp>
#include <boost/filesystem.hpp>

const double IMG_INPUT_WIDTH  = 640.0;
const double IMG_INPUT_HEIGHT = 480.0;

// -----------------------------------------------------------------------------
//
// Purpose and Method:
// Inputs:
// Outputs:
// Dependencies:
// Restrictions and Caveats:
//
// -----------------------------------------------------------------------------
int
main
  (
  int argc,
  char **argv
  )
{
  // ---------------------------------------------------------------------------
  // Determine if we get the images from a camera, a video, or a set of images
  // ---------------------------------------------------------------------------
  cv::Mat frame;
  cv::VideoCapture capture;
  bool process_image_file    = false;
  bool process_video_capture = false;

  boost::filesystem::path dir(argv[1]);
  if (boost::filesystem::exists(dir) && (boost::filesystem::is_regular_file(dir)))
  {
    frame = cv::imread(dir.c_str(), cv::IMREAD_COLOR);
    if (!frame.empty()) // Trying image file ...
    {
      PRINT("Processing an image file ...");
      process_image_file = true;
    }
    else // Trying video file ...
    {
      PRINT("Capturing from AVI file ...");
      capture.open(dir.string());
      if (!capture.isOpened())
      {
        ERROR("Could not grab images from AVI file");
        return EXIT_FAILURE;
      }
      process_video_capture = true;
    }
  }
  else
  {
    PRINT("Capturing from camera ...");
    capture.open(0);
    capture.set(cv::CAP_PROP_FRAME_WIDTH, IMG_INPUT_WIDTH);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, IMG_INPUT_HEIGHT);
    if (!capture.isOpened())
    {
      ERROR("Could not grab images from camera");
      return EXIT_FAILURE;
    }
    process_video_capture = true;
  }

  if (process_image_file)
  {
    // Convert to grayscale
    cv::Mat frame_gray;
    cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);

    // Initialize face forest
    ForestParam hp_param, mp_param;
    CV_Assert(loadConfigFile("data/config_headpose.txt", hp_param));
    CV_Assert(loadConfigFile("data/config_ffd.txt", mp_param));

    FaceForestOptions ff_options;
    ff_options.face_detection_option.path_face_cascade = "data/haarcascade_frontalface_alt.xml";
    ff_options.head_pose_forest_param = hp_param;
    ff_options.mp_forest_param = mp_param;

    FaceForest ff(ff_options);

    std::vector<Face> faces;
    //ff.analize_image(frame_gray, faces);
    Face face;
    FaceAnnotation annotation;
    annotation.bbox.x = 53;
    annotation.bbox.y = 77;
    annotation.bbox.width = 123;
    annotation.bbox.height = 123;
    ff.analize_face(frame_gray, annotation.bbox, face);
    faces.push_back(face);

    // Draw results
    ff.show_results(frame, faces);
  }

  return EXIT_SUCCESS;
};