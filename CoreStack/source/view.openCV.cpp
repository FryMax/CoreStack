#pragma once
#include "pch.h"
#include "imview.h"

#if false//(PLATFORM_ARCH == __arch_x64bit)

	#define ADDON_CV_ENABLED
	#include "opencv2/cvconfig.h"
	#include "opencv2/opencv_modules.hpp"
	#include "opencv2/opencv.hpp"
	#include "opencv2/features2d.hpp"
	//#include "opencv2/xfeatures2d.hpp"

#endif

#ifdef  ADDON_CV_ENABLED

namespace
{
	using namespace std;
	using namespace cv;
	//using namespace cv::xfeatures2d;

	const int MAX_FEATURES = 500;
	const float GOOD_MATCH_PERCENT = 0.15f;

	inline void alignImages(Mat& im1, Mat& im2, Mat& im1Reg, Mat& h)
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
	inline int cv_main()
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
	struct ImageTracker
	{
		int nearest_colors[256];
		vector<INT2> color_points[256];

		vector<INT2> _find_nearest_color(int color)
		{
			uint8_t c = nearest_colors[color];

			auto same_color_points = vector<INT2>(color_points[c]);

			return same_color_points;
		}

		void SetTrack(bitmap32& bitmap)
		{
			original_track = bitmap;

			ForArray(i, 255)
				nearest_colors[i] = -1;

			ForArray(y, original_track.size().y)
				ForArray(x, original_track.size().x)
			{
				auto p = original_track.get_pixel(x, y);

				nearest_colors[p.r] = p.r;
			}
			//log::debug << str::format::insert("Generated {} 3-keys", key3_toind.size());

			ForArray(i, 255)
			{
				if (nearest_colors[i] == -1)
				{
					uint8_t col = i;
					int col_up = i + 1;
					int col_low = i - 1;
					while (nearest_colors[i] == -1)
					{
						if (col_up > 255)
							col_up = 255;
						if (col_low < 0)
							col_low = 0;
						if (nearest_colors[col_up] != -1)
							nearest_colors[i] = col_up;
						else
							if (nearest_colors[col_low] != -1)
								nearest_colors[i] = col_low;
							else
							{
								col_up++;
								col_low--;
							}
					}
				}
			}

			ForArray(y, original_track.size().y)
				ForArray(x, original_track.size().x)
			{
				auto p = original_track.get_pixel(x, y);

				color_points[nearest_colors[p.r]].push_back(INT2{ (int)x, (int)y });
			}


		}
		INT2 FindObject(bitmap32& bitmap)
		{
			if (false && "cv")
			{
				INT2 sizeA = original_track.size();
				INT2 sizeB = bitmap.size();

				cv::Mat orig(sizeA.y, sizeA.x, CV_8UC3, cv::Scalar(255, 0, 0));
				cv::Mat find(sizeB.y, sizeB.x, CV_8UC3, cv::Scalar(255, 0, 0));

				ForArray(y, sizeA.y)
					ForArray(x, sizeA.x)
				{
					auto* sourse = original_track.raw_data(x, y);
					uchar* dest = orig.ptr(y, x);

					*(dest + 0) = sourse->b;
					*(dest + 1) = sourse->g;
					*(dest + 2) = sourse->r;
				}
				ForArray(y, sizeB.y)
					ForArray(x, sizeB.x)
				{
					auto* sourse = bitmap.raw_data(x, y);
					uchar* dest = find.ptr(y, x);

					*(dest + 0) = sourse->b;
					*(dest + 1) = sourse->g;
					*(dest + 2) = sourse->r;
				}

				//cv::imshow("orig", orig);
				//cv::imshow("find", find);

				//cv::Mat imReg, h;
				//cv_tests::alignImages(find, orig, imReg, h);
				/////////////////////////////////

				//////////////////////////////////////////////////////////////////////////
				// Convert images to grayscale
				cv::Mat im1Gray, im2Gray;
				cv::cvtColor(find, im1Gray, CV_BGR2GRAY);
				cv::cvtColor(orig, im2Gray, CV_BGR2GRAY);


				// Variables to store keypoints and descriptors
				std::vector<cv::KeyPoint> keypoints1, keypoints2;	// Стабильные тошки
				cv::Mat descriptors1, descriptors2;					// Дата тошек

				constexpr int	 MAX_FEATURES = 100;
				constexpr double GOOD_MATCH_PERCENT = 0.80;
				const	  string cv_dir = "D:\\_ram\\cv\\";

				// Detect ORB features and compute descriptors.
				cv::Ptr<cv::Feature2D> orb = cv::ORB::create(MAX_FEATURES);
				//orb->detectAndCompute(im1Gray, Mat(), keypoints1, descriptors1);
				//orb->detectAndCompute(im2Gray, Mat(), keypoints2, descriptors2);
				try
				{
					orb->detect(im1Gray, keypoints1, cv::Mat());
					orb->compute(im1Gray, keypoints1, descriptors1);

					orb->detect(im2Gray, keypoints2, cv::Mat());
					orb->compute(im2Gray, keypoints2, descriptors2);
				}
				catch (const std::exception & e)
				{
					string aaa = e.what();
				}
				// Match features.
				std::vector<cv::DMatch> matches;
				cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce-Hamming");
				matcher->match(descriptors1, descriptors2, matches, cv::Mat());

				// Sort matches by score
				std::sort(matches.begin(), matches.end());

				// Remove not so good matches
				const int numGoodMatches = int(matches.size() * GOOD_MATCH_PERCENT);
				matches.erase(matches.begin() + numGoodMatches, matches.end());


				// Draw top matches
				cv::Mat imMatches;
				cv::drawMatches(find, keypoints1, orig, keypoints2, matches, imMatches);
				//cv::imwrite(cv_dir + "3.jpg", imMatches);
				cv::imshow("mach", imMatches);

				// Extract location of good matches
				std::vector<cv::Point2f> points1, points2;

				for (size_t i = 0; i < matches.size(); i++)
				{
					points1.push_back(keypoints1[matches[i].queryIdx].pt);
					points2.push_back(keypoints2[matches[i].trainIdx].pt);
				}
				///
				cv::Mat im1Reg, h;
				// Find homography

				string buffer;

				//ForArray(i, points1.size())
				//{
				//	buffer += str::format::insert(" {}-{}  :  {}-{}  = {}-{}\n",
				//		points1[i].x,
				//		points1[i].y,
				//		points2[i].x,
				//		points2[i].y,
				//		points2[i].x - points1[i].x,
				//		points2[i].y - points1[i].y);
				//}
				//log::debug << string("KeyPoints") + buffer;

				float rare_w = 0;
				float rare_h = 0;

				struct near_fin
				{
					int id;
					FLOAT2 val;
				};

				vector<near_fin> min_near;
				min_near.resize(points1.size());

				ForArray(i, points1.size())
				{
					float xrare = 0;
					float yrare = 0;

					ForArray(j, points1.size())
						xrare += abs(points2[j].x - points1[j].x);

					ForArray(j, points1.size())
						yrare += abs(points2[i].y - points1[i].y);

					min_near[i] = { (int)i, FLOAT2{xrare, yrare } };
				}

				std::sort(min_near.begin(), min_near.end(),
					[](const near_fin& a, const near_fin& b)
					{
						return abs(a.val.x + a.val.y) < abs(b.val.x + b.val.y);
					});


				rare_w = points1[min_near[0].id].x - points2[min_near[0].id].x;
				rare_h = points1[min_near[0].id].y - points2[min_near[0].id].y;


				return { (int)rare_w, (int)rare_h };


				h = cv::findHomography(points1, points2, cv::RANSAC);

				// Use homography to warp image
				cv::warpPerspective(find, im1Reg, h, orig.size());
			}

			//////////////////////////////////////////////////////////////////////////

			float pos_x = 0;
			float pos_y = 0;
			int iters = 0;

			ForArray(y, bitmap.size().y)
				ForArray(x, bitmap.size().x)
			{
				auto p = bitmap.get_pixel(x, y);

				auto points = _find_nearest_color(p.r);

				ForArray(i, points.size())
				{
					pos_x += x + points[i].x;
					pos_y += y + points[i].y;

					iters++;
				}
			}

			log::debug << std::string("color maches:") + std::to_string(iters);

			pos_x = pos_x / iters;
			pos_y = pos_y / iters;

			return { (int)pos_x, (int)pos_y };
		}

