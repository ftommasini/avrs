
#include <rtai_lxrt.h>

#include "rttools.hpp"
#include "trackerwiimote.hpp"

TrackerWiimote::TrackerWiimote(std::string object, std::string address,
		const unsigned int read_interval) :
	_focal_length(1380), // const variable
	_read_interval(read_interval)
{
	_running = false;
	_wm = Wiimote::create(address);
	_object_name = object;
}

TrackerWiimote::~TrackerWiimote()
{
	// if thread was started
	if (_thread_id)
		stop();
}

TrackerWiimote::ptr_t TrackerWiimote::create(std::string object, std::string address,
		const unsigned int read_interval)
{
	ptr_t tw(new TrackerWiimote(object, address, read_interval));
	tw->_init();
	return tw;
}

void TrackerWiimote::_init()
{
	int nbPts;
	_object.objectPts = read_object(_object_name.c_str(), &nbPts);
	_object.nbPts = nbPts;

	// TODO: connect???

	_camera.focalLength = _focal_length;
}

void TrackerWiimote::calibrate()
{
	// get reference position
	_ref_position = _current_position;

//	_ref_position.ori.az = -1.1;
//	_ref_position.ori.el = -2.7;
//	_ref_position.ori.ro = 1.0;
//	_ref_position.pos.x = -8.9;
//	_ref_position.pos.y = 6.8;
//	_ref_position.pos.z = 58.8;
}

void TrackerWiimote::start()
{
	_running = true;
	pthread_create(&_thread_id, NULL, TrackerWiimote::_thread_wrapper, this); // create thread
	VERBOSE("Starting tracker ...");
	this->calibrate();
}

void TrackerWiimote::stop()
{
	// TODO: disconnect???

	DPRINT("Ending tracker...");

	// dummy
	void *thread_exit_status;

	_running = false;
	pthread_join(_thread_id, &thread_exit_status);

}

void *TrackerWiimote::_thread_wrapper(void *arg)
{
	return reinterpret_cast<TrackerWiimote*> (arg)->_thread(NULL);
}

void* TrackerWiimote::_thread(void *arg)
{
	RT_TASK *rt_task;
	RTIME interval = nano2count(_read_interval_ms * 1E6);

	rt_allow_nonroot_hrt();
	mlockall(MCL_CURRENT | MCL_FUTURE);

	// create task
	rt_task = rt_task_init_schmod(nam2num("TSKTRA"), 2, 0, 0, SCHED_FIFO, rttools::cpu_id(0));

	if (!rt_task)
	{
		ERROR("Cannot init TRACKER task");
		pthread_exit(NULL);
	}

	MBX *mbx_tracker = rttools::get_mbx(MBX_TRACKER_NAME, MBX_TRACKER_BLOCK * sizeof(avrs::trackerdata_t));

	if (!mbx_tracker)
	{
		ERROR("Cannot init TRACKER mailbox");
		rt_task_delete(rt_task);
		pthread_exit(NULL);
	}

    long int current_time = 0;
    long int previous_time = 0;
    ir_points_t ir_points;
    ir2object_points_t ir2obj;

	int val;

	while (_running)
	{
		// Get current IR points and time
		ir_points = _wm->getIrPoints();
		current_time = ir_points.timestamp;

		if (current_time != previous_time)
		{
			previous_time = current_time;
			this->_posit(&ir_points, &ir2obj);

			_corrected_position = _current_position - _ref_position; /// TODO temporary variable
			//_corrected_position = _current_position;
			//_corrected_position = _current_position.transform(_ref_position);

//			DPRINT("Current: (az, el, ro) = (%3.1f, %3.1f, %3.1f)",
//					_current_position.ori.az,
//					_current_position.ori.el,
//					_current_position.ori.ro);
//
//			DPRINT("Ref: (az, el, ro) = (%3.1f, %3.1f, %3.1f)",
//					_ref_position.ori.az,
//					_ref_position.ori.el,
//					_ref_position.ori.ro);

			// save in _data
			_data = _corrected_position;
			_data.timestamp = current_time;

			// send message (non-blocking)
			val = rt_mbx_send_if(mbx_tracker, &_data, sizeof(avrs::trackerdata_t));
//			val = rt_mbx_send(mbx_tracker, &_data, sizeof(avrs::trackerdata_t));

			if (-EINVAL == val)
			{
				ERROR("Mailbox is invalid");
				break;
			}

			// memory cleanup
			free_int_array(_image.rawImagePts, _image.nbPts);
			free_double_array(_image.imagePts, _image.nbPts);
			free_double_array(_image.imageVects, _image.nbPts);
			free_double_array(_image.oldImageVects, _image.nbPts);
		}

		rt_sleep(interval);  // TODO review
	}

	rt_task_delete(rt_task);
	rttools::del_mbx(MBX_TRACKER_NAME);

	return arg;
}

