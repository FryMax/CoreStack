#pragma once

#include "../shared_in.h"
#include "opencv2/cvconfig.h"
#include "opencv2/opencv_modules.hpp"

#include "opencv2/opencv.hpp"
#include "opencv2/features2d.hpp"
//#include "opencv2/xfeatures2d.hpp"


namespace addon_opencv 
{


	using namespace std;
	using namespace cv;
	//using namespace cv::xfeatures2d;

	const int MAX_FEATURES = 500;
	const float GOOD_MATCH_PERCENT = 0.15f;


	void alignImages(Mat &im1, Mat &im2, Mat &im1Reg, Mat &h)
	{
		// Convert images to grayscale
		Mat im1Gray, im2Gray;
		cvtColor(im1, im1Gray, CV_BGR2GRAY);
		cvtColor(im2, im2Gray, CV_BGR2GRAY);

		// Variables to store keypoints and descriptors
		std::vector<KeyPoint> keypoints1, keypoints2;
		Mat descriptors1, descriptors2;

		// Detect ORB features and compute descriptors.
		Ptr<Feature2D> orb = ORB::create(MAX_FEATURES);
		orb->detectAndCompute(im1Gray, Mat(), keypoints1, descriptors1);
		orb->detectAndCompute(im2Gray, Mat(), keypoints2, descriptors2);

		// Match features.
		std::vector<DMatch> matches;
		Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");
		matcher->match(descriptors1, descriptors2, matches, Mat());

		// Sort matches by score
		std::sort(matches.begin(), matches.end());

		// Remove not so good matches
		const int numGoodMatches = matches.size() * GOOD_MATCH_PERCENT;
		matches.erase(matches.begin() + numGoodMatches, matches.end());


		// Draw top matches
		Mat imMatches;
		drawMatches(im1, keypoints1, im2, keypoints2, matches, imMatches);
		imwrite("matches.jpg", imMatches);


		// Extract location of good matches
		std::vector<Point2f> points1, points2;

		for (size_t i = 0; i < matches.size(); i++)
		{
			points1.push_back(keypoints1[matches[i].queryIdx].pt);
			points2.push_back(keypoints2[matches[i].trainIdx].pt);
		}

		// Find homography
		h = findHomography(points1, points2, RANSAC);

		// Use homography to warp image
		warpPerspective(im1, im1Reg, h, im2.size());

	}


	int cv_main()
	{
		// Read reference image
		string refFilename("G:/ram/car.jpg");
		cout << "Reading reference image : " << refFilename << endl;
		Mat imReference = imread(refFilename);


		// Read image to be aligned
		string imFilename("G:/ram/num.jpg");
		cout << "Reading image to align : " << imFilename << endl;
		Mat im = imread(imFilename);


		// Registered image will be resotred in imReg. 
		// The estimated homography will be stored in h. 
		Mat imReg, h;

		// Align images
		cout << "Aligning images ..." << endl;
		alignImages(im, imReference, imReg, h);

		// Write aligned image to disk. 
		string outFilename("G:/ram/aligned.jpg");
		cout << "Saving aligned image : " << outFilename << endl;
		imwrite(outFilename, imReg);

		// Print estimated homography
		cout << "Estimated homography : \n" << h << endl;

		return 0;
	}




	class VideoCacher
	{
	protected:
		cv::VideoCapture sourse;
		std::thread  cache_thread;
		atomic<bool> cache_loop = false;

		enum { cache_size = 2000 };

		std::array <cv::Mat, cache_size> cache;
		std::array <bool, cache_size> cache_state;

		atomic<int>  next_cache_frame_id = 0;
		atomic<int>  next_get_frame_id = 0;
		atomic<int>  frames_ready = 0;

	public:

		int vd_curent_frame = 0;
		int vd_frame_count = 0;
		INT2 vd_sourse_resolution;
		INT2 vd_output_resolution;

	public:

		void CaptureSourse(cv::VideoCapture _vdc, INT2 resize_output = { 0,0 })
		{
			sourse = _vdc;
			vd_sourse_resolution = {
				(int)sourse.get(CV_CAP_PROP_FRAME_WIDTH),
				(int)sourse.get(CV_CAP_PROP_FRAME_HEIGHT)
			};

			vd_curent_frame = 0;
			vd_frame_count = sourse.get(CV_CAP_PROP_FRAME_COUNT);

			if (math::equal(resize_output, { 0,0 }))
				vd_output_resolution = vd_sourse_resolution;
			else
				vd_output_resolution = resize_output;

			start_cache_process();
		}

		//=================================// read frame
		int GetReadyFramesCount() 
		{
			return frames_ready;
		}

		result<cv::Mat> GetNextFrame()
		{
			if (next_get_frame_id >= cache_size)
				next_get_frame_id = 0;

			//while (frames_ready <= 0)
			//	std::this_thread::sleep_for(std::chrono::milliseconds(10));

			if (cache_state[next_get_frame_id] == true)
			{
				cv::Mat out;

				out = cache[next_get_frame_id];
				cache_state[next_get_frame_id] = false;

				frames_ready--;
				next_get_frame_id++;
				vd_curent_frame++;

				//log::debug << str::format::insert("Frame get {}", vd_curent_frame);

				return out;
			}

			return error_result("Cache not ready / end of stream");
		}
		void GoToFrame(int frame)
		{
			vd_curent_frame = frame;
			frames_ready = 0;

			stop_cache_process();

			sourse.set(CV_CAP_PROP_POS_FRAMES, frame);

			start_cache_process();
		}

	public:

		void stop_cache_process()
		{
			cache_loop = false;

			while (cache_thread.joinable())
				cache_thread.join();
		}
		void start_cache_process()
		{
			stop_cache_process();

			if (cache_loop != true)
			{
				cache_loop = true;
				cache_thread = std::thread(&VideoCacher::_cache_thread, this);
				cache_thread.detach();
			}
		}
		void _cache_thread()
		{
			while (cache_loop)
			{
				if (frames_ready < cache_size)
				{
					cv::Mat frame;
					sourse >> frame;

					if (!frame.empty())
					{
						auto res_size = cv::Size(vd_output_resolution.x, vd_output_resolution.y);

						if (vd_sourse_resolution != vd_output_resolution)
							cv::resize(frame,	// input
								frame,			// output
								res_size,		// output size
								INTER_LINEAR);  // filter

						// return to cache start
						if (next_cache_frame_id >= cache_size)
							next_cache_frame_id = 0;

						cache[next_cache_frame_id] = frame;
						cache_state[next_cache_frame_id] = true;

						next_cache_frame_id++;
						frames_ready++;
					}
					else
						cache_loop = false;
				}
				else // (cache filled)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}
			}
		}
	};

}