	protected:

		bitmap32 original_track;
		bitmap32 dynamic_track;

	};
}

static void imview::openCV()
{
	static cv::VideoCapture vd_capture;
	static     VideoCacher  vd_cacher;

	static bool   vd_is_play = false;
	static string vd_status_str = "";

	//////////////////////////////////////////////////////////////////////////
	static constexpr INT2 LOW_VD_RESOLUTION = {
		1440,
		900,
	};

	ImGui::Separator();
	//====================================// Load
	if (ImGui::Button("Load File"))
	{
		std::string file = ASSET_DIR + "cars.mp4";

		vd_capture.open(file);

		if (vd_capture.isOpened())
			vd_status_str = "[OK] Video stream/file successfully captured";
		else
			vd_status_str = "[FAILED] Error opening video stream/file";

		vd_cacher.CaptureSourse(vd_capture, LOW_VD_RESOLUTION);
	}
	//-----------------------------------------------| Play
	ImGui::SameLine(); if (ImGui::Button("Play")) { vd_is_play = true;  vd_status_str = "Playing..."; }
	ImGui::SameLine(); if (ImGui::Button("Pause")) { vd_is_play = false; vd_status_str = "Paused..."; }
	ImGui::SameLine(); if (ImGui::Button("Replay")) { vd_is_play = true;  vd_status_str = "Playing..."; vd_cacher.GoToFrame(0); }
	ImGui::SameLine(); if (ImGui::Button("Forward")) { vd_is_play = true;  vd_status_str = "Playing..."; vd_cacher.GoToFrame(600 + vd_cacher.vd_frame_count / 2); }
	//-----------------------------------------------| Clear
	ImGui::SameLine(); if (ImGui::Button("Release"))
	{
		vd_is_play = false;
		vd_status_str = "Released...";

		vd_capture.release();
		cv::destroyAllWindows();
	}

	//////////////////////////////////////////////////////////////////////////
	enum class track_state
	{
		ready_for_track_set,
		setting_track_a,
		setting_track_b,
		init_track_bits,

		ready_for_search_call,
		searching,
	};

	static FLOAT2 track_PA = { 0, 0 };
	static FLOAT2 track_PB = { 0, 0 };
	static FLOAT2 search_PA = { 0, 0 };
	static FLOAT2 search_PB = { 0, 0 };
	static FLOAT2 result_PA = { 0, 0 };
	static FLOAT2 result_PB = { 0, 0 };


	static track_state tr_state = track_state::ready_for_track_set;
	static bitmap32 search_img;

	static ImageTracker img_tracker;

	if (ImGui::Button("Track"))
	{
		tr_state = track_state::setting_track_a;
	}

	ImGui::SameLine();
	if (ImGui::Button("NoTrack"))
	{
		tr_state = track_state::ready_for_track_set;
		track_PA = { 0, 0 };
		track_PB = { 0, 0 };
	}

	static GpuTexture gpu_tex;

	if (ImGui::Button("Start tracking process"))
	{
		if (tr_state == track_state::ready_for_search_call)
		{
			tr_state = track_state::searching;
			search_PA = track_PA - FLOAT2(100, 100);//FLOAT2{ math::RandomFloat(10,30), math::RandomFloat(10,30) };
			search_PB = track_PB + FLOAT2(100, 100);//FLOAT2{ math::RandomFloat(10,30), math::RandomFloat(10,30) };
		}
		else
		{
			log::error << "Init track by first";
		}
	}

	//////////////////////////////////////////////////////////////////////////
	ImGui::Text(str::format::insert("Size   | {} / {}", vd_cacher.vd_sourse_resolution.x, vd_cacher.vd_sourse_resolution.y).c_str());
	ImGui::Text(str::format::insert("View   | {} / {}", vd_cacher.vd_output_resolution.x, vd_cacher.vd_output_resolution.y).c_str());
	ImGui::Text(str::format::insert("Frames | {} / {}", vd_cacher.vd_curent_frame, vd_cacher.vd_frame_count).c_str());
	ImGui::Text(str::format::insert("Cache  | {}", vd_cacher.GetReadyFramesCount()).c_str());
	ImGui::Text(str::format::insert("Status | play: {} / {}", vd_is_play, vd_status_str.c_str()).c_str());
	ImGui::Text("Track:");
	ImGui::Text(str::format::insert("Track state | {}", (int)tr_state).c_str());
	ImGui::Text(str::format::insert("Track P1    | {}-{}", track_PA.x, track_PA.y).c_str());
	ImGui::Text(str::format::insert("Track P2    | {}-{}", track_PB.x, track_PB.y).c_str());
	ImGui::Text(str::format::insert("Search P1   | {}-{}", search_PA.x, search_PA.y).c_str());
	ImGui::Text(str::format::insert("Search P2   | {}-{}", search_PB.x, search_PB.y).c_str());
	ImGui::Text(str::format::insert("Result P1   | {}-{}", result_PA.x, result_PA.y).c_str());
	ImGui::Text(str::format::insert("Result P2   | {}-{}", result_PB.x, result_PB.y).c_str());

	static int gpu_show_buffer = 0;
	ImGui::RadioButton("New Frame", &gpu_show_buffer, 0); ImGui::SameLine();
	ImGui::RadioButton("Buck Grownd", &gpu_show_buffer, 1); ImGui::SameLine();
	ImGui::RadioButton("Move Map", &gpu_show_buffer, 2);

	static bitmap32 ram_NewFrame(LOW_VD_RESOLUTION.x, LOW_VD_RESOLUTION.y); // smoth buffer
	static bitmap32 ram_BuckGrownd(LOW_VD_RESOLUTION.x, LOW_VD_RESOLUTION.y);	// smoth buffer
	static bitmap32 ram_MoveMap(LOW_VD_RESOLUTION.x, LOW_VD_RESOLUTION.y); // smoth buffer
	static bitmap32 ram_GpuTex(LOW_VD_RESOLUTION.x, LOW_VD_RESOLUTION.y); // gpu texture

	if (vd_is_play)
	{
		//--------------------------------// fps control
		const  float fps_target = 60;
		static float time_exp = 0;

		bool time_frame = false;
		static SuperTimer ST;

		time_exp += ST.GetRoundTime();

		if (time_exp > 0.999999 / fps_target)
		{
			time_exp = 0;
			time_frame = true;
		}

		//--------------------------------// Get new frame
		if (time_frame)
		{
			auto frame_getter = vd_cacher.GetNextFrame();

			if (frame_getter.IsFailed())
			{
				vd_status_str = frame_getter.GetErrorMsg();
			}
			else
			{
				//----------------------------------------------------// copy data from cv mat to buffer
				auto cv_mat_frame = frame_getter.GetValue();

				const int low_w = LOW_VD_RESOLUTION.x;
				const int low_h = LOW_VD_RESOLUTION.y;

				ForArray(y, low_h)
					ForArray(x, low_w)
				{
					const auto* pSourse = cv_mat_frame.ptr(y, x);
					auto* dest = ram_NewFrame.raw_data(x, y);

					// cv stores colors in BGR. dont ask why
					const uint8_t R = *(pSourse + 2);
					const uint8_t G = *(pSourse + 0);
					const uint8_t B = *(pSourse + 1);

					const uint8_t gray = (R + G + B) / 3; // simplest middle gray color

					dest->r = gray;
					dest->b = gray;
					dest->g = gray;
					dest->a = 255;
				}
				//-----------------------------------------------------//  once set BuckGrownd

				// todo change. sets buck to first frame
				MakeOnce
				{
					ram_BuckGrownd = ram_NewFrame;
				}

					if (true && "gray maps")
					{
						ForArray(y, low_h)
							ForArray(x, low_w)
						{
							//-----------------------------------------------------// buckgr buffer update

							auto* pColorOld = ram_BuckGrownd.raw_data(x, y);
							auto* pColorNew = ram_NewFrame.raw_data(x, y);

							const uint8_t c_old = pColorOld->r;
							const uint8_t c_new = pColorNew->r;

							const float  diff = (c_new - c_old) / 200.f;
							const int8_t sign = (diff > 0.f) ? 1 : -1;

							const int8_t c_buck = sign * (math::max(1.0f, abs(diff)));

							pColorOld->r += c_buck;
							pColorOld->g += c_buck;
							pColorOld->b += c_buck;

							//-----------------------------------------------------// move map gen

							auto* pColorMove = ram_MoveMap.raw_data(x, y);

							const int c_dec = pColorNew->r - pColorOld->r;
							const int c_mod = abs(c_dec);

							//int8_t g_dec = pColorNew->g - pColorOld->g;
							//int8_t b_dec = pColorNew->b - pColorOld->b;
							//int8_t g_mod = abs(g_dec);
							//int8_t b_mod = abs(b_dec);

							const int mod = c_dec;        //r_mod + g_mod + b_mod; // summ of color chanels change
							const int eps = 255.0 / 9.0; // *3.0;                 // color change to 10% total (3 * ~10% per chanel)

							// map cool
							const int cold_val = 255.0 / 6.0;
							pColorMove->r = math::max(0, int(pColorMove->r) - cold_val);

							if (mod > eps)
							{
								pColorMove->r = math::min(255, int(pColorMove->r + 256 / 2));
								pColorMove->g = 0;
								pColorMove->b = 0;
								pColorMove->a = 255;
							}

						}

					}
				if (false && "rgb maps")
				{
					ForArray(y, low_h)
						ForArray(x, low_w)
					{
						//-----------------------------------------------------// buckgr buffer update

						auto* cur_c = ram_BuckGrownd.raw_data(x, y);
						auto* new_c = ram_NewFrame.raw_data(x, y);

						float diff_r = (new_c->r - cur_c->r) / 100.;
						float diff_g = (new_c->g - cur_c->g) / 100.;
						float diff_b = (new_c->b - cur_c->b) / 100.;

						int8_t sign_r = (diff_r > 0) ? 1 : -1;
						int8_t sign_g = (diff_g > 0) ? 1 : -1;
						int8_t sign_b = (diff_b > 0) ? 1 : -1;

						cur_c->r += sign_r * (math::max(1.f, abs(diff_r)));
						cur_c->g += sign_g * (math::max(1.f, abs(diff_g)));
						cur_c->b += sign_b * (math::max(1.f, abs(diff_b)));

						//-----------------------------------------------------// move map gen

						auto* move = ram_MoveMap.raw_data(x, y);

						int8_t r_dec = new_c->r - cur_c->r;
						int8_t g_dec = new_c->g - cur_c->g;
						int8_t b_dec = new_c->b - cur_c->b;

						int8_t r_mod = abs(r_dec);
						int8_t g_mod = abs(g_dec);
						int8_t b_mod = abs(b_dec);

						const uint8_t mod = r_mod + g_mod + b_mod;	// summ of color chanels change
						const uint8_t eps = 255.0 / 10.0 * 3.0;		// color change to 10% total (3 * ~10% per chanel)

						move->r = math::max(0, int(move->r - 25.6)); // map cool

						if (mod > eps)
						{
							move->r = math::min(255, int(move->r + 256 / 2));
							move->g = 0;
							move->b = 0;
							move->a = 255;
						}

					}

				}

				//----------------------------------------------------// init gpu resourse

				// or size changed
				MakeOnce
				{
					//gpu_tex.Release();
					//gpu_res.Release();
					//gpu_tex.Create(LOW_VD_RESOLUTION.x, LOW_VD_RESOLUTION.y, 1, gpu_object_access::write);
					//gpu_res.CreateShaderResourse(gpu_tex.GetTexture());
				}

				//----------------------------------------------------// gpu res update part
				bitmap32* gpu_show_bitmap = &ram_NewFrame;

				if (gpu_show_buffer == 0) gpu_show_bitmap = &ram_NewFrame;
				if (gpu_show_buffer == 1) gpu_show_bitmap = &ram_BuckGrownd;
				if (gpu_show_buffer == 2) gpu_show_bitmap = &ram_MoveMap;

				gpu_tex.Update(*gpu_show_bitmap);
			}

		}

		//----------------------------------------------------// tracker
		if (tr_state != track_state::ready_for_track_set)
		{
			//----------------------------------------------------// grub track init data
			if (tr_state == track_state::init_track_bits)
			{
				FLOAT2 pos = track_PA;
				FLOAT2 size = track_PB - track_PA;

				auto search_rect_bitmap = ram_GpuTex.get_subregion_copy({ (int)pos.x, (int)pos.y }, { (int)size.x, (int)size.y });

				img_tracker.SetTrack(search_rect_bitmap);

				tr_state = track_state::ready_for_search_call;
			}

			//----------------------------------------------------// search track in bitmap
			if (tr_state == track_state::searching)
			{

				FLOAT2 pos = search_PA;
				FLOAT2 size = search_PB - search_PA;

				//search_img = RGBA_BITMAP(size.x , size.y);

				bitmap32 search_img = ram_GpuTex.get_subregion_copy({ (int)pos.x, (int)pos.y }, { (int)size.x, (int)size.y });

				INT2 find_pos = img_tracker.FindObject(search_img);

				if (find_pos.y < 50 || find_pos.x < 50)
				{
					find_pos = { 500 , 500 };
				}
				//find_pos = { 0,0 };
				//find_pos.x *= -1;
				//find_pos.y *= -1;

				result_PA = { (float)find_pos.x, (float)find_pos.y };
				result_PB = result_PA + (track_PB - track_PA);

				//search_p1 = { result_p1.x - 10, result_p1.y - 10};
				//search_p2 = { result_p2.x + 10, result_p2.y + 10 };
			}
		}



		//=================================// Show

		FLOAT2 canvas_pos = ImGui::GetCursorScreenPos();
		FLOAT2 canvas_size = { (float)LOW_VD_RESOLUTION.x, (float)LOW_VD_RESOLUTION.y }; //ImGui::GetContentRegionAvail();

		//------------------ Mouse track setter

		if (tr_state == track_state::setting_track_a ||
			tr_state == track_state::setting_track_b)
		{
			ImVec2 mouse_pos_in_canvas = ImVec2(ImGui::GetIO().MousePos.x - canvas_pos.x, ImGui::GetIO().MousePos.y - canvas_pos.y);

			if (mouse_pos_in_canvas.x > 0)
				if (mouse_pos_in_canvas.y > 0)
					if (mouse_pos_in_canvas.x < canvas_size.x)
						if (mouse_pos_in_canvas.y < canvas_size.y)
						{
							if (tr_state == track_state::setting_track_a)
							{
								if (ImGui::IsMouseDown(0))
								{
									tr_state = track_state::setting_track_b;
									track_PA = mouse_pos_in_canvas;
								}
							}
							if (tr_state == track_state::setting_track_b)
							{
								track_PB = mouse_pos_in_canvas;

								if (ImGui::IsMouseDown(0) == false)
								{
									tr_state = track_state::init_track_bits;
									track_PB = mouse_pos_in_canvas;

									FLOAT2 newp1 = { math::min(track_PA.x, track_PB.x), math::min(track_PA.y, track_PB.y) };
									FLOAT2 newp2 = { math::max(track_PA.x, track_PB.x), math::max(track_PA.y, track_PB.y) };
									track_PA = newp1;
									track_PB = newp2;
								}
							}
						}
		}

		//------------------ show image

		//FLOAT2 cur = ImGui::GetCursorScreenPos();
		//ImGui::InvisibleButton("butt", { (float)LOW_VD_RESOLUTION.x, (float)LOW_VD_RESOLUTION.y });
		//ImGui::SetCursorScreenPos(cur);

		ImItem::ShowImage(&gpu_tex, true);

		//ImGui::Image((ImTextureID)(intptr_t)gpu_tex.GetImpl()->uid,
		//	{ (float)LOW_VD_RESOLUTION.x, (float)LOW_VD_RESOLUTION.y });

		if (tr_state != track_state::ready_for_track_set)
		{
			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			FLOAT2 Sa = canvas_pos + track_PA;  //source
			FLOAT2 Sb = canvas_pos + track_PB;
			FLOAT2 Ra = canvas_pos + search_PA; //search region
			FLOAT2 Rb = canvas_pos + search_PB;
			FLOAT2 Fa = canvas_pos + search_PA + result_PA; //result
			FLOAT2 Fb = canvas_pos + search_PA + result_PB;

			draw_list->AddRectFilled(Sa, Sb, ImGui::ColorConvertFloat4ToU32({ 0.0f, 1.0f, 0.0f, 0.15f }));
			draw_list->AddRect(Sa, Sb, ImGui::ColorConvertFloat4ToU32({ 0.0f, 1.0f, 0.0f, 0.60f }));
			draw_list->AddRectFilled(Fa, Fb, ImGui::ColorConvertFloat4ToU32({ 1.0f, 0.0f, 0.0f, 0.30f }));
			draw_list->AddRect(Ra, Rb, ImGui::ColorConvertFloat4ToU32({ 1.0f, 1.0f, 0.0f, 0.60f }));
		}

	}// is_play end
}

#else

void imview::openCV(view_context*)
{
	ImGui::Text("disables");
}

#endif //  ADDON_CV_ENABLED