void TrackerWiimote::_posit(ir_points_t *ir_points, ir2object_points_t *ir2obj)
{
	static bool first_run = true;
	
	int i;
	double dfm[2], dim[2], aux[2];
	int is_max_rank;

	ir2obj->A_d = 1e10;  // set the big number

	assing_points(ir2obj, ir_points->data[0], ir_points->data[1], ir_points->data[2],
			ir_points->data[3]);
	assing_points(ir2obj, ir_points->data[0], ir_points->data[1], ir_points->data[3],
			ir_points->data[2]);
	assing_points(ir2obj, ir_points->data[0], ir_points->data[2], ir_points->data[3],
			ir_points->data[1]);
	assing_points(ir2obj, ir_points->data[0], ir_points->data[2], ir_points->data[1],
			ir_points->data[3]);
	assing_points(ir2obj, ir_points->data[0], ir_points->data[3], ir_points->data[2],
			ir_points->data[1]);
	assing_points(ir2obj, ir_points->data[0], ir_points->data[3], ir_points->data[1],
			ir_points->data[2]);

	assing_points(ir2obj, ir_points->data[1], ir_points->data[0], ir_points->data[2],
			ir_points->data[3]);
	assing_points(ir2obj, ir_points->data[1], ir_points->data[0], ir_points->data[3],
			ir_points->data[2]);
	assing_points(ir2obj, ir_points->data[1], ir_points->data[2], ir_points->data[3],
			ir_points->data[0]);
	assing_points(ir2obj, ir_points->data[1], ir_points->data[2], ir_points->data[0],
			ir_points->data[3]);
	assing_points(ir2obj, ir_points->data[1], ir_points->data[3], ir_points->data[2],
			ir_points->data[0]);
	assing_points(ir2obj, ir_points->data[1], ir_points->data[3], ir_points->data[0],
			ir_points->data[2]);

	assing_points(ir2obj, ir_points->data[2], ir_points->data[0], ir_points->data[1],
			ir_points->data[3]);
	assing_points(ir2obj, ir_points->data[2], ir_points->data[0], ir_points->data[3],
			ir_points->data[1]);
	assing_points(ir2obj, ir_points->data[2], ir_points->data[1], ir_points->data[3],
			ir_points->data[0]);
	assing_points(ir2obj, ir_points->data[2], ir_points->data[1], ir_points->data[0],
			ir_points->data[3]);
	assing_points(ir2obj, ir_points->data[2], ir_points->data[3], ir_points->data[1],
			ir_points->data[0]);
	assing_points(ir2obj, ir_points->data[2], ir_points->data[3], ir_points->data[0],
			ir_points->data[1]);

	assing_points(ir2obj, ir_points->data[3], ir_points->data[0], ir_points->data[1],
			ir_points->data[2]);
	assing_points(ir2obj, ir_points->data[3], ir_points->data[0], ir_points->data[2],
			ir_points->data[1]);
	assing_points(ir2obj, ir_points->data[3], ir_points->data[1], ir_points->data[2],
			ir_points->data[0]);
	assing_points(ir2obj, ir_points->data[3], ir_points->data[1], ir_points->data[0],
			ir_points->data[2]);
	assing_points(ir2obj, ir_points->data[3], ir_points->data[2], ir_points->data[1],
			ir_points->data[0]);
	assing_points(ir2obj, ir_points->data[3], ir_points->data[2], ir_points->data[0],
			ir_points->data[1]);

	// Clockwise
	for (i = 0; i < 2; i++)
	{
		dfm[i] = ir2obj->A_FIpto[i] - ir2obj->A_EXpto[i]; // E2
		dim[i] = ir2obj->A_INpto[i] - ir2obj->A_EXpto[i]; // E1
	}

	if (!((dim[0] * dfm[1] - dim[1] * dfm[0]) >= 0))
	{
		for (i = 0; i < 2; i++)
		{
			aux[i] = ir2obj->A_FIpto[i];
			ir2obj->A_FIpto[i] = ir2obj->A_INpto[i];
			ir2obj->A_INpto[i] = aux[i];
		}
	}

	init_object(&_object);
	init_image(&_object, &_image, ir2obj->A_INpto, ir2obj->A_FIpto, ir2obj->A_LIpto, ir2obj->A_EXpto);
	
	smooth(&_image, first_run);

//	for(i=0; i<4; i++ )
//		{
//			_image.imagePts[i][0] = _image.rawImagePts[i][0];
//			_image.imagePts[i][1] = _image.rawImagePts[i][1];
//			printf("point %d (x, y) = (%f, %f)\n",i ,_image.imagePts[i][0], _image.imagePts[i][1]);
//				
//		}
//		printf("\n");

	//TODO testCamera.focalLength = fLength;
	_camera.focalLength = _focal_length;

	// Find object matrix as pseudoInverse of matrix of object vector coordinates
	is_max_rank = pseudo_inverse(_object.objectCopy, _object.nbPts,
			_object.objectMatrix);

	// Check if matrix rank for object matrix is less than 3
	if (!is_max_rank)
	{
		// TODO Review this!!!
		show_error((char *) "object is too flat; another method is required ");
	}
	else
	{
		// Find object pose by POSIT (Pose from Orthography, Scaling and ITerations)
		posit(_object, _image, &_camera);
		euler_angles(&_camera); // Calculate Euler angles using ZYX convention;

		// Assign final data to the current position
		_current_position.ori.az = (float) _camera.yaw; // not referenced
		_current_position.ori.el = (float) _camera.pitch; // not referenced
		_current_position.ori.ro = (float) _camera.roll; // not referenced
		_current_position.pos.x = (float) _camera.translation[0]; // not referenced
		_current_position.pos.y = (float) _camera.translation[1]; // not referenced
		_current_position.pos.z = (float) _camera.translation[2]; // not referenced

//		DPRINT("(az, el, ro) = (%3.1f, %3.1f, %3.1f)",
//				_current_position.ori.az,
//				_current_position.ori.el,
//				_current_position.ori.ro);
	}
	
	first_run = false;
}

void TrackerWiimote::printRotation(double rotation[][3])
{
	int i, j;

	for (i=0; i<3;i++){
		for (j=0; j<3; j++){
			printf("%.3f ", rotation[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void TrackerWiimote::printPoints(const ir_points_t *ir)
{
	for (int i = 0; i < 4; i++) {
		printf("%d: x: %d, y: %d\n", i + 1, ir->data[i][0], ir->data[i][1]);
	}

	printf("\n");
}
